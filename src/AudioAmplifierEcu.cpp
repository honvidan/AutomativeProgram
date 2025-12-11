#include "AudioAmplifierEcu.h"

#include <iostream>

AudioAmplifierEcu::AudioAmplifierEcu() {
    // Default values
    masterVolume_ = 50; // 0-100
    frontBalance_ = 0;  // -50..+50 (L/R)
    rearBalance_  = 0;  // -50..+50 (L/R)
    bass_   = 5;        // 0-10
    mid_    = 5;        // 0-10
    treble_ = 5;        // 0-10
    temperature_ = 40;  // degrees C simulated
    muted_ = false;
}

std::vector<CanFrame> AudioAmplifierEcu::onReceive(const CanFrame &frame) {
    std::vector<CanFrame> txFrames;

    if (frame.id == CanId::AUDIO_CONTROL_CMD) {
        handleAudioControl(frame);
        // after any control, send back updated status
        txFrames.push_back(buildStatusFrame());
    } else if (frame.id == CanId::DIAG_REQUEST) {
        auto resp = handleDiagRequest(frame);
        txFrames.push_back(resp);
    }

    return txFrames;
}

CanFrame AudioAmplifierEcu::buildStatusFrame() const {
    CanFrame f;
    f.id = CanId::AUDIO_STATUS;
    f.dlc = 8;
    // Simple encoding: one byte per field (clamped)
    f.data[0] = clampToByte(masterVolume_);         // 0-100
    f.data[1] = encodeSigned(frontBalance_);        // -50..+50
    f.data[2] = encodeSigned(rearBalance_);         // -50..+50
    f.data[3] = clampToByte(bass_);                 // 0-10
    f.data[4] = clampToByte(mid_);                  // 0-10
    f.data[5] = clampToByte(treble_);               // 0-10
    f.data[6] = clampToByte(temperature_);          // 0-255 degC
    f.data[7] = muted_ ? 1 : 0;                     // mute flag
    return f;
}

uint8_t AudioAmplifierEcu::clampToByte(int value) {
    if (value < 0) value = 0;
    if (value > 255) value = 255;
    return static_cast<uint8_t>(value);
}

// Encode signed value -128..127 to uint8_t (two's complement)
uint8_t AudioAmplifierEcu::encodeSigned(int v) {
    if (v < -128) v = -128;
    if (v > 127) v = 127;
    return static_cast<uint8_t>(static_cast<int8_t>(v));
}

int AudioAmplifierEcu::decodeSigned(uint8_t b) {
    return static_cast<int>(static_cast<int8_t>(b));
}

void AudioAmplifierEcu::handleAudioControl(const CanFrame &frame) {
    if (frame.dlc < 1) return;

    uint8_t cmd = frame.data[0];
    // Simple command set:
    // 0x01: set master volume (data[1])
    // 0x02: set front/rear balance (data[1]=front, data[2]=rear, signed -50..+50)
    // 0x03: set EQ (data[1]=bass, data[2]=mid, data[3]=treble)
    // 0x04: mute/unmute (data[1]=0/1)
    // 0x05: simulate temperature increase (data[1]=delta)

    switch (cmd) {
        case 0x01:
            if (frame.dlc >= 2) {
                masterVolume_ = frame.data[1];
                std::cout << "[ECU] Set master volume to " << masterVolume_ << "\n";
            }
            break;
        case 0x02:
            if (frame.dlc >= 3) {
                frontBalance_ = decodeSigned(frame.data[1]);
                rearBalance_  = decodeSigned(frame.data[2]);
                std::cout << "[ECU] Set front balance to " << frontBalance_
                          << ", rear balance to " << rearBalance_ << "\n";
            }
            break;
        case 0x03:
            if (frame.dlc >= 4) {
                bass_   = frame.data[1];
                mid_    = frame.data[2];
                treble_ = frame.data[3];
                std::cout << "[ECU] Set EQ bass=" << bass_ << " mid=" << mid_
                          << " treble=" << treble_ << "\n";
            }
            break;
        case 0x04:
            if (frame.dlc >= 2) {
                muted_ = (frame.data[1] != 0);
                std::cout << "[ECU] " << (muted_ ? "Muted" : "Unmuted") << "\n";
            }
            break;
        case 0x05:
            if (frame.dlc >= 2) {
                temperature_ += frame.data[1];
                std::cout << "[ECU] Temperature increased to "
                          << temperature_ << " C\n";
                if (temperature_ > 90) {
                    std::cout << "[ECU] Over-temperature detected, setting DTC 0x1001\n";
                    diag_.addDtc(0x1001); // over-temp DTC
                }
            }
            break;
        default:
            std::cout << "[ECU] Unknown audio control cmd: 0x"
                      << std::hex << (int)cmd << std::dec << "\n";
            break;
    }
}

CanFrame AudioAmplifierEcu::handleDiagRequest(const CanFrame &frame) {
    CanFrame resp;
    resp.id = CanId::DIAG_RESPONSE;
    resp.dlc = 1;
    resp.data[0] = 0xFF; // default = unknown

    if (frame.dlc == 0) {
        return resp;
    }

    uint8_t sub = frame.data[0];
    // 0x01: read DTCs
    // 0x02: clear DTCs

    if (sub == 0x01) {
        // Encode up to 3 DTCs in response (2 bytes each)
        size_t count = diag_.dtcs().size();
        resp.dlc = 1; // first byte = number of DTCs
        size_t maxDtc = 3;
        size_t toSend = (count < maxDtc) ? count : maxDtc;
        resp.data[0] = static_cast<uint8_t>(toSend);
        for (size_t i = 0; i < toSend; ++i) {
            uint16_t code = diag_.dtcs()[i];
            resp.data[1 + i * 2] =
                static_cast<uint8_t>((code >> 8) & 0xFF);
            resp.data[2 + i * 2] =
                static_cast<uint8_t>(code & 0xFF);
        }
        resp.dlc = static_cast<uint8_t>(1 + toSend * 2);
    } else if (sub == 0x02) {
        diag_.clearAll();
        resp.dlc = 1;
        resp.data[0] = 0x00; // 0 = success
    }

    return resp;
}

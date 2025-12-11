#include "AudioToolClient.h"

#include <iostream>
#include <iomanip>

AudioToolClient::AudioToolClient(AudioAmplifierEcu &ecu)
    : ecu_(ecu) {}

uint8_t AudioToolClient::clampToByte(int value) {
    if (value < 0) value = 0;
    if (value > 255) value = 255;
    return static_cast<uint8_t>(value);
}

uint8_t AudioToolClient::encodeSigned(int v) {
    if (v < -128) v = -128;
    if (v > 127) v = 127;
    return static_cast<uint8_t>(static_cast<int8_t>(v));
}

void AudioToolClient::setMasterVolume(int vol) {
    if (vol < 0) vol = 0;
    if (vol > 100) vol = 100;
    CanFrame f;
    f.id = CanId::AUDIO_CONTROL_CMD;
    f.dlc = 2;
    f.data[0] = 0x01;          // cmd: set volume
    f.data[1] = (uint8_t)vol;
    sendAndHandle(f);
}

void AudioToolClient::setBalance(int front, int rear) {
    CanFrame f;
    f.id = CanId::AUDIO_CONTROL_CMD;
    f.dlc = 3;
    f.data[0] = 0x02; // cmd: set balance
    f.data[1] = encodeSigned(front);
    f.data[2] = encodeSigned(rear);
    sendAndHandle(f);
}

void AudioToolClient::setEq(int bass, int mid, int treble) {
    CanFrame f;
    f.id = CanId::AUDIO_CONTROL_CMD;
    f.dlc = 4;
    f.data[0] = 0x03; // cmd: set EQ
    f.data[1] = clampToByte(bass);
    f.data[2] = clampToByte(mid);
    f.data[3] = clampToByte(treble);
    sendAndHandle(f);
}

void AudioToolClient::setMute(bool mute) {
    CanFrame f;
    f.id = CanId::AUDIO_CONTROL_CMD;
    f.dlc = 2;
    f.data[0] = 0x04; // cmd: mute
    f.data[1] = mute ? 1 : 0;
    sendAndHandle(f);
}

void AudioToolClient::increaseTemperature(int delta) {
    CanFrame f;
    f.id = CanId::AUDIO_CONTROL_CMD;
    f.dlc = 2;
    f.data[0] = 0x05; // cmd: increase temp
    f.data[1] = clampToByte(delta);
    sendAndHandle(f);
}

void AudioToolClient::readDtcs() {
    CanFrame f;
    f.id = CanId::DIAG_REQUEST;
    f.dlc = 1;
    f.data[0] = 0x01; // read DTCs
    auto frames = ecu_.onReceive(f);
    for (auto &resp : frames) {
        handleResponse(resp);
    }
}

void AudioToolClient::clearDtcs() {
    CanFrame f;
    f.id = CanId::DIAG_REQUEST;
    f.dlc = 1;
    f.data[0] = 0x02; // clear DTCs
    auto frames = ecu_.onReceive(f);
    for (auto &resp : frames) {
        handleResponse(resp);
    }
}

void AudioToolClient::sendAndHandle(const CanFrame &f) {
    std::cout << "[TOOL] Sending: ";
    printFrame(f);
    auto frames = ecu_.onReceive(f);
    for (auto &resp : frames) {
        handleResponse(resp);
    }
}

void AudioToolClient::handleResponse(const CanFrame &f) {
    if (f.id == CanId::AUDIO_STATUS) {
        std::cout << "[TOOL] Received status: ";
        printFrame(f);
        decodeStatus(f);
    } else if (f.id == CanId::DIAG_RESPONSE) {
        std::cout << "[TOOL] Received diag response: ";
        printFrame(f);
        decodeDiagResponse(f);
    } else {
        std::cout << "[TOOL] Received unknown frame: ";
        printFrame(f);
    }
}

void AudioToolClient::decodeStatus(const CanFrame &f) {
    if (f.dlc < 7) return;
    int volume = f.data[0];
    int frontBal = static_cast<int>(static_cast<int8_t>(f.data[1]));
    int rearBal  = static_cast<int>(static_cast<int8_t>(f.data[2]));
    int bass  = f.data[3];
    int mid   = f.data[4];
    int treble= f.data[5];
    int temp  = f.data[6];
    bool mute = (f.data[7] != 0);

    std::cout << "        Volume=" << volume
              << " FrontBal=" << frontBal
              << " RearBal=" << rearBal
              << " EQ(bass,mid,treble)=" << bass << "," << mid << "," << treble
              << " Temp=" << temp << "C"
              << " Mute=" << (mute ? "Yes" : "No")
              << "\n";
}

void AudioToolClient::decodeDiagResponse(const CanFrame &f) {
    if (f.dlc < 1) return;
    uint8_t count = f.data[0];
    if (count == 0 && f.dlc == 1) {
        std::cout << "        No DTCs or clear success.\n";
        return;
    }
    std::cout << "        DTC count: " << (int)count << '\n';
    for (int i = 0; i < count; ++i) {
        int idx = 1 + i * 2;
        if (idx + 1 >= f.dlc) break;
        uint16_t code = (static_cast<uint16_t>(f.data[idx]) << 8)
                        | static_cast<uint16_t>(f.data[idx + 1]);
        std::cout << "        DTC[" << i << "] = 0x" << std::hex
                  << std::setw(4) << std::setfill('0') << code
                  << std::dec << '\n';
    }
}

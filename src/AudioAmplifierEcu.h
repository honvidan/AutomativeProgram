#pragma once

#include "CanType.h"
#include "Diagnostics.h"

#include <vector>

class AudioAmplifierEcu {
public:
    AudioAmplifierEcu();

    // Handle an incoming CAN frame from the bus
    // (control commands or diagnostic requests)
    std::vector<CanFrame> onReceive(const CanFrame &frame);

    // Build a periodic status frame (could be sent every 100 ms in real ECU)
    CanFrame buildStatusFrame() const;

private:
    // Internal state
    int masterVolume_;
    int frontBalance_;
    int rearBalance_;
    int bass_;
    int mid_;
    int treble_;
    int temperature_; // simulated
    bool muted_;
    Diagnostics diag_;

    static uint8_t clampToByte(int value);
    static uint8_t encodeSigned(int v);
    static int decodeSigned(uint8_t b);

    void handleAudioControl(const CanFrame &frame);
    CanFrame handleDiagRequest(const CanFrame &frame);
};

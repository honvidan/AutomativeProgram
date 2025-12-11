#pragma once

#include "CanType.h"
#include "AudioAmplifierEcu.h"

class AudioToolClient {
public:
    explicit AudioToolClient(AudioAmplifierEcu &ecu);

    void setMasterVolume(int vol);
    void setBalance(int front, int rear);
    void setEq(int bass, int mid, int treble);
    void setMute(bool mute);
    void increaseTemperature(int delta);

    void readDtcs();
    void clearDtcs();

private:
    AudioAmplifierEcu &ecu_;

    static uint8_t clampToByte(int value);
    static uint8_t encodeSigned(int v);

    void sendAndHandle(const CanFrame &f);
    void handleResponse(const CanFrame &f);

    void decodeStatus(const CanFrame &f);
    void decodeDiagResponse(const CanFrame &f);
};

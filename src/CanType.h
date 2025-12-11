#pragma once

#include <cstdint>
#include <string>

// Simple CAN frame representation
struct CanFrame {
    uint32_t id = 0;      // 11-bit or 29-bit identifier (we'll use 11-bit style IDs)
    uint8_t  dlc = 0;     // data length code (0-8)
    uint8_t  data[8]{};   // payload
};

namespace CanId {
    constexpr uint32_t AUDIO_CONTROL_CMD = 0x200;
    constexpr uint32_t AUDIO_STATUS      = 0x201;
    constexpr uint32_t DIAG_REQUEST      = 0x700;
    constexpr uint32_t DIAG_RESPONSE     = 0x708;
}

// Utility for printing frames (for debugging / learning)
void printFrame(const CanFrame &f, const std::string &prefix = "");

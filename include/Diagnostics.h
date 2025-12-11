#pragma once

#include <cstdint>
#include <vector>

class Diagnostics {
public:
    void addDtc(uint16_t code);
    bool hasDtc(uint16_t code) const;
    const std::vector<uint16_t>& dtcs() const;
    void clearAll();
    bool empty() const;

private:
    std::vector<uint16_t> dtcs_;
};

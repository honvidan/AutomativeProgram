#include "Diagnostics.h"

void Diagnostics::addDtc(uint16_t code) {
    if (!hasDtc(code)) {
        dtcs_.push_back(code);
    }
}

bool Diagnostics::hasDtc(uint16_t code) const {
    for (auto c : dtcs_) {
        if (c == code) return true;
    }
    return false;
}

const std::vector<uint16_t>& Diagnostics::dtcs() const {
    return dtcs_;
}

void Diagnostics::clearAll() {
    dtcs_.clear();
}

bool Diagnostics::empty() const {
    return dtcs_.empty();
}

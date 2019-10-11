#pragma once

#include <cstdint>

struct OpusEncoder;
class OpusEncoder {
public:
    OpusEncoder();
    ~OpusEncoder();

    void EncodeData(uint8_t* data, uint32_t len);

private:
    OpusEncoder* opus_enc_;
    uint8_t opus_data_[1024];
};

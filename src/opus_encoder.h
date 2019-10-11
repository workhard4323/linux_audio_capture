#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include "audio_capture.h"

struct OpusEncoder;
class OpusEncoder {
public:
    using Callback = std::function<void(uint8_t* data, uint32_t len)>;

public:
    OpusEncoder();
    ~OpusEncoder();

    void EncodeData(uint8_t* data, uint32_t len);

    void Start();

    void Stop();

    void SetCallback(Callback callback);

private:
    OpusEncoder* opus_enc_;
    uint8_t opus_data_[1024];
    bool running_ = false;
    Callback callback_;
    std::unique_ptr<AudioCapture> audio_capture_;
};

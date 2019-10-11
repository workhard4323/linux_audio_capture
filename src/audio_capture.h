#pragma once

#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <cstdint>

#include "audio_info.h"

class AudioCapture {
public:
    using Callback = std::function<void(uint8_t* data, uint32_t len)>;

public:
    AudioCapture();
    ~AudioCapture();

    void Start();

    void Stop();

    void CaptureLoop();

    void SetCallback(Callback callback);

private:
    std::thread work_thread_;
    bool running_ = false;
    std::string source_device_;
    std::string sink_device_;
    std::string default_sink_device_;

    std::vector<std::string> source_devices_vec_;
    std::vector<std::string> sink_devices_vec_;

    SinkInfo sink_info_;
    Callback callback_;
};

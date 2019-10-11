#include "audio_capture.h"

#include <chrono>
#include <iostream>
#include <string>

#include <pulse/error.h>
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>

#include "audio_utils.h"

AudioCapture::AudioCapture() {
    source_devices_vec_ = GetPulseAudioSourceDevice();
    sink_devices_vec_ = GetPulseAudioSinkDevice();

    source_device_ = source_devices_vec_[0];
    sink_device_ = sink_devices_vec_[0];
    default_sink_device_ = sink_devices_vec_[0];

    sink_info_ = GetPulseAudioSinkDeviceInfo(default_sink_device_);
    std::cout << "volume1:" << sink_info_.volume.values[0] << std::endl;
    std::cout << "volume2:" << sink_info_.volume.values[1] << std::endl;
    std::cout << "mute:" << sink_info_.mute << std::endl;
}

AudioCapture::~AudioCapture() { Stop(); }

void AudioCapture::Start() {
    if (running_) { return; }
    running_ = true;
    std::vector<int> input_list = GetPulseAudioSinkInputList();
    SetPulseAudioCurrentSinkDevice(sink_device_, input_list);
    work_thread_ = std::thread(&AudioCapture::CaptureLoop, this);
}

void AudioCapture::Stop() {
    if (!running_) { return; }
    running_ = false;
    std::vector<int> input_list = GetPulseAudioSinkInputList();
    SetPulseAudioCurrentSinkDevice(default_sink_device_, input_list);
    SetPulseAudioSinkDeviceVolume(default_sink_device_, &sink_info_.volume);
    SetPulseAudioSinkDeviceMute(default_sink_device_, sink_info_.mute);
    work_thread_.join();
}

void AudioCapture::CaptureLoop() {
    static const pa_sample_spec ss = {
            .format = PA_SAMPLE_FLOAT32LE, .rate = 48000, .channels = 2};
    pa_simple* s = NULL;
    int error;
    const int kBufSize = 960 * 4 * 2;
    std::vector<uint8_t> buffer(kBufSize);

    std::cout << "source device:" << source_device_ << std::endl;

    if (!(s = pa_simple_new(NULL, "audio recorder", PA_STREAM_RECORD,
                            source_device_.c_str(), "record", &ss, NULL, NULL,
                            &error))) {
        running_ = false;
        if (s) pa_simple_free(s);
        return;
    }

    std::cout << "loop capture start" << std::endl;
    while (running_) {
        if (pa_simple_read(s, buffer.data(), kBufSize, &error) < 0) { break; }
        std::cout << "size:" << kBufSize << std::endl;
    }

    running_ = false;
    if (s) pa_simple_free(s);
}

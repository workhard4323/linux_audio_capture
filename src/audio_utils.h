#pragma once

#include <string>
#include <vector>

#include "audio_info.h"

// 获取audio source device列表
std::vector<std::string> GetPulseAudioSourceDevice();

// 获取audio sink device列表
std::vector<std::string> GetPulseAudioSinkDevice();

// 获取指定sink device的信息，比如音量，是不是静音
SinkInfo GetPulseAudioSinkDeviceInfo(const std::string& sink_device);

// 设置指定sink device的音量
void SetPulseAudioSinkDeviceVolume(const std::string& sink_device,
                                   pa_cvolume* volume);

// 设置指定sink device是否静音
void SetPulseAudioSinkDeviceMute(const std::string& sink_device, int mute);

// 获取sinl input list
std::vector<int> GetPulseAudioSinkInputList();

// 设置当前的sink device
void SetPulseAudioCurrentSinkDevice(const std::string& sink_device,
                                    std::vector<int> sink_input_vector);

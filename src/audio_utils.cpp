#include "audio_utils.h"

#include <iostream>
#include <memory>

#include <pulse/error.h>
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>

enum class PulseAudioContextState {
    PULSE_CONTEXT_INITIALIZING,
    PULSE_CONTEXT_READY,
    PULSE_CONTEXT_FINISHED
};

void DisconnectPulseAudioContext(pa_mainloop** pa_ml, pa_context** pa_ctx) {
    assert(pa_ml);
    assert(pa_ctx);

    if (*pa_ctx) {
        pa_context_set_state_callback(*pa_ctx, NULL, NULL);
        pa_context_disconnect(*pa_ctx);
        pa_context_unref(*pa_ctx);
    }

    if (*pa_ml) pa_mainloop_free(*pa_ml);
    *pa_ml = NULL;
    *pa_ctx = NULL;
}

void PaContextStateCallback(pa_context* pa_ctx, void* userdata) {
    PulseAudioContextState* context_state = (PulseAudioContextState*)userdata;
    switch (pa_context_get_state(pa_ctx)) {
        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
            *context_state = PulseAudioContextState::PULSE_CONTEXT_FINISHED;
            break;
        case PA_CONTEXT_READY:
            *context_state = PulseAudioContextState::PULSE_CONTEXT_READY;
            break;
        default:
            break;
    }
}

int ConnectPulseAudioContext(pa_mainloop** pa_ml, pa_context** pa_ctx,
                             const char* server, const char* description) {
    int ret;
    *pa_ml = NULL;
    *pa_ml = pa_mainloop_new();
    if (!(*pa_ml)) { return -1; }

    pa_mainloop_api* pa_mlapi = NULL;
    pa_mlapi = pa_mainloop_get_api(*pa_ml);
    if (!pa_mlapi) { return -1; }

    *pa_ctx = NULL;
    *pa_ctx = pa_context_new(pa_mlapi, description);
    if (!(*pa_ctx)) { return -1; }

    PulseAudioContextState context_state =
            PulseAudioContextState::PULSE_CONTEXT_INITIALIZING;
    pa_context_set_state_callback(*pa_ctx, PaContextStateCallback,
                                  &context_state);
    if (pa_context_connect(*pa_ctx, server, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        return -1;
    }

    while (context_state == PulseAudioContextState::PULSE_CONTEXT_INITIALIZING)
        pa_mainloop_iterate(*pa_ml, 1, NULL);
    if (context_state == PulseAudioContextState::PULSE_CONTEXT_FINISHED) {
        return -1;
    }
    return 0;

}

void PulseAudioSourceDeviceCallback(pa_context* c, const pa_source_info* dev,
                                    int eol, void* userdata) {
    std::cout << "callback" << std::endl;
    std::vector<std::string>* devices = (std::vector<std::string>*)userdata;
    if (dev != nullptr && dev->monitor_of_sink != PA_INVALID_INDEX) {
        devices->push_back(dev->name);
    }
}

std::vector<std::string> GetPulseAudioSourceDevice() {
    std::vector<std::string> source_devices_vec;
    pa_mainloop* pa_ml = nullptr;
    pa_operation* pa_op = nullptr;
    pa_context* pa_ctx = nullptr;
    std::cout << "before connect" << std::endl;
    ConnectPulseAudioContext(&pa_ml, &pa_ctx, nullptr, "audio recorder");
    std::cout << "connect" << std::endl;
    std::shared_ptr<void> raii_connect(nullptr, [&](void*) {
        DisconnectPulseAudioContext(&pa_ml, &pa_ctx);
    });
    if (pa_ctx == nullptr) { return source_devices_vec; }
    pa_op = pa_context_get_source_info_list(
            pa_ctx, PulseAudioSourceDeviceCallback, &source_devices_vec);
    std::cout << "1" << std::endl;
    while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(pa_ml, 1, nullptr);
    }
    std::cout << "2" << std::endl;
    pa_operation_unref(pa_op);
    return source_devices_vec;
}

void PulseAudioSinkDeviceCallback(pa_context* c, const pa_sink_info* dev,
                                  int eol, void* userdata) {
    std::vector<std::string>* devices = (std::vector<std::string>*)userdata;
    if (dev != nullptr) { devices->push_back(dev->name); }
}

std::vector<std::string> GetPulseAudioSinkDevice() {
    std::vector<std::string> sink_devices_vec;
    pa_mainloop* pa_ml = nullptr;
    pa_operation* pa_op = nullptr;
    pa_context* pa_ctx = nullptr;
    ConnectPulseAudioContext(&pa_ml, &pa_ctx, nullptr, "audio recorder");
    std::shared_ptr<void> raii_connect(nullptr, [&](void*) {
        DisconnectPulseAudioContext(&pa_ml, &pa_ctx);
    });
    if (pa_ctx == nullptr) { return sink_devices_vec; }
    pa_op = pa_context_get_sink_info_list(pa_ctx, PulseAudioSinkDeviceCallback,
                                          &sink_devices_vec);
    while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(pa_ml, 1, nullptr);
    }
    pa_operation_unref(pa_op);
    return sink_devices_vec;
}

void PulseAudioSinkDeviceInfoCallback(pa_context* c, const pa_sink_info* info,
                                      int eol, void* userdata) {
    SinkInfo* sink_info = (SinkInfo*)userdata;
    if (info != nullptr) {
        sink_info->volume = info->volume;
        sink_info->mute = info->mute;
    }
}

SinkInfo GetPulseAudioSinkDeviceInfo(const std::string& sink_device) {
    SinkInfo sink_info;
    pa_mainloop* pa_ml = nullptr;
    pa_operation* pa_op = nullptr;
    pa_context* pa_ctx = nullptr;
    ConnectPulseAudioContext(&pa_ml, &pa_ctx, nullptr, "audio recorder");
    std::shared_ptr<void> raii_connect(nullptr, [&](void*) {
        DisconnectPulseAudioContext(&pa_ml, &pa_ctx);
    });
    if (pa_ctx == nullptr) { return sink_info; }
    pa_op = pa_context_get_sink_info_by_name(pa_ctx, sink_device.c_str(),
                                             PulseAudioSinkDeviceInfoCallback,
                                             &sink_info);
    while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(pa_ml, 1, nullptr);
    }
    pa_operation_unref(pa_op);
    return sink_info;
}

void SetPulseAudioSinkDeviceVolumeCallback(pa_context* c, int success,
                                           void* userdata) {
    if (success != 1) {
        std::cout << " SetPulseAudioSinkDeviceVolumeCallback error"
                  << std::endl;
    }
}

void SetPulseAudioSinkDeviceVolume(const std::string& sink_device,
                                   pa_cvolume* volume) {
    pa_mainloop* pa_ml = nullptr;
    pa_operation* pa_op = nullptr;
    pa_context* pa_ctx = nullptr;
    ConnectPulseAudioContext(&pa_ml, &pa_ctx, nullptr, "audio recorder");
    std::shared_ptr<void> raii_connect(nullptr, [&](void*) {
        DisconnectPulseAudioContext(&pa_ml, &pa_ctx);
    });
    if (pa_ctx == nullptr) { return; }
    pa_op = pa_context_set_sink_volume_by_name(
            pa_ctx, sink_device.c_str(), volume,
            SetPulseAudioSinkDeviceVolumeCallback, nullptr);
    while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(pa_ml, 1, nullptr);
    }
    pa_operation_unref(pa_op);
}

void SetPulseAudioSinkDeviceMuteCallback(pa_context* c, int success,
                                         void* userdata) {
    if (success != 1) {
        std::cout << "SetPulseAudioSinkDeviceMuteCallback error" << std::endl;
    }
}

void SetPulseAudioSinkDeviceMute(const std::string& sink_device, int mute) {
    pa_mainloop* pa_ml = nullptr;
    pa_operation* pa_op = nullptr;
    pa_context* pa_ctx = nullptr;
    ConnectPulseAudioContext(&pa_ml, &pa_ctx, nullptr, "audio recorder");
    std::shared_ptr<void> raii_connect(nullptr, [&](void*) {
        DisconnectPulseAudioContext(&pa_ml, &pa_ctx);
    });
    if (pa_ctx == nullptr) { return; }
    pa_op = pa_context_set_sink_mute_by_name(
            pa_ctx, sink_device.c_str(), mute,
            SetPulseAudioSinkDeviceMuteCallback, nullptr);

    while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(pa_ml, 1, nullptr);
    }
    pa_operation_unref(pa_op);
}

void PulseAudioSinkInputListCallback(pa_context* c,
                                     const pa_sink_input_info* info, int eol,
                                     void* userdata) {
    std::vector<int>* devices = (std::vector<int>*)userdata;
    if (info != nullptr) { devices->push_back(info->index); }
}

std::vector<int> GetPulseAudioSinkInputList() {
    std::vector<int> input_vec;
    pa_mainloop* pa_ml = nullptr;
    pa_operation* pa_op = nullptr;
    pa_context* pa_ctx = nullptr;
    ConnectPulseAudioContext(&pa_ml, &pa_ctx, nullptr, "audio recorder");
    std::shared_ptr<void> raii_connect(nullptr, [&](void*) {
        DisconnectPulseAudioContext(&pa_ml, &pa_ctx);
    });
    if (pa_ctx == nullptr) { return input_vec; }

    pa_op = pa_context_get_sink_input_info_list(
            pa_ctx, PulseAudioSinkInputListCallback, &input_vec);

    while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(pa_ml, 1, nullptr);
    }
    pa_operation_unref(pa_op);
    return input_vec;
}

void PulseAudioCurrentSinkCallback(pa_context* c, int success, void* userdata) {
    if (success != 1) {
        std::cout << "PulseAudioCurrentSinkCallback error" << std::endl;
    }
}

// 将所有的槽都切换为我们指定的槽
void SetPulseAudioCurrentSinkDevice(const std::string& sink_device,
                                    std::vector<int> sink_input_vector) {
    pa_mainloop* pa_ml = nullptr;
    pa_operation* pa_op = nullptr;
    pa_operation* move_operation = nullptr;
    pa_context* pa_ctx = nullptr;
    ConnectPulseAudioContext(&pa_ml, &pa_ctx, nullptr, "audio recorder");
    std::shared_ptr<void> raii_connect(nullptr, [&](void*) {
        DisconnectPulseAudioContext(&pa_ml, &pa_ctx);
    });
    if (pa_ctx == nullptr) { return; }

    pa_op = pa_context_set_default_sink(pa_ctx, sink_device.c_str(),
                                        PulseAudioCurrentSinkCallback, nullptr);

    for (int i = 0; i < (int)sink_input_vector.size(); ++i) {
        move_operation = pa_context_move_sink_input_by_name(
                pa_ctx, sink_input_vector[i], sink_device.c_str(),
                PulseAudioCurrentSinkCallback, nullptr);
        while (pa_operation_get_state(move_operation) == PA_OPERATION_RUNNING) {
            pa_mainloop_iterate(pa_ml, 1, nullptr);
        }
        pa_operation_unref(move_operation);
    }

    while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(pa_ml, 1, nullptr);
    }
    pa_operation_unref(pa_op);
}

#include "opus_encoder.h"

#include <opus/opus.h>
#include <iostream>

OpusEncoder::OpusEncoder() : audio_capture_(new AudioCapture()) {
    int error;
    opus_enc_ = opus_encoder_create(48000, 2, OPUS_APPLICATION_AUDIO, &error);
    if (error == 0) {
        // 设置编码码率，如果想要压缩比大一点，那么可以将码率设置小一点
        opus_encoder_ctl(opus_enc_, OPUS_SET_BITRATE(128000));
        // 设置是否是动态码率1:动态，0:静态，编码器默认为动态码率
        opus_encoder_ctl(opus_enc_, OPUS_SET_VBR(1));
    }
}

OpusEncoder::~OpusEncoder() {
    if (opus_enc_) { opus_encoder_destroy(opus_enc_); }
}

void OpusEncoder::EncodeData(uint8_t* data, uint32_t len) {}

void OpusEncoder::Start() {
    if (running_ || !opus_enc_) { return; }
    audio_capture_->SetCallback([this](uint8_t* data, uint32_t len) {
        if (!opus_enc_ || !data) { return; }
        opus_int32 opus_len;
        opus_len = opus_encode_float(opus_enc_, (const float*)data, len / 8,
                                     opus_data_, sizeof(opus_data_));
        callback_(opus_data_, opus_len);
        // std::cout << "opus_len:" << opus_len << std::endl;
    });
    audio_capture_->Start();
    running_ = true;
}

void OpusEncoder::Stop() {
    if (!running_) { return; }
    audio_capture_->Stop();
    running_ = false;
}

void OpusEncoder::SetCallback(Callback callback) {
    callback_ = std::move(callback);
}

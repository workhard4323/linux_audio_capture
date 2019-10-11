# linux_audio_capture
linux下使用PulseAudio捕获音频数据
linux下使用PulseAudio捕获音频
一、source device和sink device
1、source device：source可以译为源泉的意思，表示数据的来源
2、sink device：sink可以译为水槽的意思，表示接收的一端
可以理解为水（音频数据）从source开始流出，然后流入到水槽（sink）中

二、捕获音频数据
1、GetDefaultSourceDevice：获取默认的source设备
2、pa_simple_new：
3、pa_simple_read
4、pa_simple_free

三、通过API获取音频信息
1、ConnectPulseAudioContext
2、GetPulseAudioInfo
3、DisconnectPulseAudioContext

四、ConnectPulseAudioContext
1、pa_mainloop_new
2、pa_mainloop_get_api
3、pa_context_new
4、pa_context_set_state_callback
5、pa_context_connect

五、使用opus编码
// 创建编码器
OpusEncoder *opus_encoder_create(
    opus_int32 Fs,
    int channels,
    int application,
    int *error
)

// 修改编码器参数
int opus_encoder_ctl(
    OpusEncoder *st,
    int request, ...
)

// 创建解码器
OpusDecoder *opus_decoder_create(
    opus_int32 Fs,
    int channels,
    int *error
)

// 将PCM编码成opus
opus_int32 opus_encode(
    OpusEncoder *st,
    const opus_int16 *pcm,
    int frame_size,
    unsigned char *data,
    opus_int32 max_data_bytes
)

// 从opus中译码出PCM
int opus_decode(
    OpusDecoder *st,
    const unsigned char *data,
    opus_int32 len,
    opus_int16 *pcm,
    int frame_size,
    int decode_fec
)

// 销毁编码器
int opus_encoder_destroy (OpusEncoder *st)

// 销毁解码器
int opus_decoder_destroy (OpusDecoder *st)

// 设置静/动态码率，默认为动态
opus_encoder_ctl(_encoder, OPUS_SET_VBR(0));  // 0固定码率，1动态码率

// 设置编码比特率，比特率越小，压缩比越大
opus_encoder_ctl(_encoder, OPUS_SET_BITRATE(BITRATE));

// 如果你用于语音而不是音乐，那你完全可以设置如下，以使编码器针对语音模式做优化处理
opus_encoder_ctl(_encoder,OPUS_SET_APPLICATION(OPUS_APPLICATION_VOIP));
opus_encoder_ctl(_encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));

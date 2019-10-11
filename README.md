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

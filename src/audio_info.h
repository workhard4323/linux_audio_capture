#pragma once

/*sink:槽，接收数据的一方
 *source:源泉，从源泉中获取数据，然后给到sink
 *好比source是水流的起点，sink是槽接收水的一方
 */

#include <pulse/pulseaudio.h>

struct SinkInfo {
    pa_cvolume volume;  //音量
    int mute;           //是不是静音，1表示静音，0表示非静音
};

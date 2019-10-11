#include <iostream>

#include "audio_capture.h"

int main(void) {
    AudioCapture audio_capture;
    std::cout << "start" << std::endl;
    audio_capture.Start();
    getchar();
    return 0;
}

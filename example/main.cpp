#include <iostream>

#include "opus_encoder.h"

int main(void) {
    OpusEncoder opus_encoder;
    opus_encoder.SetCallback([&](uint8_t* data, uint32_t len) {
        std::cout << "len:" << len << std::endl;
    });
    opus_encoder.Start();
    getchar();
    return 0;
}

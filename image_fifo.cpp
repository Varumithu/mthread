//
// Created by valmit on 19/11/18.
//

#include "image_fifo.h"

#include <algorithm>
#include <cassert>

ImageFIFO::ImageFIFO(size_t blockSize, size_t maxBlocks) {

    free.resize(maxBlocks);
    std::for_each(free.begin(), free.end(), [blockSize](void*& cur){ cur = operator new(blockSize); }); // seems to work now

}

void ImageFIFO::addReady(void *data) {
    std::lock_guard<std::mutex> guard(ImageGuard);
    ready.push_back(data);
}

void* ImageFIFO::getReady() {
    void* return_val = nullptr;

    std::lock_guard<std::mutex> guard(ImageGuard);
    if (ready.size() > 0) {
        return_val = ready.front();
        InUseReading.insert(return_val);
        ready.erase(ready.begin());
    }
    return return_val;
}

void ImageFIFO::addFree(void *data) {
    std::lock_guard<std::mutex> guard(ImageGuard);
    free.push_back(data);
}

void* ImageFIFO::getFree() {
    void* return_val = nullptr;
    std::lock_guard<std::mutex> guard(ImageGuard);
    if (free.size() > 0) {
        return_val = free.front();
        InUseWriting.insert(return_val);
        free.erase(free.begin());
    }
    return return_val;
}

ImageFIFO::~ImageFIFO() {
    std::for_each(free.begin(), free.end(), [](void*& cur){ operator delete(cur); cur = nullptr; } );
    std::for_each(ready.begin(), ready.end(), [](void*& cur){ operator delete(cur); cur = nullptr; } );
}

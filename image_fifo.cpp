//
// Created by valmit on 19/11/18.
//

#include "image_fifo.h"

#include <algorithm>
#include <cassert>

ImageFIFO::ImageFIFO(size_t blockSize, size_t maxBlocks) {

    //free.resize(maxBlocks);
   // std::for_each(free.begin(), free.end(), [blockSize](void*& cur){ cur = operator new(blockSize); });
    for (size_t i = 0; i < maxBlocks; ++i) {
        free.push_back({operator new(blockSize), i});
    }
}

void ImageFIFO::addReady(void *data) {
    std::lock_guard<std::mutex> guard(ImageGuard);
    auto ind = InUseWriting.find(data);
    assert(ind != InUseWriting.end()); // since it is not good to throw exceptions from shared stuff and something is really broke in this case, i just assert. The lecturer said we could do so
    ready.push_back(*ind);
    InUseWriting.erase(ind);
}

void* ImageFIFO::getReady() {
    void* return_val = nullptr;

    std::lock_guard<std::mutex> guard(ImageGuard);
    if (ready.size() > 0) {
        return_val = ready.front().first;
        InUseReading[return_val] = ready.front().second;
        ready.erase(ready.begin());
    }
    return return_val;
}

void ImageFIFO::addFree(void *data) {
    std::lock_guard<std::mutex> guard(ImageGuard);
    auto ind = InUseReading.find(data);
    assert(ind != InUseReading.end()); // since it is not good to throw exceptions from shared stuff and something is really broke in this case, i just assert. The lecturer said we could do so
    free.push_back(*ind);
    InUseReading.erase(ind);
}

void* ImageFIFO::getFree() {
    void* return_val = nullptr;
    std::lock_guard<std::mutex> guard(ImageGuard);
    if (free.size() > 0) {
        return_val = free.front().first;
        InUseWriting[return_val] = free.front().second;
        free.erase(free.begin());
    }
    return return_val;
}

ImageFIFO::~ImageFIFO() {
    std::for_each(free.begin(), free.end(), [](std::pair<void*, size_t>& cur){ operator delete(cur.first); cur.first = nullptr; } );
    std::for_each(ready.begin(), ready.end(), [](std::pair<void*, size_t>& cur){ operator delete(cur.first); cur.first = nullptr; } );
}

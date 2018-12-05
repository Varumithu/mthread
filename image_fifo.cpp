//
// Created by valmit on 19/11/18.
//

#include "image_fifo.h"

#include <algorithm>
#include <cassert>

ImageFIFO::ImageFIFO(size_t blockSize, size_t maxBlocks) {

    free.resize(maxBlocks);
    buffer = operator new(blockSize * maxBlocks);
    auto cur = free.begin();
    for (size_t i = 0; i < maxBlocks; ++i) {
        *cur = reinterpret_cast<void*>(reinterpret_cast<char*>(buffer) + i * blockSize);
        ++cur;
    }
}

void ImageFIFO::addReady(void *data) {
    std::lock_guard<std::mutex> guard(ImageGuard);
    auto ind = InUseWriting.find(data);
    assert(ind != InUseWriting.end()); // since it is not good to throw exceptions from shared stuff and something is really broke in this case, i just assert. The lecturer said we could do so
    ready.push_back({ind->second, data});
    InUseWriting.erase(ind);
}

void* ImageFIFO::getReady() {
    void* return_val = nullptr;
    std::lock_guard<std::mutex> guard(ImageGuard);
    auto min_ready = std::min_element(ready.begin(), ready.end(),
                     [](std::pair<size_t, void*>& first, std::pair<size_t, void*>& second){ return first.first < second.first; });
    auto min_InUse = std::min_element(InUseWriting.begin(), InUseWriting.end(),
                        [](const std::pair<void*, size_t>& first, const std::pair<void*, size_t>& second){ return first.second < second.second; });
    if (min_ready != ready.end() && (min_InUse == InUseWriting.end() || min_ready->first < min_InUse->second)) {
        return_val = min_ready->second;
        InUseReading.insert(return_val);
        ready.erase(min_ready);
    }
    return return_val;
}

void ImageFIFO::addFree(void *data) {
    std::lock_guard<std::mutex> guard(ImageGuard);
    auto ind = InUseReading.find(data);
    assert(ind != InUseReading.end()); // since it is not good to throw exceptions from shared stuff and something is really broke in this case, i just assert. The lecturer said we could do so
    free.push_back(data);
    InUseReading.erase(ind);
}

void* ImageFIFO::getFree() {
    void* return_val = nullptr;
    std::lock_guard<std::mutex> guard(ImageGuard);

    if (free.size() > 0) {
        return_val = *free.begin();
        InUseWriting[return_val] = this->counter;
        ++counter;
        free.erase(free.begin());
    }
    return return_val;
}

ImageFIFO::~ImageFIFO() {
    operator delete(buffer);
}

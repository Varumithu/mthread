//
// Created by valmit on 19/11/18.
//

#ifndef MULTITHREAD_IMAGE_FIFO_H
#define MULTITHREAD_IMAGE_FIFO_H

#include <set>
#include <mutex>
#include <list>

class ImageFIFO final{
private:
    std::mutex ImageGuard;
    std::list<void*> free, ready;
    std::set<void*> InUseReading, InUseWriting;
    //size_t blockSize, maxBlocks; // I don't use them anywhere except in ctor but I sort of feel like I should? Like some assertions or something?
public:
    ~ImageFIFO();
    explicit ImageFIFO(size_t blockSize, size_t maxBlocks);
    void * getFree();
    void addReady(void * data);
    void * getReady();
    void addFree(void *data);
};

#endif //MULTITHREAD_IMAGE_FIFO_H

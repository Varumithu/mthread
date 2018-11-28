//
// Created by valmit on 19/11/18.
//

#ifndef MULTITHREAD_IMAGE_FIFO_H
#define MULTITHREAD_IMAGE_FIFO_H

#include <map>
#include <mutex>
#include <list>
#include <set>

class ImageFIFO final{
private:
    size_t counter = 0; // i don't think it needs to be atomic if it is only used in mutex'd sections
    std::mutex ImageGuard;
    std::list<void*> free;
    std::list<std::pair<size_t, void*>> ready;
    std::map<void*, size_t> InUseWriting;
    std::set<void*> InUseReading;
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

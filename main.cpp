//
// Created by valmit on 20/11/18.
//

#include <gtest/gtest.h>

#include <thread>
#include <vector>
#include <fstream>
#include <cstring>
#include <memory>

#include "image_fifo.h"

void test_function_writer(ImageFIFO& shared) {
    int arr[5] = {0, 1, 2, 3, 4};
    int* p = nullptr;
    size_t i = 0;
    for (;;) {
        p = reinterpret_cast<int*>(shared.getFree());
        if (p != nullptr) {
            *p = arr[i];
            shared.addReady(p);
            p = nullptr;
            ++i;
        }
        else
            std::this_thread::yield();
        if (i == 5)
            break;
    }
}

void test_function_reader(ImageFIFO& shared, std::vector<int>& res) {
    size_t count = 0;
    while(1 == 1) {
        void* p = shared.getReady();
        if (p != nullptr) {
            ++count;
            res.push_back(*reinterpret_cast<int*>(p));
            shared.addFree(p);
            p = nullptr;
        }
        else
            std::this_thread::yield();
        if (count == 5)
            break;
    }
}



TEST(FIFO_testing, single_thread_test) {
    ImageFIFO imfif(sizeof(int), 10);
    int arr[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int* cur;
    for (size_t i = 0; i < 10; ++i) {
        cur = reinterpret_cast<int*>(imfif.getFree());
        *cur = arr[i];
        imfif.addReady(cur);
    }
    for (size_t i = 0; i < 10; ++i) {
        cur = reinterpret_cast<int*>(imfif.getReady());
        ASSERT_EQ(arr[i], *cur);
        imfif.addFree(cur);
    }
}

TEST(FIFO_testing, SmallTestTwoThreads) {
    ImageFIFO imfif(sizeof(int), 5);
    std::vector<int> res;
    std::thread t1(test_function_writer, std::ref(imfif));
    std::thread t2(test_function_reader, std::ref(imfif), std::ref(res));
    t2.join();
    t1.join();
    std::vector<int> check{0, 1, 2, 3, 4};
    ASSERT_EQ(res, check);
}

TEST(FIFO_testing, SmallTestNotEnoughSpaceForAll) {
    ImageFIFO imfif(sizeof(int), 2); // only two spots in FIFO so writer will have to wait for reader to free spot sometimes i hope
    std::vector<int> res;
    std::thread t1(test_function_writer, std::ref(imfif));
    std::thread t2(test_function_reader, std::ref(imfif), std::ref(res));
    t2.join();
    t1.join();
    std::vector<int> check{0, 1, 2, 3, 4};
    ASSERT_EQ(res, check);
}

void test_function_picture_reader(ImageFIFO& imfif, std::vector<char*>& res, size_t file_size) {
    size_t count = 0;
    while (1 == 1) {
        void* p = imfif.getReady();
        if (p != nullptr) {
            std::memcpy(res[count], p, file_size);
            ++count;
            imfif.addFree(p);
            p = nullptr;
        }
        else
            std::this_thread::yield();
        if (count == 10)
            break;
    }
}

void test_function_picture_writer(ImageFIFO& imfif, std::ifstream& kitty, long file_size) {
    size_t count = 0;
    while(1 == 1) {
        void* p = imfif.getFree();
        if (p != nullptr) {
            kitty.get(reinterpret_cast<char*>(p), file_size, EOF);
            ++count;
            imfif.addReady(p);
            p = nullptr;
        }
        else
            std::this_thread::yield();
        if (count == 10) {
            break;
        }
    }
}

TEST(FIFO_testing, PictureTest) {
    std::ifstream kitty_jpg("/home/valmit/Documents/QtProjects/multithread/kitty.jpg", std::ios::in | std::ios::ate | std::ios::binary);
    ASSERT_TRUE(kitty_jpg.is_open());
    size_t file_size = static_cast<size_t>(kitty_jpg.tellg());
    kitty_jpg.seekg(0); // this is a spell to get file size, open file with position at the end, record size = position, put position at the beginning
    ImageFIFO imfif(file_size, 10);
    std::vector<char*> res;
    for(size_t i = 0; i < 10; ++i) {
        res.push_back(reinterpret_cast<char*>(operator new(sizeof(char) * file_size)));
    }
    std::thread writer(test_function_picture_writer, std::ref(imfif), std::ref(kitty_jpg), file_size);
    std::thread reader(test_function_picture_reader, std::ref(imfif), std::ref(res), file_size);
    writer.join();
    reader.join();
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



#ifndef IO_TEST_H
#define IO_TEST_H

#include "ThreadPool.h" // forward declaration with smart pointer

#include <string>
#include <memory>
#include <cassert>
#include <vector>
#include <chrono>

#include <iostream>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

const uint64_t KiB = 1 << 10;
const uint64_t MiB = 1 << 20;

// class ThreadPool;

std::vector<std::uint64_t> generate_data(size_t n);

struct Task {
    Task(std::uint64_t s, std::uint64_t c, std::string f)
        :chunk_size(s), count(c), filename(std::move(f))
    {
    }
    std::uint64_t operator()() {
        int fd = open(filename.c_str(), O_CREAT|O_RDWR, 00777);
        if (fd < 0) {
            printf("open file %s error %d\n", filename.c_str(), fd);
            return fd;
        }
        using namespace std::chrono;
        milliseconds all(0);
        std::uint64_t ret = 0;
            auto start = system_clock::now();
        for (int i = 0; i < count; ++i) {
            auto d{std::move(generate_data(chunk_size))}; // Return Value Optimization --> remove std::move
            ssize_t r = write(fd, static_cast<void*>(d.data()), chunk_size);
            if (r < 0) {
                printf("write file %s error %d\n", filename.c_str(), fd);
                return r;
            }
            ret += r;
        }
        auto r = close(fd);
        if (r < 0) {
            printf("close file %s error %d\n", filename.c_str(), fd);
            return r;
        }
            auto delta = system_clock::now() - start;
            all += duration_cast<milliseconds>(delta);
        std::cout << "task end: " << all.count() << std::endl;
        return ret;
    }

    uint64_t chunk_size;
    int count;
    std::string filename;
};

class IOTest {
public:
    IOTest() = default;
    ~IOTest();
    void set_chunk_size(std::string s);
    void set_thread_num(int i) { thread_num_ = i; }
    void set_path(std::string p);
    void set_count(int c) { count_ = c; }
    void set_file_num(int c) { file_num_ = c; }
    void start_test();
private:
    int thread_num_{8};
    std::string path_;
    std::uint64_t chunk_size_;
    int count_{10};
    std::uint64_t file_num_{1};

    std::unique_ptr<ThreadPool> pool_;
};

#endif // IO_TEST_H

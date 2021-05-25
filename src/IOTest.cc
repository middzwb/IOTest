#include "IOTest.h"
// #include "ThreadPool.h"

#include <random>
#include <regex>
#include <iostream>
#include <stdexcept>

std::vector<std::uint64_t> generate_data(size_t n) {
    assert(n % sizeof(std::uint64_t) == 0);
    using namespace std::chrono;
    std::vector<std::uint64_t> ret(n / sizeof(std::uint64_t));

    // std::mt19937 rd{std::random_device{}()};
    // std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
    // std::iota(ret.begin(), ret.end(), dist(rd));
    
    // std::shuffle(ret.begin(), ret.end(), std::mt19937{std::random_device{}()});
    return ret;
}


IOTest::~IOTest() {
    if (pool_) 
        pool_->drain();
}

void IOTest::set_path(std::string p) {
    if (p.back() != '/') {
        p += '/';
    }
    path_ = std::move(p);
}

void IOTest::set_chunk_size(std::string s) {
    std::regex size_reg("([1-9][0-9]*)([mk]*)");
    std::smatch sm;
    using namespace std;
    if (std::regex_search(s, sm, size_reg)) {
        if (sm.prefix().matched || sm.suffix().matched || sm.size() != 3 || sm[2].str().size() > 1) {
            throw std::invalid_argument("invalid chunk size: " + s);
        }
        for (auto i = 0; i < sm.size(); ++i) {
            // cout << i << ": " << sm[i] << endl;
        }
        uint64_t base = std::stoi(sm[1]);
        auto unit = sm[2].str();
        if (unit == "k") {
            base *= KiB;
        }
        else if (unit == "m") {
            base *= MiB;
        }
        chunk_size_ = base;
    }
}

void IOTest::start_test() {
    std::cout << file_num_ << " " << chunk_size_ << " " << count_ << " " << thread_num_ << " " << path_ << std::endl;
    pool_ = std::make_unique<ThreadPool>(thread_num_);
    for (int i = 0; i < file_num_; ++i) {
        Task task(chunk_size_, count_, path_ + "iotest_" + std::to_string(i) + ".file");
        auto fu = pool_->enqueue(task);
    }
    pool_->drain();
}

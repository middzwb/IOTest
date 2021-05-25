#include "IOTest.h"

#include <cstdint>
#include <ostream>
#include <iostream>
#include <cassert>

#include <unistd.h>
#include <getopt.h>

void show_usage() {
    using namespace std;
    cout << "-t <num>       thread num(default: 8)" << endl
         << "-p <path>      test file path(default: current path)" << endl
         << "-s <num>       chunk size(default: 4MB, support m/M/k/K)" << endl
         << "-c <num>       chunk count(default: 10)" << endl;
}

static struct option long_opt[] = {
    { "files", required_argument, NULL, 'f' },
    {NULL, 0, NULL, 0}
};

int main(int argc, char** argv) {
    using namespace std;
    IOTest io_tester;
    int c = 0;
    try {
        int index = 0;
        while ((c = getopt_long(argc, argv, "t:p:s:c:h", long_opt, &index)) != -1) {
            switch(c) {
                case 't':
                    io_tester.set_thread_num(std::stoi(optarg));
                    break;
                case 'p':
                    io_tester.set_path(optarg);
                    break;
                case 's':
                    io_tester.set_chunk_size(optarg);
                    break;
                case 'c':
                    io_tester.set_count(std::stoi(optarg));
                    break;
                case '?':
                    show_usage();
                    cout << optopt << endl;
                    return 0;
                case 'h':
                    show_usage();
                    return 0;
                case 'f':
                    io_tester.set_file_num(std::stoi(optarg));
                    break;
                default:
                    return 0;
            }
        }
    }
    catch (const std::exception& e) {
        std::cout << e.what() << endl;
        return -1;
    }

    using namespace chrono;
    auto start = system_clock::now();
    io_tester.start_test();
    auto delta = system_clock::now() - start;
    cout << duration_cast<milliseconds>(delta).count() << endl;
}

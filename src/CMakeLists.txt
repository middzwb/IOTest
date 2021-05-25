set(io_srcs
    IOTest.cc
    generate.cc
    )

add_executable(io ${io_srcs})

find_package(Threads)

target_link_libraries(io
    ${CMAKE_THREAD_LIBS_INIT})

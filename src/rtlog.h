
#pragma once


#define RT_PRINT_LOG

#ifdef RT_PRINT_LOG

#include <stdio.h>
#include <chrono>
using namespace std::chrono;

#define RT_LOG(message, ...) printf(message"\n", ##__VA_ARGS__)
#define RT_TIME_STMT(name, statement)                   \
{                                                       \
    auto startTime = high_resolution_clock::now();      \
    statement;                                          \
    auto stopTime = high_resolution_clock::now();       \
    auto timeTaken_ns = (stopTime - startTime).count(); \
    RT_LOG(                                             \
        name " took %f secs (%f ms)",                   \
        (float) timeTaken_ns / 1'000'000'000,           \
        (float) timeTaken_ns / 1'000'000                \
    );                                                  \
}

#else

#define RT_LOG(message, ...)
#define RT_TIME_STMT(name, statement) statement

#endif

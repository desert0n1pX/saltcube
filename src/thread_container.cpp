/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "thread_container.hpp"
#include "logging.hpp"
#include <cstddef>
#include <string>
#include <thread>
#include <vector>

thread_container::thread_container(std::vector<std::thread> &new_threads) : my_threads(new_threads) {
    logging::atomic_log(logging::DEBUG, "Thread container created");
}

thread_container::~thread_container(){
    logging::atomic_log(logging::DEBUG, "Threads are being destroyed! Safely waiting for termination.");
    for (size_t idx = 0 ; idx < my_threads.size() ; idx++){
        logging::atomic_log(logging::DEBUG, "Trying to join thread: " + std::to_string(idx));
        if (my_threads[idx].joinable()) {
            logging::atomic_log(logging::DEBUG, "Joining thread: " + std::to_string(idx));
            my_threads[idx].join();
            logging::atomic_log(logging::DEBUG, "Thread joined: " + std::to_string(idx));
        }
    }
}

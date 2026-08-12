/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef THREAD_CONTAINER_H
#define THREAD_CONTAINER_H

#include <thread>
#include <vector>

class thread_container {
    std::vector<std::thread>    &my_threads;

    // Public Constructors/Destructors
    //////////////////////////////////////////////////////////////////////

    public:

    /**
     * @brief Construct a new thread container object
     * 
     * @param 
     */
    thread_container(std::vector<std::thread> &);
    ~thread_container();
};

#endif
/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef WORKER_EXCEPTIONS_H
#define WORKER_EXCEPTIONS_H

#include "saltcube_exception.hpp"

class worker_hash_error final : public saltcube_exception {
    void noinit() override;

    public:
    worker_hash_error(const char * what, const std::string where, int line, const std::string why);
};

#endif
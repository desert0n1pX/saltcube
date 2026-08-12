/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "worker_exceptions.hpp"

worker_hash_error::worker_hash_error(const char * what, const std::string where, int line, const std::string why) : 
            saltcube_exception(what, where, line, why) {}

void worker_hash_error::noinit(){}
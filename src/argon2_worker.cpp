/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "argon2_worker.hpp"

#include "logging.hpp"
#include "signal_handler.hpp"
#include "worker_exceptions.hpp"

#include <sodium.h>
#include <string>


void worker_argon2_hash(char *      hash_buff,
                      const void *  password,
                      size_t        password_len,
                      size_t        iterations,
                      size_t        KiB_memory){

    size_t memory = KiB_memory * 1024;

    logging::log(logging::INFO, "Started hashing, running argon2 with parameters memory=" + std::to_string(memory) + " (" + std::to_string(KiB_memory) + " in KiB) and iterations=" + std::to_string(iterations));
    SIGCHECK
    if (crypto_pwhash_str(hash_buff, static_cast<const char *>(password), password_len, iterations, memory) != 0) {
        throw worker_hash_error(__PRETTY_FUNCTION__, __FILE__, __LINE__, "argon worker had an abnormal exit code");
    }
    SIGCHECK
}

int worker_argon2_check(const char *    hash,
                        const void *    password,
                        size_t          password_len) {
    logging::log(logging::INFO, "Started argon2 verification");
    SIGCHECK
    if (crypto_pwhash_str_verify(hash, static_cast<const char *>(password), password_len) == 0) {
        SIGCHECK
        return 0;
    } else {
        SIGCHECK
        return 1;
    }
}

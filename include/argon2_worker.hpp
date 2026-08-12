/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef ARGON2_WORKER_HPP
#define ARGON2_WORKER_HPP

#include <cstddef>

#define argon2_pwhash_ITERATIONS_MIN        1
#define argon2_pwhash_ITERATIONS_DEFAULT    16
#define argon2_pwhash_ITERATIONS_MAX        32768
#define argon2_pwhash_MEMORY_MIN            1024
#define argon2_pwhash_MEMORY_DEFAULT        262144

/**
 * @brief Check a password against an argon2 hash and salt
 * 
 * @param hash Hash to validate against
 * @param password Plaintext Password, prefereably allocated with `sodium_malloc()`
 * @param password_len Plaintext Password Length
 * @return int 0 for success, 1 otherwise
 */
int worker_argon2_check(const char *    hash,
                        const void *    password,
                        size_t          password_len);


/** * @brief Use argon2 to fill hash_buff with the hash and salt of a password of length password_len
 *
 * @throws worker_hash_error Abnormal exit code from hash worker
 * 
 * @param hash_buff Address to place hash in
 * @param password Plaintext Password, prefereably allocated with `sodium_malloc()`
 * @param password_len Plaintext Password Length
 * @param iterations (optional) Number of times to rehash
 * @param memory (optional) KiB to allocate for argon
 */
void worker_argon2_hash(char *      hash_buff,
                      const void *  password,
                      size_t        password_len,
                      size_t        iterations = argon2_pwhash_ITERATIONS_DEFAULT,  // Note options::options dictates defaults
                      size_t        memory = argon2_pwhash_MEMORY_DEFAULT); // Note options::options dictates defaults

#endif
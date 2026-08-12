/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef SHA512_WORKER_HPP
#define SHA512_WORKER_HPP

#include "cube_memory.hpp"

#include <cstddef>

#define sha512_pwhash_ITERATIONS_MIN     1024
#define sha512_pwhash_ITERATIONS_DEFAULT 1048576
#define sha512_pwhash_ITERATIONS_MAX     134217728
#define sha512_pwhash_PARALLEL_DEFAULT   4
#define sha512_pwhash_PARALLEL_MAX_PREF  64

/**
 * @brief Check a password against an sha512 hash and salt
 *
 * @throw std::invalid_argument Invalid hash string
 * 
 * @param hash Hash to validate against
 * @param password Plaintext Password
 * @param password_len Plaintext Password Length
 * @return int 0 for success, 1 otherwise
 */
int worker_sha512_check(const char * hash,
                        const void * password,
                        size_t       password_len);


/**
 * @brief 
 * 
 * @param digest_buff   digest output buffer
 * @param password      password
 * @param pass_len      password length
 * @param salt          salt (in memory object)
 * @param iterations    iterations to do on EACH thread
 * @return int 
 */
void worker_sha512_digest(unsigned char *       digest_buff,
                         const void *           password,
                         size_t                 pass_len,
                         const cube_mem_secure  *salt, 
                         size_t                 iterations); 

/**
 * @brief Use sha512 to fill hash_buff with the hash and salt of a password of length password_len
 * 
 * @param hash_buff Address to place hash in
 * @param password Plaintext Password
 * @param password_len Plaintext Password Length
 * @return int 0 for success, 1 otherwise
 */
int worker_sha512_hash(char *       hash_buff,
                       const void * password,
                       size_t       password_len,
                       size_t       iterations = sha512_pwhash_ITERATIONS_DEFAULT, // Note options::options dictates defaults
                       size_t       threads = 1,
                       const void * salt = nullptr,
                       size_t       salt_len = 0);

#endif
/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "sha512_worker.hpp"

#include "cube_memory.hpp"
#include "logging.hpp"
#include "signal_handler.hpp"
#include "thread_container.hpp"
#include "util.hpp"
#include "worker_exceptions.hpp"

#include <cstddef>
#include <openssl/sha.h>
#include <sodium.h>
#include <sstream>
#include <string.h>

#include <string>
#include <thread>
#include <vector>

#define DEFAULT_SALT_LEN 16
#define SHA512_DIGEST_B64_LEN sodium_base64_ENCODED_LEN(SHA512_DIGEST_LENGTH, sodium_base64_VARIANT_ORIGINAL_NO_PADDING)
#define SALT_MAX_B64 sodium_base64_ENCODED_LEN(128, sodium_base64_VARIANT_ORIGINAL_NO_PADDING)

int worker_sha512_check(const char * hash_str, const void * password, size_t password_len){
    SIGCHECK
    // [username]$<type>$<parameters>$<salt>$<hash>
    std::vector<std::string> entry = split(hash_str, '$');

    if (entry.size() < 5)
        return -1;

    std::string parameters = entry[2];
    std::string salt_b64 = entry[3];
    std::string hash_b64 = entry[4];
    long long iterations;
    long long num_threads;

    try {
        iterations = std::stoll(find_parameter(parameters, "i"));
        num_threads = std::stoll(find_parameter(parameters, "p"));
    } catch (std::invalid_argument &) {
        throw value_improper_format_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "worker parameter is invalid");
    }
    if (iterations < sha512_pwhash_ITERATIONS_MIN || iterations > sha512_pwhash_ITERATIONS_MAX || num_threads < 1 || num_threads > std::thread::hardware_concurrency())
        throw value_improper_format_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "worker parameters invalid");

    cube_mem new_entry(SALT_MAX_B64 + SHA512_DIGEST_B64_LEN + 16); // This is safe as the worker wont make a string longer than this
    cube_mem original_salt_bin(salt_b64.size()); // It cant be possible for the binary to be smaller than the b64

    // We dont need both of these here but this comment should remain for reference
    size_t written; // inplace of bin_len, pass as reference to track bytes length
    // char * end; inplace of b64_end, pass as reference to get pointer to get address of last written character
    sodium_base642bin(  original_salt_bin.get_as_uchar(), original_salt_bin.size(), // Put in here, max len
                        salt_b64.c_str(), salt_b64.size(), "$", // take from here, salt str size, ignore chars
                        &written, nullptr, sodium_base64_VARIANT_ORIGINAL_NO_PADDING);

    int worker_rtn = worker_sha512_hash(new_entry.get_as_char(), password, password_len, iterations, num_threads, original_salt_bin.get_ptr(), written);

    if (worker_rtn)
        throw worker_hash_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "worker returned a non zero exit code whilst trying to verify a hash");

    if (strcmp(hash_str, new_entry.get_as_char()) == 0)
        return 0;
    else
        return 1;
}

int worker_sha512_hash(char * hash_buff, const void * password, size_t password_len, size_t iterations, size_t num_threads, const void * in_salt, size_t salt_len){
    SIGCHECK
    // Handle or make the salt
    cube_mem salt = (in_salt == nullptr) ? cube_mem(DEFAULT_SALT_LEN) : cube_mem(salt_len);
    if ( in_salt == nullptr ){
        salt.zero();
        randombytes_buf(salt.get_ptr(), DEFAULT_SALT_LEN);
    } else {
        memcpy(salt.get_ptr(), in_salt, salt_len);
    }

    // Set up for each thread
    std::vector<cube_mem_secure> thread_salts;
    std::vector<cube_mem_secure> thread_digests;
    std::vector<std::thread> threads;

    // Be safe out there!
    thread_container thread_watcher(threads);

    // Setup Data for threads and dispatch
    logging::log(logging::INFO, "Setting up worker thread data");
    for (size_t i = 0 ; i < num_threads ; i++) {
        thread_salts.emplace_back(SHA512_DIGEST_LENGTH);
        thread_digests.emplace_back(SHA512_DIGEST_LENGTH);

        if (i == 0) {
            SHA512(salt.get_as_uchar(), salt.size(), thread_salts[i].get_as_uchar());
        } else {
            SHA512(thread_salts[i-1].get_as_uchar(), thread_salts[i-1].size(), thread_salts[i].get_as_uchar());
        }
    }

    logging::log(logging::INFO, "Dispatching worker threads");
    for (size_t i = 0 ; i < num_threads ; i++) {
        threads.emplace_back(worker_sha512_digest, thread_digests[i].get_as_uchar(), password, password_len, &thread_salts[i], iterations);
    }

    // Wait for and destroy threads
    thread_watcher.~thread_container();
    logging::log(logging::INFO, "Worker threads done.");

    SIGCHECK

    // Allocate memory for the results of each thread to be concatenated
    cube_mem end_digest(thread_digests.size() * SHA512_DIGEST_LENGTH);

    // concatenate
    for (size_t pos = 0 ; pos < thread_digests.size() ; pos++) {
        append(end_digest.get_ptr(), pos * SHA512_DIGEST_LENGTH, thread_digests[pos].get_ptr(), thread_digests[pos].size());
    }

    // Allocate final digest
    cube_mem final_digest(SHA512_DIGEST_LENGTH);

    // Hash all the digests
    SHA512(end_digest.get_as_uchar(), end_digest.size(), final_digest.get_as_uchar());

    // Make space for the text representation of the salt and digest
    cube_mem final_hash_b64(SHA512_DIGEST_B64_LEN);
    cube_mem final_salt_b64(sodium_base64_encoded_len(salt.size(), sodium_base64_VARIANT_ORIGINAL_NO_PADDING));

    // Convert into text representation
    sodium_bin2base64(final_hash_b64.get_as_char(), final_hash_b64.size(), final_digest.get_as_uchar(), final_digest.size(), sodium_base64_VARIANT_ORIGINAL_NO_PADDING);
    sodium_bin2base64(final_salt_b64.get_as_char(), final_salt_b64.size(), salt.get_as_uchar(), salt.size(), sodium_base64_VARIANT_ORIGINAL_NO_PADDING);

    // Convert to a full format
    std::stringstream entry;
    entry << "$sha512" 
    "$i=" << std::to_string(iterations) <<
    ",p=" << std::to_string(num_threads) <<
    "$" << final_salt_b64.get_as_char() <<
    "$" << final_hash_b64.get_as_char();

    // Do a final copy into the destination
    strcpy(hash_buff, entry.str().c_str());

    return 0;
}

void worker_sha512_digest(unsigned char * digest_buff, const void * password, size_t password_len, const cube_mem_secure *salt, size_t iterations){
    cube_mem digest_src(SHA512_DIGEST_LENGTH + salt->size());
    cube_mem digest_dst(SHA512_DIGEST_LENGTH);
    cube_mem_secure initial_digest(password_len + salt->size());

    combine(initial_digest.get_ptr(), salt->get_ptr(), salt->size(), password, password_len);

    SHA512(initial_digest.get_as_uchar(), initial_digest.size(), digest_dst.get_as_uchar());
    
    initial_digest.zero();

    logging::atomic_log(logging::DEBUG, "sha512 worker thread started");

    for (size_t itr = 0 ; itr < iterations - 1 ; itr++){
        combine(digest_src.get_ptr(), salt->get_ptr(), salt->size(), digest_dst.get_ptr(), SHA512_DIGEST_LENGTH);
        SHA512(digest_src.get_as_uchar(), digest_src.size(), digest_dst.get_as_uchar());
        if (itr % 50 == 0){
            if (get_signal() !=0)
                return;
        }
    }

    logging::atomic_log(logging::DEBUG, "sha512 worker memcpy() into final buffer");
    memcpy(digest_buff, digest_dst.get_ptr(), digest_dst.size());
}
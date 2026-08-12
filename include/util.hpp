/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef UTIL_H
#define UTIL_H

#include "saltcube_exception.hpp"

#include <cstddef>
#include <string>
#include <vector>

/**
 * @brief Write data starting at pos
 * 
 * @param destination 
 * @param starting_pos 
 * @param src 
 * @param src_len 
 */
void append(void * destination,
            size_t starting_pos,
            const void * src,
            size_t src_len);
/**
 * @brief Combine some data with a salt and store it
 * 
 * @param destination Location to place the salted data in
 * @param src1 Raw data as salt
 * @param src1_len Length of the salt
 * @param src2 Raw data to be salted
 * @param src2_len Length of the raw data to be salted
 */
void combine(void * destination,
                     const void *      src1,
                     size_t            src1_len,
                     const void *      src2,
                     const std::size_t src2_len);

/**
 * @brief Find a parameter in list such as 'a=1,b=2,c=3'
 *
 * @throws key_not_found_error Key not found
 * 
 * @param parameters String of parameters
 * @param key Key to search for
 * @return std::string 
 */
std::string find_parameter(std::string parameters, std::string key);

/**
 * @brief Get the avail system mem in KiB
 *
 * @throws cube_mem_init_error Couldn't get the system memory details
 * 
 * @return size_t 
 */
size_t get_avail_mem_in_KiB();

/**
 * @brief Check if target has and substrings from array of strings
 * 
 * @param target target to check in
 * @param check_array things to check for
 * @return true 
 * @return false 
 */
bool has_in_string(std::string &target, std::vector<const char *> check_array);

/**
 * @brief Sparate a c++ string by a delimeter
 * 
 * @param target String to split
 * @param delimeter Delimiter to split by
 * @return std::vector<std::string> 
 */
std::vector<std::string> split(std::string target, char delimeter);

//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////

class key_not_found_error final : public saltcube_exception {
    void noinit();

    public:
    key_not_found_error(const char * what, const std::string where, int line, const std::string why);
};

class value_improper_format_error final : public saltcube_exception {
    void noinit();

    public:
    value_improper_format_error(const char * what, const std::string where, int line, const std::string why);
};

#endif
/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "util.hpp"

#include "cube_memory.hpp"

#include <cstddef>
#include <errno.h>
#include <string>
#include <unistd.h>
#include <vector>

void append(void * destination,
            size_t starting_pos,
            const void * src,
            size_t data_len){

    unsigned char * char_dst       = reinterpret_cast<unsigned char *>(destination);
    const unsigned char * char_src = reinterpret_cast<const unsigned char *>(src);

    for (size_t i = starting_pos ; i < data_len; i++) {
        char_dst[i] = char_src[i - starting_pos];
    }
}

void combine(void * destination,
            const void * src1,
            const std::size_t src1_len,
            const void * src2,
            const std::size_t src2_len){
    unsigned char *        char_dst = reinterpret_cast<unsigned char *>(destination);
    const unsigned char * char_src1 = reinterpret_cast<const unsigned char *>(src1);
    const unsigned char * char_src2 = reinterpret_cast<const unsigned char *>(src2);

    for (size_t i = 0 ; i < src1_len + src2_len ; i++) {
        if (i < src1_len) {
            char_dst[i] = char_src1[i];
        } else {
            char_dst[i] = char_src2[i - src1_len];
        }
    }
}

size_t get_avail_mem_in_KiB(){
    errno = 0;
    size_t pagesize_bytes = sysconf(_SC_PAGESIZE);
    if (errno != 0)
        throw cube_mem_init_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "Unable to retrieve pagesize");

    size_t avail_pages    = sysconf(_SC_PHYS_PAGES);
    if (errno != 0)
        throw cube_mem_init_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__,"Unable to retrieve available pages");

    size_t avail_bytes = pagesize_bytes * avail_pages;

    return avail_bytes/1024;
}

bool has_in_string(std::string &target, std::vector<const char *>  check_array){
    for (const char * check : check_array) {
        if (target.contains(check))
            return true;
    }
    return false;
}

std::string find_parameter(std::string parameters, std::string key){
    std::vector<std::string> param_list = split(parameters, ',');

    for (std::string param : param_list){
        std::vector<std::string> entry = split(param, '=');
        if (entry.front() == key){
            return entry.back();
        }
    }

    throw key_not_found_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "Key not found");
}

std::vector<std::string> split(std::string target, char delimeter){
    std::vector<std::string> output;

    std::string tmp_string;
    for (char this_char : target) {
        if (this_char != delimeter){
            tmp_string += this_char;
        } else {
            output.push_back(tmp_string);
            tmp_string.clear();
        }
    }

    output.push_back(tmp_string);
    tmp_string.clear();

    return output;
}


//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////


key_not_found_error::key_not_found_error(const char * what, const std::string where, int line, const std::string why) : 
            saltcube_exception(what, where, line, why) {}

void key_not_found_error::noinit(){}

value_improper_format_error::value_improper_format_error(const char * what, const std::string where, int line, const std::string why) : 
            saltcube_exception(what, where, line, why) {}

void value_improper_format_error::noinit(){}
/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "cube_memory.hpp"

#include "saltcube_exception.hpp"

#include <sodium.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>


// class cube_mem
//////////////////////////////////////////////////////////////////////

cube_mem::cube_mem(size_t size){
    data = std::malloc(size);
    data_size = size;
    zero();

    if (data == nullptr){
        throw cube_mem_alloc_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, MALLOC_FAIL_ERR);
    }
}

cube_mem::cube_mem(cube_mem &&donor) : data(donor.data), data_size(donor.data_size) {
    donor.data = nullptr;
}

cube_mem::~cube_mem(){
    if (data != nullptr) {
        std::free(data);
        data = nullptr;
    }
}

char * cube_mem::get_as_char() const {
    return (char *) data;
}

unsigned char * cube_mem::get_as_uchar() const {
    return (unsigned char *) data;
}

void * cube_mem::get_ptr() const {
    return data;
}

void cube_mem::grow(){
    grow_to(data_size * 2);
}

void cube_mem::grow_to(size_t new_size){
    if (new_size <= data_size)
        throw cube_mem_alloc_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, MEM_SHRINK_ERR);

    void * new_data = std::malloc(new_size);
    if (new_data == nullptr)
        throw cube_mem_alloc_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, MALLOC_FAIL_ERR);

    std::memcpy(new_data, data, data_size);
    data_size = new_size;
    free(data);
    data = new_data;
}

size_t cube_mem::size() const {
    return data_size;
}

void cube_mem::zero() {
    for (size_t idx = 0 ; idx < data_size ; idx++) {
        ((unsigned char *) data)[idx] = '\0';
    }
}


// class cube_mem_secure : cube_mem
//////////////////////////////////////////////////////////////////////


cube_mem_secure::cube_mem_secure(size_t size) {
    cube_mem_init();
    data = sodium_malloc(size);
    data_size = size;
    if (data == nullptr){
        throw cube_mem_alloc_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, MALLOC_FAIL_ERR);
    }
    zero();
}

cube_mem_secure::cube_mem_secure(cube_mem_secure &&donor) {
    data = donor.data;
    donor.data = nullptr;
    data_size = donor.data_size;
}
    
cube_mem_secure::~cube_mem_secure() {
    if (data != nullptr) {
        sodium_free(data);
        data = nullptr;
    }
}


void cube_mem_secure::cube_mem_init() {
    if (sodium_init() < 0)
        throw cube_mem_init_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "unable to initialize libsodium for safe use");
}

void cube_mem_secure::grow() {
    grow_to(data_size * 2);
}

void cube_mem_secure::grow_to(size_t new_size) {
    if (new_size <= data_size)
        throw cube_mem_alloc_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, MEM_SHRINK_ERR);

    void * new_data = sodium_malloc(new_size);
    if (new_data == nullptr)
        throw cube_mem_alloc_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, MALLOC_FAIL_ERR);

    std::memcpy(new_data, data, data_size);
    data_size = new_size;
    sodium_free(data);
    data = new_data;
}

//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////

cube_mem_alloc_error::cube_mem_alloc_error(const char * what, const std::string where, int line, const std::string why) : 
            saltcube_exception(what, where, line, why) {}

void cube_mem_alloc_error::noinit(){}

cube_mem_init_error::cube_mem_init_error(const char * what, const std::string where, int line, const std::string why) : 
            saltcube_exception(what, where, line, why) {}

void cube_mem_init_error::noinit(){}
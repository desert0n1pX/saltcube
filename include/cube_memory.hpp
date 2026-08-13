/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef CUBE_MEMORY_H
#define CUBE_MEMORY_H

#include "saltcube_exception.hpp"

#include <cstddef>

#define MALLOC_FAIL_ERR "failed to allocate memory"
#define MEM_SHRINK_ERR "unable to shrink memory"

/*
It should be noted from the docs: https://doc.libsodium.org/doc/usage

sodium_init() initializes the library and should be called before any other function provided by Sodium.
It is safe to call this function more than once and from different threads – subsequent calls won’t have any effects.
*/

class cube_mem {

    // Protected Fields
    //////////////////////////////////////////////////////////////////////

    protected:

    void * data;
    size_t data_size;

    // Protected Default Constructor
    //////////////////////////////////////////////////////////////////////

    cube_mem() = default;

    // Public Constructors/Destructors
    //////////////////////////////////////////////////////////////////////

    public:

    /**
     * @brief Construct a new cube mem object of size
     *
     * @throws cube_mem_alloc_error unable to allocate memory
     * 
     */
    cube_mem(size_t);
    cube_mem(cube_mem &) = delete;
    cube_mem(cube_mem &&);
    
    virtual ~cube_mem();


    // Public Methods
    //////////////////////////////////////////////////////////////////////

    /**
     * @brief Get a pointer to the data as an unsigned char
     * 
     * @return unsigned char *
     */
    char *          get_as_char() const;

    /**
     * @brief Get a pointer to the data as an unsigned char
     * 
     * @return unsigned char *
     */
    unsigned char * get_as_uchar() const;

    /**
     * @brief Get a pointer to the data
     * 
     * @return void* 
     */
    void *          get_ptr() const;

    /**
     * @brief Double memory size
     * 
     * @throws cube_mem_alloc_error unable to allocate memory
     *
     */
    virtual void    grow();

    /**
     * @brief Grow memory to size
     * 
     * @throws cube_mem_alloc_error unable to allocate memory
     * 
     */
    virtual void    grow(size_t);

    /**
     * @brief Get size of the 
     * 
     * @return size_t 
     */
    size_t          size() const;

    /**
     * @brief Zero the memory
     * 
     */
    void            zero();
};

class cube_mem_secure final : public cube_mem {
    // Public Constructors/Destructors
    //////////////////////////////////////////////////////////////////////

    public:

    /**
     * @brief Construct a new cube mem object of size
     * 
     */
    cube_mem_secure(size_t);
    cube_mem_secure(cube_mem_secure &) = delete;
    cube_mem_secure(cube_mem_secure &&);
    
    ~cube_mem_secure() override;


    // Public Static Methods
    //////////////////////////////////////////////////////////////////////

    static void cube_mem_init();

    // Public Methods
    //////////////////////////////////////////////////////////////////////

    /**
     * @brief Double memory size
     * 
     * @return true 
     * @return false 
     */
    virtual void grow() override;

    /**
     * @brief Grow memory to size
     * 
     * @return true Success
     * @return false fail
     */
    virtual void grow(size_t) override;
};

class cube_mem_alloc_error final : public saltcube_exception {

    void noinit() override;

    public:
    cube_mem_alloc_error(const char * what, const std::string where, int line, const std::string why);
};

class cube_mem_init_error final : public saltcube_exception {

    void noinit() override;

    public:
    cube_mem_init_error(const char * what, const std::string where, int line, const std::string why);
};

#endif
/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef PRACTICE_HPP
#define PRACTICE_HPP

#include "options.hpp"

/**
 * @brief Add hash to savefile derived from password
 * 
 * @return int program retrn code
 */
int pw_add(options &);

/**
 * @brief Delete hash from savefile derived from password
 * 
 * @return int program retrn code
 */
int pw_del(options &);

/**
 * @brief Get a hash
 * 
 * @return int program retrn code
 */
int pw_hash(options &);

/**
 * @brief Test yourself
 * 
 * @return int program retrn code
 */
int pw_test(options &);

/**
 * @brief Add hash to savefile
 * 
 * @return int program retrn code
 */
int hash_add(options &);

/**
 * @brief Delete hash from savefile
 * 
 * @return int program retrn code
 */
int hash_del(options &);

/**
 * @brief Verify a password hash from stdin
 * 
 * @return int program retrn code
 */
int hash_ver(options &);

#endif
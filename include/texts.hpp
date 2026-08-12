/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef TEXTS_H
#define TEXTS_H

#include "saltcube_exception.hpp"

/**
 * @brief Show the help page and throw show_page_exception
 * 
 * @param binary_name name of this binary from argv**
 * 
 * @throws show_text_exception Always throws as help was shown
 * 
 */
void help(const char * const binary_name);

/**
 * @brief Show the license and throw show_text_exception
 * 
 * @throws show_text_exception Always throws as license was shown
 * 
 */
void license();

/**
 * @brief Show the full license text and throw show_text_exception
 * 
 * @throws show_text_exception Always throws as license was shown
 * 
 */
void license_full();

//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////

class show_text_exception final : public saltcube_exception {
    void noinit() override;

    public:
    show_text_exception(const char * what, const std::string where, int line, const std::string why);
};

#endif
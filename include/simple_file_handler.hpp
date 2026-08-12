/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef SIMPLE_FILE_HANDLER_H
#define SIMPLE_FILE_HANDLER_H

#include "saltcube_exception.hpp"

#include <string>
#include <vector>

class simple_file {

    // Private Fields Methods
    //////////////////////////////////////////////////////////////////////

    std::vector<std::string> lines;
    std::string filename;

    // Public Constructors
    //////////////////////////////////////////////////////////////////////

    public:

    simple_file()=delete;

    /**
     * @brief Construct a new simple file object for a filename
     * 
     * @param filename 
     */
    simple_file(const std::string &filename);


    // Public Methods
    //////////////////////////////////////////////////////////////////////

    /**
     * @brief Get a reference to the file lines in vector form
     * 
     * @return std::vector<std::string>& 
     */
    std::vector<std::string> &get_lines();

    /**
     * @brief Load or reload the file
     * 
     * @throw file_not_found File open error
     * @param mode read mode
     */
    void load_file();

    /**
     * @brief Write the vector of lines to the file
     * 
     * @throw file_write_error error while writing file
     * @param mode write mode
     */
    void write_file() const;
};

//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////

class file_not_found final : public saltcube_exception {
    void noinit() override;

    public:
    file_not_found(const char * what, const std::string where, int line, const std::string why);
};

class file_write_error final : public saltcube_exception {
    void noinit() override;

    public:
    file_write_error(const char * what, const std::string where, int line, const std::string why);
};

#endif
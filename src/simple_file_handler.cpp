/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "simple_file_handler.hpp"

#include "logging.hpp"
#include "signal_handler.hpp"

#include <fstream>
#include <ostream>
#include <string>
#include <vector>


// Public Constructors
//////////////////////////////////////////////////////////////////////

simple_file::simple_file(const std::string &filename){
    this->filename = filename;
    this->lines = std::vector<std::string>();
}

// Public Methods
//////////////////////////////////////////////////////////////////////

std::vector<std::string> &simple_file::get_lines() {
    return this->lines;
}

void simple_file::load_file(){
    logging::log(logging::DEBUG, "Load file " + this->filename);
    this->lines = std::vector<std::string>();

    std::ifstream file;

    file.open(this->filename);
    
    if (file.is_open()) {
        std::string current_line;
        while (std::getline(file, current_line)) {
            SIGCHECK
            if (current_line.size() != 0)
                lines.push_back(current_line);
        }
        file.close();

    } else {
        throw file_not_found(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "file \"" + filename + "\" not found or could not be opened");
    }
}

void simple_file::write_file() const {
    logging::log(logging::DEBUG, "Write file " + this->filename);
    std::ofstream file;

    file.open(this->filename);

    if (file.is_open()) {
        auto pos = lines.begin();
        while (pos != lines.end()) {
            if (pos->size() != 0)
                file << *pos++;
            if (pos != lines.end())
                std::endl(file);
        }
        file.close();

    } else {
        throw file_write_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "could not open file \"" + filename + "\" for writing");
    }
}

//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////


file_not_found::file_not_found(const char * what, const std::string where, int line, const std::string why) : 
            saltcube_exception(what, where, line, why){}

void file_not_found::noinit(){}

//////////////////////////////////////////////////////////////////////

file_write_error::file_write_error(const char * what, const std::string where, int line, const std::string why) : 
            saltcube_exception(what, where, line, why){}

void file_write_error::noinit(){}
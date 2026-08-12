/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "argon2_worker.hpp"
#include "entry.hpp"
#include "logging.hpp"
#include "options.hpp"
#include "saltcube_exception.hpp"
#include "sha512_worker.hpp"
#include "util.hpp"

#include <cstring>
#include <format>
#include <stdexcept>
#include <string>
#include <vector>

#define INVALID_MODE_ERROR "Invalid mode"
#define ENTRY_CREATE_ERR "Unable to create hash strings not all required fields filled"
#define ENTRY_SPLIT_ERR "Unable to process input, not enough fields"

// Public Constructors
//////////////////////////////////////////////////////////////////////

entry::entry(std::string line){
    unparsed = line;
    parse(line);
}

entry::entry(std::string line, std::string username) : entry (line) {
    this->username = username;
}

// Public Methods
//////////////////////////////////////////////////////////////////////

const std::string entry::get_entry(){
    switch (mode) {

    case ARGON2:
        //  0        1   2     3    4
        // $argon2id$ver$param$salt$hash
        entrystr = std::format("{5}${0}${1}${2}${3}${4}",
                            modestring,
                            version,
                            parameters,
                            saltb64,
                            hashb64,
                            username);
        return entrystr;
        break;

    case SHA512:
        //  0      1     2    3
        // $sha512$param$salt$hash
        entrystr = std::format("{4}${0}${1}${2}${3}",
                            modestring,
                            parameters,
                            saltb64,
                            hashb64,
                            username);
        return entrystr;
        break;
    
    default:
        throw std::logic_error("Enum in entry::get_hashstr() missed a case");
    }
}


const std::string entry::get_hashb64() const {
    return hashb64;
};

enum hash_mode entry::get_mode_enum() const {
    return mode;
}

const std::string entry::get_modestring() const{
    return modestring;
}

const std::string entry::get_parameters() const{
    return parameters;
}

const std::string entry::get_passwd_hash() {
    switch (mode) {

    case ARGON2:
        if (modestring.size() == 0 || version.size() == 0 || parameters.size() == 0 || saltb64.size() == 0 || hashb64.size() == 0)
            throw incomplete_entry_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "required parameter(s) for argon2 not provided");
        //  0        1   2     3    4
        // $argon2id$ver$param$salt$hash
        hashstr = std::format("${0}${1}${2}${3}${4}",
                            modestring,
                            version,
                            parameters,
                            saltb64,
                            hashb64);
        return hashstr;
        break;

    case SHA512:
        if (modestring.size() == 0 || parameters.size() == 0 || saltb64.size() == 0 || hashb64.size() == 0)
            throw incomplete_entry_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "required parameter(s) for sha512 not provided");
        //  0      1     2    3
        // $sha512$param$salt$hash
        hashstr = std::format("${0}${1}${2}${3}",
                            modestring,
                            parameters,
                            saltb64,
                            hashb64);
        return hashstr;
        break;
    
    default:
        throw std::logic_error("Enum in entry::get_hashstr() missed a case");
    }
}

const std::string entry::get_saltb64() const{
    return saltb64;
}

const std::string entry::get_unparsed() const {
    return unparsed;
};

const std::string entry::get_username() const {
    return username;
}

const std::string entry::get_version_param() const{
    return version;
}

// Private Methods
//////////////////////////////////////////////////////////////////////

void entry::parse(std::string unsanitized){
    logging::log(logging::DEBUG, "Parsing the first 2 fields of string: " + unsanitized);
    std::vector<std::string> fields = split(unsanitized, '$');

    /*
    <user>$argon2id$ver$param$salt$hash
    <user>$sha512$param$salt$hash
    */
    if (fields.size() < 2)
        throw invalid_entry_exception(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "Not enough fields in hash string: " + unsanitized); 

    username = fields[0];
    set_modestring(fields[1]);

    switch (mode) {
    case ARGON2:
        parse(ARGON2, fields);
        break;

    case SHA512:
        parse(SHA512, fields);
        break;

    default:
        throw std::logic_error("Enum in entry::parse(std::string) missed a case");
    }
}

void entry::parse(enum hash_mode variant, std::vector<std::string> &fields){
    logging::log(logging::DEBUG, "Parsing the remaining fields");
    
    switch (variant) {
    case ARGON2:
        if (fields.size() < 6)
            throw invalid_entry_exception(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "not enough fields provided for an argon2 entry"); 

        // 0      1        2   3     4    5
        // <user>$argon2id$ver$param$salt$hash
        version = fields[2];
        parameters = fields[3];
        saltb64 = fields[4];
        hashb64 =fields[5];
        break;

    case SHA512:
        if (fields.size() < 5)
            throw invalid_entry_exception(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "not enough fields provided for a sha512 entry");

        // 0      1      2     3    4
        // <user>$sha512$param$salt$hash
        parameters = fields[2];
        saltb64 = fields[3];
        hashb64 = fields[4];
    }
}

void entry::set_modestring(std::string & mode_str){
    if (mode_str.starts_with("argon2") && mode_str.size() <= 8) {
        mode = ARGON2;
    } else if (mode_str == "sha512") {
        mode = SHA512;
    } else {
        throw invalid_entry_exception(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, mode_str + " is not a valid mode");
    }

    modestring = mode_str;
}

//////////////////////////////////////////////////////////////////////
// Non-members / Static
//////////////////////////////////////////////////////////////////////

entry create_entry_from_password(options &opts, const char * passwd){
    char hash_buff[128];

    if (opts.get_algo() == "sha512") {
        worker_sha512_hash(hash_buff, passwd, strlen(passwd), opts.get_iterations(), opts.get_parallel());

    } else if (opts.get_algo() == "argon2") {
        worker_argon2_hash(hash_buff, passwd, strlen(passwd), opts.get_iterations(), opts.get_memory_KiB());
    }

    return entry(hash_buff, opts.get_username());
}

//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////

incomplete_entry_error::incomplete_entry_error(const char * what, const std::string where, int line, const std::string why) :
            saltcube_exception(what, where, line, why){}

void incomplete_entry_error::noinit(){}

invalid_entry_exception::invalid_entry_exception(const char * what, const std::string where, int line, const std::string why) : 
            saltcube_exception(what, where, line, why){}

void invalid_entry_exception::noinit(){}
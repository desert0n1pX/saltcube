/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef ENTRY_H
#define ENTRY_H

#include "saltcube_exception.hpp"
#include "options.hpp"

#include <vector>
#include <string>

enum hash_mode {
    ARGON2,
    SHA512
};

class entry {
    
    // Private Fields
    //////////////////////////////////////////////////////////////////////

    enum hash_mode mode;
    std::string hashb64;
    std::string modestring;
    std::string parameters;
    std::string saltb64;
    std::string username;
    std::string version;

    // Full strings
    std::string entrystr;
    std::string hashstr;
    std::string unparsed;

    public:

    // Public Constructors
    //////////////////////////////////////////////////////////////////////

    entry()=delete;

    /**
     * @brief Construct a new entry object from an existing entry
     *
     * @throws invalid_entry_exception Invalid string provided as entry
     * 
     */
    entry(std::string entry_string);

    /**
     * @brief Construct a new entry object from a hash and username
     *
     * @throws invalid_entry_exception Invalid string provided as entry
     * 
     */
    entry(std::string hash, std::string username);

    // Public Methods
    //////////////////////////////////////////////////////////////////////

    /**
     * @brief Get the entrystr, includes username
     * 
     * @return std::string
     */
    const std::string get_entry();

    /**
     * @brief Get the hashb64 string
     * 
     */
    const std::string get_hashb64() const;

    /**
     * @brief Get the mode enum
     * 
     * @return char* 
     */
    enum hash_mode get_mode_enum() const;

    /**
     * @brief Get the mode string
     * 
     * @return char* 
     */
    const std::string get_modestring() const;

    /**
     * @brief Get the parameters string
     * 
     */
    const std::string get_parameters() const;

    /**
     * @brief Get the hashstr not including username
     * 
     * @return std::string 
     */
    const std::string get_passwd_hash();

    /**
     * @brief Get the saltb64 string
     * 
     */
    const std::string get_saltb64() const;

    /**
     * @brief Get the unparsed string
     * 
     */
    const std::string get_unparsed() const;

    /**
     * @brief Get the username string
     * 
     */
    const std::string get_username() const;

    /**
     * @brief Get the version string
     * 
     */
    const std::string get_version_param() const;

    // Private Methods
    //////////////////////////////////////////////////////////////////////

    private:

    /**
     * @brief Parse the first 2 fields
     *
     * @throws invalid_entry_exception Invalid string provided as entry
     * 
     */
    void parse(std::string);

    /**
     * @brief Parse the remaining fields for a given mode
     *
     * @throws invalid_entry_exception Invalid string provided as entry
     * 
     */
    void parse(enum hash_mode, std::vector<std::string> &);

    /**
     * @brief Set the modestring and choose from the enum
     * 
     * @throws invalid_entry_exception Unknown mode
     *
     * @param mode_str 
     */
    void set_modestring(std::string & mode_str);
};

//////////////////////////////////////////////////////////////////////
// Non-members / Static
//////////////////////////////////////////////////////////////////////

/**
 * @brief Create an entry from password object
 *
 * @throw hash_worker_error Worker had an abnormal exit code
 * 
 * @param opts 
 * @param passwd 
 * @return entry Entry object
 */
entry create_entry_from_password(options &opts, const char * passwd);

//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////

class incomplete_entry_error final : public saltcube_exception {
    void noinit() override;

    public:
    incomplete_entry_error(const char * what, const std::string where, int line, const std::string why);
};

class invalid_entry_exception final : public saltcube_exception {
    void noinit() override;

    public:
    invalid_entry_exception(const char * what, const std::string where, int line, const std::string why);
};

#endif
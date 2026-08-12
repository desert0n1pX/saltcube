/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef MODES_HELPER_H
#define MODES_HELPER_H

#include "entry.hpp"
#include "options.hpp"
#include "password_handler.hpp"
#include "simple_file_handler.hpp"

#include <memory>

/**
 * @brief Check an entry against a password
 * 
 * @param entry 
 * @param passwd 
 * @return true 
 * @return false 
 */
bool check_entry_against_hash(entry &entry, const char * passwd);

/**
 * @brief Loop to prompt user for password
 * 
 * @param opts 
 * @param passwd_container 
 * @param existing_entry 
 */
void check_loop(options &opts, password_handler &passwd_container, std::unique_ptr<entry> &existing_entry);

/**
 * @brief Look for and delete entry
 *
 * @throws file_write_error error writing
 * 
 * @param savefile file to edit
 * @param entry_from_file entry to insert
 */
void delete_entry(simple_file &savefile, entry *entry_from_file);

/**
 * @brief Get an entry and put in existing_entry
 * 
 * @param opts 
 * @param passwd_container 
 * @param existing_entry 
 * @param savefile 
 */
void get_entry_from_file(options &opts, password_handler &passwd_container, std::unique_ptr<entry> &existing_entry, simple_file &savefile);


#endif
/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "argon2_worker.hpp"
#include "entry.hpp"
#include "logging.hpp"
#include "modes_helper.hpp"
#include "options.hpp"
#include "password_handler.hpp"
#include "sha512_worker.hpp"
#include "simple_file_handler.hpp"
#include "util.hpp"

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

bool check_entry_against_hash(entry &entry, const char * passwd){
    switch (entry.get_mode_enum()) {

    case ARGON2:
        if (worker_argon2_check(entry.get_passwd_hash().c_str(), passwd, strlen(passwd)) == 0) {
            return true;
        } else {
            return false;
        }
    case SHA512:
        if (worker_sha512_check(entry.get_passwd_hash().c_str(), passwd, strlen(passwd)) == 0) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}


std::unique_ptr<entry> find_entry_by_pwhash(std::vector<std::string> &data, password_handler &passwd_container, options &opts){
    std::unique_ptr<entry> line_entry;
    int linenum = 0;
    std::cout << "Looking for your password by hash, this may take a minute...\n";
    for (std::string line : data) {
        
        if (line.size() == 0){
            logging::log(logging::INFO, "line " + std::to_string(linenum) + " is a blank line, skipping");
            continue;
        }

        try {
            line_entry = std::make_unique<entry>(line);
            if (check_entry_against_hash(*line_entry, passwd_container.get_canidate())){
                if (!opts.get_nocache()) {
                    passwd_container.save();
                }
                return line_entry;
            }
        } catch (invalid_entry_exception &except){
            logging::log(logging::ERROR, "Invalid entry: " + line, except);
            continue;

        } catch (key_not_found_error &except){
            logging::log(logging::ERROR, "Invalid entry doesn't contain key for hashing parameters: " + line, except);
            continue;

        } catch (value_improper_format_error &except){
            logging::log(logging::ERROR, "Invalid entry contains improper parameters: " + line, except);
            continue;
        }
    }
    return nullptr;
}

std::unique_ptr<entry> find_entry_by_username(std::vector<std::string> &data, std::string username){
    std::unique_ptr<entry> line_entry;
    int linenum = 0;
    for (std::string line : data) {
        linenum++;
        if (line.size() == 0){
            logging::log(logging::INFO, "line " + std::to_string(linenum) + " is a blank line, skipping");
            continue;
        }

        try {
            line_entry = std::make_unique<entry>(line);
        } catch (invalid_entry_exception &except){
            logging::log(logging::ERROR, "Invalid entry: " + line + " line " + std::to_string(linenum), except);
            continue;
        }

        if (line_entry->get_username() == username)
            return line_entry;
    }
    return nullptr;
}

void delete_entry(simple_file &savefile, entry *entry_from_file){
    std::vector<std::string> &data = savefile.get_lines();
    for (auto pos = data.begin() ; pos != data.end() ; ){
        if (*pos == entry_from_file->get_unparsed()){
            data.erase(pos);
        } else {
            pos++;
        }
    }
    savefile.write_file();
}


void get_entry_from_file(options &opts, password_handler &passwd_container, std::unique_ptr<entry> &existing_entry, simple_file &savefile) {
    if (opts.get_username().size() > 0) {
        existing_entry = find_entry_by_username(savefile.get_lines(), opts.get_username());
    } else {
        passwd_container.collect();

        existing_entry = find_entry_by_pwhash(savefile.get_lines(), passwd_container, opts);
    }
}

void reset_canidate(bool correct_password, password_handler &passwd_container, options &opts) {
    if (correct_password) {
        std::cout << COLOR_FG_GREEN "Password correct.\n" COLOR_FG_DEFAULT;
        if (!opts.get_nocache() && !passwd_container.is_set()) {
            passwd_container.save();
        }
        passwd_container.clear_canidate();
     } else {
        std::cout << COLOR_FG_RED "Password incorrect.\n" COLOR_FG_DEFAULT;
        passwd_container.clear_canidate();
    }
}

void pre_loop_messages(options &opts, password_handler &passwd_container) {
  if (opts.get_nocache()) {
    std::cout << COLOR_FG_YELLOW "Nocache is enabled, you will be unable to use the \"remind\" "
                 "command.\n" COLOR_FG_DEFAULT
                 "Type \"" COLOR_FG_GREEN "exit" COLOR_FG_DEFAULT "\" to exit.\n";

  } else if (passwd_container.is_set()) {
    std::cout << "Type \"" COLOR_FG_GREEN "remind" COLOR_FG_DEFAULT "\" to see your password.\n"
                 "Type \"" COLOR_FG_GREEN "exit" COLOR_FG_DEFAULT "\" to exit.\n";

  } else {
    std::cout << "Type \"" COLOR_FG_GREEN "remind" COLOR_FG_DEFAULT "\" to see your password after typing it "
                 "correctly.\nType \"" COLOR_FG_GREEN "exit" COLOR_FG_DEFAULT "\" to exit.\n";
  }
std::cout << "Type \"" COLOR_FG_GREEN "clear" COLOR_FG_DEFAULT "\" to clear the screen or "
                 "\"" COLOR_FG_GREEN "clear all" COLOR_FG_DEFAULT "\" to also clear the full terminal.\n";
}

void check_loop(options &opts, password_handler &passwd_container, std::unique_ptr<entry> &existing_entry) {
    pre_loop_messages(opts, passwd_container);

    while (true) {
    passwd_container.collect();

    if (passwd_container.equals_canidate("exit")) {
      std::cout << COLOR_FG_GREEN TERM_HOME TERM_ERASE_SCREEN TERM_ERASE_SAVED "Terminal cleared before exit.\n" TERM_FORMAT_RESET;
      break;

    } else if (passwd_container.equals_canidate("remind")) {
      if (opts.get_nocache()) {
        std::cout << COLOR_FG_YELLOW "Unable to use this command, nocache is enabled\n" COLOR_FG_DEFAULT;
      } else if (!passwd_container.is_set()) {
        std::cout << COLOR_FG_YELLOW "Unable to use this command, you must first enter your "
                     "password correctly once\n" COLOR_FG_DEFAULT;
      } else {
        passwd_container.show_password();
      }

    } else if (passwd_container.equals_canidate("clear")) {
        std::cout << TERM_ERASE_SCREEN COLOR_FG_GREEN "Current screen cleared\n" COLOR_FG_DEFAULT;

    } else if (passwd_container.equals_canidate("clear all")) {
        std::cout << TERM_ERASE_SCREEN TERM_ERASE_SAVED COLOR_FG_GREEN "Terminal cleared\n" COLOR_FG_DEFAULT;
    
    } else {
      if (passwd_container.is_set()) {
        reset_canidate(passwd_container.check_password(), passwd_container, opts);

      } else {
        bool hashes_correctly = check_entry_against_hash(*existing_entry, passwd_container.get_canidate());
        reset_canidate(hashes_correctly, passwd_container, opts);
      }
    }
  }
}
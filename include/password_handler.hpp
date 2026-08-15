/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef PASSWORD_HANDLER_H
#define PASSWORD_HANDLER_H

#include "cube_memory.hpp"

class password_handler {

    // Private Fields
    //////////////////////////////////////////////////////////////////////

    cube_mem_secure password;
    cube_mem_secure canidate;

    // Public Constructors
    //////////////////////////////////////////////////////////////////////

    public:

    /**
     * @brief Construct a new password handler object
     * 
     */
    password_handler();


    // Public Static Methods
    //////////////////////////////////////////////////////////////////////

    /**
    * @brief Disable terminal echo while typing
    * 
    */
    static void disable_echo();

    /**
    * @brief Enable terminal echo while typing
    * 
    */
    static void enable_echo();
    
    /**
    * @brief Disable terminal echo while typing
    * 
    */
    static void set_connonical();

    /**
    * @brief Enable terminal echo while typing
    * 
    */
    static void set_nonconnonical();

    // Public Methods
    //////////////////////////////////////////////////////////////////////

    /**
     * @brief Is the canidate the same as the password
     * 
     * @return true 
     * @return false 
     */
    bool check_password() const;

    /**
     * @brief Zero the memory holding the possible password
     * 
     */
    void clear_canidate();

    /**
     * @brief Zero the memory holding the password
     * 
     */
    void clear_password();

    /**
     * @brief Read a password from stdin
     * 
     */
    void collect();

    /**
     * @brief Does the string equal the canidate
     * 
     * @return true canidate equals input
     * @return true canidate inequals to input
     */
    bool equals_canidate(const char *) const;

    /**
     * @brief Get the canidate string
     * 
     * @return const char* canidate
     */
    const char * get_canidate() const;

    /**
     * @brief Is there a password saved
     * 
     * @return true password set
     * @return false password unset 
     */
    bool is_set() const;

    /**
     * @brief Show the password as the user types it
     * 
     */
    void follow();
    
    /**
     * @brief Save the canidate as the password
     * 
     * @return true The canidate was saved
     * @return false Canidate was not saved as there is already a password saved
     */
    bool save();

    /**
     * @brief Print password to stdout
     * 
     */
    void show_password() const;
};


#endif
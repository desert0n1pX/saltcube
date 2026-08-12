/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef SALTCUBE_EXCEPTION_H
#define SALTCUBE_EXCEPTION_H

#include <exception>
#include <string>

class saltcube_exception : public std::exception {

    // Protected Fields
    //////////////////////////////////////////////////////////////////////

    protected:

    const char *      what_failed;
    const std::string where_happened;
    const std::string why_happened;
          std::string full_message_str;

    public:

    /**
     * @brief Construct a new saltcube exception object
     * 
     * @param what  What call caused the exception
     * @param where What file threw the error
     * @param line  What line threw the error
     * @param why   Why is this an error
     */
    saltcube_exception(
        const char *      what,
        const std::string where,
        int               line,
        const std::string why) noexcept;

    /**
     * @brief What function call caused the exception
     * 
     */
    virtual const char * what() const noexcept override;

    /**
     * @brief What file threw the error and what line
     *
     * @see __THIS_FILE__
     * @see __LINE__
     * 
     */
    virtual const std::string where() const noexcept;

    /**
     * @brief Why is this an error
     * 
     */
    virtual const std::string why() const noexcept;

    virtual const char * full_message() noexcept;

    private:
    /**
     * @brief make class abstract, this function does absolutely nothing
     * 
     */
    virtual void noinit() = 0;
};

#endif
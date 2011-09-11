/* -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set expandtab shiftwidth=4 tabstop=4: */

/**
 * \file modp_ascii.h
 * <PRE>
 * MODP_ASCII -- Simple ascii manipulation (uppercase, lowercase, etc)
 * http://code.google.com/p/stringencoders/
 *
 * Copyright &copy; 2007, Nick Galbreath -- nickg [at] modp [dot] com
 * All rights reserved.
 *
 * Released under bsd license.  See modp_ascii.c for details.
 * </PRE>
 *
 */

#ifndef COM_MODP_STRINGENCODERS_ASCII
#define COM_MODP_STRINGENCODERS_ASCII

#ifdef __cplusplus
#define BEGIN_C extern "C" {
#define END_C }
#else
#define BEGIN_C
#define END_C
#endif

BEGIN_C

/*
 * \param[in,out] str the input string
 * \param[in] len the length of input string (the strlen)
 */
void modp_toupper(char* str, int size_t);

/** \brief make lower case copy of input string
 *
 * \param[out] output buffer, with at least 'len + 1' bytes allocated
 * \param[in] str the input string
 * \param[in] len the length of input string (the strlen)
 *
 * Please make sure dest has been allocation with at least 'len+1'
 * bytes.  This appends a trailing NULL character at the end of
 * dest!
 *
 * This is based on the algorithm by Paul Hsieh
 * http://www.azillionmonkeys.com/qed/asmexample.html
 */
void modp_toupper_copy(char* dest, const char* str, size_t len);

/** \brief lower case a string in place
 *
 * \param[in, out] str the input string
 * \param[in] len the length of input string (the strlen)
 *
 */
void modp_tolower(char* str, size_t size);

/** \brief make lower case copy of input string
 *
 * \param[out] output buffer, with at least 'len + 1' bytes allocated
 * \param[in] str the input string
 * \param[in] len the length of input string (the strlen)
 *
 * Please make sure dest has been allocation with at least 'len+1'
 * bytes.  This appends a trailing NULL character at the end of
 * dest!
 *
 * This is based on the algorithm by Paul Hsieh
 * http://www.azillionmonkeys.com/qed/asmexample.html
 */
void modp_tolower_copy(char* dest, const char* str, size_t len);

/** \brief turn a string into 7-bit printable ascii.
 *
 * By "printable" we means all characters between 32 and 126.
 * All other values are turned into '?'
 *
 * \param[in, out] str the input string
 * \param[in] len the length of input string (the strlen)
 *
 */
void modp_toprint(char* str, size_t len);

/** \brief make a printable copy of a string
 *
 * By "printable" we means all characters between 32 and 126.
 * All other values are turned into '?'
 *
 * \param[out] output buffer, with at least 'len + 1' bytes allocated
 * \param[in] str the input string
 * \param[in] len the length of input string (the strlen)
 *
 * Please make sure dest has been allocation with at least 'len+1'
 * bytes.  This appends a trailing NULL character at the end of
 * dest!
 */
void modp_toprint_copy(char* dest, const char* str, size_t len);

END_C

#endif  /* MODP_ASCII */

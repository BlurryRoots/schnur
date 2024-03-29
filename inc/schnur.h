// Copyright (c) 2013 - ∞ Sven Freiberg. All rights reserved.
// See license.md for details.
/*! \file schnuerchen.h
	\brief Declares functions to manage a string of characters known as schnur.

	^^
*/

#include <wchar.h>

#ifndef Blurryroots_String_Library_h
#define Blurryroots_String_Library_h

/**
 * The block size used for resize / reallocations.
 */
#define SCHNUR_BLOCK_SIZE 32

/// Wraps a narrow character or string.
#define SCHNUR_N(t) t
/// Wraps a wide character or string.
#define SCHNUR_W(t) L##t

/// The null terminator for a narrow character type.
#define SCHNUR_NC_NULL SCHNUR_N ('\0')
/// The null terminator for a wide character type.
#define SCHNUR_WC_NULL SCHNUR_W ('\0')
#if defined(NULL)
/// A null pointer for use with pointers to schnur_t.
#define SCHNUR_NULL NULL
#else
/// A null pointer for use with pointers to schnur_t.
#define SCHNUR_NULL 0
#endif

/**
    @brief: Represents a narrow character.
*/
typedef char schnur_narrow_t;
/**
    @brief: Represents a wide character.
*/
typedef wchar_t schnur_wide_t;

/**
	@brief: Represents a string of characters.
*/
struct schnur;
/**
 * @brief Convenience typedef for struct schnur.
 * @see struct schnur
 */
typedef
	struct schnur
	schnur_t;

/**
 * @brief      Creates a new schnur_t instance.
 *
 * @return     Pointer to new schnur_t instance.
 */
struct schnur*
schnur_new (void);

/**
 * @brief      Creates a new schnur_t instance.
 * 
 * @param      s  The string to use as inital value.
 *
 * @return     Pointer to new schnur_t instance.
 */
struct schnur*
schnur_new_s (const schnur_wide_t* s);

/**
 * @brief      Creates a new schnur_t instance from narrow utf-8 string.
 * 
 * @param      s  The string to use as inital value.
 *
 * @return     Pointer to new schnur_t instance.
 */
struct schnur*
schnur_new_su (const schnur_narrow_t* s);

/**
 * @brief      Frees given schnur_t object.
 *
 * @param      self  A schnur pointer.
 */
int
schnur_free (struct schnur* self);

/**
 * @brief      Retrieves the data pointer.
 *
 * @param      self  A schnur pointer.
 *
 * @return     The raw data pointer;
 */
void*
schnur_raw (const struct schnur* self);

/**
 * @brief      Retrieves the number of bytes for the raw storage of this schnur.
 * 
 * This will retrieve the total bytes available for the internal raw data 
 * storage.
 *
 * @param      self  A schnur pointer.
 *
 * @return     The size in bytes of this schnur's raw data storage.
 */
size_t
schnur_raw_size (const struct schnur* self);


/**
 * @brief      Retrieves a copy of the contents in wide character encoding.
 *
 * @param      self  A schnur pointer.
 *
 * @return     Contents as wide string. Needs to be free'd;
 */
schnur_wide_t*
schnur_wide (const struct schnur* self);

/**
 * @brief      Allocates and fills a multi-byte string of given schnur's contents.
 *
 * @param      self  A schnur pointer.
 *
 * @return     Content as multi-byte/narrow string. Needs to be free'd;
 */
schnur_narrow_t*
schnur_narrow (const struct schnur* self);

/**
 * @brief      Retrieves a wide character at given index.
 *
 * @param      self  A schnur pointer.
 * @param[in]  i     Index of character to fetch.
 *
 * @return     Character at index or '\0' if arguments where invalid.
 */
schnur_wide_t
schnur_get (const struct schnur* self, size_t i);

/**
 * @brief      Sets a wide character at given index.
 *
 * @param      self  A schnur pointer.
 * @param[in]  c     Character to set.
 * @param[in]  n     Index where to set the character.
 *
 * @return     1 on success, 0 otherwise.
 */
int
schnur_set (struct schnur* self, size_t i, schnur_wide_t c);

/**
 * @brief      Retrieves the number of currently stored characters.
 *
 * @param      self  A schnur pointer.
 *
 * @return     The number of characters used by this schnur.
 */
size_t
schnur_length (const struct schnur* self);

/**
 * @brief      Retrieves the maximum number of characters to be potentially stored.
 *
 * @param      self  A schnur pointer.
 *
 * @return     The maximum number of characters able to be used by this schnur.
 */
size_t
schnur_capacity (const struct schnur* self);

/**
 * @brief      Will change this schnur to end at given index.
 * 
 * This will null terminate this schnur at given index regardless of previous
 * length. Has to be smaller than maximum capacity though.
 *
 * @param      self  A schnur pointer.
 * @param[in]  n     Number of characters to fill schnur with.
 *
 * @return     1 on success, 0 otherwise.
 */
int
schnur_terminate (struct schnur* self, size_t n);

/**
 * @brief      Fills string with given character.
 * 
 * This fills the entire string (full capacity) and not only the used length.
 *
 * @param      self  A schnur pointer.
 * @param[in]  c     Character used for filling.
 *
 * @return     1 on success, 0 otherwise.
 */
int
schnur_fill (struct schnur* self, schnur_wide_t c);

/**
 * @brief      Fills n places of string with given character.
 *
 * @param      self  A schnur pointer.
 * @param[in]  c     Character used for filling.
 * @param[in]  n     Number of characters to fill schnur with.
 *
 * @return     1 on success, 0 otherwise.
 */
int
schnur_fill_n (struct schnur* self, schnur_wide_t c, size_t n);

/**
 * @brief      Expands the capacity of string according to SCHNUR_BLOCK_SIZE.
 *
 * @see SCHNUR_BLOCK_SIZE
 *
 * @param      self  A schnur pointer.
 *
 * @return     1 on success, 0 otherwise.
 */
int
schnur_expand (struct schnur* self);

/**
 * @brief      Compacts string capacity to minimize allocated space, while still
 * able to hold all used places (length).
 * 
 * 
 * @param      self  A schnur pointer.
 *
 * @return     1 on success, 0 otherwise.
 */
int
schnur_compact (struct schnur* self);

/**
 * @brief      Copies the actually used data (length) of other to self.
 *
 * If other is longer than the capacity of self, self is expanded.
 *
 * @param      self   A schnur pointer.
 * @param[in]  other  Another schnur pointer.
 *
 * @return     1 on success, 0 otherwise.
 */
int
schnur_copy (struct schnur* self, const struct schnur* other);

/**
 * @brief      Copies given character array to self.
 *
 * If array is longer than the capacity of self, self is expanded.
 *
 * @param      self   A schnur pointer.
 * @param[in]  other  A character array pointer.
 *
 * @return     1 on success, 0 otherwise.
 */
int
schnur_copy_cstr (struct schnur* self, const schnur_wide_t* other);

/**
 * @brief      Appends given character.
 *
 * Capacity of string might get expanded to hold the additional character.
 *
 * @param      self  A schnur pointer.
 * @param[in]  c     A character to append.
 *
 * @return     1 on success, 0 otherwise.
 */
int
schnur_append (struct schnur* self, schnur_wide_t c);

/**
 * @brief      Appends given character array to string.
 *
 * @param      self   A schnur pointer.
 * @param[in]  other  A character array pointer.
 *
 * @return     1 on success, 0 otherwise.
 */
int
schnur_append_cstr (struct schnur* self, const schnur_wide_t* other);

/**
 * @brief      Appends given string to string.
 *
 * @param      self   A schnur pointer.
 * @param[in]  other  Another schnur pointer.
 *
 * @return     1 on success, 0 otherwise.
 */
int
schnur_append_string (struct schnur* self, const struct schnur* other);

/**
 * @brief      Check if used data of self equals given character array.
 *
 * @param[in]  self   A schnur pointer.
 * @param[in]  other  A character array pointer.
 *
 * @return     1 on equality, 0 otherwise.
 */
int
schnur_equal_cstr (const struct schnur* self, const schnur_wide_t* other);

/**
 * @brief      Check if used data of self equals used data of other.
 *
 * @param[in]  self   A schnur pointer.
 * @param[in]  other  Another schnur pointer.
 *
 * @return     1 on equality, 0 otherwise.
 */
int
schnur_equal (const struct schnur* self, const struct schnur* other);

/**
 * @brief      Reverses actually used data of string.
 *
 * @param      self  A schnur pointer.
 *
 * @return     1 on success, 0 when given a nullpointer.
 */
int
schnur_reverse (struct schnur* self);

/**
 * @brief      Helper function, to determine if the current build environment
 * is supporting wide <-> narrow (aka. multi-byte) character conversion.
 * 
 * This is sensitive to the definition of symbol SCHNUR_WITH_ASSERT. If not
 * defined, the function will always return -1. If defined, it will run a check
 * to see if the current build environment supports converting wide character
 * strings to narrow / multi-byte strings.
 *
 * @return     1 if supporting multi-byte, 0 otherwise. -1 if SCHNUR_WITH_ASSERT is not defined.
 */
int
schnur_supports_multibytes ();

/**
 * @brief      Default handler, used in '*_SCOPED' convenice macros. Will print
 * an error message to stderr containing the variable name which did not properly
 * initialize by default.
 */
int
schnur_scoped_default_error_handler (const schnur_narrow_t* sname);

int
schnur_narrow_free (schnur_narrow_t* self);

int
schnur_wide_free (schnur_wide_t* self);

/// Generates the header for a scope / code block, which makes sure, that the
/// schnur_t* with 'sname', created via 'creation', is properly freed after
/// the block is exited. If anything goes wrong in the creation, it will invoke
/// the handler function provided at 'error_handler'.
#define SCHNUR_SCOPED_HANDLE(sname, creation, error_handler) \
for ( \
    schnur_t *sname = creation; \
    NULL != sname; \
    (NULL != sname \
        ? (schnur_free (sname) && (sname = NULL)) \
        : (error_handler (#sname))) \
)
/// Generates a scope / code block by using SCHNUR_SCOPED_HANDLE with the
/// default error handler function.
#define SCHNUR_SCOPED(sname, creation) \
    SCHNUR_SCOPED_HANDLE(sname, creation, schnur_scoped_default_error_handler)
/// Generates a scope / code block by using SCHNUR_SCOPED with the
//// default schnur creation function (aka. 'schnur_new ()').
#define SCHNUR_SCOPED_EMPTY(sname) \
    SCHNUR_SCOPED(sname, schnur_new ())

/// Generates the header for a scope / code block, which makes sure, that the
/// schnur_narrow_t* with 'strname', created from the schnur_t* 'sname',
/// is properly freed after the block is exited. If anything goes wrong in the
/// creation, it will invoke the handler function provided at 'error_handler'.
#define SCHNUR_NARROW_SCOPED_HANDLE(sname, strname, error_handler) \
for ( \
    schnur_narrow_t *strname = schnur_narrow (sname); \
    NULL != strname; \
    (NULL != strname \
        ? (schnur_narrow_free (strname) && (strname = NULL)) \
        : (error_handler (#strname))) \
)
/// Generates header by using SCHNUR_NARROW_SCOPED_HANDLE with the default
/// error handler function.
#define SCHNUR_NARROW_SCOPED(sname, strname) \
    SCHNUR_NARROW_SCOPED_HANDLE(sname, strname, schnur_scoped_default_error_handler)

/// Generates the header for a scope / code block, which makes sure, that the
/// schnur_wide_t* with 'strname', created from the schnur_t* 'sname',
/// is properly freed after the block is exited. If anything goes wrong in the
/// creation, it will invoke the handler function provided at 'error_handler'.
#define SCHNUR_WIDE_SCOPED_HANDLE(sname, strname, error_handler) \
for ( \
    schnur_wide_t *strname = schnur_wide (sname); \
    NULL != strname; \
    ((NULL != strname) \
        ? (schnur_wide_free (strname) && (strname = NULL)) \
        : (error_handler (#strname))) \
)
/// Generates header by using SCHNUR_WIDE_SCOPED_HANDLE with the default
/// error handler function.
#define SCHNUR_WIDE_SCOPED(sname, strname) \
    SCHNUR_WIDE_SCOPED_HANDLE(sname, strname, schnur_scoped_default_error_handler)

#endif

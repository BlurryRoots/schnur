/*
schnuerchen License 1.0

Copyright (c) 2013 - ∞ Sven Freiberg

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. This software may not be used in products designed to harm,
   injure or oppress living creatures.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <wchar.h>

#ifndef Blurryroots_String_Library_h
#define Blurryroots_String_Library_h

#define SCHNUR_BLOCK_SIZE 32

/*
	@brief: Represents a string of characters.
*/
struct schnur {
	/*
		@brief: Character array containing all data used by this string object.
	*/
	wchar_t* data;

	/*
		@brief: Number of characters used. In other words the position of the
				  null terminator (\0).
	*/
	size_t length;
	/*
		@brief: Maximum number of characters this string can hold.
	*/
	size_t capacity;
};
typedef
	struct schnur
	schnur_t;

/*
	@brief: Creates a new schnur_t instance.

	@returns: Pointer to new schnur_t instance.
*/
schnur_t*
schnur_new (void);

/*
	@brief: Frees given schnur_t object.
*/
void
schnur_free (schnur_t* self);

/*
	@brief: Fills string with given character.

	This fills the entire string (full capacity) and not
	only the used length.

	@returns: 1 on success, 0 otherwise.
*/
int
schnur_fill (schnur_t* self, wchar_t c);

/*
	@brief: Fills n places of string with given character.

	@returns: 1 on success, 0 otherwise.
*/
int
schnur_fill_n (schnur_t* self, wchar_t c, size_t n);

/*
	@brief: Expands the capacity of string according to SCHNUR_BLOCK_SIZE.

	@see SCHNUR_BLOCK_SIZE

	@returns: 1 on success, 0 otherwise.
*/
int
schnur_expand (schnur_t* self);

/*
	@brief: Compacts string capacity to minimize allocated space, while still
	        able to hold all used places (length).

	@returns: 1 on success, 0 otherwise.
*/
int
schnur_compact (schnur_t* self);

/*
	@brief: Copies the actually used data (length) of other to self.

	If other is longer than the capacity of self, self is
	expanded.

	@returns: 1 on success, 0 otherwise.
*/
int
schnur_copy (schnur_t* self, const schnur_t* other);

/*
	@brief: Copies given character array to self.
	
	If array is longer than the capacity of self, self is
	expanded.

	@returns: 1 on success, 0 otherwise.
*/
int
schnur_copy_cstr (schnur_t* self, const wchar_t* other);

/*
	@brief: Appends given character.

	Capacity of string might get expanded to hold the additional character.

	@returns: 1 on success, 0 otherwise.
*/
int
schnur_append (schnur_t* self, wchar_t c);

/*
	@brief: Appends given character array to string.

	@returns: 1 on success, 0 otherwise.
*/
int
schnur_append_cstr (schnur_t* self, const wchar_t* other);

/*
	@brief: Appends given string to string.

	@returns: 1 on success, 0 otherwise.
*/
int
schnur_append_string (schnur_t* self, const schnur_t* other);

/*
	@brief: Check if used data of self equals given character array.

	@returns: 1 on equality, 0 otherwise.
*/
int
schnur_equal_cstr (const schnur_t* self, const wchar_t* other);

/*
	@brief: Check if used data of self equals used data of other.

	@returns: 1 on equality, 0 otherwise.
*/
int
schnur_equal (const schnur_t* self, const schnur_t* other);

/*
	@brief: Reverses actually used data of string.

	@returns: 1 on success, 0 when given a nullpointer.
*/
int
schnur_reverse (schnur_t* self);

#endif

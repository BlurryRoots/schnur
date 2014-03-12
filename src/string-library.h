/*
Copyright (c) 2013, BlurryRoots aka Sven Freiberg

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#include <wchar.h>

#ifndef _STRING_LIBRARY_
#define _STRING_LIBRARY_

#define STRING_BLOCK_SIZE 32

/*
    String structure
*/
struct string_t
{
    /*
        Character array containing all data used by
        this string object.
    */
    wchar_t* data;

    /*
        Number of characters used. In other words
        the position of the null terminator (\0).
    */
    size_t length;
    /*
        Maximum number of characters this string can hold.
    */
    size_t capacity;
};
typedef
    struct string_t
    string_t;

/*
    Creates a new string_t instance.

    @returns: Pointer to new string_t instance.
*/
string_t*
string_new();

/*
    Frees given string_t object.
*/
void
string_free( string_t* self );

/*
    Fills string with given character.
    This fills the entire string (full capacity) and not
    only the used length.

    @returns: 1 on success, 0 otherwise.
*/
int
string_fill( string_t* self, wchar_t c );

/*
    Fills n places of string with given character.

    @returns: 1 on success, 0 otherwise.
*/
int
string_fill_n( string_t* self, wchar_t c, size_t n );

/*
    Expands the capacity of string according to STRING_BLOCK_SIZE.

    @returns: 1 on success, 0 otherwise.
*/
int
string_expand( string_t* self );

/*
    Compacts string capacity to minimize allocated space, while
    still able to hold all used places (length).

    @returns: 1 on success, 0 otherwise.
*/
int
string_compact( string_t* self );

/*
    Copies the actually used data (length) of other to self.
    If other is longer than the capacity of self, self is
    expanded.

    @returns: 1 on success, 0 otherwise.
*/
int
string_copy( string_t* self, const string_t* other );

/*
    Copies given character array to self.
    If array is longer than the capacity of self, self is
    expanded.

    @returns: 1 on success, 0 otherwise.
*/
int
string_copy_cstr( string_t* self, const wchar_t* other );

/*
    Appends given character. Capacity of string might get
    expanded to hold the additional character.

    @returns: 1 on success, 0 otherwise.
*/
int
string_append( string_t* self, wchar_t c );

/*
    Appends given character array to string.
    @returns: 1 on success, 0 otherwise.
*/
int
string_append_cstr( string_t* self, const wchar_t* other );

/*
    Appends given string to string.
    @returns: 1 on success, 0 otherwise.
*/
int
string_append_string( string_t* self, const string_t* other );

/*
    Check if used data of self equals given character array.

    @returns: 1 on equality, 0 otherwise.
*/
int
string_equal_cstr( const string_t* self, const wchar_t* other );

/*
    Check if used data of self equals used data of other.

    @returns: 1 on equality, 0 otherwise.
*/
int
string_equal( const string_t* self, const string_t* other );

/*
    Reverses actually used data of string.

    @returns: 1 on success, 0 when given a nullpointer.
*/
int
string_reverse( string_t* self );

#endif

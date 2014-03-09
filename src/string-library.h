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

struct string_t
{
    wchar_t* data;

    size_t length;
    size_t capacity;
};
typedef
    struct string_t
    string_t;

string_t*
string_new();

void
string_free( string_t* self );

void
string_fill( string_t* self, wchar_t c );

int
string_expand( string_t* self );

int
string_compact( string_t* self );

int
string_copy( string_t* self, const string_t* other );

int
string_copy_cstr( string_t* self, const wchar_t* other );

int
string_append( string_t* self, wchar_t c );

int
string_equal_cstr( const string_t* self, const wchar_t* other );

int
string_equal( const string_t* self, const string_t* other );

int
string_reverse( string_t* self );

#endif

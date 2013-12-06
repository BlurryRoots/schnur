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

#include <stdlib.h>

#ifndef _STRING_T_
#define _STRING_T_

struct string_t
{
    char* value;
    size_t length;
};
typedef
    struct string_t
    string_t;

size_t cstring_length( const char* someString );

string_t string_create();

void string_free( string_t* someStringT );

size_t string_length( string_t* someStringT );

char* string_value( string_t* someStringT );

int string_append( string_t* someStringT, char c );

int string_append_cstring( string_t* someStringT, const char* other );

int string_append_string( string_t* someStringT, string_t other );

int string_copy( string_t* someStringT, string_t other );

int string_compare( string_t someStringT, string_t other );

/*  TODO:
        add functions for comparing string_t and c-string
        add functions for copying c-string into string_t
*/

#endif

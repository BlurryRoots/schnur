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

#include "string-library.h"

size_t cstring_length( const char* someString )
{
    size_t i;

    if( ! someString )
    {
        return 0;
    }

    for( i = 0; someString[i] != '\0'; ++i )
    {}

    return i;
}

string_t string_create()
{
    string_t s;

    s.value = (char*)calloc( 1, sizeof(char) );
    s.value[0] = '\0';
    s.length = 0;

    return s;
}

void string_free( string_t* someStringT )
{
    if( ! someStringT )
    {
        return;
    }

    if( someStringT->value != NULL )
    {
        free( someStringT->value );
    }

    someStringT->value = NULL;
    someStringT->length = 0;
}

size_t string_length( string_t* someStringT )
{
    if( ! someStringT )
    {
        return 0;
    }

    return someStringT->length;
}

char* string_value( string_t* someStringT )
{
    if( ! someStringT )
    {
        return NULL;
    }

    return someStringT->value;
}

int string_append( string_t* someStringT, char c )
{
    size_t i;
    char copy[someStringT->length];

    if( ! someStringT )
    {
        return 0;
    }

    for( i = 0; i < someStringT->length; ++i )
    {
        copy[i] = someStringT->value[i];
    }

    free( someStringT->value );

    someStringT->value = (char*)calloc( someStringT->length + 2, sizeof(char) );
    if( ! someStringT->value )
    {
        return 0;
    }

    for( i = 0; i < someStringT->length; ++i )
    {
        someStringT->value[i] = copy[i];
    }
    someStringT->value[someStringT->length] = c;
    someStringT->value[someStringT->length + 1] = '\0';
    someStringT->length++;

    return 1;
}

int string_append_cstring( string_t* someStringT, const char* someOtherString )
{
    size_t i, otherLength;
    char copy[someStringT->length];

    if( ! someStringT || ! someOtherString )
    {
        return 0;
    }

    for( i = 0; i < someStringT->length; ++i )
    {
        copy[i] = someStringT->value[i];
    }

    free( someStringT->value );

    otherLength = cstring_length( someOtherString );

    someStringT->value = (char*)calloc( someStringT->length + otherLength + 1, sizeof(char) );
    if( ! someStringT->value )
    {
        return 0;
    }

    for( i = 0; i < someStringT->length; ++i )
    {
        someStringT->value[i] = copy[i];
    }
    for( i = 0; i < otherLength; ++i )
    {
        someStringT->value[i + someStringT->length] = someOtherString[i];
    }
    someStringT->value[someStringT->length + otherLength] = '\0';
    someStringT->length += otherLength;

    return 1;
}

int string_append_string( string_t* someStringT, string_t someOtherStrintT )
{
    if( ! someStringT )
    {
        return 0;
    }

    return string_append_cstring( someStringT, someOtherStrintT.value );
}

int string_copy( string_t* someStringT, string_t someOtherStrintT )
{
    size_t i;
    string_t s;

    if( ! someStringT || ! someOtherStrintT.value )
    {
        return 0;
    }

    if( someOtherStrintT.length == 0 )
    {
        s = string_create();
        someStringT->value = s.value;
        someStringT->length = s.length;

        return 1;
    }

    string_free( someStringT );

    someStringT->value = (char*)calloc( someOtherStrintT.length + 1, sizeof( char ) );
    if( ! s.value )
    {
        someStringT->length = 0;
        someStringT->value = NULL;
        
        return 0;
    }

    someStringT->length = someOtherStrintT.length;
    for( i = 0; i < someStringT->length; ++i )
    {
        someStringT->value[i] = someOtherStrintT.value[i];
    }
    someStringT->value[someStringT->length] = '\0';

    return 1;
}

int string_compare( string_t someStringT, string_t someOtherStrintT )
{
    size_t i;

    if( someStringT.length != someOtherStrintT.length )
    {
        return 0;
    }

    for( i = 0; i < someStringT.length; ++i )
    {
        if( someStringT.value[i] != someOtherStrintT.value[i] )
        {
            return 0;
        }
    }

    return 1;
}

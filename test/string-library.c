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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

extern "C"
{
    #include "../src/string-library.h"

    #include <stdlib.h>
    #include <stdio.h>
    #include <wchar.h>
    #include <locale.h>
}

#include <iostream>

#define SHOW_DEBUG 0

TEST_CASE( "string util", "[string]" )
{
}

TEST_CASE( "string manipulation", "[string]" )
{
    string_t* s = NULL;

    SECTION( "string_new" )
    {
        s = string_new();
        REQUIRE( s != NULL );

        REQUIRE( s->data != NULL );
        REQUIRE( s->capacity == STRING_BLOCK_SIZE );
        REQUIRE( s->length == 0 );

        string_free( s );
    }

    SECTION( "string_expand" )
    {
        s = string_new();
        REQUIRE( s != NULL );

        string_expand( s );

        REQUIRE( s->data != NULL );
        REQUIRE( s->capacity == 2 * STRING_BLOCK_SIZE );

        string_free( s );
    }

    SECTION( "string_compact" )
    {
        s = string_new();
        REQUIRE( s != NULL );

        string_expand( s );
        string_expand( s ); // 3 * STRING_BLOCK_SIZE

        REQUIRE( s->data != NULL );
        REQUIRE( s->capacity == 3 * STRING_BLOCK_SIZE );

        string_fill( s, L'=' );

        s->length = 
            (2 * STRING_BLOCK_SIZE - 1)
            - (STRING_BLOCK_SIZE / 2);
        // produces a memory offset of half the block size
        // and freeable memory of 1 * STRING_BLOCK_SIZE
        s->data[s->length] = '\0';

        REQUIRE( string_compact( s ) == 1 );
        REQUIRE( s->capacity == 2 * STRING_BLOCK_SIZE );

        string_free( s );
    }

    SECTION( "string_copy_cstr" )
    {
        const wchar_t* w = L"Hänsel mag Soße!";
        const size_t l =  wcslen( w );

        s = string_new();
        REQUIRE( s != NULL );

        REQUIRE( wcslen( w ) == l );
        string_copy_cstr( s, w );

        REQUIRE( wcsncmp( s->data, w, l ) == 0 );

        string_free( s );
    }

    SECTION( "string_copy" )
    {
        const wchar_t* w = L"ευχαριστημένος";
        const size_t l = wcslen( w );
        string_t* o;

        s = string_new();
        REQUIRE( s != NULL );

        REQUIRE( wcslen( w ) == l );
        string_copy_cstr( s, w );

        REQUIRE( wcsncmp( s->data, w, l ) == 0 );

        o = string_new();
        REQUIRE( s != NULL );

        string_copy( o, s );
        REQUIRE( wcsncmp( s->data, o->data, l ) == 0 );

        string_free( o );
        string_free( s );
    }

    SECTION( "string_append" )
    {
        const wchar_t* b = L"?";

        s = string_new();
        REQUIRE( s != NULL );

        REQUIRE( string_append( s, b[0] ) == 1 );

        REQUIRE( wcsncmp( s->data, b, 1 ) == 0 );

        string_free( s );
    }

    SECTION( "string_append mass" )
    {
        const int Count = 23000;
        const wchar_t* t = L"0123456789";

        s = string_new();
        REQUIRE( s != NULL );

        for( int i = 0; i < Count; ++i )
        {
            REQUIRE( string_append( s, t[i % 10] ) == 1 );
        }

        for( int i = 0; i < Count; ++i )
        {
            REQUIRE( s->data[i] == t[i % 10] );
        }

        // printf( "c: %i\n", s->capacity );

        string_free( s );
    }

    SECTION( "string_equal_cstr" )
    {
        const wchar_t* w = L"舒适";

        s = string_new();
        REQUIRE( s != NULL );

        string_copy_cstr( s, w );

        REQUIRE( string_equal_cstr( s, w ) == 1 );

        string_free( s );
    }

    SECTION( "string_equal" )
    {
        const wchar_t* w = L"décontracté";
        string_t* o;

        s = string_new();
        REQUIRE( s != NULL );
        string_copy_cstr( s, w );

        o = string_new();
        REQUIRE( o != NULL );
        string_copy_cstr( o, w );

        REQUIRE( s != o );

        REQUIRE( string_equal( s, o ) == 1 );

        string_free( o );
        string_free( s );
    }

    SECTION( "string_reverse" )
    {
        const wchar_t* w = L"солнце";
        const size_t l = wcslen( w );

        s = string_new();
        REQUIRE( NULL != s );
        string_copy_cstr( s, w );

#if 1 == SHOW_DEBUG
        setlocale( LC_ALL, "" );

        wprintf( L"d: %ls\n", s->data );
#endif

        REQUIRE( 1 == string_reverse( s ) );

#if 1 == SHOW_DEBUG
        wprintf( L"d: %ls\n", s->data );
#endif

        for( size_t i = 0; i < l; ++i )
        {
            REQUIRE( s->data[i] == w[l - i - 1] );
        }

        string_free( s );
    }
}

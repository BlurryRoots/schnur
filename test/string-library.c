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

    s = string_new();
    REQUIRE( NULL != s );

#if 1 == SHOW_DEBUG
    setlocale( LC_ALL, "" );
#endif

    SECTION( "string_new" )
    {
        REQUIRE( NULL != s->data );
        REQUIRE( STRING_BLOCK_SIZE == s->capacity );
        REQUIRE( 0 == s->length );
    }

    SECTION( "string_expand" )
    {
        string_expand( s );

        REQUIRE( NULL != s->data );
        REQUIRE( (2 * STRING_BLOCK_SIZE) == s->capacity );
    }

    SECTION( "string_compact" )
    {
        string_expand( s );
        string_expand( s ); // 3 * STRING_BLOCK_SIZE

        REQUIRE( NULL != s->data );
        REQUIRE( (3 * STRING_BLOCK_SIZE) == s->capacity );

        string_fill( s, L'=' );

        s->length = 
            (2 * STRING_BLOCK_SIZE - 1)
            - (STRING_BLOCK_SIZE / 2);
        // produces a memory offset of half the block size
        // and freeable memory of 1 * STRING_BLOCK_SIZE
        s->data[s->length] = '\0';

        REQUIRE( 1 == string_compact( s ) );
        REQUIRE( (2 * STRING_BLOCK_SIZE) == s->capacity );
    }

    SECTION( "string_copy_cstr" )
    {
        const wchar_t* w = L"Hänsel mag Soße!";
        const size_t l =  wcslen( w );

        REQUIRE( wcslen( w ) == l );
        string_copy_cstr( s, w );

        REQUIRE( 0 == wcsncmp( s->data, w, l ) );
    }

    SECTION( "string_copy" )
    {
        const wchar_t* w = L"ευχαριστημένος";
        const size_t l = wcslen( w );
        string_t* o;

        REQUIRE( wcslen( w ) == l );
        string_copy_cstr( s, w );

        REQUIRE( 0 == wcsncmp( s->data, w, l ) );

        o = string_new();
        REQUIRE( NULL != o );

        string_copy( o, s );
        REQUIRE( 0 == wcsncmp( s->data, o->data, l ) );

        string_free( o );
    }

    SECTION( "string_append" )
    {
        const wchar_t* b = L"?";

        REQUIRE( 1 == string_append( s, b[0] ) );

        REQUIRE( 0 == wcsncmp( s->data, b, 1 ) );
    }

    SECTION( "string_append mass" )
    {
        const int Count = 23000;
        const wchar_t* t = L"0123456789";

        for( int i = 0; i < Count; ++i )
        {
            REQUIRE( 1 == string_append( s, t[i % 10] ) );
        }

        for( int i = 0; i < Count; ++i )
        {
            REQUIRE( s->data[i] == t[i % 10] );
        }

        // printf( "c: %i\n", s->capacity );
    }

    SECTION( "string_append_cstr" )
    {
        const wchar_t* a = L"Viel";
        const wchar_t* b = L" Spaß!";
        const wchar_t* both = L"Viel Spaß!";
        const size_t l = wcslen( both );

        string_copy_cstr( s, a );

        REQUIRE( 1 == string_append_cstr( s, b ) );

        REQUIRE( 0 == wcsncmp( s->data, both, l ) );
    }

    SECTION( "string_append_string" )
    {
        const wchar_t* a = L"Viel";
        const wchar_t* b = L" Spaß!";
        const wchar_t* both = L"Viel Spaß!";
        const size_t l = wcslen( both );

        string_t* s2;

        string_copy_cstr( s, a );

        s2 = string_new();
        REQUIRE( NULL != s2 );
        string_copy_cstr( s2, b );

        REQUIRE( 1 == string_append_string( s, s2 ) );

        REQUIRE( 0 == wcsncmp( s->data, both, l ) );

        string_free( s2 );
    }

    SECTION( "string_equal_cstr" )
    {
        const wchar_t* w = L"舒适";

        string_copy_cstr( s, w );

        REQUIRE( 1 == string_equal_cstr( s, w ) );
    }

    SECTION( "string_equal" )
    {
        const wchar_t* w = L"décontracté";
        string_t* o;

        string_copy_cstr( s, w );

        o = string_new();
        REQUIRE( NULL != o );
        string_copy_cstr( o, w );

        REQUIRE( s != o );

        REQUIRE( 1 == string_equal( s, o ) );

        string_free( o );
    }

    SECTION( "string_reverse" )
    {
        const wchar_t* w = L"солнце";
        const size_t l = wcslen( w );

        string_copy_cstr( s, w );

#if 1 == SHOW_DEBUG
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
    }

    // free string for all test
    string_free( s );
}

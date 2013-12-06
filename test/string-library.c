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
#include <string-library.h>
}

#include <cstring>

TEST_CASE( "string-library", "[string-library]" )
{
    // 
    string_t aString;

    SECTION( "Test length function for naked c-strings." )
    {
        const char* str = "123 test test";
        const size_t len = 13;

        REQUIRE( cstring_length( str ) == len );
    }

    SECTION( "Test the creation of a string." )
    {
        aString = string_create();

        REQUIRE( aString.value != NULL );
        REQUIRE( aString.value[0] == '\0' );

        REQUIRE( aString.length == 0 );

        string_free( &aString );

        REQUIRE( aString.value == NULL );
        REQUIRE( aString.length == 0 );
    }

    SECTION( "Append character to string." )
    {
        const char first = '?';
        const char second = '!';
        const char* combined = "?!";

        aString = string_create();

        string_append( &aString, first );

        REQUIRE( aString.value != NULL );
        REQUIRE( aString.value[0] == first );
        REQUIRE( aString.value[1] == '\0' );
        REQUIRE( aString.length == 1 );

        string_append( &aString, second );

        REQUIRE( aString.length == 2 );
        REQUIRE( strcmp( aString.value, combined ) == 0 );

        string_free( &aString );
    }

    SECTION( "Append c-string to string." )
    {
        const char* str = "answer = ";
        const size_t len = 9;
        const char* str2 = "42";
        const size_t len2 = 2;
        const char* result = "answer = 42";
        const size_t lenResult = len + len2;

        aString = string_create();

        string_append_cstring( &aString, str );

        REQUIRE( aString.value != NULL );
        REQUIRE( aString.length == len );
        REQUIRE( strcmp( aString.value, str ) == 0 );

        string_append_cstring( &aString, str2 );

        REQUIRE( aString.value != NULL );
        REQUIRE( aString.length == lenResult );
        REQUIRE( strcmp( aString.value, result ) == 0 );

        string_free( &aString );
    }

    /*  TODO:
            write test for remaining functions
    */
}

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

    SECTION( "Get length of naked c-strings. - cstring_length -" )
    {
        const char* str = "123 test test";
        const size_t len = 13;

        REQUIRE( cstring_length( str ) == len );
    }

    SECTION( "Creation of a string. - string_create and string_free -" )
    {
        aString = string_create();

        REQUIRE( aString.value != NULL );
        REQUIRE( aString.value[0] == '\0' );

        REQUIRE( aString.length == 0 );

        string_free( &aString );

        REQUIRE( aString.value == NULL );
        REQUIRE( aString.length == 0 );
    }

    SECTION( "Creation of a string. - string_create_from and string_free -" )
    {
        const char* str = "rincewind";
        aString = string_create_from( str );

        REQUIRE( aString.value != NULL );
        REQUIRE( strcmp( aString.value, str ) == 0 );

        REQUIRE( aString.length == cstring_length( str ) );

        string_free( &aString );

        REQUIRE( aString.value == NULL );
        REQUIRE( aString.length == 0 );
    }

    SECTION( "Append character to string. - string_append -" )
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

    SECTION( "Append c-string to string. - string_append_cstring -" )
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

    SECTION( "Copying string. - string_copy -" )
    {
        const char* str = "hanswurst";
        string_t another = string_create();

        aString = string_create();

        string_append_cstring( &aString, str );
        string_copy( &another, aString );
        REQUIRE( strcmp( another.value, str ) == 0 );

        string_free( &aString );
        string_free( &another );
    }

    SECTION( "Comparing string and string. - string_equals -" )
    {
        const char* str = "hanswurst";
        const char* str2 = "hanswurst";
        string_t another = string_create_from( str );

        aString = string_create_from( str2 );

        REQUIRE( string_equals( aString, another ) == 1 );

        string_free( &aString );
        string_free( &another );
    }

    SECTION( "Comparing string and string. - string_equals_cstring -" )
    {
        const char* str = "hanswurst";
        string_t another = string_create_from( str );

        REQUIRE( string_equals_cstring( another, str ) == 1 );

        string_free( &aString );
        string_free( &another );
    }

    /*  TODO:
            write test for remaining functions
    */
}

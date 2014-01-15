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

#include <stdlib.h>

string_t*
string_new()
{
    string_t* s;
    size_t i;

    s = malloc( sizeof( string_t ) );
    if( ! s )
    {
        return 0;
    }

    s->data = calloc( STRING_BLOCK_SIZE, sizeof( wchar_t ) );
    if( ! s->data )
    {
        free( s );
        return 0;
    }

    s->capacity = STRING_BLOCK_SIZE;
    s->length = 0;

    return s;
}

void
string_free( string_t* self )
{
    size_t i;

    if( self == NULL )
    {
        return;
    }

    if( self->data != NULL )
    {
        string_fill( self, L'#' );
        free( self->data );
    }

    free( self );
}

void
string_fill( string_t* self, wchar_t c )
{
    size_t i;

    if( self == NULL )
    {
        return;
    }

    for( i = 0; i < self->capacity; ++i )
    {
        self->data[i] = c;
    }
    self->data[self->capacity - 1] = '\0';    
}

int
string_expand( string_t* self )
{
    size_t i;
    wchar_t* buffer;

    if( self == NULL )
    {
        return 0;
    }

    buffer = calloc(
        self->capacity + STRING_BLOCK_SIZE,
        sizeof( wchar_t )
    );
    if( ! buffer )
    {
        return 0;
    }

    wcsncpy( buffer, self->data, self->length );
    buffer[self->length] = L'\0';

    string_fill( self, L'#' );
    free( self->data );

    self->data = buffer;
    self->capacity += STRING_BLOCK_SIZE;

    return 1;
}

int
string_compact( string_t* self )
{
    size_t diff, memoff;
    wchar_t* buffer;

    if( self == NULL )
    {
        return 0;
    }

    diff = self->capacity - (self->length + 1); // \0

    memoff = diff % STRING_BLOCK_SIZE;
    if( memoff > 0 )
    {
        buffer = calloc(
            self->length + 1 + memoff, // len + \0 + offset
            sizeof( wchar_t )
        );
        if( buffer == NULL )
        {
            return 0;
        }

        wcsncpy( buffer, self->data, self->length + 1 + memoff );

        string_fill( self, L'#' );
        free( self->data );
        self->data = buffer;

        self->capacity = self->length + 1 + memoff;
    }

    return 1;
}


int
string_copy_cstr( string_t* self, const wchar_t* other )
{
    size_t i, ol;

    if( self == NULL || other == NULL )
    {
        return 0;
    }

    ol = wcslen( other );
    while( self->capacity <= ol )
    {
        if( ! string_expand( self ) )
        {
            return 0;
        }
    }

    wcsncpy( self->data, other, ol );
    self->length = ol;
    self->data[self->length] = L'\0';

    return 1;
}

int
string_append( string_t* self, wchar_t c )
{
    if( self == NULL )
    {
        return 0;
    }

    if( (self->length + 1) >= self->capacity )
    {
        if( ! string_expand( self ) )
        {
            return 0;
        }
    }

    self->data[self->length++] = c;
    self->data[self->length] = '\0';

    return 1;
}

int
string_equal_cstr( const string_t* self, const wchar_t* other )
{
    size_t so;

    if( self == NULL )
    {
        return 0;
    }

    so = wcslen( other );

    if( self->length == so
     && wcsncmp( self->data, other, so ) == 0 )
    {
        return 1;
    }

    return 0;
}

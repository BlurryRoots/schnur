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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

extern "C" {
	#include <schnuerchen.h>

	#include <stdlib.h>
	#include <stdio.h>
	#include <wchar.h>
	#include <locale.h>
}

#include <iostream>

#define SHOW_DEBUG 0

TEST_CASE ("string util", "[string]") {
}

TEST_CASE ("string manipulation", "[string]") {
	schnur_t* s = NULL;

	s = schnur_new ();
	REQUIRE (NULL != s);

#if 1 == SHOW_DEBUG
	setlocale (LC_ALL, "");
#endif

	SECTION ("schnur_new") {
		REQUIRE (NULL != s->data);
		REQUIRE (SCHNUR_BLOCK_SIZE == s->capacity);
		REQUIRE (0 == s->length);
	}

	SECTION ("schnur_expand") {
		schnur_expand (s);

		REQUIRE (NULL != s->data);
		REQUIRE ((2 * SCHNUR_BLOCK_SIZE) == s->capacity);
	}

	SECTION ("schnur_compact") {
		schnur_expand (s);
		schnur_expand (s); // 3 * SCHNUR_BLOCK_SIZE

		REQUIRE (NULL != s->data);
		REQUIRE ((3 * SCHNUR_BLOCK_SIZE) == s->capacity);

		schnur_fill (s, L'=');

		s->length =
			(2 * SCHNUR_BLOCK_SIZE - 1)
			- (SCHNUR_BLOCK_SIZE / 2);
		// produces a memory offset of half the block size
		// and freeable memory of 1 * SCHNUR_BLOCK_SIZE
		s->data[s->length] = '\0';

		REQUIRE (1 == schnur_compact (s));
		REQUIRE ((2 * SCHNUR_BLOCK_SIZE) == s->capacity);
	}

	SECTION ("schnur_copy_cstr") {
		const wchar_t* w = L"Hänsel mag Soße!";
		const size_t l =  wcslen (w);

		REQUIRE (wcslen (w) == l);
		schnur_copy_cstr (s, w);

		REQUIRE (0 == wcsncmp (s->data, w, l));
	}

	SECTION ("schnur_copy") {
		const wchar_t* w = L"ευχαριστημένος";
		const size_t l = wcslen (w);
		schnur_t* o;

		REQUIRE (wcslen (w) == l);
		schnur_copy_cstr (s, w);

		REQUIRE (0 == wcsncmp (s->data, w, l));

		o = schnur_new ();
		REQUIRE (NULL != o);

		schnur_copy (o, s);
		REQUIRE (0 == wcsncmp (s->data, o->data, l));

		schnur_free (o);
	}

	SECTION( "schnur_append" ) {
		const wchar_t* b = L"?";

		REQUIRE (1 == schnur_append (s, b[0]));

		REQUIRE (0 == wcsncmp (s->data, b, 1));
	}

	SECTION ("schnur_append mass") {
		const int Count = 23000;
		const wchar_t* t = L"0123456789";

		for (int i = 0; i < Count; ++i) {
			REQUIRE (1 == schnur_append (s, t[i % 10]));
		}

		for (int i = 0; i < Count; ++i) {
			REQUIRE (s->data[i] == t[i % 10]);
		}

		REQUIRE (Count == s->length);
	}

	SECTION ("schnur_append_cstr") {
		const wchar_t* a = L"Viel";
		const wchar_t* b = L" Spaß!";
		const wchar_t* both = L"Viel Spaß!";
		const size_t l = wcslen( both );

		schnur_copy_cstr (s, a);

		REQUIRE (1 == schnur_append_cstr (s, b));

		REQUIRE (0 == wcsncmp (s->data, both, l));
	}

	SECTION ("schnur_append_string") {
		const wchar_t* a = L"Viel";
		const wchar_t* b = L" Spaß!";
		const wchar_t* both = L"Viel Spaß!";
		const size_t l = wcslen (both);

		schnur_t* s2;

		schnur_copy_cstr (s, a);

		s2 = schnur_new ();
		REQUIRE (NULL != s2);
		schnur_copy_cstr (s2, b);

		REQUIRE (1 == schnur_append_string (s, s2));

		REQUIRE (0 == wcsncmp (s->data, both, l));

		schnur_free (s2);
	}

	SECTION ("schnur_equal_cstr") {
		const wchar_t* w = L"舒适";

		schnur_copy_cstr (s, w);

		REQUIRE (1 == schnur_equal_cstr (s, w));
	}

	SECTION ("schnur_equal") {
		const wchar_t* w = L"décontracté";
		schnur_t* o;

		schnur_copy_cstr (s, w);

		o = schnur_new ();
		REQUIRE (NULL != o);
		schnur_copy_cstr (o, w);

		REQUIRE (s != o);

		REQUIRE (1 == schnur_equal (s, o));

		schnur_free (o);
	}

	SECTION ("schnur_reverse") {
		const wchar_t* w = L"солнце";
		const size_t l = wcslen (w);

		schnur_copy_cstr (s, w);

#if 1 == SHOW_DEBUG
		wprintf (L"d: %ls\n", s->data);
#endif

		REQUIRE (1 == schnur_reverse (s));

#if 1 == SHOW_DEBUG
		wprintf (L"d: %ls\n", s->data);
#endif

		for (size_t i = 0; i < l; ++i) {
			REQUIRE (s->data[i] == w[l - i - 1]);
		}
	}

	// free string for all test
	schnur_free (s);
}

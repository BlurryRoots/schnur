// Copyright (c) 2013 - ∞ Sven Freiberg. All rights reserved.
// See license.md for details.
/*! \file main.cpp
	\brief Declares functions to manage a string of characters as a structure.

	^^
*/

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

extern "C" {
	#include <stdlib.h>
	#include <stdio.h>
	#include <wchar.h>
	#include <locale.h>

	#include <schnur.h>
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
		REQUIRE (NULL != schnur_data (s));
		REQUIRE (SCHNUR_BLOCK_SIZE == schnur_capacity (s));
		REQUIRE (0 == schnur_length (s));
	}

	SECTION ("schnur_expand") {
		schnur_expand (s);

		REQUIRE (NULL != schnur_data (s));
		REQUIRE ((2 * SCHNUR_BLOCK_SIZE) == schnur_capacity (s));
	}

	SECTION ("schnur_compact") {
		schnur_expand (s);
		schnur_expand (s); // 3 * SCHNUR_BLOCK_SIZE

		REQUIRE (NULL != schnur_data (s));
		REQUIRE ((3 * SCHNUR_BLOCK_SIZE) == schnur_capacity (s));

		REQUIRE (1 == schnur_fill (s, SCHNUR_C ('X')));

		REQUIRE ((3 * SCHNUR_BLOCK_SIZE) == schnur_length (s));		

		size_t cutoff_length =
			(2 * SCHNUR_BLOCK_SIZE - 1)
			- (SCHNUR_BLOCK_SIZE / 2);
		// produces a memory offset of half the block size
		// and freeable memory of 1 * SCHNUR_BLOCK_SIZE
		REQUIRE (1 == schnur_terminate (s, cutoff_length));

		REQUIRE (1 == schnur_compact (s));
		REQUIRE ((2 * SCHNUR_BLOCK_SIZE) == schnur_capacity (s));
	}

	SECTION ("schnur_copy_cstr") {
		const wchar_t* w = SCHNUR ("Hänsel mag Soße!");
		const size_t l =  wcslen (w);

		REQUIRE (wcslen (w) == l);
		schnur_copy_cstr (s, w);

		REQUIRE (0 == wcsncmp ((wchar_t*)schnur_data (s), w, l));
	}

	SECTION ("schnur_copy") {
		const wchar_t* w = SCHNUR ("ευχαριστημένος");
		const size_t l = wcslen (w);
		schnur_t* o;

		REQUIRE (wcslen (w) == l);
		schnur_copy_cstr (s, w);

		REQUIRE (0 == wcsncmp ((wchar_t*)schnur_data (s), w, l));

		o = schnur_new ();
		REQUIRE (NULL != o);

		schnur_copy (o, s);
		REQUIRE (0 == wcsncmp ((wchar_t*)schnur_data (s), (wchar_t*)schnur_data (o), l));

		schnur_free (o);
	}

	SECTION( "schnur_append" ) {
		const wchar_t* b = SCHNUR ("?");

		REQUIRE (1 == schnur_append (s, b[0]));

		REQUIRE (0 == wcsncmp ((wchar_t*)schnur_data (s), b, 1));
	}

	SECTION ("schnur_append mass") {
		const int Count = 23000;
		const wchar_t* t = SCHNUR ("0123456789");

		for (int i = 0; i < Count; ++i) {
			REQUIRE (1 == schnur_append (s, t[i % 10]));
		}

		for (int i = 0; i < Count; ++i) {
			REQUIRE (((wchar_t*)schnur_data (s))[i] == t[i % 10]);
		}

		REQUIRE (Count == schnur_length (s));
	}

	SECTION ("schnur_append_cstr") {
		const wchar_t* a = SCHNUR ("Viel");
		const wchar_t* b = SCHNUR (" Spaß!");
		const wchar_t* both = SCHNUR ("Viel Spaß!");
		const size_t l = wcslen( both );

		schnur_copy_cstr (s, a);

		REQUIRE (1 == schnur_append_cstr (s, b));

		REQUIRE (0 == wcsncmp ((wchar_t*)schnur_data (s), both, l));
	}

	SECTION ("schnur_append_string") {
		const wchar_t* a = SCHNUR ("Viel");
		const wchar_t* b = SCHNUR (" Spaß!");
		const wchar_t* both = SCHNUR ("Viel Spaß!");
		const size_t l = wcslen (both);

		schnur_t* s2;

		schnur_copy_cstr (s, a);

		s2 = schnur_new ();
		REQUIRE (NULL != s2);
		schnur_copy_cstr (s2, b);

		REQUIRE (1 == schnur_append_string (s, s2));

		REQUIRE (0 == wcsncmp ((wchar_t*)schnur_data (s), both, l));

		schnur_free (s2);
	}

	SECTION ("schnur_equal_cstr") {
		const wchar_t* w = SCHNUR ("舒适");

		schnur_copy_cstr (s, w);

		REQUIRE (1 == schnur_equal_cstr (s, w));
	}

	SECTION ("schnur_equal") {
		const wchar_t* w = SCHNUR ("décontracté");
		schnur_t* o;

		schnur_copy_cstr (s, w);

		o = schnur_new ();
		REQUIRE (NULL != o);
		schnur_copy_cstr (o, w);

		REQUIRE (s != o);

		REQUIRE (1 == schnur_equal (s, o));

		schnur_free (o);
	}

	SECTION ("schnur_get") {
		schnur_t* w = schnur_new_s (SCHNUR ("Hänsel mag Soße!"));

		REQUIRE (SCHNUR_C ('ä') == schnur_get (w, 1));

		schnur_free (w);
	}

	SECTION ("schnur_reverse") {
		const wchar_t* w = SCHNUR ("солнце");
		const size_t l = wcslen (w);

		schnur_copy_cstr (s, w);

#if 1 == SHOW_DEBUG
		wprintf (L"d: %ls\n", schnur_data (s));
#endif

		REQUIRE (1 == schnur_reverse (s));

#if 1 == SHOW_DEBUG
		wprintf (SCHNUR ("d: %ls\n"), schnur_data (s));
#endif

		for (size_t i = 0; i < l; ++i) {
			REQUIRE (((wchar_t*)schnur_data (s))[i] == w[l - i - 1]);
		}
	}

	// free string for all test
	schnur_free (s);
}

// Copyright (c) 2013 - ∞ Sven Freiberg. All rights reserved.
// See license.md for details.
/*! \file main.cpp
	\brief Declares functions to manage a string of characters as a structure.

	^^
*/

#include <catch.hpp>

extern "C" {
	#include <stdlib.h>
	#include <stdio.h>
	#include <wchar.h>
	#include <string.h>
	#include <locale.h>

	#include <schnur.h>
}

#include <iostream>

#define WCS_ERROR ((size_t)-1)
#define SHOW_DEBUG 0

TEST_CASE ("creation/new", "[string]") {
	char* locale = strdup (setlocale (LC_ALL, NULL));
	#if _WIN32
	setlocale (LC_ALL, ".UTF8");
	#else
	ssetlocale (LC_ALL, "en_US.UTF-8");
	#endif

	SECTION ("data sizes") {
		const wchar_t* base = L"живот";
		const size_t base_len = wcslen (base);
		REQUIRE (5 == base_len);
		
		const size_t bl = (base_len+1) * sizeof (wchar_t);
		char mbstr[bl];
		size_t mbn = wcstombs (mbstr, base, bl);
		printf ("Converted multi-byte string(%zu, max: %zu): %s\n", mbn, bl, mbstr);
		REQUIRE (10 == mbn);
	}

	SECTION ("schnur_new") {
		schnur_t* s = schnur_new ();

		REQUIRE (NULL != schnur_data (s));
		REQUIRE (SCHNUR_BLOCK_SIZE == schnur_capacity (s));
		REQUIRE (0 == schnur_length (s));

		schnur_free (s);
	}

	SECTION ("schnur_new_s") {
		const wchar_t* base = L"живи и воли, живот је кратак";
		schnur_t* new_s = schnur_new_s (base);
		REQUIRE (NULL != new_s);
		REQUIRE (SCHNUR_BLOCK_SIZE == schnur_capacity (new_s));
		REQUIRE (wcslen (base) == schnur_length (new_s));
	}

	SECTION ("schnur_new_su") {		
		const char* raw_base = "\xD0\x89\xD1\x83\xD0\xB1\xD0\xB0\xD0\xB2";
		printf ("Raw length: %zu => %s\n", strlen (raw_base), raw_base);
		schnur_t* new_su = schnur_new_su (raw_base);
		REQUIRE (NULL != new_su);
		REQUIRE (SCHNUR_BLOCK_SIZE == schnur_capacity (new_su));
		REQUIRE (5 == schnur_length (new_su));

		const wchar_t* base = L"љубав";
		schnur_t* new_s = schnur_new_s (base);
		REQUIRE (NULL != new_s);
		REQUIRE (SCHNUR_BLOCK_SIZE == schnur_capacity (new_s));
		size_t new_s_l = schnur_length (new_s);
		REQUIRE (5 == new_s_l);
		wchar_t* new_s_base = (wchar_t*)schnur_data (new_s);
		REQUIRE (0 == wcscmp (new_s_base, base));

		wprintf (L"??(%zu): '%ls'\n", new_s_l, new_s_base);

		size_t n = new_s_l*4;
		char* kaknase = (char*)malloc(n);
		size_t k = wcstombs (kaknase, base, n);
		printf ("data(l: %zu): '%s'\n", k, kaknase);
		free (kaknase);
	}

	SECTION ("energy") {22
		schnur_t* wow = schnur_new_s (L"๐๓๓๗.๛");
		wprintf (L"What?!: %s\n", schnur_data (wow));
		char* exp = schnur_export (wow);
		printf ("exp(%zu): '%s'\n", strlen (exp), exp);
		free (exp);
		schnur_set (wow, 0, L'ყ');
		schnur_set (wow, 1, L'ა');
		schnur_set (wow, 2, L'ვ');
		schnur_set (wow, 3, L'ა');
		schnur_set (wow, 4, L'\0');
		exp = schnur_export (wow);
		printf ("exp(%zu): '%s'\n", strlen (exp), exp);
		schnur_t* pop = schnur_new_su ("\xE1\x83\xA7\xE1\x83\x90\xE1\x83\x95\xE1\x83\x90");
		char* pop_exp = schnur_export (pop);
		REQUIRE (0 == strcmp (exp, pop_exp));
		free (pop_exp);
		free (exp);
		schnur_free (pop);
		schnur_free (wow);
	}

	setlocale (LC_ALL, locale);
	free (locale);
}

TEST_CASE ("wide/narrow encoding", "[string]") {
	setlocale (LC_ALL, "");

	SECTION ("schnur_export") {
		schnur_t* schnex = NULL;
		schnex = schnur_new_s (L"живи и воли, живот је кратак");
		REQUIRE (NULL != schnex);
		size_t ls = schnur_length (schnex);
		char* export_string = schnur_export (schnex);
		printf ("Exported wide string contents to: '%s'\n", export_string);
		size_t le = strlen (export_string);
		REQUIRE (50 == le);
		free (export_string);
	}
}

TEST_CASE ("appending", "[string]") {
	setlocale (LC_ALL, "");

	SECTION( "schnur_append" ) {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

		const wchar_t* b = SCHNUR ("?");

		REQUIRE (1 == schnur_append (s, b[0]));

		REQUIRE (0 == wcsncmp ((wchar_t*)schnur_data (s), b, 1));

		schnur_free (s);
	}

	SECTION ("schnur_append mass") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

		const size_t Count = 128000;
		const wchar_t* t = SCHNUR ("0123456789");

		INFO ("About to append numbers 0-9 for 128.000 times.");
		for (size_t i = 0; i < Count; ++i) {
			REQUIRE (1 == schnur_append (s, t[i % 10]));
		}

		for (size_t i = 0; i < Count; ++i) {
			REQUIRE (((wchar_t*)schnur_data (s))[i] == t[i % 10]);
		}

		REQUIRE (Count == schnur_length (s));

		schnur_free (s);
	}

	SECTION ("schnur_append_cstr") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

		const wchar_t* a = SCHNUR ("Viel");
		const wchar_t* b = SCHNUR (" Spaß!");
		const wchar_t* both = SCHNUR ("Viel Spaß!");
		const size_t l = wcslen( both );

		schnur_copy_cstr (s, a);

		REQUIRE (1 == schnur_append_cstr (s, b));

		REQUIRE (0 == wcsncmp ((wchar_t*)schnur_data (s), both, l));

		schnur_free (s);
	}

	SECTION ("schnur_append_string") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

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
		schnur_free (s);
	}
}

TEST_CASE ("copy", "[string]") {
	setlocale (LC_ALL, "");

	SECTION ("schnur_copy_cstr") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

		const wchar_t* w = SCHNUR ("Hänsel mag Soße!");
		const size_t l =  wcslen (w);

		REQUIRE (wcslen (w) == l);
		schnur_copy_cstr (s, w);

		REQUIRE (0 == wcsncmp ((wchar_t*)schnur_data (s), w, l));
		schnur_free (s);
	}

	SECTION ("schnur_copy") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

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
		schnur_free (s);
	}
}

TEST_CASE ("expand/compact", "[string]") {
	setlocale (LC_ALL, "");

	SECTION ("schnur_expand") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

		schnur_expand (s);

		REQUIRE (NULL != schnur_data (s));
		REQUIRE ((2 * SCHNUR_BLOCK_SIZE) == schnur_capacity (s));

		schnur_free (s);
	}

	SECTION ("schnur_compact") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

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
		schnur_free (s);
	}
}

TEST_CASE ("equal", "[string]") {
	setlocale (LC_ALL, "");

	SECTION ("schnur_equal_cstr") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

		const wchar_t* w = SCHNUR ("舒适");

		schnur_copy_cstr (s, w);

		REQUIRE (1 == schnur_equal_cstr (s, w));
		schnur_free (s);
	}

	SECTION ("schnur_equal") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

		const wchar_t* w = SCHNUR ("décontracté");
		schnur_t* o;

		schnur_copy_cstr (s, w);

		o = schnur_new ();
		REQUIRE (NULL != o);
		schnur_copy_cstr (o, w);

		REQUIRE (s != o);

		REQUIRE (1 == schnur_equal (s, o));

		schnur_free (o);
		schnur_free (s);
	}
}

TEST_CASE ("access", "[string]") {
	setlocale (LC_ALL, "");

	SECTION ("schnur_get") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

		schnur_t* w = schnur_new_s (SCHNUR ("Hänsel mag Soße!"));

		REQUIRE (SCHNUR_C ('ä') == schnur_get (w, 1));

		schnur_free (w);
		schnur_free (s);
	}
}

TEST_CASE ("string manipulation", "[string]") {
	setlocale (LC_ALL, "");

	SECTION ("schnur_reverse") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

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

		schnur_free (s);
	}
}

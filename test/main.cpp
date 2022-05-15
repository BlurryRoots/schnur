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

#define WCS_ERROR ((size_t)-1)
#define SHOW_DEBUG 0

TEST_CASE ("creation/new", "[string]") {
	schnur_narrow_t* locale = strdup (setlocale (LC_ALL, NULL));
	#if _WIN32
	setlocale (LC_ALL, ".UTF8");
	#else
	ssetlocale (LC_ALL, "en_US.UTF-8");
	#endif

	SECTION ("data sizes") {
		constexpr schnur_wide_t* base = L"живот";
		constexpr size_t base_len = 5;
		REQUIRE (base_len == wcslen (base));
		
		constexpr size_t bl = (base_len+1) * sizeof (schnur_wide_t);
		schnur_narrow_t mbstr[bl];
		size_t mbn = wcstombs (mbstr, base, bl);
		printf ("Converted multi-byte string(%zu, max: %zu): %s\n", mbn, bl, mbstr);
		REQUIRE (10 == mbn);
	}

	SECTION ("schnur_new") {
		schnur_t* s = schnur_new ();

		REQUIRE (NULL != s);
		REQUIRE (NULL != schnur_raw (s));
		REQUIRE (SCHNUR_BLOCK_SIZE == schnur_capacity (s));
		REQUIRE (0 == schnur_length (s));

		schnur_free (s);
	}

	SECTION ("schnur_new_s") {
		const schnur_wide_t* base = L"живи и воли, живот је кратак";
		schnur_t* new_s = schnur_new_s (base);
		REQUIRE (NULL != new_s);
		REQUIRE (SCHNUR_BLOCK_SIZE == schnur_capacity (new_s));
		REQUIRE (wcslen (base) == schnur_length (new_s));
		schnur_free (new_s);
	}

	SECTION ("schnur_new_su") {		
		const schnur_narrow_t* raw_base = "\xD0\x89\xD1\x83\xD0\xB1\xD0\xB0\xD0\xB2";
		printf ("Raw length: %zu => %s\n", strlen (raw_base), raw_base);
		schnur_t* new_su = schnur_new_su (raw_base);
		REQUIRE (NULL != new_su);
		REQUIRE (SCHNUR_BLOCK_SIZE == schnur_capacity (new_su));
		REQUIRE (5 == schnur_length (new_su));
		schnur_free (new_su);

		const schnur_wide_t* base = L"љубав";
		schnur_t* new_s = schnur_new_s (base);

		REQUIRE (NULL != new_s);
		REQUIRE (SCHNUR_BLOCK_SIZE == schnur_capacity (new_s));
		size_t new_s_l = schnur_length (new_s);
		REQUIRE (5 == new_s_l);

		SCHNUR_WIDE_SCOPED (new_s, new_s_base) {
			REQUIRE (0 == wcscmp (new_s_base, base));
			wprintf (L"??(%zu): '%ls'\n", new_s_l, new_s_base);
		}

		schnur_free (new_s);
	}

	SECTION("schnur_new_s scoped") {
		const schnur_wide_t* base = L"живи и воли, живот је кратак";
		SCHNUR_SCOPED(new_s, schnur_new_s(base)) {
			REQUIRE(NULL != new_s);
			REQUIRE(NULL != schnur_raw (new_s));
			REQUIRE(SCHNUR_BLOCK_SIZE == schnur_capacity(new_s));
			REQUIRE(wcslen(base) == schnur_length(new_s));
		}
	}

	SECTION ("energy") {
		const schnur_wide_t* leet = SCHNUR_W ("๐๓๓๗.๛");
		SCHNUR_SCOPED(wow, schnur_new_s(leet)) {
			SCHNUR_WIDE_SCOPED(wow, wow_wstr) {
				wprintf(L"What?!: %s\n", wow_wstr);
			}

			SCHNUR_NARROW_SCOPED(wow, wow_nstr) {
				printf("exp(%zu): '%s'\n", strlen(wow_nstr), wow_nstr);
			}

			schnur_set(wow, 0, L'ყ');
			schnur_set(wow, 1, L'ა');
			schnur_set(wow, 2, L'ვ');
			schnur_set(wow, 3, L'ა');
			schnur_terminate(wow, 4);

			const schnur_narrow_t* utf8_codepoints =
				"\xE1\x83\xA7" "\xE1\x83\x90"
				"\xE1\x83\x95" "\xE1\x83\x90";
			SCHNUR_SCOPED(pop, schnur_new_su(utf8_codepoints)) {
				SCHNUR_NARROW_SCOPED(pop, pop_ex) {
					SCHNUR_NARROW_SCOPED(wow, wow_ex) {
						printf("exp(%zu): '%s'\n", strlen(pop_ex), pop_ex);
						REQUIRE(0 == strcmp(wow_ex, pop_ex));
					}
				}
			}
		}
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
		schnur_narrow_t* export_string = schnur_narrow (schnex);
		printf ("Exported wide string contents to: '%s'\n", export_string);
		size_t le = strlen (export_string);
		REQUIRE (50 == le);
		free (export_string);
	}
}

TEST_CASE ("appending", "[string]") {
	setlocale (LC_ALL, "");

	SECTION( "schnur_append" ) {
		//SCHNUR_SCOPED (s, schnur_new ()) { // can be appreviated to:
		SCHNUR_SCOPED_EMPTY(s) {
			REQUIRE(NULL != s);

			const schnur_wide_t* b = SCHNUR_W ("?");

			REQUIRE(1 == schnur_append(s, b[0]));

			SCHNUR_WIDE_SCOPED(s, sw) {
				REQUIRE(0 == wcsncmp(sw, b, 1));
			}
		}
	}

	SECTION ("schnur_append mass") {
		//SCHNUR_SCOPED (s, schnur_new ()) { // can be appreviated to:
		SCHNUR_SCOPED_EMPTY (s) {
			REQUIRE (NULL != s);

			const size_t count = 12000;
			const schnur_wide_t* t = SCHNUR_W ("0123456789");

			INFO ("About to append numbers 0-9 for 128.000 times.");
			for (size_t i = 0; i < count; ++i) {
				schnur_wide_t cc = t[i % 10];
				REQUIRE (1 == schnur_append (s, cc));
			}

			SCHNUR_WIDE_SCOPED (s, sw) {
				for (size_t i = 0; i < count; ++i) {
					schnur_wide_t c = sw[i];
					schnur_wide_t cc = t[i % 10];
					REQUIRE (c == cc);
				}
			}

			REQUIRE (count == schnur_length (s));
		}
	}

	SECTION ("schnur_append_cstr") {
		SCHNUR_SCOPED_EMPTY (s) {
			REQUIRE (NULL != s);

			const schnur_wide_t* a = SCHNUR_W ("Viel");
			const schnur_wide_t* b = SCHNUR_W (" Spaß!");
			const schnur_wide_t* both = SCHNUR_W ("Viel Spaß!");
			const size_t l = wcslen( both );

			schnur_copy_cstr (s, a);

			REQUIRE (1 == schnur_append_cstr (s, b));

			SCHNUR_WIDE_SCOPED (s, sw) {
				REQUIRE (0 == wcsncmp (sw, both, l));
			}
		}
	}

	SECTION ("schnur_append_string") {
		SCHNUR_SCOPED_EMPTY (s) {
			REQUIRE (NULL != s);

			const schnur_wide_t* a = SCHNUR_W ("Viel");
			const schnur_wide_t* b = SCHNUR_W (" Spaß!");
			const schnur_wide_t* both = SCHNUR_W ("Viel Spaß!");
			const size_t l = wcslen (both);

			schnur_copy_cstr (s, a);

			SCHNUR_SCOPED_EMPTY (s2) {
				REQUIRE (NULL != s2);

				schnur_copy_cstr (s2, b);

				REQUIRE (1 == schnur_append_string (s, s2));
				REQUIRE (l == schnur_length (s));

				SCHNUR_WIDE_SCOPED (s, sw) {
					REQUIRE (0 == wcsncmp (sw, both, l));
				}
			}
		}
	}
}

TEST_CASE ("copy", "[string]") {
	setlocale (LC_ALL, "");

	SECTION ("schnur_copy_cstr") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

		const schnur_wide_t* w = SCHNUR_W ("Hänsel mag Soße!");
		const size_t l =  wcslen (w);

		REQUIRE (wcslen (w) == l);
		schnur_copy_cstr (s, w);

		SCHNUR_WIDE_SCOPED (s, sw) {
			REQUIRE (0 == wcsncmp (sw, w, l));
		}

		schnur_free (s);
	}

	SECTION ("schnur_copy") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

		const schnur_wide_t* w = SCHNUR_W ("ευχαριστημένος");
		const size_t l = wcslen (w);

		REQUIRE (wcslen (w) == l);
		schnur_copy_cstr (s, w);

		SCHNUR_WIDE_SCOPED (s, sw) {
			REQUIRE (0 == wcsncmp (sw, w, l));

			schnur_t* o = schnur_new ();
			REQUIRE (NULL != o);

			schnur_copy (o, s);
			SCHNUR_WIDE_SCOPED (o, ow) {
				REQUIRE (0 == wcsncmp (sw, ow, l));
			}

			schnur_free (o);
		}

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

		SCHNUR_WIDE_SCOPED (s, sw) {
			REQUIRE (NULL != sw);
		}
		REQUIRE ((2 * SCHNUR_BLOCK_SIZE) == schnur_capacity (s));

		schnur_free (s);
	}

	SECTION ("schnur_compact") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);
		REQUIRE (NULL != schnur_raw (s));
		schnur_expand (s);
		REQUIRE (NULL != schnur_raw (s));
		schnur_expand (s); // 3 * SCHNUR_BLOCK_SIZE
		REQUIRE (NULL != schnur_raw (s));
		REQUIRE ((3 * SCHNUR_BLOCK_SIZE) == schnur_capacity (s));

		REQUIRE (1 == schnur_fill (s, SCHNUR_W ('X')));

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

		const schnur_wide_t* w = SCHNUR_W ("舒适");

		schnur_copy_cstr (s, w);

		REQUIRE (1 == schnur_equal_cstr (s, w));
		schnur_free (s);
	}

	SECTION ("schnur_equal") {
		schnur_t* s = NULL;
		s = schnur_new ();
		REQUIRE (NULL != s);

		const schnur_wide_t* w = SCHNUR_W ("décontracté");
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

		schnur_t* w = schnur_new_s (SCHNUR_W ("Hänsel mag Soße!"));

		REQUIRE (SCHNUR_W ('ä') == schnur_get (w, 1));

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

		const schnur_wide_t* w = SCHNUR_W ("солнце");
		const size_t l = wcslen (w);

		schnur_copy_cstr (s, w);

		#if 1 == SHOW_DEBUG
		SCHNUR_WIDE_SCOPED (s, sw) {
			wprintf (L"d: %ls\n", sw);
		}
		#endif

		REQUIRE (1 == schnur_reverse (s));

		SCHNUR_WIDE_SCOPED (s, sw) {
			#if 1 == SHOW_DEBUG
			wprintf (SCHNUR_W ("d: %ls\n"), sw);
			#endif

			for (size_t i = 0; i < l; ++i) {
				schnur_wide_t c = sw[i];
				REQUIRE (c == w[l - i - 1]);
			}
		}

		schnur_free (s);
	}
}

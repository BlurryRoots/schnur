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

/*
	Creates a new string_t instance.

	@returns: Pointer to new string_t instance.
*/
string_t*
string_new (void) {
	string_t* s;

	s = malloc (sizeof (string_t));
	if (NULL == s) {
		return 0;
	}

	s->data = calloc (STRING_BLOCK_SIZE, sizeof (wchar_t));
	if (NULL == s->data) {
		free (s);
		s = NULL;
	}
	else {
		s->capacity = STRING_BLOCK_SIZE;
		s->length = 0;
	}

	return s;
}

/*
	Frees given string_t object.
*/
void
string_free (string_t* self) {
	if (NULL == self) {
		return;
	}

	if (NULL != self->data) {
		free (self->data);
	}

	free (self);
}

/*
	Raw implementation for string filling.
*/
void
raw_string_fill_n (string_t* self, wchar_t c, size_t n) {
	size_t i;

	if (0 == n) {
		return;
	}

	for (i = 0; i < n; ++i) {
		self->data[i] = c;
	}
	self->data[n - 1] = '\0';
}

/*
	Fills string with given character.
	This fills the entire string (full capacity) and not
	only the used length.

	@returns: 1 on success, 0 otherwise.
*/
int
string_fill (string_t* self, wchar_t c) {
	if (NULL == self) {
		return 0;
	}

	raw_string_fill_n (self, c, self->capacity);

	return 1;
}

/*
	Fills n places of string with given character.

	@returns: 1 on success, 0 otherwise.
*/
int
string_fill_n (string_t* self, wchar_t c, size_t n)
{
	if (NULL == self
	 || n > self->capacity) {
		return 0;
	}

	raw_string_fill_n (self, c, n);

	return 1;
}

/*
	Expands the capacity of string according to STRING_BLOCK_SIZE.

	@returns: 1 on success, 0 otherwise.
*/
int
string_expand (string_t* self) {
	wchar_t* buffer;

	if (NULL == self) {
		return 0;
	}

	buffer = calloc (
		self->capacity + STRING_BLOCK_SIZE,
		sizeof (wchar_t)
	);
	if (NULL == buffer) {
		return 0;
	}

	wcsncpy (buffer, self->data, self->length);
	buffer[self->length] = L'\0';

	free (self->data);

	self->data = buffer;
	self->capacity += STRING_BLOCK_SIZE;

	return 1;
}

/*
	Compacts string capacity to minimize allocated space, while
	still able to hold all used places (length).

	@returns: 1 on success, 0 otherwise.
*/
int
string_compact (string_t* self) {
	size_t diff, memoff;
	wchar_t* buffer;

	if (NULL == self) {
		return 0;
	}

	diff = self->capacity - (self->length + 1); // \0

	memoff = diff % STRING_BLOCK_SIZE;
	if (memoff > 0) {
		buffer = calloc (
			self->length + 1 + memoff, // len + \0 + offset
			sizeof (wchar_t)
		);
		if (NULL == buffer) {
			return 0;
		}

		wcsncpy (buffer, self->data, self->length + 1 + memoff);

		free (self->data);
		self->data = buffer;

		self->capacity = self->length + 1 + memoff;
	}

	return 1;
}

/*
	Copies the actually used data (length) of other to self.
	If other is longer than the capacity of self, self is
	expanded.

	@returns: 1 on success, 0 otherwise.
*/
int
string_copy (string_t* self, const string_t* other) {
	if (NULL == self || NULL == other) {
		return 0;
	}

	while (self->capacity <= other->length) {
		if (! string_expand (self)) {
			return 0;
		}
	}

	wcsncpy (self->data, other->data, other->length);
	self->length = other->length;
	self->data[self->length] = L'\0';

	return 1;
}

/*
	Copies given character array to self.
	If array is longer than the capacity of self, self is
	expanded.

	@returns: 1 on success, 0 otherwise.
*/
int
string_copy_cstr (string_t* self, const wchar_t* other) {
	size_t ol;

	if (NULL == self || NULL == other) {
		return 0;
	}

	ol = wcslen (other);

	while (self->capacity <= ol) {
		if (! string_expand (self)) {
			return 0;
		}
	}

	wcsncpy (self->data, other, ol);
	self->length = ol;
	self->data[self->length] = L'\0';

	return 1;
}

/*
	Appends given character. Capacity of string might get
	expanded to hold the additional character.

	@returns: 1 on success, 0 otherwise.
*/
int
string_append (string_t* self, wchar_t c) {
	if (NULL == self) {
		return 0;
	}

	if ((self->length + 1) >= self->capacity) {
		if (! string_expand (self)) {
			return 0;
		}
	}

	self->data[self->length++] = c;
	self->data[self->length] = '\0';

	return 1;
}

/*
	Appends given character array to string.
	@returns: 1 on success, 0 otherwise.
*/
int
string_append_cstr (string_t* self, const wchar_t* other) {
	size_t len, i;

	if (NULL == self
	 || NULL == other) {
		return 0;
	}

	len = wcslen (other);
	if (0 == len) {
		return 1;
	}

	while (self->capacity < (self->length + len + 1)) {
		string_expand (self);
	}

	for (i = 0; i < len; ++i) {
		self->data[self->length + i] = other[i];
	}
	self->data[self->length + len] = '\0';

	return 1;
}

/*
	Appends given string to string.
	@returns: 1 on success, 0 otherwise.
*/
int
string_append_string (string_t* self, const string_t* other) {
	size_t i;

	if (NULL == self
	 || NULL == other) {
		return 0;
	}

	while (self->capacity < (self->length + other->length + 1)) {
		string_expand (self);
	}

	for (i = 0; i < other->length; ++i) {
		self->data[self->length + i] = other->data[i];
	}

	return 1;
}

/*
	Check if used data of self equals given character array.

	@returns: 1 on equality, 0 otherwise.
*/
int
string_equal_cstr (const string_t* self, const wchar_t* other) {
	size_t ol;

	if (NULL == self
	 || NULL == other) {
		return 0;
	}

	ol = wcslen (other);

	return self->length == ol
		&& 0 == wcsncmp (self->data, other, ol);
}

/*
	Check if used data of self equals used data of other.

	@returns: 1 on equality, 0 otherwise.
*/
int
string_equal (const string_t* self, const string_t* other) {
	if (NULL == self
	 || NULL == other) {
		return 0;
	}

	return self->length == other->length
		&& 0 == wcsncmp (self->data, other->data, self->length);
}

/*
	Reverses actually used data of string in place.

	@returns: 1 on success, 0 when given a nullpointer.
*/
int
string_reverse (string_t* self) {
	wchar_t buffer;
	int mid, n, i;

	if (NULL == self) {
		return 0;
	}

	if (0 == self->length) {
		return 1;
	}

	i = 0;
	n = self->length - 1;
	mid = self->length / 2;
	while (i < mid) {
		buffer        = self->data[i];
		self->data[i] = self->data[n];
		self->data[n] = buffer;

		--n;
		++i;
	}

	return 1;
}

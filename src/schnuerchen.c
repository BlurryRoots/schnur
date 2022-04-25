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


#include <stdlib.h>

#include <schnuerchen.h>

schnur_t*
schnur_new (void) {
	schnur_t* s;

	s = malloc (sizeof (schnur_t));
	if (NULL == s) {
		return 0;
	}

	s->data = calloc (SCHNUR_BLOCK_SIZE, sizeof (wchar_t));
	if (NULL == s->data) {
		free (s);
		s = NULL;
	}
	else {
		s->capacity = SCHNUR_BLOCK_SIZE;
		s->length = 0;
	}

	return s;
}

void
schnur_free (schnur_t* self) {
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
__schnur_fill_n (schnur_t* self, wchar_t c, size_t n) {
	size_t i;

	if (0 == n) {
		return;
	}

	for (i = 0; i < n; ++i) {
		self->data[i] = c;
	}
	self->data[n - 1] = '\0';
}

int
schnur_fill (schnur_t* self, wchar_t c) {
	if (NULL == self) {
		return 0;
	}

	__schnur_fill_n (self, c, self->capacity);

	return 1;
}

int
schnur_fill_n (schnur_t* self, wchar_t c, size_t n)
{
	if (NULL == self
	 || n > self->capacity) {
		return 0;
	}

	__schnur_fill_n (self, c, n);

	return 1;
}

int
schnur_expand (schnur_t* self) {
	wchar_t* buffer;

	if (NULL == self) {
		return 0;
	}

	buffer = calloc (
		self->capacity + SCHNUR_BLOCK_SIZE,
		sizeof (wchar_t)
	);
	if (NULL == buffer) {
		return 0;
	}

	wcsncpy (buffer, self->data, self->length);
	buffer[self->length] = L'\0';

	free (self->data);

	self->data = buffer;
	self->capacity += SCHNUR_BLOCK_SIZE;

	return 1;
}

int
schnur_compact (schnur_t* self) {
	size_t diff, memoff;
	wchar_t* buffer;

	if (NULL == self) {
		return 0;
	}

	diff = self->capacity - (self->length + 1); // \0

	memoff = diff % SCHNUR_BLOCK_SIZE;
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

int
schnur_copy (schnur_t* self, const schnur_t* other) {
	if (NULL == self || NULL == other) {
		return 0;
	}

	while (self->capacity <= other->length) {
		if (! schnur_expand (self)) {
			return 0;
		}
	}

	wcsncpy (self->data, other->data, other->length);
	self->length = other->length;
	self->data[self->length] = L'\0';

	return 1;
}

int
schnur_copy_cstr (schnur_t* self, const wchar_t* other) {
	size_t ol;

	if (NULL == self || NULL == other) {
		return 0;
	}

	ol = wcslen (other);

	while (self->capacity <= ol) {
		if (! schnur_expand (self)) {
			return 0;
		}
	}

	wcsncpy (self->data, other, ol);
	self->length = ol;
	self->data[self->length] = L'\0';

	return 1;
}

int
schnur_append (schnur_t* self, wchar_t c) {
	if (NULL == self) {
		return 0;
	}

	if ((self->length + 1) >= self->capacity) {
		if (! schnur_expand (self)) {
			return 0;
		}
	}

	self->data[self->length++] = c;
	self->data[self->length] = '\0';

	return 1;
}

int
schnur_append_cstr (schnur_t* self, const wchar_t* other) {
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
		schnur_expand (self);
	}

	for (i = 0; i < len; ++i) {
		self->data[self->length + i] = other[i];
	}
	self->data[self->length + len] = '\0';

	return 1;
}

int
schnur_append_string (schnur_t* self, const schnur_t* other) {
	size_t i;

	if (NULL == self
	 || NULL == other) {
		return 0;
	}

	while (self->capacity < (self->length + other->length + 1)) {
		schnur_expand (self);
	}

	for (i = 0; i < other->length; ++i) {
		self->data[self->length + i] = other->data[i];
	}

	return 1;
}

int
schnur_equal_cstr (const schnur_t* self, const wchar_t* other) {
	size_t ol;

	if (NULL == self
	 || NULL == other) {
		return 0;
	}

	ol = wcslen (other);

	return self->length == ol
		&& 0 == wcsncmp (self->data, other, ol);
}

int
schnur_equal (const schnur_t* self, const schnur_t* other) {
	if (NULL == self
	 || NULL == other) {
		return 0;
	}

	return self->length == other->length
		&& 0 == wcsncmp (self->data, other->data, self->length);
}

int
schnur_reverse (schnur_t* self) {
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

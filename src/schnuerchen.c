// Copyright (c) 2013 - ∞ Sven Freiberg. All rights reserved.
// See license.md for details.

#include <stdlib.h>

#include <schnuerchen.h>

/**
	@brief: Represents a string of characters.
*/
struct schnur {
	/**
		@brief: Character array containing all data used by this string object.
	*/
	wchar_t* data;

	/**
		@brief: Number of characters used. In other words the position of the
				  null terminator (\0).
	*/
	size_t length;

	/**
		@brief: Maximum number of characters this string can hold.
	*/
	size_t capacity;
};
/**
 * @brief Convenience typedef for struct schnur.
 * @see struct schnur
 */
typedef
	struct schnur
	schnur_t;

struct schnur*
schnur_new (void) {
	struct schnur* s;

	s = malloc (sizeof (struct schnur));
	if (NULL == s) {
		return NULL;
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

struct schnur*
schnur_new_s (const wchar_t* str) {
	struct schnur* s = schnur_new ();
	if (NULL == s) return NULL;

	if (0 == schnur_copy_cstr (s, str)) {
		schnur_free (s);
		return NULL;
	}

	return s;
}

void
schnur_free (struct schnur* self) {
	if (NULL == self) {
		return;
	}

	if (NULL != self->data) {
		free (self->data);
	}

	free (self);
}

wchar_t
schnur_get (const struct schnur* self, size_t i) {
	if (NULL == self) return SCHNUR_NULL;
	if (i >= self->length) return SCHNUR_NULL;

	return self->data[i];
}

int
schnur_set (struct schnur* self, size_t i, wchar_t c) {
	if (NULL == self) return 0;
	if (i >= self->length) return 0;

	self->data[i] = c;

	return 1;
}

void*
schnur_data(const struct schnur* self) {
	if (NULL == self) {
		return NULL;
	}

	return self->data;
}

size_t
schnur_data_size(const struct schnur* self) {
	if (NULL == self) {
		return 0;
	}

	if (0 < self->capacity) {
		return sizeof (self->data[0]) * self->capacity;
	}

	return 0;
}

size_t
schnur_length(const struct schnur* self) {
	if (NULL == self) {
		return 0;
	}

	return self->length;
}

size_t
schnur_capacity(const struct schnur* self) {
	if (NULL == self) {
		return 0;
	}

	return self->capacity;
}

int
schnur_terminate (struct schnur* self, size_t i) {
	if (NULL == self) {
		return 0;
	}

	if (self->capacity <= i) {
		return 0;
	}

	self->length = i + 1;
	self->data[i] = L'\0';

	return 1;
}

/*
	Raw implementation for string filling.
*/
int
__schnur_fill_n (struct schnur* self, wchar_t c, size_t n) {
	size_t i;

	if (0 == n) {
		return 0;
	}

	for (i = 0; i < n; ++i) {
		self->data[i] = c;
	}
	self->data[n - 1] = L'\0';

	self->length = n;

	return 1;
}

int
schnur_fill (struct schnur* self, wchar_t c) {
	if (NULL == self) {
		return 0;
	}

	return __schnur_fill_n (self, c, self->capacity);
}

int
schnur_fill_n (struct schnur* self, wchar_t c, size_t n) {
	if (NULL == self
	 || n > self->capacity) {
		return 0;
	}

	return __schnur_fill_n (self, c, n);
}

int
schnur_expand (struct schnur* self) {
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
schnur_compact (struct schnur* self) {
	size_t diff, memoff;
	wchar_t* buffer;

	if (NULL == self) {
		return 0;
	}

	diff = self->capacity - (self->length + 1); // \0

	memoff = diff % SCHNUR_BLOCK_SIZE;
	if (memoff > 0) {
		buffer = realloc (self->data,
			sizeof (wchar_t) * (self->length + 1 + memoff) // len + \0 + offset
		);
		if (NULL == buffer) {
			return 0;
		}

		//wcsncpy (buffer, self->data, self->length + 1 + memoff);

		//free (self->data);
		self->data = buffer;

		self->capacity = self->length + 1 + memoff;

		return 1;
	}

	return 0;
}

int
schnur_copy (struct schnur* self, const struct schnur* other) {
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
schnur_copy_cstr (struct schnur* self, const wchar_t* other) {
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
schnur_append (struct schnur* self, wchar_t c) {
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
schnur_append_cstr (struct schnur* self, const wchar_t* other) {
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
schnur_append_string (struct schnur* self, const struct schnur* other) {
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
schnur_equal_cstr (const struct schnur* self, const wchar_t* other) {
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
schnur_equal (const struct schnur* self, const struct schnur* other) {
	if (NULL == self
	 || NULL == other) {
		return 0;
	}

	return self->length == other->length
		&& 0 == wcsncmp (self->data, other->data, self->length);
}

int
schnur_reverse (struct schnur* self) {
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

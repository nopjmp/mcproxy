#include "bytebuf.h"

#include <stdlib.h>
#include <memory.h>

#include <arpa/inet.h>

#define BUFCHECK(self, size)			\
    if ((self->pos + size) > self->len) return 0

#define BUFACCESS(self) self->buf[self->pos++]

bytebuf *bytebuf_new(size_t len)
{
    bytebuf *b = malloc(sizeof(bytebuf));
    b->buf = malloc(len * sizeof(uint8_t));
    b->len = len;
    b->pos = 0;
    return b;
}

void bytebuf_free(bytebuf * self)
{
    free(self->buf);
    free(self);
}

int bytebuf_resize(bytebuf * self, size_t len)
{
    uint8_t* res = realloc(self->buf, len);
    if (res == NULL) return -1;
    self->buf = res;
    self->buf[len - 1] = 0;
    self->len = len;
    return 1;
}

int bytebuf_read_varint(bytebuf *self, int32_t *value)
{
    int i = 0;
    uint8_t b = 0;
    *value = 0;
    do {
	BUFCHECK(self, 1);
	uint8_t b = BUFACCESS(self);
	*value |= (b & 0x7F) << 7 * i;
	i++;
    } while ((b & 0x80) != 0 && i < 5);
    return 1;
}

int bytebuf_write_varint(bytebuf *self, int32_t value)
{
    uint8_t b;
    do {
	BUFCHECK(self, 1);
	b = (uint8_t)(value & 0x7F);
	value >>= 7;
	if (value != 0) {
	    b |= 0x80;
	}
	BUFACCESS(self) = b;
    } while( value != 0 );
    return 1;
}

int bytebuf_read_varlong(bytebuf *self, int64_t *value)
{
    int i = 0;
    uint8_t b = 0;
    *value = 0;
    do {
	BUFCHECK(self, 1);
	uint8_t b = BUFACCESS(self);
	*value |= (b & 0x7F) << 7 * i;
	i++;
    } while ((b & 0x80) != 0 && i < 10);
    return 1;
}

int bytebuf_write_varlong(bytebuf *self, int64_t value)
{
    uint8_t b;
    do {
	BUFCHECK(self, 1);
	b = (uint8_t)(value & 0x7F);
	value >>= 7;
	if (value != 0) {
	    b |= 0x80;
	}
	BUFACCESS(self) = b;
    } while( value != 0 );
    return 1;
}

int bytebuf_read_byte(bytebuf *self, uint8_t *value)
{
    BUFCHECK(self, 1);
    *value = BUFACCESS(self);
    return 1;
}

int bytebuf_write_byte(bytebuf *self, uint8_t value)
{
    BUFCHECK(self, 1);
    BUFACCESS(self) = value;
    return 1;
}

int bytebuf_read_short(bytebuf *self, int16_t *value)
{
    BUFCHECK(self, 2);
    memcpy(value, self->buf, sizeof(int16_t));
    self->pos += 2;
    *value = ntohs(*value);
    return 1;
}

int bytebuf_write_short(bytebuf *self, int16_t value)
{
    BUFCHECK(self, 2);
    value = htons(value);
    memcpy(self->buf, &value, sizeof(int16_t));
    self->pos += 2;
    return 1;
}

int bytebuf_read_unsigned_short(bytebuf *self, uint16_t *value)
{
    BUFCHECK(self, 2);
    memcpy(value, self->buf, sizeof(uint16_t));
    self->pos += 2;
    *value = ntohs(*value);
    return 1;
}

int bytebuf_write_unsigned_short(bytebuf *self, uint16_t value)
{
    BUFCHECK(self, 2);
    value = htons(value);
    memcpy(self->buf, &value, sizeof(uint16_t));
    self->pos += 2;
    return 1;
}

int bytebuf_read_int(bytebuf *self, int32_t *value)
{
    BUFCHECK(self, 4);
    memcpy(self->buf, value, sizeof(int32_t));
    self->buf += 4;
    *value = ntohs(*value);
    return 1;
}

int bytebuf_write_int(bytebuf *self, int32_t value)
{
    BUFCHECK(self, 4);
    value = htons(value);
    memcpy(self->buf, &value, sizeof(int32_t));
    self->pos += 4;
    return 1;
}

int bytebuf_read_long(bytebuf *self, int64_t *value)
{
    BUFCHECK(self, 8);
    memcpy(value, self->buf, sizeof(int64_t));
    self->pos += 8;
    *value = ntohs(*value);
    return 1;
}

int bytebuf_write_long(bytebuf *self, int64_t value)
{
    BUFCHECK(self, 8);
    value = htons(value);
    memcpy(self->buf, &value, sizeof(int64_t));
    self->pos += 8;
    return 1;
}

int bytebuf_read_string(bytebuf *self, char ** str)
{
    int32_t length;
    if (!bytebuf_read_varint(self, &length))
	return 0;
    BUFCHECK(self, length);
    *str = realloc(*str, length);
    memcpy(*str, self->buf, length);
    self->pos += length;
    return 1;
}

int bytebuf_write_string(bytebuf *self, char *str, size_t len)
{
    BUFCHECK(self, 1 + len);
    bytebuf_write_varint(self, len);
    memcpy(self->buf, str, len);
    self->pos += len;
    return 1;
}

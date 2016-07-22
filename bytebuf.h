#ifndef __BYTEBUF_H
#define __BYTEBUF_H

#include <stdio.h>
#include <stdint.h>

struct bytebuf_t {
    uint8_t* buf;
    size_t len;
    size_t pos;
};

typedef struct bytebuf_t bytebuf;

bytebuf *bytebuf_new(size_t len);
void bytebuf_free(bytebuf *);

int bytebuf_read_varint(bytebuf *, int32_t *);
int bytebuf_write_varint(bytebuf *, int32_t);

int bytebuf_read_varlong(bytebuf *, int64_t *);
int bytebuf_write_varlong(bytebuf *, int64_t);

int bytebuf_read_byte(bytebuf *, uint8_t *);
int bytebuf_write_byte(bytebuf *, uint8_t);

int bytebuf_read_short(bytebuf *, int16_t *);
int bytebuf_write_short(bytebuf *, int16_t);

int bytebuf_read_unsigned_short(bytebuf *, uint16_t *);
int bytebuf_write_unsigned_short(bytebuf *, uint16_t);

int bytebuf_read_int(bytebuf *, int32_t *);
int bytebuf_write_int(bytebuf *, int32_t);

int bytebuf_read_long(bytebuf *, int64_t *);
int bytebuf_write_long(bytebuf *, int64_t);

int bytebuf_read_string(bytebuf *, char **);
int bytebuf_write_string(bytebuf *, char *, size_t);

#endif

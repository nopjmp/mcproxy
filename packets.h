#ifndef __PACKETS_H
#define __PACKETS_H

#include "bytebuf.h"
#include "stream.h"

enum direction {
    CLIENT,
    SERVER
};

#define PACKET_FUNC_DEF(NAME)				\
    int process_packet_ ## NAME (stream *s, bytebuf *b, enum direction dir)

// initaliztion packet

// 0x00
PACKET_FUNC_DEF(handshake);
// 0xFE
PACKET_FUNC_DEF(legacy_handshake);

#endif

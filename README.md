# mcproxy
A simple Minecraft protocol proxy written in C.

ISC Licensed

## Requirements
libev needs to be installed on the system.
GCC 5 and clang were only tested

## Setup

You need to make sure the minecraft server is not in online mode.

## Notice

All this does is directly pass the packets right now.

Does not make decisions.

## TODO
* Encryption processing
* Compression processing
* Multi-server configuration
* Custom statuses (cumulative player count)
* Support BungeeCord configuration packets

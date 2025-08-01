#ifndef PACK_H
#define PACK_H

#include <stdio.h>
#include <stdint.h>

/*
 * Bytes to uint8_t.
 */
uint8_t unpack_u8(const uint8_t **);

/*
 * Bytes to uint16_t.
 */
uint16_t unpack_u16(const uint8_t **);

/*
 * Bytes to uint32_t.
 */
uint32_t unpack_u32(const uint8_t **);

uint8_t *unpack_bytes(const uint8_t **, size_t, uint8_t *);

/*
 * Unpack a string prefixed by its length as a uint16_t.
 */
uint16_t unpack_string16(uint8_t **buf, uint8_t **dest);

/*
 * Append a uint8_t onto the byte string.
 */
void pack_u8(uint8_t **, uint8_t);

/*
 * Append a uint16_t onto the byte string.
 */
void pack_u16(uint8_t **, uint16_t);

/*
 * Append a uint32_t onto the byte string.
 */
void pack_u32(uint8_t **, uint32_t);

/*
 * Append bytes to the byte string.
 */
void pack_bytes(uint8_t **, uint8_t *);

#endif
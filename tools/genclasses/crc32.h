/***************************************************************************
 GenClasses - MUI class dispatcher generator
 Copyright (C) 2001 by Andrew Bell <mechanismx@lineone.net>
***************************************************************************/

#ifndef CRC32_H
#define CRC32_H

unsigned long crc32(const void *mem, long size, unsigned long crc);

#define INVALID_CRC    0xdeadbeef

#endif /* CRC32_H */

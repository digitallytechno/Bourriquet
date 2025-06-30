/***************************************************************************
Bourriquet MorphOS
Copyright (C) 2024-2025 Digitally

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
***************************************************************************/
#ifndef  SERVERMET_H
#define SERVERMET_H

#include <exec/types.h>
#include "Stringsizes.h"

#define OP_EMULEINFO         0x01
#define OP_OFFERFILES        0x15

#define ST_SERVERNAME        0x01
#define ST_DESCRIPTION       0x0B
#define ST_PING              0x0C
#define ST_FAIL              0x0D
#define ST_PREFERENCE        0x0E
#define ST_PORT              0x0F
#define ST_IP                0x10
#define ST_DYNIP             0x85
#define ST_MAXUSERS          0x87
#define ST_SOFTFILES	     0x88    // <uint32>
#define ST_HARDFILES	     0x89    // <uint32>
#define ST_LASTPING	     0x90    // <uint32>
#define ST_VERSION	     0x91    //
#define ST_UDPFLAGS	     0x92    // <uint32>
#define ST_AUXPORTSLIST	     0x93    // <string>
#define ST_LOWIDUSERS	     0x94    // <uint32>
#define ST_UDPKEY	     0x95    // <uint32>
#define ST_UDPKEYIP	     0x96    // <uint32>
#define ST_TCPPORTOBFUSCATION 0x97    // <uint16>
#define ST_UDPPORTOBFUSCATION 0x98

#define FT_FILENAME          0x01
#define FT_FILESIZE          0x02
#define FT_FILETYPE          0x03
#define FT_FILEFORMAT        0x04
#define FT_LASTSEENCOMPLETE  0x05
#define FT_TRANSFERED        0x08
#define FT_GAPSTART          0x09
#define FT_GAPEND            0x0A
#define FT_PARTFILENAME      0x12
#define FT_OLDDLPRIORITY     0x13
#define FT_STATUS            0x14
#define FT_SOURCES           0x15
#define FT_PERMISSIONS       0x16
#define FT_COMPLETE_SOURCES  0x30

#define CT_NAME              0x01
#define CT_VERSION           0x11
#define CT_PORT              0x0f
#define CT_FRIENDSHARING     0x66

LONG ParseServerMetFile(const char *smf);
int parse_opcodes(unsigned char *buf,  struct Server *sv);
void servers_cleanup(void);

// une macro pour construire la phrase nécessaire pour utiliser une image dans un objet NList
#define SI_STR(id)  SI_STR2(id)
#define SI_STR2(id) "\033o[" #id "]"

#endif /* SERVERMET_H */


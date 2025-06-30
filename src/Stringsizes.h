/***************************************************************************
Bourriquet MorphOS
 Copyright (C) 2024-2025 digitally
 
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
#ifndef STRINGSIZES_H
#define STRINGSIZES_H

#define SIZE_USERID       60
#define SIZE_PASSWORD     80
#define SIZE_ADDRESS     150
#define SIZE_HOST         80
#define SIZE_REALNAME     40
#define SIZE_SUBJECT     200
#define SIZE_MSGID       999 
#define SIZE_MFILE        30
#define SIZE_COMMAND     120
#define SIZE_CTYPE        40
#define SIZE_NAME         30
#define SIZE_PATH        512 // longueur maximum possible pour le chemin d'un répertoire (512 devrait être enough)
#define SIZE_FILE        108 // longueur maximum possible du nom de fichier (107+1)
#define SIZE_SMALL        16
#define SIZE_DEFAULT      80
#define SIZE_LARGE       512
#define SIZE_LINE       1001 
#define SIZE_RCPTS      4096
#define SIZE_INTRO       200
#define SIZE_PATTERN     160
#define SIZE_PATHFILE    (SIZE_PATH+SIZE_FILE)
#define SIZE_URL         (SIZE_HOST+SIZE_PATHFILE)
#define SIZE_EXALLBUF  32768
#define SIZE_FILEBUF   65536 // la taille du buffer pour notre fopen() 
#define SIZE_STACK     65536 // taille de la pile pour la tâche principale et les threads
#define SIZE_DSTRCHUNK  1024 // doit être une puissance de 2

#define MAXSERVERS          50
#define MAXERR            50

#endif 

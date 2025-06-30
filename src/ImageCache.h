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
#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <exec/nodes.h>
#include <mui/TheBar_mcc.h>
#include "HashTable.h"

struct Screen;
struct BitMap;

// D�finition de l'imageCacheNode lequel contient
// toutes les informations du fichier image � charger, 
// incluant l'objet dataype de l'image charg�
struct ImageCacheNode
{
 	struct HashEntryHeader hash; // Ent�te standard de la table de hachage
  	char *id;                    // pointeur de l'id
  	char *filename;              // pointeur du nom de fichier
  	Object *dt_obj;              // l'objet datatype
  	struct Screen *screen;       // pointeur de l�cran de l'image
  	int openCount;               // compte combien de fois l'image est ouverte/utilis�e
  	APTR pixelArray;             // pointeur de lecture de la matrice de pixels par PDTM_READPIXELARRAY
  	ULONG pixelFormat;           // le format de pixel de la matrice(PBPAFMT_ARGB)
  	struct BitMap *bitmap;
  	PLANEPTR mask;
  	ULONG width;
  	ULONG height;
  	ULONG depth;
  	ULONG bytesPerPixel;
  	ULONG bytesPerRow;
  	UBYTE masking;
  	BOOL initialLayout;          // avons nous appel� DTM_PROCLAYOUT d�j� avant ?
  	BOOL delayedDispose;         // voulons-nous supprimer l'image � partir du cache si openCount atteint z�ro ?
};

// Fonctions publiques
BOOL ImageCacheSetup(void);
void ImageCacheCleanup(void);
struct ImageCacheNode *ObtainImage(const char *id, const char *filename, const struct Screen *scr);
void ReleaseImage(const char *id, BOOL dispose);
BOOL IsImageInCache(const char *id);

#endif /* IMAGECACHE_H */

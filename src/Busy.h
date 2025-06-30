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
#include <exec/nodes.h>

struct BusyNode
{
 	struct MinNode node;         // à lier dans une MinList Exec
  	ULONG type;                  // busy type, voir plus bas 
	ULONG progressCurrent;       // progression actuelle (seulement pour la barre de progression)
  	ULONG progressMax;           // progression maximum (seulement pour la barre de progression)
  	char infoText[SIZE_DEFAULT];  // texte d'information à montrer
  	BOOL wasVisible;             // cette action était-elle visible déjà avant ?
};

#define BUSY_TEXT             1 // un simple texte d'information seulement, en usage normal 
#define BUSY_PROGRESS         2 // une barre avec une gauge de progression, usage normal
#define BUSY_PROGRESS_ABORT   3 // une barre avec une gauge de progression et un bouton d'annulation, usage normal

struct BusyNode *BusyBegin(ULONG type);
void BusyText(struct BusyNode *busy, const char *text, const char *param);
BOOL BusyProgress(struct BusyNode *busy, int progress, int max);
void BusyEnd(struct BusyNode *busy);
void BusyCleanup(void);

/* BUSY_H */

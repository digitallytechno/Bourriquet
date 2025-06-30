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
#ifndef THEMES_H
#define THEMES_H

#include <exec/types.h>
#include "Stringsizes.h"

// images de la configuration 
#define CI_FIRST              			0
#define CI_FIRSTSTEP          		0
#define CI_FIRSTSTEPBIG       		1
#define CI_LOOKFEEL           		2
#define CI_LOOKFEELBIG        		3
#define CI_NETWORK           		4
#define CI_NETWORKBIG        		5
#define CI_UPDATE            			6
#define CI_UPDATEBIG         		7
#define CI_IDENTITIES        			8
#define CI_IDENTITIESBIG     		9
#define CI_TRANSFERT        		10
#define CI_TRANSFERTBIG    		11
#define CI_MAX               			12

// images 
#define SI_FIRST              					0
#define SI_SERVERLIST             			0
#define SI_NEWSERVER             	 	1
#define SI_CONNECT							2
#define SI_CONNECT_CANCEL		3
#define SI_DISCONNECT					4
#define SI_SERVER								5
#define SI_SERVER_BIG						6
#define SI_TRANSFERT						7
#define SI_TRANSFERT_BIG				8
#define SI_RESEARCH						9
#define SI_RESEARCH_BIG				10
#define SI_SHARED							11
#define SI_SHARED_BIG					12
#define SI_PREFS								13
#define SI_PREFS_BIG						14
#define SI_CONNECTEDNOTNOT 	15
#define SI_INFO									16
#define SI_LOG									17
#define SI_SERVERINFO						18
#define SI_SERVERSUPDATE				19
#define SI_UPDOWN							20
#define SI_USER									21
#define SI_STATUS								22
#define SI_SEARCHPARAMS				23
#define SI_SEARCHRESULTS				24
#define SI_UPLOAD							25
#define SI_DOWNLOAD 					26
#define SI_MAX               					27

struct Theme
{
 	char *name;
 	char *author;
 	char *url;
 	char *version;
 	char directory[SIZE_PATHFILE];
 	BOOL loaded;
	char *configImages[CI_MAX];
	char *itemImages[SI_MAX];
};

void AllocTheme(struct Theme *theme, const char *themeName);
void FreeTheme(struct Theme *theme);
LONG ParseThemeFile(const char *themeFile, struct Theme *theme);
void LoadTheme(struct Theme *theme, const char *themeName);
void UnloadTheme(struct Theme *theme);

// une macro pour construire la phrase nécessaire pour utiliser une image dans un objet NList
#define SI_STR(id)  SI_STR2(id)
#define SI_STR2(id) "\033o[" #id "]"

#endif /* THEMES_H */


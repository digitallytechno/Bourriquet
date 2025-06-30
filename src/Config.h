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
#ifndef CONFIG_H
#define CONFIG_H

#include <exec/lists.h>
#include <libraries/mui.h>
#include "Stringsizes.h"

struct Config
{
 	int version; // la version du fichier de configuration chargé
  	struct MinList DownloadList;   // liste des fichiers du répertoire de fichiers téléchargés
  	struct MinList TempList;   // liste des fichiers temporaires
  	struct MinList ShareList;       // liste des fichiers à partager de disponible
 	char UserName[SIZE_REALNAME];
    int ClientPort;
	BOOL ConfigIsSaved;
    char DownloadDir[SIZE_PATH];  // Répertoire des fichiers téléchargés (download)
    char TempDir[SIZE_PATH];  // Répertoire des fichiers temporaires (temp)
    char ShareDir[SIZE_PATH]; // Répertoire des fichiers à partager (upload)
	char ThemeName[SIZE_FILE];
};

extern struct Config *C;
extern struct Config *CE;

struct Config *AllocConfig(void);
void FreeConfig(struct Config *co);
void ClearConfig(struct Config *co);
int LoadConfig(struct Config *co, const char *fname);
BOOL SaveConfig(struct Config *co, const char *fname);
BOOL IsValidConfig(const struct Config *co);
void ValidateConfig(struct Config *co, BOOL update, BOOL saveChanges);

#endif /* CONFIG_H */

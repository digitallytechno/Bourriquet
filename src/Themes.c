/***************************************************************************
Bourriquet MorphOS
Copyright (C) 2022 Digitally
 
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
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <proto/dos.h>
#include <proto/icon.h>

#include "Extrasrc.h"

#include "Bourriquet.h"
#include "Global.h"
#include "Locale.h"
#include "ImageCache.h"
#include "Themes.h"
#include "Debug.h"

#define THEME_VERSION 1

/// status image IDs
static const char * const itemImageIDs[SI_MAX] =
	{
  		"serverlist",
  		"newserver",
		"connect",
		"connect_cancel",
		"disconnect",
		"server",
		"server_big",
		"transfert",
		"transfert_big",
		"research",
		"research_big",
		"shared",
		"shared_big",
		"prefs",
		"prefs_big",
		"connectednotnot",
		"info",
		"log",
		"serverinfo",
		"serverupdate",
		"updown", 
		"user",
		"status",
		"searchparams",
		"searchresults",
		"upload",
		"download"
	};
	
static const char * const configImageIDs[CI_MAX] =
	{
  		"config_firststep", "config_firststep_big",
  		"config_lookfeel",  "config_lookfeel_big",
  		"config_network",   "config_network_big",
  		"config_update",    "config_update_big",
		"config_identities", "config_identities_big",
  		"config_transfert", "config_transfert_big"
	};

// fonctions publiques

/// AllocTheme
// alloue tout le nécessaire pour un thème tout en mettant au départ
// à la valeur par défaut.
void AllocTheme(struct Theme *theme, const char *themeName)
	{
  		int i;
		//int j;
  		char dirname[SIZE_PATHFILE];
  		char filepath[SIZE_PATHFILE];

  		ENTER();
  		theme->name = NULL;
  		theme->author = NULL;
  		theme->version = NULL;
  		AddPath(theme->directory, G->ThemesDir, themeName, sizeof(theme->directory));
  		D(DBF_THEME, "theme directory: %s G->ThemesDir %s, themeName %s", theme->directory, G->ThemesDir, themeName);
		// construction des chemins des images de la configuration
		AddPath(dirname, theme->directory, "config", sizeof(dirname));
  		for(i=CI_FIRST; i < CI_MAX; i++)
  			{
    			AddPath(filepath, dirname, configImageIDs[i], sizeof(filepath));
    			theme->configImages[i] = strdup(filepath);
  			}
		// construction des chemins du status des images
  		AddPath(dirname, theme->directory, "images", sizeof(dirname));
  		for(i=SI_FIRST; i < SI_MAX; i++)
  			{
    			AddPath(filepath, dirname, itemImageIDs[i], sizeof(filepath));
    			theme->itemImages[i] = strdup(filepath);
  			}
  		theme->loaded = FALSE;
  		LEAVE();
	}
///
/// ParseThemeFile
// Analyse un fichier de thème complet et retourne > 0 dans le cas de succès
LONG ParseThemeFile(const char *themeFile, struct Theme *theme)
	{
  		LONG result = 0; // signale une erreur
  		FILE *fh;

  		ENTER();
  		if((fh = fopen(themeFile, "r")) != NULL)
  			{
    			char *buf = NULL;
    			size_t buflen = 0;

    			setvbuf(fh, NULL, _IOFBF, SIZE_FILEBUF);
    			if(getline(&buf, &buflen, fh) >= 3 && strnicmp(buf, "EFM", 3) == 0)
    				{
      					int version = atoi(&buf[3]);

      					// vérifie si c'est la bonne version du thème
      					if(version == THEME_VERSION)
      						{
        						while(getline(&buf, &buflen, fh) > 0)
        							{
          								char *p;
          								char *id;
          								char *value;

          								if((p = strpbrk(buf, ";#\r\n")) != NULL) *p = '\0';
          								if((value = strchr(buf, '=')) != NULL)
          									{
            									*value++ = '\0';
            									value = Trim(value);
          									}
          								else
          									{
            									// suppose un fichier vide
            									value = (char *)"";
         									}
          								id = Trim(buf);
          								if(id[0] != '\0')
          									{
												int i;
												//int j;
												BOOL found = FALSE;

												// description du thème
												if(stricmp(id, "Name") == 0)
													{
												  		free(theme->name);
												  		theme->name = strdup(value);
												  		found = TRUE;
													}
												else if(stricmp(id, "Author") == 0)
													{
												  		free(theme->author);
												  		theme->author = strdup(value);
												  		found = TRUE;
													}
												else if(stricmp(id, "URL") == 0)
													{
												  		free(theme->url);
												  		theme->url = strdup(value);
												  		found = TRUE;
													}
												else if(stricmp(id, "Version") == 0)
													{
												  		free(theme->version);
												  		theme->version = strdup(value);
												  		found = TRUE;
													}
												else if(stricmp(id, "IgnoreMissingImages") == 0)
													{
												  		G->NoImageWarning = Txt2Bool(value);
												  		found = TRUE;
													}
            									else
            										{
              											char *image;

													  	if(strchr(value, ':') == NULL)
													  		{
																// Nom du fichier image est relatif au répertoire du thème
																if(asprintf(&image, "%s/%s", theme->directory, value) == -1) image = NULL;
														  		D(DBF_THEME, "valeur %s", value);
													  		}
													  	else
													  		{
																// laisser le nom tel qu'il est 
																image = value;
													  		}
													  	if(image != NULL)
													  		{
																// images config
																for(i=CI_FIRST; i < CI_MAX && found == FALSE; i++)
                													{
																		if(stricmp(id, configImageIDs[i]) == 0)
																	  		{
																				free(theme->configImages[i]);
																				theme->configImages[i] = strdup(image);
																				found = TRUE;
																	  		}
																	}
																// images
																for(i=SI_FIRST; i < SI_MAX && found == FALSE; i++)
																	{
														  				if(stricmp(id, itemImageIDs[i]) == 0)
														  					{
																				free(theme->itemImages[i]);
																				theme->itemImages[i] = strdup(image);
																				found = TRUE;
														  					}
																	}
				                  								// libére le nom de l'image si il a été construit à partir du chemin du thème
                												if(image != value) free(image);
              												}
            										}
            									if(found == FALSE)  W(DBF_IMAGE, "ATTENTION ! paramètre de thème inconnu '%s' = '%s'", id, value);  
            									result = 1;  // succès
          									}
									}
        					}		
      					else
      						{
        						W(DBF_THEME, "ATTENTION ! version %ld de thème trouvée incorrect != %ld", version, THEME_VERSION);
        						result = -1; // signale un problème de version
      						}
    				}
    			else W(DBF_THEME, "ATTENTION ! En-tête de fichier dans .theme invalide");
    			free(buf);
    			fclose(fh);
  			}
  		else W(DBF_THEME, "ATTENTION ! Impossible d'ouvrir le fichier .theme '%s'", themeFile);
  	
		RETURN(result);
  		return result;
	}
///
/// FreeTheme
// Libère toutes les phrases d'un thème
void FreeTheme(struct Theme *theme)
	{
  		int i;

  		ENTER();
  		if(theme->loaded == TRUE) UnloadTheme(theme);
  		free(theme->name);
  		theme->name = NULL;
		free(theme->author);
  		theme->author = NULL;
  		free(theme->url);
  		theme->url = NULL;
  		free(theme->version);
  		theme->version = NULL;
		for(i=CI_FIRST; i < CI_MAX; i++)
  			{
    			free(theme->configImages[i]);
    			theme->configImages[i] = NULL;
  			}
		for(i=SI_FIRST; i < SI_MAX; i++)
  			{
    			free(theme->itemImages[i]);
    			theme->itemImages[i] = NULL;
  			}
		LEAVE();
	}
///
/// LoadTheme
// Charge toutes les images d'un thème
void LoadTheme(struct Theme *theme, const char *themeName)
	{
  		char themeFile[SIZE_PATHFILE];
  		LONG res;
		int i;

  		ENTER();
  		AllocTheme(theme, themeName);
  		AddPath(themeFile, theme->directory, ".theme", sizeof(themeFile));
  		res = ParseThemeFile(themeFile, theme);
  		if(res > 0) D(DBF_THEME, "Fichier theme '%s' analysé avec succés", themeFile);
  		else
  			{
    			if(stricmp(themeName, "default") == 0)
    				{
      					D(DBF_THEME, "Analyse du fichier theme '%s' erronée! ignoré...", themeFile);
      					if(res == -1) ER_NewError(tr(MSG_ER_THEMEVER_IGNORE));
      					else ER_NewError(tr(MSG_ER_THEME_FATAL));
      					FreeTheme(theme);
      					LEAVE();
      					return;
    				}
    			else
    				{
      					D(DBF_THEME, "L'analyse du fichier thème '%s' est erroné! thème par défaut est pris à la place...", themeFile);
      					if(res == -1) ER_NewError(tr(MSG_ER_THEMEVER_FALLBACK), themeName);
      					else ER_NewError(tr(MSG_ER_THEME_FALLBACK), themeName);
      					FreeTheme(theme);
      					AllocTheme(theme, "default");
      					AddPath(themeFile, theme->directory, ".theme", sizeof(themeFile));
      					if(ParseThemeFile(themeFile, theme) <= 0)
      						{
        						if(res == -1) ER_NewError(tr(MSG_ER_THEMEVER_IGNORE));
        						else ER_NewError(tr(MSG_ER_THEME_FATAL));
						        FreeTheme(theme);
						        LEAVE();
        						return;
      						}
    				}
  			}
		for(i=CI_FIRST; i < CI_MAX; i++)
  			{
    			char *image = theme->configImages[i];

    			if(IsStrEmpty(image) == FALSE)
    				{
      					if(ObtainImage(configImageIDs[i], image, NULL) == NULL) W(DBF_THEME, "Bourriquet ne peut obtenir l'image '%s' du théme '%s'", image, theme->directory);
      					else ReleaseImage(configImageIDs[i], FALSE);
    				}
  			}
		for(i=SI_FIRST; i < SI_MAX; i++)
  			{
    			char *image = theme->itemImages[i];

    			if(IsStrEmpty(image) == FALSE)
    				{
      					if(ObtainImage(itemImageIDs[i], image, NULL) == NULL) W(DBF_THEME, "Bourriquet ne  peut pas obtenir l'image '%s' du thème '%s'", image, theme->directory);
      					else ReleaseImage(itemImageIDs[i], FALSE);
    				}
  			}
  		theme->loaded = TRUE;
  		LEAVE();
	}
///
/// UnloadTheme
// Décharge toutes les images d'un thème
void UnloadTheme(struct Theme *theme)
	{
		int i;
		
  		ENTER();
		for(i=CI_FIRST; i < CI_MAX; i++)
			{
    			ReleaseImage(configImageIDs[i], TRUE);
			}
		for(i=SI_FIRST; i < SI_MAX; i++) 
			{
				ReleaseImage(itemImageIDs[i], TRUE);
			}
  		theme->loaded = FALSE;
  		LEAVE();
	}

///

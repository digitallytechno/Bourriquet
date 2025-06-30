/***************************************************************************
Bourriquet MorphOS
Copyright (C) 2023 Digitally
 
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

 $Id$

 Superclass:  MUIC_Group
 Description: Provides GUI elements et les routines pour montrer les différents themes 
 									disponibles pour Bourriquet.

***************************************************************************/

#include "ThemeListGroup_cl.h"

#include <string.h>

#include <proto/dos.h>
#include <proto/muimaster.h>
#include <mui/NBalance_mcc.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>

#include "Bourriquet.h"

#include "mui/ImageArea.h"
#include "mui/ThemeList.h"

#include "Config.h"
#include "Locale.h"
#include "MUIObjects.h"
#include "Requesters.h"

#include "Debug.h"

/* CLASSDATA
struct Data
	{
  		Object *NL_THEMELIST;
  		Object *TX_THEMELABEL;
  		Object *GR_PREVIEW;
  		Object *IM_PREVIEW;
  		Object *BT_ACTIVATE;
  		Object *TX_AUTHOR;
  		Object *TX_URL;
  		char themeName[SIZE_DEFAULT];
	};
*/

/* Overloaded Methods */
/// OVERLOAD(OM_NEW)
OVERLOAD(OM_NEW)
{
 	Object *themeListObject;
  	Object *themeTextObject;
  	Object *previewImageObject;
  	Object *imageGroupObject;
  	Object *activateButtonObject;
  	Object *authorTextObject;
  	Object *urlTextObject;

  	ENTER();
  	if((obj = DoSuperNew(cl, obj,
										  	MUIA_Group_Horiz,       TRUE,
											MUIA_ContextMenu,       FALSE,
											Child, VGroup,
														GroupFrameT(GetStr(MSG_CO_THEMES_TITLE)),
														MUIA_HorizWeight, 100,
														Child, NListviewObject,
														  			MUIA_CycleChain, TRUE,
														  			MUIA_NListview_NList, themeListObject = ThemeListObject, End,
														End,
														Child, activateButtonObject = MakeButton(GetStr(MSG_CO_THEMES_ACTIVATE)),  
											End,
											Child, VGroup, 
														GroupFrameT(GetStr(MSG_CO_THEMES_INFORMATION)),
														MUIA_HorizWeight, 200,
														Child, themeTextObject = TextObject,
																  											MUIA_Text_PreParse, "\033b\033c",
																  											MUIA_Text_Copy,     FALSE,
														End,
														Child, RectangleObject,
														  			MUIA_Rectangle_HBar, TRUE,
														  			MUIA_FixHeight,      4,
														End,
														Child, HGroup,
														  			Child, HSpace(0),
														  			Child, TextObject,
																					MUIA_Font,          MUIV_Font_Tiny,
																					MUIA_HorizWeight,   0,
																					MUIA_Text_Contents, GetStr(MSG_CO_THEMES_PREVIEW),
																					MUIA_Text_Copy,     FALSE,
														  			End,
														  			Child, HSpace(0),
														End,
           												Child, HVSpace,
           												Child, HGroup,
           												Child, HSpace(0),
           												Child, imageGroupObject = VGroup,
        		       																							Child, previewImageObject = ImageAreaObject,
																																										  MUIA_ImageArea_ShowLabel,   FALSE,
																																										  MUIA_ImageArea_MaxWidth,    300,
																																										  MUIA_ImageArea_MaxHeight,   200,
																																										  MUIA_ImageArea_NoMinHeight, FALSE,
																												End,
																											End,
           												Child, HSpace(0),
           												End,
           												Child, HVSpace,
														Child, RectangleObject,
															  			MUIA_Rectangle_HBar, TRUE,
															  			MUIA_FixHeight,      4,
														End,
														Child, ColGroup(2),
																	  Child, Label2(GetStr(MSG_CO_THEMES_AUTHOR)),
																	  Child, authorTextObject = TextObject,
																															TextFrame,
																															MUIA_Background,  MUII_TextBack,
																															MUIA_Text_SetMin, TRUE,
																															MUIA_Text_Copy,   FALSE,
																	  End,
																	  Child, Label2(GetStr(MSG_CO_THEMES_URL)),
																	  Child, urlTextObject = TextObject,
																													TextFrame,
																													MUIA_Background,  MUII_TextBack,
																													MUIA_Text_SetMin, TRUE,
																													MUIA_Text_Copy,   FALSE,
																	  End,
														End,
        									End,
        							TAG_MORE, inittags(msg))) != NULL)
  		{
    		GETDATA;
    		data->NL_THEMELIST = themeListObject;
    		data->TX_THEMELABEL = themeTextObject;
    		data->GR_PREVIEW = imageGroupObject;
    		data->IM_PREVIEW = previewImageObject;
    		data->BT_ACTIVATE = activateButtonObject;
    		data->TX_AUTHOR = authorTextObject;
    		data->TX_URL = urlTextObject;
    		// set notifies
    		DoMethod(themeListObject, MUIM_Notify, MUIA_NList_SelectChange, TRUE, obj, 1, METHOD(SelectionChanged));
    		DoMethod(themeListObject, MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime, obj, 1, METHOD(ActivateTheme));
    		DoMethod(activateButtonObject, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, METHOD(ActivateTheme));
  		}
  	RETURN((IPTR)obj);
  	return((IPTR)obj);
}

/* Public Methods */
/// DECLARE(Update)
DECLARE(Update)
{
 	GETDATA;
  	BOOL result = FALSE;
  	APTR context;

  	ENTER();
  	// Nettoie la NList
  	DoMethod(data->NL_THEMELIST, MUIM_NList_Clear);
  	// prépare pour ExamineDir()
  	if((context = ObtainDirContextTags(EX_StringName, (ULONG)G->ThemesDir, EX_DataFields, EXF_TYPE|EXF_NAME, TAG_DONE)) != NULL)
  		{
    		struct ExamineData *ed;
    		LONG error;

    		// iterate through the entries of the Themes directory
    		while((ed = ExamineDir(context)) != NULL)
    			{
      				// Vérifie que cette entrée est un tiroir
      				// parce que nous n'acceptons pas de fichiers içi
      				if(EXD_IS_DIRECTORY(ed))
      					{
        					struct Theme theme;
        					char filename[SIZE_PATHFILE];

        					D(DBF_THEME, "Trouver répertoire '%s' dans le tiroir themes", ed->Name);
        					// efface notre structure themes temporaire 
        					memset(&theme, 0, sizeof(struct Theme));
        					// maintenant nous vérifions si c'est un tiroir qui contient un
        					// fichier ".theme" lequel devra être un theme Bourriquet
        					AddPath(theme.directory, G->ThemesDir, ed->Name, sizeof(theme.directory));
        					AddPath(filename, theme.directory, ".theme", sizeof(filename));
        					// Analyse le fichier .theme pour vérifier si c'est un theme valide ou pas
        					if(ParseThemeFile(filename, &theme) > 0)
        						{
          							D(DBF_THEME, "Trouver un fichier .theme '%s' valide", filename);
          							// Ajoute le theme à notre NList which in fact will allocate/free everything the
          							// ParseThemeFile() function did allocate previously.
          							DoMethod(data->NL_THEMELIST, MUIM_NList_InsertSingle, &theme, MUIV_NList_Insert_Sorted);
          							result = TRUE;
        						}
        					else
        						{
          							W(DBF_THEME, "Ne peut pas analyser le fichier .theme '%s'", filename);
          							FreeTheme(&theme);
        						}
      					}
      				else W(DBF_THEME, "fichier '%s' inconnu dans le répertoire themes... ignoré", ed->Name);
    			}
    		error = IoErr();
    		if(error != 0 && error != ERROR_NO_MORE_ENTRIES) E(DBF_THEME, "ExamineDir() défaillant, erreur %ld", error);
			// maintenant nous avons à vérifier quel élément nous devrons rendre actif
			if(xget(data->NL_THEMELIST, MUIA_NList_Entries) > 1)
				{
					// walk through our list and check if the theme is the currently
				  	// active one, and if so we go and make it the currently selected one.
				  	ULONG pos;
				  	BOOL found = FALSE;
				  
					for(pos = 0;;pos++)
				  		{
							struct Theme *theme = NULL;

							DoMethod(data->NL_THEMELIST, MUIM_NList_GetEntry, pos, &theme);
							if(theme == NULL) break;
							if(stricmp(FilePart(theme->directory), CE->ThemeName) == 0)
								{
					  				set(data->NL_THEMELIST, MUIA_NList_Active, pos);
					  				found = TRUE;
					  				break;
								}
				  		}
      				if(found == FALSE) set(data->NL_THEMELIST, MUIA_NList_Active, MUIV_NList_Active_Top);
    			}
    		else set(data->NL_THEMELIST, MUIA_NList_Active, MUIV_NList_Active_Top);
    		ReleaseDirContext(context);
  		}
  	else E(DBF_THEME, "Pas de répertoire de themes trouvé!");
  	RETURN(result);
  	return(result);
}

///
/// DECLARE(SelectionChanged)
DECLARE(SelectionChanged)
{
 	GETDATA;
  	struct Theme *theme = NULL;

  	ENTER();
  	// get the currently selected entry
  	DoMethod(data->NL_THEMELIST, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &theme);
  	if(theme != NULL)
  		{
    		if(DoMethod(data->GR_PREVIEW, MUIM_Group_InitChange))
    			{
      				char filename[SIZE_PATHFILE];

      				AddPath(filename, theme->directory, "preview", sizeof(filename));
      				// set the new attributes, the old image will be deleted from the cache
      				xset(data->IM_PREVIEW, MUIA_ImageArea_ID,  filename, MUIA_ImageArea_Filename, filename);
      				// and force a cleanup/setup pair
      				DoMethod(data->GR_PREVIEW, OM_REMMEMBER, data->IM_PREVIEW);
      				DoMethod(data->GR_PREVIEW, OM_ADDMEMBER, data->IM_PREVIEW);
      				DoMethod(data->GR_PREVIEW, MUIM_Group_ExitChange);
    			}
    		snprintf(data->themeName, sizeof(data->themeName), "%s - %s", theme->name, theme->version);
    		set(data->TX_THEMELABEL, MUIA_Text_Contents, data->themeName);
    		set(data->TX_AUTHOR, MUIA_Text_Contents, theme->author);
    		set(data->TX_URL, MUIA_Text_Contents, theme->url);
  		}
  	RETURN(0);
  	return(0);
}

///
/// DECLARE(ActivateTheme)
DECLARE(ActivateTheme)
{
	GETDATA;
  	struct Theme *theme = NULL;

  	ENTER();
  	// get the currently selected entry
  	DoMethod(data->NL_THEMELIST, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &theme);
  	if(theme != NULL)
  		{
    		const char *themeName = FilePart(theme->directory);
    			
			// check that this theme isn't already the
    		// active one.
    		if(stricmp(themeName, CE->ThemeName) != 0)
    			{
    				// now we activate the theme and we warn the user about
    				// the fact that a restart is required for the new theme to
    				// be activated.
    				strlcpy(CE->ThemeName, themeName, sizeof(CE->ThemeName));
    				// redraw the NList.
    				DoMethod(data->NL_THEMELIST, MUIM_NList_Redraw, MUIV_NList_Redraw_All);
    				// remind the users to save the configuration and
    				// restart bourriquet.
    				MUI_Request(_app(obj), _win(obj), MUIF_NONE, "Erreur theme activé", "ok req", "ER_THEME_ACTIVATED");
    			}
    		else W(DBF_THEME, "Le theme '%s' est déjà actif !", themeName);
  		}
  	RETURN(0);
  	return(0);
}

///

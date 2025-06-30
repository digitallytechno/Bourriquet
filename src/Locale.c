/***************************************************************************
 Bourriquet MorphOS
 Copyright (C) 2022 digitally
 
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
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/locale.h>
#include <proto/codesets.h>

#include "Locale.h"
#include "Bourriquet.h"
#include "Debug.h"

struct fcstr Bourriquet_Strings[201] =
	{
  		{ "Project", NULL, 0 },
  		{ "About Bourriquet...", NULL, 1 },
  		{ "About MUI...", NULL, 2 },
  		{ "About Bourriquet...", NULL, 3 },
  		{ "M\000MUI...", NULL, 4 },
  		{ "Hide", NULL, 5 },
  		{ "Quit", NULL, 6 },
  		{ "\nBourriquet \n (Emule for MorphOS)\n 2025 digitally\n", NULL, 7 },
  		{ "Version Bourriquet", NULL, 8 },
  		{ "Compilation date :", NULL, 9 },
  		{ "This program is free software\n", NULL, 10 },
  		{ "This program is free software\n", NULL, 11 },
  		{ "Emule for MorphOS", NULL, 12 },
  		{ "_Ok", NULL, 13 },
  		{ "_Cancel", NULL, 14 },
  		{ "_Yes", NULL, 15 },
  		{ "_No", NULL, 16 },
  		{ "Creating user interface...", NULL, 17 },
  		{ "Initialising libraries...", NULL, 18 },
  		{ "Initialize timer", NULL, 19 },
  		{ "Loading graphics...", NULL, 20 },
  		{ "Loading configuration...", NULL, 21 },
  		{ "Couldnt open MUI custom class %s V%lu.%lu.\nThe class is not installed.", NULL, 22 },
  		{ "Couldnt open MUI custom class %s V%lu.%lu.\n\nCurrently installed is V%lu.%lu, please upgrade!", NULL, 23 },
  		{ "Couldnt open MUI custom class %s V%lu.%lu.\n\nCurrently installed is V%lu.%lu, please upgrade!", NULL, 24 },
  		{ "Try again|Quit", NULL, 25 },
  		{ "Opening user interface...", NULL, 26 },
  		{ "Do you really want to quit Bourriquet ?", NULL, 27 },
  		{ "Confirmation request", NULL, 28 },
  		{ "*_Yes|_No", NULL, 29 },
  		{ "Bourriquet %s", NULL, 30 },
  		{ "Quit", NULL, 31 },
  		{ "Error during startup", NULL, 32 },
  		{ "Cant open %s, version v%lu.%lu", NULL, 33 },
  		{ "Bourriquet cant create a apllication", NULL, 34 },
  		{ "Bourriquet cant create MUI custom classes", NULL, 35 },
  		{ "Bourriquet cant init timer", NULL, 36 },
  		{ "Bourriquet cant init task", NULL, 37 },
  		{ "Bourriquet cant init signal", NULL, 38 },
  		{ "Bourriquet cant init methodtask", NULL, 39 },
  		{ "Bourriquet cant init threads", NULL, 40 },
  		{ "Bourriquet cant find url lib %s, V%lu.%lu %s", NULL, 41 },
  		{ "Bourriquet cant create semaphore", NULL, 42 },
  		{ "Quit home page", NULL, 43 },
  		{ "Current Developers:", NULL, 44 },
  		{ "Contributors:", NULL, 45 },
  		{ "Localization Contributors:", NULL, 46 },
  		{ "This program is free software; youre welcome to\ndistribute and or modify it under the terms of the\nGNU Public License. See the enclosed documents for\nmore details.", NULL, 47 },
  		{ "This program uses the following third party software:", NULL, 48 },
  		{ "The latest news about Bourriquet can be found at the\nBourriquet Support Site at \0335http:///\033n", NULL, 49 },
  		{ "About Bourriquet", NULL, 50 },
  		{ "Shutting down Bourriquet...", NULL, 51 },
  		{ "Bourriquet warning...", NULL, 52 },
  		{ "While trying to cleanup Bourriquet still running sub threads\nhave been found.\nDo you want to retry to quit Bourriquet or ignore the still\nrunning sub thread (which could end up in later crashes)?", NULL, 53 },
  		{ "_Retry|_Ignore", NULL, 54 },
  		{ "Connect", NULL, 55 },
  		{ "Cancel", NULL, 56 },
  		{ "Disconnect", NULL, 57 },
  		{ "Server", NULL, 58 },
  		{ "Transfer", NULL, 59 },
  		{ "Research", NULL, 60 },
  		{ "Shared", NULL, 61 },
  		{ "Preferences", NULL, 62 },
  		{ "Server List", NULL, 63 },
  		{ "Our gateway to the Edonkey network", NULL, 64 },
  		{ "Server List", NULL, 65 },
  		{ "Server List (%d)", NULL, 66 },
  		{ "New server", NULL, 67 },
  		{ "IP", NULL, 68 },
  		{ "Port", NULL, 69 },
  		{ "Name", NULL, 70 },
  		{ "Add server", NULL, 71 },
  		{ "Update server.met since URL", NULL, 72 },
  		{ "Update", NULL, 73 },
  		{ "My infos", NULL, 74 },
  		{ "Name", NULL, 75 },
  		{ "IP", NULL, 76 },
  		{ "Description", NULL, 77 },
  		{ "Ping", NULL, 78 },
  		{ "Users", NULL, 79 },
  		{ "Max Users", NULL, 80 },
  		{ "Files", NULL, 81 },
  		{ "Prefs", NULL, 82 },
  		{ "Failed", NULL, 83 },
  		{ "Static", NULL, 84 },
  		{ "SoftFiles", NULL, 85 },
  		{ "HardFiles", NULL, 86 },
  		{ "Version", NULL, 87 },
  		{ "LowIDUsers", NULL, 88 },
  		{ "Server infos", NULL, 89 },
  		{ "Log", NULL, 90 },
  		{ "Low", NULL, 91 },
  		{ "Normal", NULL, 92 },
  		{ "High", NULL, 93 },
  		{ "Connect to this server", NULL, 94 },
  		{ "Priority", NULL, 95 },
  		{ "Add to the list of static servers", NULL, 96 },
  		{ "Remove to the list of static servers", NULL, 97 },
  		{ "Erase this server", NULL, 98 },
  		{ "Client IP of your new server", NULL, 99 },
  		{ "Client port of your new server", NULL, 100 },
  		{ "Name of your new server", NULL, 101 },
  		{ "Added a new personal server to the server list", NULL, 102 },
  		{ "Update url address", NULL, 103 },
  		{ "Update", NULL, 104 },
  		{ "Not connected to the Edonkey network ", NULL, 105 },
  		{ "Users (%ld) | Files (%ld) ", NULL, 106 },
  		{ "Download %d/%d | Upload %d/%d ", NULL, 107 },
  		{ "Research List", NULL, 108 },
  		{ "The Ali Baba cave of the network Edonkey", NULL, 109 },
  		{ "Search parameters", NULL, 110 },
  		{ "File name", NULL, 111 },
  		{ "File type", NULL, 112 },
  		{ "Start", NULL, 113 },
  		{ "Abort", NULL, 114 },
  		{ "Erase all", NULL, 115 },
  		{ "Search results", NULL, 116 },
  		{ "File name", NULL, 117 },
  		{ "Size", NULL, 118 },
  		{ "Availability", NULL, 119 },
  		{ "Full", NULL, 120 },
  		{ "Type", NULL, 121 },
  		{ "Artist", NULL, 122 },
  		{ "Album", NULL, 123 },
  		{ "Title", NULL, 124 },
  		{ "Duration", NULL, 125 },
  		{ "Rate", NULL, 126 },
  		{ "Codec", NULL, 127 },
  		{ "Known", NULL, 128 },
  		{ "Archive", NULL, 129 },
  		{ "Picture", NULL, 130 },
  		{ "Audio", NULL, 131 },
  		{ "Video", NULL, 132 },
  		{ "List of receipts and emissions", NULL, 133 },
  		{ "Receive and exchange your files freely", NULL, 134 },
  		{ "Downloads", NULL, 135 },
  		{ "Name", NULL, 136 },
  		{ "Size", NULL, 137 },
  		{ "Done", NULL, 138 },
  		{ "Speed", NULL, 139 },
  		{ "Progress", NULL, 140 },
  		{ "Source", NULL, 141 },
  		{ "Priority", NULL, 142 },
  		{ "Status", NULL, 143 },
  		{ "Remaining", NULL, 144 },
  		{ "AddTo", NULL, 145 },
  		{ "Uploads", NULL, 146 },
  		{ "Name User", NULL, 147 },
  		{ "File", NULL, 148 },
  		{ "Speed", NULL, 149 },
  		{ "Transferred", NULL, 150 },
  		{ "Excepted", NULL, 151 },
  		{ "Send Time", NULL, 152 },
  		{ "Status", NULL, 153 },
  		{ "Parts obtained", NULL, 154 },
  		{ "List of shared files", NULL, 155 },
  		{ "Your treasures to share on the Edonkey network", NULL, 156 },
  		{ "File name", NULL, 157 },
  		{ "Size", NULL, 158 },
  		{ "Type", NULL, 159 },
  		{ "Priority", NULL, 160 },
  		{ "Requests", NULL, 161 },
  		{ "Tranferred", NULL, 162 },
  		{ "Shared parts", NULL, 163 },
  		{ "Complete", NULL, 164 },
  		{ "Share EDonkey", NULL, 165 },
  		{ "Configuration", NULL, 166 },
  		{ "Configuration of Bourriquet", NULL, 167 },
  		{ "Save", NULL, 168 },
  		{ "Use", NULL, 169 },
  		{ "Abort", NULL, 170 },
  		{ "General", NULL, 171 },
  		{ "User Name", NULL, 172 },
  		{ "Client Port", NULL, 173 },
  		{ "Directories", NULL, 174 },
  		{ "Paths", NULL, 175 },
  		{ "Incoming directory", NULL, 176 },
  		{ "Temporary directory", NULL, 177 },
  		{ "Shared directory", NULL, 178 },
  		{ "Themes", NULL, 179 },
  		{ "Appearance", NULL, 180 },
  		{ "Activate theme", NULL, 181 },
  		{ "Preview", NULL, 182 },
  		{ "Author", NULL, 183 },
  		{ "Url", NULL, 184 },
  		{ "Information", NULL, 185 },
  		{ "Update", NULL, 186 },
  		{ "Updating the application and its components", NULL, 187 },
  		{ "Update check frequency", NULL, 188 },
  		{ "Never", NULL, 189 },
  		{ "Daily", NULL, 190 },
  		{ "Weekly", NULL, 191 },
  		{ "Monthly", NULL, 192 },
  		{ "Search Now", NULL, 193 },
  		{ "Connection", NULL, 194 },
  		{ "Abort", NULL, 195 },
  		{ "The currently activated theme '%s' was found to be of an incompatible version and wasn't loaded. The default theme was therefore activated as a fallback.\n\nPlease make sure to update the theme to a compatible version or disable it in the configuration.", NULL, 196 },
  		{ "While trying to load the default theme of Bourriquet, it was found to be of an incorrect version.\n\nThe theme loading process was aborted and you are adviced to upgrade the default theme to a compatible version.", NULL, 197 },
  		{ "Bourriquet was unable to load the default theme and aborted the theme loading completly.\n\nPlease make sure that your themes directory is readable and contains the correct default theme distributed with Bourriquet.", NULL, 198 },
  		{ "The currently activated theme '%s' is either inaccesible or it is corrupt. The default theme was therefore activated as a fallback.\n\nPlease make sure your selected theme is readable and a valid Bourriquet theme.", NULL, 199 },
  		{ "end", NULL, 200 }
	};

/*** fonctions du catalogue ***/
/// InitCatalogue()
void InitCatalogue(void)
	{
  		int i;
  		struct fcstr *fc;

  		ENTER();
  		for(i=0, fc = (struct fcstr *)Bourriquet_Strings;  i < 201;  i++, fc++)
  			{
    			fc->msg = fc->defmsg;
  			}
 	 	LEAVE();
	}

char *GetStr(APTR fcstr)
	{
		char *defaultstr = ((struct fcstr *)fcstr)->defmsg;

		if(LocaleBase)
			{
				return (char *)GetCatalogStr(G->Catalog, ((struct fcstr *)fcstr)->id, (STRPTR)defaultstr);
			}
	  	return defaultstr;
	}

///
/// OpenCatalogue()
BOOL OpenCatalogue(void)
	{
  		BOOL retry = TRUE;
  		BOOL expunged = FALSE;
  		BOOL success = TRUE;
  		struct fcstr *fc = NULL;
  		int i = 0;

  		ENTER();
  		if(G->Locale != NULL && G->Catalog == NULL)
  			{
    			do
    				{
      					if(G->NoCatalogTranslation == FALSE)
      						{
        						G->Catalog = OpenCatalog(NULL,
                                 							(STRPTR)"Bourriquet.catalog",
                                 							OC_BuiltInLanguage, "english",
                                 							OC_BuiltInCodeSet, 4, // 4 = iso-8859-1
                                							TAG_END);
      						}
      					if(G->Catalog != NULL)
      						{
        						if(G->Catalog->cat_Version != 1)
        							{
          								W(DBF_STARTUP, "La version (%d) du catalogue est différente de la version (-1) requise", G->Catalog->cat_Version);
          								CloseCatalog(G->Catalog);
          								G->Catalog = NULL;
								        if(expunged == FALSE)
          									{
            									RemLibrary((struct Library *)LocaleBase);
            									//D(DBF_STARTUP, "expunged locale.library");
            									expunged = TRUE;
            									retry = TRUE;
          									}
          								else
          									{
            									struct EasyStruct es;

            									DisplayBeep(NULL);
            									es.es_StructSize = sizeof(es);
            									es.es_Flags      = 0;
									            es.es_Title        = (STRPTR)"Version de Bourriquet.catalog incompatible!";
            									es.es_TextFormat   = (STRPTR)"Kitty a trouvé un fichier catalogue de traduction dans votre langue\n"
                                                                                  		"Il ne peut pas être activé car cette version est trop vieille\n"
																						"avec la version nécessaire de Kitty\n";
            									es.es_GadgetFormat = (STRPTR)"Retry|Continue|Quit";
									            switch(EasyRequestArgs(NULL, &es, NULL, NULL))
            										{
              											default:
              												case 0: 
                												retry = FALSE;
                												success = FALSE;
              													break;
											              case 1: 
                												retry = TRUE;
                												expunged = FALSE;
              													break;
											              case 2: // continue
                												retry = FALSE;
                												success = TRUE;
              													break;
            										}
          									}
        							}
        						else
        							{
          								D(DBF_STARTUP, "Bourriquet.catalog v%d.%d (language: '%s', codeset: %d) ouvert avec succés", G->Catalog->cat_Version, G->Catalog->cat_Revision, G->Catalog->cat_Language, G->Catalog->cat_CodeSet);
          								retry = FALSE;
          								success = TRUE;
        							}
      						}
      					else if(G->NoCatalogTranslation == FALSE)
      						{
        						if(IoErr() != 0) W(DBF_STARTUP, "Kitty ne peut pas ouvrir 'Bourriquet.catalog': IoErr: %d", IoErr());
        						else W(DBF_STARTUP, "Kitty ne pourra pas ouvrir 'Bourriquet.catalog': les phrases prédéfinies seront utilisées à la place");
						        retry = FALSE;
      						}
    				}
    			while(retry == TRUE);
  			}
  		else W(DBF_STARTUP, "La locale.library n'était pas ouverte ou le catalogue est déjà ouvert.");
  		if(G->Locale != NULL && G->Catalog != NULL)
  			{
    			for(i=0, fc = (struct fcstr *)Bourriquet_Strings;  i < 201;  i++, fc++)
    				{
      					const char *cs;

      					cs = GetCatalogStr(G->Catalog, fc->id, (STRPTR)fc->defmsg);
      					fc->msg = CodesetsUTF8ToStr(CSA_Source, cs, TAG_DONE);
    				}
  			}
  		RETURN(success);
  		return success;
	}

///
/// CloseCatalogue()
void CloseCatalogue(void)
	{
  		int i;
  		struct fcstr *fc;

  		ENTER();
  		for(i=0, fc = (struct fcstr *)Bourriquet_Strings;  i < 201;  i++, fc++)
  			{
    			if(fc->msg != NULL && fc->msg != fc->defmsg)
    				{
      					CodesetsFreeA((STRPTR)fc->msg, NULL);
      					fc->msg = fc->defmsg;
   		 			}
  			}
  		if(G->Locale != NULL && G->Catalog != NULL)
  			{
    			CloseCatalog(G->Catalog);
    			G->Catalog = NULL;
 	 		}
  		LEAVE();
	}

///

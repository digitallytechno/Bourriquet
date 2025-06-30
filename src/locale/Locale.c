/***************************************************************************
 Kitty MorphOS
 Copyright (C) 2022 digitally
***************************************************************************/
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/locale.h>
#include <proto/codesets.h>

#include "Locale.h"
#include "Kitty.h"
#include "Debug.h"

struct fcstr Kitty_Strings[36] =
{
  { "Project", NULL, 0 },
  { "About Kitty...", NULL, 1 },
  { "About MUI...", NULL, 2 },
  { "?\000About Kitty...", NULL, 3 },
  { "M\000MUI...", NULL, 4 },
  { "H\000Hide...", NULL, 5 },
  { "Q\000Quit...", NULL, 6 },
  { "\nKitty \n Copyright © 2022 digitally\n", NULL, 7 },
  { "Version Kitty", NULL, 8 },
  { "Compilation date :", NULL, 9 },
  { "This program is free software\n", NULL, 10 },
  { "This application uses the following third party software: \n", NULL, 11 },
  { "Emule for MorphOS", NULL, 12 },
  { "\033c_Ok", NULL, 13 },
  { "_Cancel", NULL, 14 },
  { "_Yes", NULL, 15 },
  { "_No", NULL, 16 },
  { "Creating user interface...", NULL, 17 },
  { "Initialising libraries...", NULL, 18 },
  { "Loading graphics...", NULL, 19 },
  { "Couldn't open MUI custom class '%s' V%lu.%lu.\nThe class is not installed.", NULL, 20 },
  { "Couldn't open MUI custom class '%s' V%lu.%lu.\n\nCurrently installed is V%lu.%lu, please upgrade!", NULL, 21 },
  { "Couldn't open MUI custom class '%s' V%lu.%lu.\n\nCurrently installed is V%lu.%lu, please upgrade!", NULL, 22 },
  { "Try again|Quit", NULL, 23 },
  { "Opening user interface...", NULL, 24 },
  { "Do you really want to quit Kitty ?", NULL, 25 },
  { "Kitty version 0.1", NULL, 26 },
  { "Quit", NULL, 27 },
  { "Error during Startup", NULL, 28 },
  { "Can't open '%s', version v%lu.%lu", NULL, 29 },
  { "Kitty can't create a apllication", NULL, 30 },
  { "Kitty can't create MUI custom classes", NULL, 31 },
  { "Kitty can't init timer", NULL, 32 },
  { "Kitty can't init task", NULL, 33 },
  { "Kitty can't init signal", NULL, 34 },
  { "(end)", NULL, 35 }
};

/*** Fonctions du catalogue ***/
/// InitCatalogue()
void InitCatalogue(void)
{
	int i;
	struct fcstr *fc;

	ENTER();
  	// initialise toutes les phrases à leurs chaînes intégrées par défaut 
	for(i=0, fc = (struct fcstr *)Kitty_Strings;  i < 36;  i++, fc++)
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
	// nous essayons d'ouvrir le catalogue
	if(G->Locale != NULL && G->Catalog == NULL)
		{
			do
				{
      				// nous essayons d'ouvrir le fichier du catalogue dans la version spécifique
					if(G->NoCatalogTranslation == FALSE)
						{
							G->Catalog = OpenCatalog(NULL,
													(STRPTR)"Kitty.catalog",
													OC_BuiltInLanguage, "english",
													OC_BuiltInCodeSet, 4, // 4 = iso-8859-1
													TAG_END);
						}
      				// nous vérifions si une erreur est survenue.
					if(G->Catalog != NULL)
						{
        					// nous vérifions si la catalogue ouvert a la version correcte ou pas
							if(G->Catalog->cat_Version != 1)
								{
									W(DBF_STARTUP, "La version (%d) du catalogue est différente de la version requise (1)", G->Catalog->cat_Version);
						          	CloseCatalog(G->Catalog);
									G->Catalog = NULL;
									if(expunged == FALSE)
										{
											RemLibrary((struct Library *)LocaleBase);
											D(DBF_STARTUP, "La locale.library fermée");
											expunged = TRUE;
											retry = TRUE;
										}
									else
										{
											struct EasyStruct es;

											DisplayBeep(NULL);
											es.es_StructSize = sizeof(es);
											es.es_Flags      = 0;
											es.es_Title        = (STRPTR)"Version de Kitty.catalog incompatible!";
											es.es_TextFormat   = (STRPTR)"Kitty a trouvé un fichier de catalogue dans votre language naturel de disponible\n"
																		"Toutefois,\n"
																		"Il ne pourra être activé car sa version est\n"
																		"incompatible avec la version 1 dont Kitty requiert.\n"
																		"\n"
																		"Veuillez vérifier si il y a une version 1 de votre\n"
																		"catalogue de disponible ou enlever l'ancien";
											es.es_GadgetFormat = (STRPTR)"Réssayer|Continuer|Quitter";
											switch(EasyRequestArgs(NULL, &es, NULL, NULL))
												{
													default:
													case 0: // quitter
														retry = FALSE;
														success = FALSE;
														break;
													case 1: // réessayer
														retry = TRUE;
														expunged = FALSE;
														break;
													case 2: // continuer
														retry = FALSE;
														success = TRUE;
														break;
												}
										}
								}
							else
								{
									D(DBF_STARTUP, "Kitty.catalog v%d.%d (langage: '%s', codeset: %d) ouvert avec succès", G->Catalog->cat_Version, G->Catalog->cat_Revision, G->Catalog->cat_Language, G->Catalog->cat_CodeSet);
									retry = FALSE;
									success = TRUE;
								}
						}
					else if(G->NoCatalogTranslation == FALSE)
						{
        					// nous vérifions le résultat de l'erreur
							if(IoErr() != 0) W(DBF_STARTUP, "Ne peut pas ouvrir 'Kitty.catalog': IoErr: %d", IoErr());
							else W(DBF_STARTUP, "Ne peut pas ouvrir 'Kitty.catalog': les phrases pré-définies seront utilisées à la place");
							retry = FALSE;
						}
				}
			while(retry == TRUE);
		}
	else W(DBF_STARTUP, "La locale.library ne peut pas être ouverte ou le catalogue est déjà ouvert.");
	if(G->Locale != NULL && G->Catalog != NULL)
		{
			for(i=0, fc = (struct fcstr *)Kitty_Strings;  i < 36;  i++, fc++)
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
	for(i=0, fc = (struct fcstr *)Kitty_Strings;  i < 36;  i++, fc++)
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

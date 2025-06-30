/***************************************************************************
 GenClasses - MUI class dispatcher generator
 Copyright (C) 2001 by Andrew Bell <mechanismx@lineone.net>
***************************************************************************/
#ifndef GC_H
#define GC_H 1

#ifndef LISTS_H
#include "lists.h"
#endif

#define BUCKET_CNT   255

#define EDIT_WARNING "Ce fichier est automatiquement généré avec GenClasses - NE PAS EDITER!!!"
#define HEADER_NAME  "Classes.h"
#define SOURCE_NAME  "Classes.c"
#define OBJECT_NAME  "Classes.o"
#define CRC_NAME     "Classes.crc"

#define KEYWD_SUPERCLASS  "Superclass:"
#define KEYWD_DESC        "Description:"
#define KEYWD_CLASSDATA   "CLASSDATA"
#define KEYWD_OVERLOAD    "OVERLOAD"
#define KEYWD_DECLARE     "DECLARE"
#define KEYWD_ATTR        "ATTR"
#define KEYWD_EXPORT      "EXPORT"
#define KEYWD_INCLUDE     "INCLUDE"

struct overloaddef
{
  char *name;
};

struct declaredef
{
  char *name;
  char *params;
};

struct attrdef
{
  char *name;
};

struct exportdef
{
  char *exporttext;
};

struct includedef
{
  char *includetext;
};

struct classdef
{
  char *name;                 /* i.e. Searchwindow */
  char *superclass;           /* i.e. MUIC_Window */
  struct classdef *supernode; /* pointer to private superclass */
  int index;                  /* index within the final list */
  int finished;               /* finished marking during breadth search first */
  char *desc;
  char *classdata;
  struct list overloadlist;   /* list of extracted OVERLOAD() macros */
  struct list declarelist;    /* list of extracted DECLARE() macros */
  struct list attrlist;       /* list of extracted ATTR() macros */
  struct list exportlist;     /* list of extracted EXPORT blocks */
  struct list includelist;    /* list of extracted INCLUDE blocks */
};

#endif /* GC_H */


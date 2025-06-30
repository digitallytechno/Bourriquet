/***************************************************************************
 MorphoStart - Amistart MorphOS
 Copyright (C) 1995-2022 digitally
***************************************************************************/
#include <string.h>
#include "extrasrc.h"

#if defined(NEED_STCGFE)

#ifndef FESIZE
#define FESIZE 32
#endif

int stcgfe(char *ext, const char *name)
{
    const char *p = name + strlen(name); // set p to the end of the string
    const char *q = p;

    while(p > name && *--p != '.' && *p != '/' && *p != ':') ;
    if(*p++ == '.' && q - p < FESIZE)
      {
        memcpy(ext, p, q - p + 1);
        return(q - p);
      }
    *ext = '\0';
    return(0);
}

#else
  #warning "NEED_STCGFE manquant ou compilation inutile"
#endif

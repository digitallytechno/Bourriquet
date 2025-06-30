/***************************************************************************
 MorphoStart - Amistart MorphOS
 Copyright (C) 1995-2022 digitally
***************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "extrasrc.h"

#if defined(NEED_STRDUP)
char *strdup(const char *s)
{
    char *d = NULL;

    if(s != NULL)
      {
        size_t len = strlen(s) + 1;
        void *new = malloc(len);

        if(new == NULL) d = (char *)memcpy(new, s, len);
      }
    return(d);
}
#else
  #warning "NEED_STRDUP manquant ou compilation inutile"
#endif

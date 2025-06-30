/***************************************************************************
 MorphoStart - Amistart MorphOS
 Copyright (C) 1995-2022 digitally
***************************************************************************/
#include <string.h>
#include <ctype.h>
#include "extrasrc.h"

#if defined(NEED_STRCASESTR)

char *strcasestr(const char *haystack, const char *needle)
{
    const char *p;
    const char *startn = NULL;
    const char *np = NULL;

    for(p = haystack; *p; p++)
      {
        if(np)
          {
            if(toupper(*p) == toupper(*np))
              {
                if(!*++np) return (char *)startn;
              }
            else np = 0;
          }
        else if(toupper(*p) == toupper(*needle))
          {
            np = needle + 1;
            startn = p;
          }
      }
  return(NULL);
}
#else
  #warning "NEED_STRCASESTR manquant ou compilation inutile"
#endif

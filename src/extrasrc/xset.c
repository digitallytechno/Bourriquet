/***************************************************************************
 MorphoStart - Amistart MorphOS
 Copyright (C) 1995-2022 digitally
***************************************************************************/
#include "SDI_compiler.h"
#include "SDI_stdarg.h"

#include <proto/intuition.h>
#include <proto/utility.h>
#include "extrasrc.h"

#if defined(NEED_XSET)

#warning possible incomplete tag list parsing!!

ULONG VARARGS68K xset(Object *obj, ...)
{
    VA_LIST args;
    struct TagItem *tags;
    struct TagItem *tag;
    struct TagItem newTags[6];
    struct TagItem *newTag = newTags;
    int i;

    VA_START(args, obj);
    tags = (struct TagItem *)VA_ARG(args, struct TagItem *);
    i = 0;
    while(i < (sizeof(newTags) / sizeof(newTags[0])) - 1 && (tag = NextTagItem(&tags)) != NULL)
      {
        if(tag->ti_Tag >= TAG_USER)
          {
            newTag->ti_Tag = tag->ti_Tag;
            newTag->ti_Data = tag->ti_Data;
            newTag++;
            i++;
          }
      }
    VA_END(args);
    newTag->ti_Tag = TAG_DONE;
    return(SetAttrsA(obj, newTags));
}
#else
  #warning "NEED_XSET manquant ou compilation inutile"
#endif

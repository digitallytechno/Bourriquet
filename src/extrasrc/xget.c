/***************************************************************************
 MorphoStart - Amistart MorphOS
 Copyright (C) 1995-2022 digitally
***************************************************************************/
#include <proto/intuition.h>
#include "extrasrc.h"

#if defined(NEED_XGET)
ULONG xget(Object *obj, const ULONG attr)
{
    ULONG b = 0;
  
    GetAttr(attr, obj, &b);
    return(b);
}
#else
  #warning "NEED_XGET manquant ou compilation inutile"
#endif

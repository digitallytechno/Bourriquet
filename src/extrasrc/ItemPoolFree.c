/***************************************************************************
 Kitty - MorphOS
 Copyright (C) 2022 digitally
***************************************************************************/
#include <exec/types.h>
#include <proto/exec.h>
#include "Extrasrc.h"
#include "ItemPool.h"

#define DEBUG_USE_MALLOC_REDEFINE
#include "Debug.h"
#if defined(NEED_ITEMPOOLFREE)

APTR ItemPoolFree(APTR poolHeader, APTR item)
{
    struct ItemPool *pool = poolHeader;

    FreePooled(pool->pool, item, pool->itemSize);
    return(item);
}
#else
  #warning "NEED_ITEMPOOLFREE manquant or compilation inutile"
#endif

///

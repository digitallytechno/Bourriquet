/***************************************************************************
 Kitty - MorphOS
 Copyright (C) 2022 digitally
***************************************************************************/
#include <exec/types.h>
#include <proto/exec.h>

#include "Extrasrc.h"
#define DEBUG_USE_MALLOC_REDEFINE
#include "Debug.h"

#if defined(NEED_FREEVECPOOLED)
void FreeVecPooled(APTR poolHeader, APTR memory)
{
    ULONG *mem = (ULONG *)memory;
    ULONG memSize;

    ENTER();
    memSize = *(--mem);
    FreePooled(poolHeader, mem, memSize);
    LEAVE();
}
#else
  #warning "NEED_FREEVECPOOLED manquant ou compilation inutile"
#endif
///

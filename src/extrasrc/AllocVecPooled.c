/****************************************************************
	Kitty - MorphOS
	Copyright 2022 Digitally
****************************************************************/
#include <exec/types.h>
#include <proto/exec.h>

#include "Extrasrc.h"
#define DEBUG_USE_MALLOC_REDEFINE
#include "Debug.h"
#if defined(NEED_ALLOCVECPOOLED)

APTR AllocVecPooled(APTR poolHeader, ULONG memSize)
{
    ULONG *memory;

    ENTER();
    memSize += sizeof(ULONG);
    if((memory = (ULONG *)AllocPooled(poolHeader, memSize)) != NULL) *memory++ = memSize;
    RETURN(memory);
    return(memory);
}
#else
  #warning "NEED_ALLOCVECPOOLED manquant ou compilation inutile"
#endif
///

/***************************************************************************
 MorphoStart - Amistart MorphOS
 Copyright (C) 1995-2022 digitally
***************************************************************************/
#ifndef ITEMPOOL_H
#define ITEMPOOL_H 1

#include <exec/semaphores.h>
#include <exec/types.h>

struct ItemPool
{
    APTR pool;
    ULONG itemSize;
};

#endif /* ITEMPOOL_H */

/***************************************************************************
 Kitty - MorphOS
 Copyright (C) 2022 digitally
***************************************************************************/
#include <exec/lists.h>
#include "Extrasrc.h"

#if defined(NEED_NEWMINLIST)
void NewMinList(struct MinList *list)
{
    list->mlh_Head = (struct MinNode *)&list->mlh_Tail;
    list->mlh_Tail = NULL;
    list->mlh_TailPred = (struct MinNode *)&list->mlh_Head;
}
#else
  #warning "NEED_NEWMINLIST manquant ou compilation inutile"
#endif

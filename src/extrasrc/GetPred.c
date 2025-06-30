/***************************************************************************
 Kitty - MorphOS
 Copyright (C) 1995-2022 digitally
***************************************************************************/
#include <exec/lists.h>
#include "Extrasrc.h"

#if defined(NEED_GETPRED)
struct Node *GetPred(struct Node *node)
{
    struct Node *result = NULL;

    if(node != NULL && node->ln_Pred != NULL && node->ln_Pred->ln_Pred != NULL) result = node->ln_Pred;
    return(result);
}
#else
  #warning "NEED_GETPRED manquant or compilation inutile"
#endif

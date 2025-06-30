/***************************************************************************
 Kitty - MorphOS
 Copyright (C) 2022 digitally
***************************************************************************/
#include <proto/intuition.h>
#include <clib/alib_protos.h>

#include "SDI_compiler.h"
#include "SDI_stdarg.h"
#include "Extrasrc.h"

#if defined(NEED_DOSUPERNEW)

#if(defined(__GNUC__) && __GNUC__ == 2)

// DoSuperNew()
// Calls parent NEW method within a subclass
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
    Object *rc;
    VA_LIST args;

    VA_START(args, obj);
    rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, VA_ARG(args, ULONG), NULL);
    VA_END(args);
    return(rc);
}

#endif

#else
  #warning "NEED_DOSUPERNEW manquant ou compilation inutile"
#endif

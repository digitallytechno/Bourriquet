/***************************************************************************
 MorphoStart - Amistart MorphOS
 Copyright (C) 1995-2022 digitally
***************************************************************************/
#include "SDI_compiler.h"
#include "SDI_stdarg.h"
#include <stdarg.h>
#include <stdio.h>
#include "extrasrc.h"

#if defined(NEED_SNPRINTF)
int snprintf(char *s, size_t maxlen, const char *format, ...)
{
    va_list ap;
    int ret;

    va_start(ap, format);
    ret = (int)vsnprintf(s, maxlen, format, ap);
    va_end(ap);
    return(ret);
}
#else
  #warning "NEED_SNPRINTF manquant ou compilation inutile"
#endif

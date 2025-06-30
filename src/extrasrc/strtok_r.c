/* Reentrant string tokenizer.  Generic version.
   Copyright (C) 1991,1996-1999,2001,2004,2007 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. */

#include <string.h>
#include "extrasrc.h"

#if defined(NEED_STRTOK_R)
char *strtok_r(char *s, const char *delim, char **save_ptr)
{
    char *token;

    if(s == NULL) s = *save_ptr;
    s += strspn(s, delim);
    if(*s == '\0')
      {
        *save_ptr = s;
        return NULL;
      }
    token = s;
    s = strpbrk(token, delim);
    if(s == NULL) *save_ptr = strchr(token, '\0');
    else
      {
        *s = '\0';
        *save_ptr = s + 1;
      }
    return(token);
}
#else
  #warning "NEED_STRTOK_R manquant ou compilation inutile"
#endif

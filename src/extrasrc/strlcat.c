/* $OpenBSD: strlcat.c,v 1.11 2003/06/17 21:56:24 millert Exp $ */

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char *rcsid = "$OpenBSD: strlcat.c,v 1.11 2003/06/17 21:56:24 millert Exp $";
#endif /* LIBC_SCCS and not lint */

#include <string.h>
#include "extrasrc.h"
#if defined(NEED_STRLCAT)
size_t strlcat(char *dst, const char *src, size_t siz)
{
    register char *d = dst;
    register const char *s = src;
    register size_t n = siz;
    size_t dlen;

    while (n-- != 0 && *d != '\0') d++;
    dlen = d - dst;
    n = siz - dlen;
    if(n == 0) return(dlen + strlen(s));
    while(*s != '\0') 
      {
        if(n != 1) 
          {
            *d++ = *s;
            n--;
          }
        s++;
      }
    *d = '\0';
  return(dlen + (s - src));  /* count does not include NUL */
}
#else
  #warning "NEED_STRLCAT manquant ou compilation inutile"
#endif

/***************************************************************************
 Kitty - MorphOS
 Copyright (C) 2022 digitally
***************************************************************************/
#include "SDI_compiler.h"
#include "Extrasrc.h"

#if defined(NEED_VSNPRINTF)

#define HAVE_STRING_H
#define HAVE_CTYPE_H
#define HAVE_STDLIB_H
#undef HAVE_VA_COPY
#define WITH_FLOATINGPOINT
#define WITH_OCTAL
#define WITH_POINTER
#define WITH_N_INT
#undef WITH_WIDE_CHAR
#undef HAVE_LONG_LONG
#undef HAVE_LONG_DOUBLE

#ifdef TEST_SNPRINTF /* need math library headers for testing */
#include <math.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#include <stddef.h>
#include <stdarg.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if defined(HAVE_SNPRINTF) && defined(HAVE_VSNPRINTF) && defined(HAVE_C99_VSNPRINTF)
/* only include stdio.h if we are not re-defining snprintf or vsnprintf */
#include <stdio.h>
 /* make the compiler happy with an empty file */
 void dummy_snprintf(void) {}
#else

#ifdef HAVE_LONG_DOUBLE
#define LDOUBLE long double
#else
#define LDOUBLE double
#endif

#ifdef HAVE_LONG_LONG
#define LLONG long long
#else
#define LLONG long
#endif

/* free memory if the pointer is valid and zero the pointer */
#ifndef SAFE_FREE
#define SAFE_FREE(x) do { free((x)); (x)=NULL; } while(0)
#endif

#ifndef VA_COPY
#ifdef HAVE_VA_COPY
#define VA_COPY(dest, src) __va_copy(dest, src)
#else
#define VA_COPY(dest, src) (dest) = (src)
#endif
#endif

static size_t dopr(char *buffer, size_t maxlen, const char *format,
       va_list args_in);
static void fmtstr(char *buffer, size_t *currlen, size_t maxlen,
        char *value, int flags, int min, int max);
static void fmtint(char *buffer, size_t *currlen, size_t maxlen,
        long value, int base, int min, int max, int flags);
#ifdef WITH_FLOATINGPOINT
static void fmtfp(char * buffer, size_t *currlen, size_t maxlen,
       LDOUBLE fvalue, int min, int max, int flags);
#endif
static void dopr_outch(char *buffer, size_t *currlen, size_t maxlen, char c);

/*
 * dopr(): poor man's version of doprintf
 */

/* format read states */
#define DP_S_DEFAULT 0
#define DP_S_FLAGS   1
#define DP_S_MIN     2
#define DP_S_DOT     3
#define DP_S_MAX     4
#define DP_S_MOD     5
#define DP_S_CONV    6
#define DP_S_DONE    7

/* format flags - Bits */
#define DP_F_MINUS   (1 << 0)
#define DP_F_PLUS    (1 << 1)
#define DP_F_SPACE   (1 << 2)
#define DP_F_NUM     (1 << 3)
#define DP_F_ZERO    (1 << 4)
#define DP_F_UP      (1 << 5)
#define DP_F_UNSIGNED   (1 << 6)

/* Conversion Flags */
#define DP_C_SHORT   1
#define DP_C_LONG    2
#define DP_C_LDOUBLE 3
#define DP_C_LLONG   4

#define char_to_int(p) ((p)- '0')
#ifndef MAX
#define MAX(p,q) (((p) >= (q)) ? (p) : (q))
#endif

static size_t dopr(char *buffer, size_t maxlen, const char *format, va_list args_in)
{
    char ch;
    LLONG value;
#ifdef WITH_FLOATINGPOINT
  LDOUBLE fvalue;
#endif
    char *strvalue;
    int min;
    int max;
    int state;
    int flags;
    int cflags;
    size_t currlen;
    va_list args;

    VA_COPY(args, args_in);
    state = DP_S_DEFAULT;
    currlen = flags = cflags = min = 0;
    max = -1;
    ch = *format++;
    while(state != DP_S_DONE)
      {
        if(ch == '\0') state = DP_S_DONE;
        switch(state) 
          {
            case DP_S_DEFAULT:
              if(ch == '%') state = DP_S_FLAGS;
              else dopr_outch (buffer, &currlen, maxlen, ch);
              ch = *format++;
              break;
            case DP_S_FLAGS:
              switch(ch) 
                {
                  case '-':
        flags |= DP_F_MINUS;
        ch = *format++;
        break;
      case '+':
        flags |= DP_F_PLUS;
        ch = *format++;
        break;
      case ' ':
        flags |= DP_F_SPACE;
        ch = *format++;
        break;
      case '#':
        flags |= DP_F_NUM;
        ch = *format++;
        break;
      case '0':
        flags |= DP_F_ZERO;
        ch = *format++;
        break;
      default:
        state = DP_S_MIN;
        break;
      }
      break;
    case DP_S_MIN:
      if(isdigit((unsigned char)ch))
      {
        min = 10*min + char_to_int (ch);
        ch = *format++;
      }
      else if(ch == '*')
      {
        min = va_arg(args, int);
        ch = *format++;
        state = DP_S_DOT;
      }
      else
      {
        state = DP_S_DOT;
      }
      break;
    case DP_S_DOT:
      if(ch == '.')
      {
        state = DP_S_MAX;
        ch = *format++;
      }
      else
      {
        state = DP_S_MOD;
      }
      break;
    case DP_S_MAX:
      if(isdigit((unsigned char)ch))
      {
        if(max < 0)
          max = 0;
        max = 10*max + char_to_int (ch);
        ch = *format++;
      }
      else if(ch == '*')
      {
        max = va_arg(args, int);
        ch = *format++;
        state = DP_S_MOD;
      }
      else
      {
        state = DP_S_MOD;
      }
      break;
    case DP_S_MOD:
      switch(ch)
      {
      case 'h':
        cflags = DP_C_SHORT;
        ch = *format++;
        break;
      case 'l':
        cflags = DP_C_LONG;
        ch = *format++;
#ifdef HAVE_LONG_LONG
        if(ch == 'l')
        {  /* It's a long long */
          cflags = DP_C_LLONG;
          ch = *format++;
        }
#endif
        break;
#ifdef WITH_FLOATINGPOINT
      case 'L':
        cflags = DP_C_LDOUBLE;
        ch = *format++;
        break;
#endif
      default:
        break;
      }
      state = DP_S_CONV;
      break;
    case DP_S_CONV:
      switch(ch)
      {
      case 'd':
      case 'i':
        if(cflags == DP_C_SHORT)
          value = va_arg(args, int);
        else if(cflags == DP_C_LONG)
          value = va_arg(args, long int);
#ifdef HAVE_LONG_LONG
        else if(cflags == DP_C_LLONG)
          value = va_arg(args, LLONG);
#endif
        else
          value = va_arg(args, int);
        fmtint (buffer, &currlen, maxlen, value, 10, min, max, flags);
        break;
#ifdef WITH_OCTAL
      case 'o':
        flags |= DP_F_UNSIGNED;
        if(cflags == DP_C_SHORT)
          value = va_arg(args, unsigned int);
        else if(cflags == DP_C_LONG)
          value = (long)va_arg(args, unsigned long int);
#ifdef HAVE_LONG_LONG
        else if(cflags == DP_C_LLONG)
          value = (long)va_arg(args, unsigned LLONG);
#endif
        else
          value = (long)va_arg(args, unsigned int);
        fmtint (buffer, &currlen, maxlen, value, 8, min, max, flags);
        break;
#endif /* WITH_OCTAL */
      case 'u':
        flags |= DP_F_UNSIGNED;
        if(cflags == DP_C_SHORT)
          value = va_arg(args, unsigned int);
        else if(cflags == DP_C_LONG)
          value = (long)va_arg(args, unsigned long int);
#ifdef HAVE_LONG_LONG
        else if(cflags == DP_C_LLONG)
          value = (LLONG)va_arg(args, unsigned LLONG);
#endif
        else
          value = (long)va_arg(args, unsigned int);
        fmtint (buffer, &currlen, maxlen, value, 10, min, max, flags);
        break;
      case 'X':
        flags |= DP_F_UP;
      case 'x':
        flags |= DP_F_UNSIGNED;
        if(cflags == DP_C_SHORT)
          value = va_arg(args, unsigned int);
        else if(cflags == DP_C_LONG)
          value = (long)va_arg(args, unsigned long int);
#ifdef HAVE_LONG_LONG
        else if(cflags == DP_C_LLONG)
          value = (LLONG)va_arg(args, unsigned LLONG);
#endif
        else
          value = (long)va_arg(args, unsigned int);
        fmtint (buffer, &currlen, maxlen, value, 16, min, max, flags);
        break;
    #ifdef WITH_FLOATINGPOINT
      case 'f':
        if(cflags == DP_C_LDOUBLE)
          fvalue = va_arg(args, LDOUBLE);
        else
          fvalue = va_arg(args, double);
        /* um, floating point? */
        fmtfp (buffer, &currlen, maxlen, fvalue, min, max, flags);
        break;
      case 'E':
        flags |= DP_F_UP;
      case 'e':
        if(cflags == DP_C_LDOUBLE)
          fvalue = va_arg(args, LDOUBLE);
        else
          fvalue = va_arg(args, double);
        break;
      case 'G':
        flags |= DP_F_UP;
      case 'g':
        if(cflags == DP_C_LDOUBLE)
          fvalue = va_arg(args, LDOUBLE);
        else
          fvalue = va_arg(args, double);
        break;
    #endif
      case 'c':
        dopr_outch (buffer, &currlen, maxlen, va_arg(args, int));
        break;
      case 's':
          strvalue = va_arg(args, char *);
        if(!strvalue)
          strvalue = (char *)"(NULL)";
        if(max == -1)
          max = strlen(strvalue);
        if(min > 0 && max >= 0 && min > max) max = min;
        fmtstr (buffer, &currlen, maxlen, strvalue, flags, min, max);
        break;
#ifdef WITH_POINTER
      case 'p':
        strvalue = va_arg(args, void *);
        fmtint (buffer, &currlen, maxlen, (long) strvalue, 16, min, max, flags);
        break;
#endif /* WITH_POINTER */
#ifdef WITH_N_INT
      case 'n':
        if(cflags == DP_C_SHORT)
        {
          short int *num;
          num = va_arg(args, short int *);
          *num = currlen;
        }
        else if(cflags == DP_C_LONG)
        {
          long int *num;
          num = va_arg(args, long int *);
          *num = (long int)currlen;
#ifdef HAVE_LONG_LONG
        }
        else if(cflags == DP_C_LLONG)
        {
          LLONG *num;
          num = va_arg(args, LLONG *);
          *num = (LLONG)currlen;
#endif
        }
        else
        {
          int *num;
          num = va_arg(args, int *);
          *num = currlen;
        }
        break;
#endif /* WITH_N_INT */
      case '%':
        dopr_outch(buffer, &currlen, maxlen, ch);
        break;
#ifdef WITH_WIDE_CHAR
      case 'w':
        /* not supported yet, treat as next char */
        ch = *format++;
        break;
#endif /* WITH_WIDE_CHAR */
      default:
        /* Unknown, skip */
        break;
      }
      ch = *format++;
      state = DP_S_DEFAULT;
      flags = cflags = min = 0;
      max = -1;
      break;
    case DP_S_DONE:
      break;
    default:
      /* hmm? */
      break; /* some picky compilers need this */
    }
  }
  if(maxlen != 0) {
    if(currlen < maxlen - 1)
      buffer[currlen] = '\0';
    else if(maxlen > 0)
      buffer[maxlen - 1] = '\0';
  }

  return currlen;
}

static void fmtstr(char *buffer, size_t *currlen, size_t maxlen,
        char *value, int flags, int min, int max)
{
  int padlen;     /* amount to pad */
  int cnt = 0;

#ifdef DEBUG_SNPRINTF
  printf("fmtstr min=%d max=%d s=[%s]\n", min, max, value);
#endif
  padlen = min - strlen(value);
  if(padlen < 0)
    padlen = 0;
  if(flags & DP_F_MINUS)
    padlen = -padlen; /* Left Justify */

  while((padlen > 0) && (cnt < max))
  {
    dopr_outch (buffer, currlen, maxlen, ' ');
    --padlen;
    ++cnt;
  }
  while(*value && (cnt < max))
  {
    dopr_outch (buffer, currlen, maxlen, *value++);
    ++cnt;
  }
  while((padlen < 0) && (cnt < max))
  {
    dopr_outch (buffer, currlen, maxlen, ' ');
    ++padlen;
    ++cnt;
  }
}

/* Have to handle DP_F_NUM (ie 0x and 0 alternates) */

static void fmtint(char *buffer, size_t *currlen, size_t maxlen,
        long value, int base, int min, int max, int flags)
{
  int signvalue = 0;
  unsigned long uvalue;
  char convert[20];
  int place = 0;
  int spadlen = 0; /* amount to space pad */
  int zpadlen = 0; /* amount to zero pad */
  int caps = 0;

  if(max < 0)
    max = 0;

  uvalue = value;

  if(!(flags & DP_F_UNSIGNED)) {
    if( value < 0 ) {
      signvalue = '-';
      uvalue = -value;
    } else {
      if(flags & DP_F_PLUS)  /* Do a sign (+/i) */
        signvalue = '+';
      else if(flags & DP_F_SPACE)
        signvalue = ' ';
    }
  }

  if(flags & DP_F_UP) caps = 1; /* Should characters be upper case? */

  do {
    convert[place++] =
      (caps? "0123456789ABCDEF":"0123456789abcdef")
      [uvalue % (unsigned)base  ];
    uvalue = (uvalue / (unsigned)base );
  } while(uvalue && (place < 20));
  if(place == 20) place--;
  convert[place] = 0;

  zpadlen = max - place;
  spadlen = min - MAX (max, place) - (signvalue ? 1 : 0);
  if(zpadlen < 0) zpadlen = 0;
  if(spadlen < 0) spadlen = 0;
  if(flags & DP_F_ZERO) {
    zpadlen = MAX(zpadlen, spadlen);
    spadlen = 0;
  }
  if(flags & DP_F_MINUS)
    spadlen = -spadlen; /* Left Justifty */

#ifdef DEBUG_SNPRINTF
  printf("zpad: %d, spad: %d, min: %d, max: %d, place: %d\n",
         zpadlen, spadlen, min, max, place);
#endif

  /* Spaces */
  while(spadlen > 0) {
    dopr_outch (buffer, currlen, maxlen, ' ');
    --spadlen;
  }

  /* Sign */
  if(signvalue)
    dopr_outch (buffer, currlen, maxlen, signvalue);

  /* Zeros */
  if(zpadlen > 0) {
    while(zpadlen > 0) {
      dopr_outch (buffer, currlen, maxlen, '0');
      --zpadlen;
    }
  }

  /* Digits */
  while(place > 0)
    dopr_outch (buffer, currlen, maxlen, convert[--place]);

  /* Left Justified spaces */
  while(spadlen < 0) {
    dopr_outch (buffer, currlen, maxlen, ' ');
    ++spadlen;
  }
}

#ifdef WITH_FLOATINGPOINT
static LDOUBLE abs_val(LDOUBLE value)
{
  LDOUBLE result = value;

  if(value < 0)
    result = -value;

  return result;
}
#endif

#ifdef WITH_FLOATINGPOINT
static LDOUBLE POW10(int exp)
{
  LDOUBLE result = 1;

  while(exp) {
    result *= 10;
    exp--;
  }

  return result;
}
#endif

#ifdef WITH_FLOATINGPOINT
static LLONG ROUND(LDOUBLE value)
{
  LLONG intpart;

  intpart = (LLONG)value;
  value = value - intpart;
  if(value >= 0.5) intpart++;

  return intpart;
}
#endif

#ifdef WITH_FLOATINGPOINT
/* a replacement for modf that doesn't need the math library. Should
   be portable, but slow */
static double my_modf(double x0, double *iptr)
{
  int i;
  long l = 0;
  double x = x0;
  double f = 1.0;

  for (i=0;i<100;i++) {
    l = (long)x;
    if(l <= (x+1) && l >= (x-1)) break;
    x *= 0.1;
    f *= 10.0;
  }

  if(i == 100) {
    /* yikes! the number is beyond what we can handle. What do we do? */
    (*iptr) = 0;
    return 0;
  }

  if(i != 0) {
    double i2;
    double ret;

    ret = my_modf(x0-l*f, &i2);
    (*iptr) = l*f + i2;
    return ret;
  }

  (*iptr) = l;
  return x - (*iptr);
}
#endif

#ifdef WITH_FLOATINGPOINT
static void fmtfp (char *buffer, size_t *currlen, size_t maxlen,
       LDOUBLE fvalue, int min, int max, int flags)
{
  int signvalue = 0;
  double ufvalue;
  static char iconvert[311];
  static char fconvert[311];
  size_t iplace = 0;
  size_t fplace = 0;
  int padlen = 0; /* amount to pad */
  int zpadlen = 0;
  int caps = 0;
  int index;
  double intpart;
  double fracpart;
  double temp;

  /*
   * AIX manpage says the default is 0, but Solaris says the default
   * is 6, and sprintf on AIX defaults to 6
   */
  if(max < 0)
    max = 6;

  ufvalue = abs_val (fvalue);

  if(fvalue < 0) {
    signvalue = '-';
  } else {
    if(flags & DP_F_PLUS) { /* Do a sign (+/i) */
      signvalue = '+';
    } else {
      if(flags & DP_F_SPACE)
        signvalue = ' ';
    }
  }

#if 0
  if(flags & DP_F_UP) caps = 1; /* Should characters be upper case? */
#endif

#if 0
   if(max == 0) ufvalue += 0.5; /* if max = 0 we must round */
#endif

  /*
   * Sorry, we only support 16 digits past the decimal because of our
   * conversion method
   */
  if(max > 16)
    max = 16;

  /* We "cheat" by converting the fractional part to integer by
   * multiplying by a factor of 10
   */

  temp = ufvalue;
  my_modf(temp, &intpart);

  fracpart = ROUND((POW10(max)) * (ufvalue - intpart));

  if(fracpart >= POW10(max)) {
    intpart++;
    fracpart -= POW10(max);
  }


  /* Convert integer part */
  do {
    temp = intpart*0.1;
    my_modf(temp, &intpart);
    index = (int) ((temp -intpart +0.05)* 10.0);
    /* index = (int) (((double)(temp*0.1) -intpart +0.05) *10.0); */
    /* printf ("%llf, %f, %x\n", temp, intpart, index); */
    iconvert[iplace++] =
      (caps? "0123456789ABCDEF":"0123456789abcdef")[index];
  } while(intpart && (iplace < sizeof(iconvert)));
  if(iplace == sizeof(iconvert)) iplace--;
  iconvert[iplace] = 0;

  /* Convert fractional part */
  if(fracpart)
  {
    do {
      temp = fracpart*0.1;
      my_modf(temp, &fracpart);
      index = (int) ((temp -fracpart +0.05)* 10.0);
      /* index = (int) ((((temp/10) -fracpart) +0.05) *10); */
      /* printf ("%lf, %lf, %ld\n", temp, fracpart, index); */
      fconvert[fplace++] =
      (caps? "0123456789ABCDEF":"0123456789abcdef")[index];
    } while(fracpart && (fplace < sizeof(fconvert)));
    if(fplace == sizeof(fconvert)) fplace--;
  }
  fconvert[fplace] = 0;

  /* -1 for decimal point, another -1 if we are printing a sign */
  padlen = min - iplace - max - 1 - ((signvalue) ? 1 : 0);
  zpadlen = max - fplace;
  if(zpadlen < 0) zpadlen = 0;
  if(padlen < 0)
    padlen = 0;
  if(flags & DP_F_MINUS)
    padlen = -padlen; /* Left Justifty */

  if((flags & DP_F_ZERO) && (padlen > 0)) {
    if(signvalue) {
      dopr_outch (buffer, currlen, maxlen, signvalue);
      --padlen;
      signvalue = 0;
    }
    while(padlen > 0) {
      dopr_outch (buffer, currlen, maxlen, '0');
      --padlen;
    }
  }
  while(padlen > 0) {
    dopr_outch (buffer, currlen, maxlen, ' ');
    --padlen;
  }
  if(signvalue)
    dopr_outch (buffer, currlen, maxlen, signvalue);

  while(iplace > 0)
    dopr_outch (buffer, currlen, maxlen, iconvert[--iplace]);

#ifdef DEBUG_SNPRINTF
  printf("fmtfp: fplace=%d zpadlen=%d\n", fplace, zpadlen);
#endif

  /*
   * Decimal point.  This should probably use locale to find the correct
   * char to print out.
   */
  if(max > 0) {
    dopr_outch (buffer, currlen, maxlen, '.');

    while(zpadlen > 0) {
      dopr_outch (buffer, currlen, maxlen, '0');
      --zpadlen;
    }

    while(fplace > 0)
      dopr_outch (buffer, currlen, maxlen, fconvert[--fplace]);
  }

  while(padlen < 0) {
    dopr_outch (buffer, currlen, maxlen, ' ');
    ++padlen;
  }
}
#endif

static void dopr_outch(char *buffer, size_t *currlen, size_t maxlen, char c)
{
  if(*currlen < maxlen)
  {
    buffer[(*currlen)] = c;
  }
  (*currlen)++;
}

/* yes this really must be a ||. Don't muck with this (tridge) */
#if !defined(HAVE_VSNPRINTF) || !defined(HAVE_C99_VSNPRINTF)
int vsnprintf(char *s, size_t maxlen, const char *format, va_list ap)
{
  return (int)dopr(s, maxlen, format, ap);
}
#endif

#endif

#ifndef HAVE_VSYSLOG
#ifdef HAVE_SYSLOG
void vsyslog (int facility_priority, char *format, va_list arglist)
{
  char *msg = NULL;

  vasprintf(&msg, format, arglist);
  if(!msg)
    return;
  syslog(facility_priority, "%s", msg);
  SAFE_FREE(msg);
}
#endif /* HAVE_SYSLOG */
#endif /* HAVE_VSYSLOG */

#ifdef TEST_SNPRINTF

int sprintf(char *,const char * fmt,...);

int main (void)
{
  char buf1[1024];
  char buf2[1024];
  char *fp_fmt[] = {
    "%1.1f",
    "%-1.5f",
    "%1.5f",
    "%123.9f",
    "%10.5f",
    "% 10.5f",
    "%+22.9f",
    "%+4.9f",
    "%01.3f",
    "%4f",
    "%3.1f",
    "%3.2f",
    "%.0f",
    "%f",
    "-16.16f",
    NULL
  };
  double fp_nums[] = { 6442452944.1234, -1.5, 134.21, 91340.2, 341.1234, 0203.9, 0.96, 0.996,
           0.9996, 1.996, 4.136, 5.030201, 0};
  char *int_fmt[] = {
    "%-1.5d",
    "%1.5d",
    "%123.9d",
    "%5.5d",
    "%10.5d",
    "% 10.5d",
    "%+22.33d",
    "%01.3d",
    "%4d",
    "%d",
    NULL
  };
  long int_nums[] = { -1, 134, 91340, 341, 0203, 0};
  char *str_fmt[] = {
    "10.5s",
    "5.10s",
    "10.1s",
    "0.10s",
    "10.0s",
    "1.10s",
    "%s",
    "%.1s",
    "%.10s",
    "%10s",
    NULL
  };
  char *str_vals[] = {"hello", "a", "", "a longer string", NULL};
  int x, y;
  int fail = 0;
  int num = 0;

  printf ("Testing snprintf format codes against system sprintf...\n");

  for (x = 0; fp_fmt[x] ; x++) {
    for (y = 0; fp_nums[y] != 0 ; y++) {
      int l1 = snprintf(NULL, 0, fp_fmt[x], fp_nums[y]);
      int l2 = snprintf(buf1, sizeof(buf1), fp_fmt[x], fp_nums[y]);
      sprintf (buf2, fp_fmt[x], fp_nums[y]);
      if(strcmp (buf1, buf2)) {
        printf("snprintf doesn't match Format: %s\n\tsnprintf = [%s]\n\t sprintf = [%s]\n",
               fp_fmt[x], buf1, buf2);
        fail++;
      }
      if(l1 != l2) {
        printf("snprintf l1 != l2 (%d %d) %s\n", l1, l2, fp_fmt[x]);
        fail++;
      }
      num++;
    }
  }

  for (x = 0; int_fmt[x] ; x++) {
    for (y = 0; int_nums[y] != 0 ; y++) {
      int l1 = snprintf(NULL, 0, int_fmt[x], int_nums[y]);
      int l2 = snprintf(buf1, sizeof(buf1), int_fmt[x], int_nums[y]);
      sprintf (buf2, int_fmt[x], int_nums[y]);
      if(strcmp (buf1, buf2)) {
        printf("snprintf doesn't match Format: %s\n\tsnprintf = [%s]\n\t sprintf = [%s]\n",
               int_fmt[x], buf1, buf2);
        fail++;
      }
      if(l1 != l2) {
        printf("snprintf l1 != l2 (%d %d) %s\n", l1, l2, int_fmt[x]);
        fail++;
      }
      num++;
    }
  }

  for (x = 0; str_fmt[x] ; x++) {
    for (y = 0; str_vals[y] != 0 ; y++) {
      int l1 = snprintf(NULL, 0, str_fmt[x], str_vals[y]);
      int l2 = snprintf(buf1, sizeof(buf1), str_fmt[x], str_vals[y]);
      sprintf (buf2, str_fmt[x], str_vals[y]);
      if(strcmp (buf1, buf2)) {
        printf("snprintf doesn't match Format: %s\n\tsnprintf = [%s]\n\t sprintf = [%s]\n",
               str_fmt[x], buf1, buf2);
        fail++;
      }
      if(l1 != l2) {
        printf("snprintf l1 != l2 (%d %d) %s\n", l1, l2, str_fmt[x]);
        fail++;
      }
      num++;
    }
  }

  printf ("%d tests failed out of %d.\n", fail, num);

  printf("seeing how many digits we support\n");
  {
    double v0 = 0.12345678901234567890123456789012345678901;
    for (x=0; x<100; x++) {
      double p = pow(10, x);
      double r = v0*p;
      snprintf(buf1, sizeof(buf1), "%1.1f", r);
      sprintf(buf2,                "%1.1f", r);
      if(strcmp(buf1, buf2)) {
        printf("we seem to support %d digits\n", x-1);
        break;
      }
    }
  }

  return 0;
}
#endif /* SNPRINTF_TEST */

#else
  #warning "NEED_VSNPRINTF missing or compilation unnecessary"
#endif

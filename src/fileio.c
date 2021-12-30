/**
 *  libkohn_avi
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: LGPL 2.1
 *
 * Copyright 2005-2021 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int write_int(FILE *out, uint32_t n)
{
  putc((n & 0xff), out);
  putc(((n >> 8) & 0xff), out);
  putc(((n >> 16) & 0xff), out);
  putc(((n >> 24) & 0xff) ,out);

  return 0;
}

int write_short(FILE *out, uint32_t n)
{
  putc((n & 0xff), out);
  putc(((n >> 8) & 0xff), out);

  return 0;
}

int write_chars(FILE *out, char *s)
{
  int t;

  t = 0;

  while(s[t] != 0 && t < 255)
  {
    putc(s[t++], out);
  }

  return 0;
}

int write_chars_bin(FILE *out, char *s, int count)
{
  int t;

  for (t = 0; t < count; t++)
  {
    putc(s[t], out);
  }

  return 0;
}


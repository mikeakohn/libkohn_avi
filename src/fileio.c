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

int read_int(FILE *in)
{
  int c;

  c = getc(in);
  c |= getc(in) << 8;
  c |= getc(in) << 16;
  c |= getc(in) << 24;

  return c;
}

int read_short(FILE *in)
{
  int c;

  c = getc(in);
  c |= getc(in) << 8;

  return c;
}

int read_chars(FILE *in, char *s, int count)
{
  int t;

  for (t = 0; t < count; t++)
  {
    s[t] = getc(in);
  }

  s[t] = 0;

  return 0;
}

int read_chars_bin(FILE *in, char *s, int count)
{
  int t;

  for (t = 0; t < count; t++)
  {
    s[t] = getc(in);
  }

  return 0;
}


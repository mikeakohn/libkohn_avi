/**
 *  libkohn_avi
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: LGPL 2.1
 *
 * Copyright 2021 by Michael Kohn
 *
 */

#ifndef FILE_IO_H
#define FILE_IO_H

int write_int(FILE *out, unsigned int n);
int write_short(FILE *out, unsigned int n);
int write_chars(FILE *out, char *s);
int write_chars_bin(FILE *out, char *s, int count);

#endif


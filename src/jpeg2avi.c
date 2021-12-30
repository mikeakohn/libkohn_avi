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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "kohn_avi.h"

int get_jpeg_dims(FILE *in, int *width, int *height)
{
  int ch, size, type, n;

  ch = getc(in);
  ch = getc(in);

  while (1)
  {
    ch = getc(in);
    if (ch != 0xff) { break; }
    if (ch == EOF) { break; }
    type = getc(in);

    //if (ch == 0xff)
    {
      ch = getc(in);

      size = (ch << 8) + getc(in);

      //printf("size=%d type=0xff 0%02x\n", size, type);

      if (type == 0xc0)
      {
        //ch = getc(in);
        //ch = getc(in);
        ch = getc(in);
        ch = getc(in);
        *height = (ch << 8) | getc(in);
        ch = getc(in);
        *width = (ch << 8) | getc(in);
        return 0;
      }
    }

    for (n = 0; n < size - 2; n++) { getc(in); }
  }

  return -1;
}

int count_frames(char *filename_template)
{
  FILE *in;
  char filename[1024] = { 0 };
  int t;

  printf("Scanning...\n");

  t = 0;

  while (1)
  {
    sprintf(filename,filename_template,t);
    //printf("Trying %s\n", filename);

    in = fopen(filename, "r");

    if (in == NULL) { break; }
    fclose(in);

    t++;

    if ((t % 1000) == 0) { printf("%d frames so far...\n",t); }
    if (strcmp(filename, filename_template) == 0) { break; }
  }

  return t;
}

int main(int argc, char *argv[])
{
  FILE *in;
  int fd;
  struct kavi_t *kavi;
  char filename[1024];
  uint8_t *buffer=0;
  int buffer_len=0;
  int fps,count,t,l,c,r;
  int width,height;
  struct stat file_stat;

  printf(
    "\njpeg2avi routines - Copyright 2005-2011 by Michael Kohn\n"
    "http://www.mikekohn.net/\n"
    "Version Date: 2011-Jan-30\n\n");

  if (argc!=4)
  {
    printf(
      "This program will take a set of numbered jpegs's and\n"
      "creates an avi/jpeg movie file.\n\n"
      "Usage: jpeg2avi <outfile.avi> <printf formatted filename> <frames per second>\n\n"
      "Example: jpeg2avi out.avi frames%%05d.jpeg 10\n\n");

    exit(0);
  }

  fps = atoi(argv[3]);

  if (fps < 0)
  {
    printf("Illegal value for frames per second\n");
    exit(1);
  }

  count = count_frames(argv[2]);
  printf("Found %d frames\n", count);

  sprintf(filename, argv[2], 0);
  in = fopen(filename, "rb");

  if (in == 0)
  {
    printf("Cannot open jpeg %s\n", filename);
    exit(1);
  }

  if (get_jpeg_dims(in, &width, &height) != 0)
  {
    printf("Cannot find width/height of jpeg\n");
    exit(1);
  }

  fclose(in);

  printf("Video width=%d height=%d\n", width, height);

  kavi = kavi_open(argv[1], width, height, "MJPG", atoi(argv[3]), NULL);

  if (kavi == NULL)
  {
    printf("Couldn't open AVI for writing\n");
    exit(1);
  }

  for (t = 0; t < count; t++)
  {
    sprintf(filename, argv[2], t);
    fd = open(filename, O_RDONLY);
    if (fd == -1) { break; }

    fstat(fd, &file_stat);
    l = file_stat.st_size;

    if (l > buffer_len)
    {
      if (buffer_len != 0) { free(buffer); }

      buffer_len = l;
      buffer = (uint8_t *)malloc(buffer_len);
    }

    c = 0;

    while(c < l)
    {
      r = read(fd, buffer + c, l - c);

      c = c + r;
      if (r <= 0) { break; }
    }

    close(fd);

    kavi_add_frame(kavi,buffer, l);

    if ((t % 1000) == 0) { printf("%d frames so far...\n", t); }
  }

  if (buffer_len != 0) { free(buffer); }

  kavi_close(kavi);

  return 0;
}


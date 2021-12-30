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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "kohn_avi.h"

int running = 1;

void handler(int signal_num)
{
  printf("Signal caught\n");
  running=0;
}

int get_jpeg_dims(FILE *in, int *width, int *height)
{
  int ch;

  while((ch = getc(in)) != EOF)
  {
    if (ch == 0xff)
    {
      ch = getc(in);

      if (ch == 0xc0)
      {
        ch = getc(in);
        ch = getc(in);
        ch = getc(in);
        ch = getc(in);
        *height = (ch << 8) | getc(in);
        ch = getc(in);
        *width = (ch << 8) | getc(in);
        return 0;
      }
    }
  }

  return -1;
}

int main(int argc, char *argv[])
{
  FILE *in;
  struct kavi_t *kavi;
  uint8_t *buffer = 0;
  int buffer_len=0;
  int fps, t, c, r, delay;
  int width, height;

  printf(
    "\naxis2avi routines - Copyright 2005-2022 by Michael Kohn\n"
    "http://www.mikekohn.net/\n"
    "Version Date: 2021-Dec-30\n\n");

  if (argc != 4)
  {
    printf(
      "This program will take a set of numbered jpegs's and\n"
      "creates an avi/jpeg movie file.\n\n"
      "Usage: axis2avi <outfile.avi> <capture delay in second> <frames per second>\n\n"
      "Example: axis2avi out.avi 60 10\n\n");

    exit(0);
  }

  delay = atoi(argv[2]);
  fps = atoi(argv[3]);

  if (fps < 0)
  {
    printf("Illegal value for frames per second\n");
    exit(1);
  }

  in = popen("/usr/bin/wget -O - http://root:root@192.168.1.211/jpg/image.jpg","r");

  if (in == NULL)
  {
    printf("Cannot open jpeg\n");
    exit(1);
  }

  if (get_jpeg_dims(in, &width, &height) != 0)
  {
    printf("Cannot find width/height of jpeg\n");
    exit(1);
  }

  pclose(in);

  printf("Video width=%d height=%d\n", width, height);

  kavi = kavi_open(argv[1], width, height, "MJPG", fps, NULL);

  if (kavi == NULL)
  {
    printf("Couldn't open AVI for writing\n");
    exit(1);
  }

  signal(SIGTERM,handler);
  signal(SIGINT,handler);
  t = 0;

  while (running == 1)
  {
    in = popen("/usr/bin/wget -O - http://root:root@192.168.1.211/jpg/image.jpg","r");
    if (in == NULL) { break; }

    // FIXME: What in the world?
    // if (l > buffer_len)
    {
      if (buffer_len != 0) { free(buffer); }
      buffer_len = 128000;
      buffer = (uint8_t *)malloc(buffer_len);
    }

    c = 0;

    while(1)
    {
      r = fread(buffer + c, 1, 1024, in);

      if (r <= 0) { break; }
      c = c + r;
    }

    pclose(in);

    kavi_add_frame(kavi,buffer, c);

    if (delay != 0)
    {
      sleep(delay);
    }
      else
    {
       int ch = getchar();
       if (ch == 'q' || ch == 'Q') { running = 0; }
    }

    if ((t % 1000) == 0) { printf("%d frames so far...\n", t); }
    t++;
  }

  if (buffer_len != 0) { free(buffer); }

  kavi_close(kavi);

  return 0;
}


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

#include "kohn_avi.h"

static int get_jpeg_dims(FILE *in, int *width, int *height, int *file_len)
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
        fseek(in, 0, SEEK_END);
        *file_len = (int)ftell(in);
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
  int buffer_len = 0;
  int t;
  int width,height;
  uint8_t snd_buffer[22050];
  struct kohn_avi_audio_t audio;

  printf("\njpeg2avi routines - Copyright 2005-2021 by Michael Kohn\n");
  printf("https://www.mikekohn.net/\n");

  in = fopen("test.jpeg", "rb");

  if (in == 0)
  {
    printf("Cannot open test.jpeg\n");
    exit(1);
  }

  if (get_jpeg_dims(in, &width, &height, &buffer_len) != 0)
  {
    printf("Cannot find width/height of jpeg\n");
    exit(1);
  }

  fseek(in, 0, SEEK_SET);

  // DEBUG
  width = 320;
  height = 240;

  printf("jpeg is %dx%d with a filesize of %d\n", width, height, buffer_len);
  buffer = (uint8_t *)malloc(buffer_len);
  t = fread(buffer, 1, buffer_len, in);
  fclose(in);

  // Make some stupid sound.
  for (t = 0; t < 22050; t++)
  {
    snd_buffer[t] = t & 0xff;
  }

  audio.channels = 1;
  audio.bits = 8;
  audio.samples_per_second = 22050;

  kavi = kavi_open("out.avi", width, height, "MJPG", 30, &audio);

  if (kavi == NULL)
  {
    printf("Couldn't open AVI for writing\n");
    exit(1);
  }

  for (t = 0; t < 300; t++)
  {
    kavi_add_frame(kavi, buffer, buffer_len);

    if ((t + 1) % 30 == 0)
    {
      kavi_add_audio(kavi, snd_buffer, 22050);
    }

    if ((t % 1000) == 0) { printf("%d frames so far...\n",t); }
  }

  if (buffer_len != NULL) { free(buffer); }

  kavi_close(kavi);

  return 0;
}


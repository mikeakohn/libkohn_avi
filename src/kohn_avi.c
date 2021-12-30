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
#include "fileio.h"

static int write_avi_header(FILE *out, struct kohn_avi_header_t *avi_header)
{
  int marker, t;

  write_chars_bin(out, "avih", 4);
  marker = ftell(out);
  write_int(out, 0);

  write_int(out, avi_header->time_delay);
  write_int(out, avi_header->data_rate);
  write_int(out, avi_header->reserved);
  write_int(out, avi_header->flags);
  write_int(out, avi_header->number_of_frames);
  write_int(out, avi_header->initial_frames);
  write_int(out, avi_header->data_streams);
  write_int(out, avi_header->buffer_size);
  write_int(out, avi_header->width);
  write_int(out, avi_header->height);
  write_int(out, avi_header->time_scale);
  write_int(out, avi_header->playback_data_rate);
  write_int(out, avi_header->starting_time);
  write_int(out, avi_header->data_length);

  t = ftell(out);
  fseek(out,marker, SEEK_SET);
  write_int(out, t - marker - 4);
  fseek(out, t, SEEK_SET);

  return 0;
}

static int write_stream_header(FILE *out, struct kohn_stream_header_t *stream_header)
{
  int marker,t;

  write_chars_bin(out, "strh", 4);
  marker = ftell(out);
  write_int(out, 0);

  write_chars_bin(out, stream_header->data_type, 4);
  write_chars_bin(out, stream_header->codec, 4);
  write_int(out, stream_header->flags);
  write_int(out, stream_header->priority);
  write_int(out, stream_header->initial_frames);
  write_int(out, stream_header->time_scale);
  write_int(out, stream_header->data_rate);
  write_int(out, stream_header->start_time);
  write_int(out, stream_header->data_length);
  write_int(out, stream_header->buffer_size);
  write_int(out, stream_header->quality);
  write_int(out, stream_header->sample_size);
  write_int(out, 0);
  write_int(out, 0);

  t = ftell(out);
  fseek(out, marker, SEEK_SET);
  write_int(out, t - marker - 4);
  fseek(out, t, SEEK_SET);

  return 0;
}

static int write_stream_format_v(FILE *out, struct kohn_stream_format_v_t *stream_format_v)
{
  int marker, t;

  write_chars_bin(out, "strf", 4);
  marker = ftell(out);
  write_int(out, 0);

  write_int(out, stream_format_v->header_size);
  write_int(out, stream_format_v->width);
  write_int(out, stream_format_v->height);
  write_short(out, stream_format_v->num_planes);
  write_short(out, stream_format_v->bits_per_pixel);
  write_int(out, stream_format_v->compression_type);
  write_int(out, stream_format_v->image_size);
  write_int(out, stream_format_v->x_pels_per_meter);
  write_int(out, stream_format_v->y_pels_per_meter);
  write_int(out, stream_format_v->colors_used);
  write_int(out, stream_format_v->colors_important);

  if (stream_format_v->colors_used != 0)
  {
    for (t = 0; t < stream_format_v->colors_used; t++)
    {
      putc(stream_format_v->palette[t] & 0xff, out);
      putc((stream_format_v->palette[t] >> 8) & 0xff, out);
      putc((stream_format_v->palette[t] >> 16) & 0xff, out);
      putc(0, out);
    }
  }

  t = ftell(out);
  fseek(out, marker, SEEK_SET);
  write_int(out, t - marker - 4);
  fseek(out, t, SEEK_SET);

  return 0;
}

static int write_stream_format_a(FILE *out, struct kohn_stream_format_a_t *stream_format_a)
{
  int marker,t;

  write_chars_bin(out, "strf", 4);
  marker = ftell(out);
  write_int(out, 0);

  write_short(out, stream_format_a->format_type);
  write_short(out, stream_format_a->channels);
  write_int(out, stream_format_a->sample_rate);
  write_int(out, stream_format_a->bytes_per_second);
  write_short(out, stream_format_a->block_align);
  write_short(out, stream_format_a->bits_per_sample);
  write_short(out, stream_format_a->size);

  t = ftell(out);
  fseek(out, marker, SEEK_SET);
  write_int(out, t - marker - 4);
  fseek(out, t, SEEK_SET);

  return 0;
}

static int write_junk_chunk(FILE *out)
{
  int marker, t;
  int r, l, p;
  char *junk = { "JUNK IN THE CHUNK! " };

  write_chars_bin(out, "JUNK", 4);
  marker = ftell(out);
  write_int(out, 0);

  r = 4096 - ftell(out);
  l = strlen(junk);
  p = 0;

  for (t = 0; t < r; t++)
  {
    putc(junk[p++], out);
    if (p >= l) { p = 0; }
  }

  t = ftell(out);
  fseek(out, marker, SEEK_SET);
  write_int(out, t - marker - 4);
  fseek(out, t, SEEK_SET);

  return 0;
}

static int write_avi_header_chunk(struct kavi_t *kavi)
{
  long marker,t;
  long sub_marker;
  FILE *out=kavi->out;

  write_chars_bin(out, "LIST", 4);
  marker = ftell(out);
  write_int(out, 0);
  write_chars_bin(out, "hdrl", 4);
  write_avi_header(out, &kavi->avi_header);

  write_chars_bin(out, "LIST", 4);
  sub_marker = ftell(out);
  write_int(out, 0);
  write_chars_bin(out, "strl", 4);
  write_stream_header(out, &kavi->stream_header_v);
  write_stream_format_v(out, &kavi->stream_format_v);

  t = ftell(out);
  fseek(out,sub_marker, SEEK_SET);
  write_int(out, t - sub_marker - 4);
  fseek(out, t, SEEK_SET);

  if (kavi->avi_header.data_streams == 2)
  {
    write_chars_bin(out, "LIST", 4);
    sub_marker = ftell(out);
    write_int(out, 0);
    write_chars_bin(out, "strl", 4);
    write_stream_header(out, &kavi->stream_header_a);
    write_stream_format_a(out, &kavi->stream_format_a);

    t = ftell(out);
    fseek(out, sub_marker, SEEK_SET);
    write_int(out, t - sub_marker - 4);
    fseek(out, t, SEEK_SET);
  }

  t = ftell(out);
  fseek(out, marker, SEEK_SET);
  write_int(out, t - marker - 4);
  fseek(out, t, SEEK_SET);

  write_junk_chunk(out);

  return 0;
}

static int write_index(FILE *out, int count, unsigned int *offsets)
{
  int marker, t;
  int offset = 4;

  if (offsets == 0) { return -1; }

  write_chars_bin(out, "idx1", 4);
  marker = ftell(out);
  write_int(out, 0);

  for (t = 0; t < count; t++)
  {
    if ((offsets[t] & 0x80000000) == 0)
    {
      write_chars(out, "00dc");
    }
      else
    {
      write_chars(out, "01wb");
      offsets[t] &= 0x7fffffff;
    }

    write_int(out, 0x10);
    write_int(out, offset);
    write_int(out, offsets[t]);
    offset = offset + offsets[t] + 8;
  }

  t = ftell(out);
  fseek(out,marker, SEEK_SET);
  write_int(out, t - marker - 4);
  fseek(out, t, SEEK_SET);

  return 0;
}

#ifdef RLE8
static int color_distance(int c1, int c2)
{
  int d;

  // Note: This function is slooooow.

  d = abs((c1 & 0xff) - (c2 & 0xff)) +
      abs(((c1 >> 8) & 0xff) - ((c2 >> 8) & 0xff)) +
      abs(((c1 >> 16) & 0xff) - ((c2 >> 16) & 0xff));

  return d / 3;
}

int get_color(int col, struct stream_format_t *stream_format_v)
{
  int d, c;

  for (c = 0; c < stream_format_v->palette_count; c++)
  {
    if (col == stream_format_v->palette[c]) { return c; }
  }

  if (stream_format_v->palette_count != 256)
  {
    stream_format_v->palette[stream_format_v->palette_count++] = col;
    return c;
  }

  // If this part of the function is reached.. life will be slow.

  for (d = 1;  d < 255; d++)
  {
    for (c=0; c<stream_format_v->palette_count; c++)
    {
      if (color_distance(col,stream_format_v->palette[c])<=d) return c;
    }
  }

  return 0;
}

int rle8_write_line(
  FILE *out,
  unsigned int *image,
  int width,
  struct stream_format_v_t *stream_format_v)
{
  int count;
  int r, t, k, out_count;

  out_count = 0;
  t = 0;

  while (t < width)
  {
    if (t == width - 1)
    {
      putc(1, out);
      putc(get_color(image[t],stream_format_v), out);
      t++;
      break;
    }
      else
    if (image[t] == image[t + 1])
    {
      count=1;
      k=t;
      while (image[t] == image[t + 1])
      {
        count++;
        t++;
        if ((t + 1) == width) { break;
        if (count == 255) { break; }
      }

      putc(count, out);
      putc(get_color(image[t], stream_format_v), out);
      out_count = out_count + 2;
    }
      else
    {
      r = t;
      count = 0;

      while (image[t] != image[t + 1])
      {
        count++;
        t++;
        if ((t + 1) == width) { break; }
        if (count == 255) { break; }
      }

      if ((t + 1) == width && count != 255) { count++; t++; }

      if (count == 1)
      {
        putc(1, out);
        putc(get_color(image[r], stream_format_v), out);
        out_count += 2;
        continue;
      }
        else
      if (count == 2)
      {
        putc(1, out);
        putc(get_color(image[r + 0], stream_format_v), out);
        putc(1,out);
        putc(get_color(image[r + 1], stream_format_v), out);
        out_count+=4;
        continue;
      }
        else
      {
        putc(0, out);
        putc(count, out);

        t = r;
        for (k = 0; k < count; k++)
        {
          putc(get_color(image[t++], stream_format_v), out);
        }
        if ((count % 2) != 0) { putc(0,out); }

        out_count += (2 + count);
        continue;
      }

      t--;

    }
    t++;
  }

  putc(0, out);
  putc(0, out);

  return 0;
}
#endif

struct kavi_t *kavi_open(
  char *filename,
  int width,
  int height,
  char *fourcc,
  int fps,
  struct kohn_avi_audio_t *audio)
{
  struct kavi_t *kavi;
  FILE *out;

  out = fopen(filename,"wb+");

  if (out == NULL)
  {
    return NULL;
  }

  kavi = (struct kavi_t *)malloc(sizeof(struct kavi_t));
  memset(kavi, 0, sizeof(struct kavi_t));

  kavi->out = out;

  // Set avi header.
  kavi->avi_header.time_delay = 1000000 / fps;
  kavi->avi_header.data_rate = width * height * 3;
  kavi->avi_header.flags = 0x10;
  if (audio == NULL)
  {
    kavi->avi_header.data_streams = 1;
  }
    else
  {
    kavi->avi_header.data_streams = 2;
  }
  kavi->avi_header.number_of_frames = 0; // FIXME: This needs to be reset.
  kavi->avi_header.width = width;
  kavi->avi_header.height = height;
  kavi->avi_header.buffer_size = width * height * 3;

  // Set stream header.
  memcpy(kavi->stream_header_v.data_type, "vids", 4);
  memcpy(kavi->stream_header_v.codec, fourcc, 4);
  kavi->stream_header_v.time_scale = 1;
  kavi->stream_header_v.data_rate = fps;
  kavi->stream_header_v.buffer_size = width * height * 3;
  kavi->stream_header_v.data_length = 0;

  // Set stream format.
  kavi->stream_format_v.header_size = 40;
  kavi->stream_format_v.width = width;
  kavi->stream_format_v.height = height;
  kavi->stream_format_v.num_planes = 1;
  kavi->stream_format_v.bits_per_pixel = 24;
  kavi->stream_format_v.compression_type =
    ((uint32_t)fourcc[3] << 24) |
    ((uint32_t)fourcc[2] << 16) |
    ((uint32_t)fourcc[1] << 8) |
    ((uint32_t)fourcc[0]);
  kavi->stream_format_v.image_size = width * height * 3;
  kavi->stream_format_v.colors_used = 0;
  kavi->stream_format_v.colors_important = 0;

  kavi->stream_format_v.palette = 0;
  kavi->stream_format_v.palette_count = 0;

/*
  for (t = 0; t < stream_format_v->colors_used; t++)
  {
    stream_format_v->palette[t] = 0;
  }
*/

  if (kavi->avi_header.data_streams == 2)
  {
    // Set stream header.
    memcpy(kavi->stream_header_a.data_type, "auds", 4);
    //memcpy(kavi->stream_header_a.codec,uncompressed, 4);
    //kavi->stream_header_a.codec[0]  =0;
    kavi->stream_header_a.codec[0] = 1;
    kavi->stream_header_a.codec[1] = 0;
    kavi->stream_header_a.codec[2] = 0;
    kavi->stream_header_a.codec[3] = 0;
    kavi->stream_header_a.time_scale =1;
    kavi->stream_header_a.data_rate = audio->samples_per_second;
    kavi->stream_header_a.buffer_size =
      audio->channels * (audio->bits / 8) * audio->samples_per_second;
    kavi->stream_header_a.quality = -1;
    kavi->stream_header_a.sample_size = (audio->bits / 8) * audio->channels;

    // Set stream format.
    kavi->stream_format_a.format_type = 1;
    //kavi->stream_format_a.format_type = 85;
    kavi->stream_format_a.channels = audio->channels;
    kavi->stream_format_a.sample_rate = audio->samples_per_second;
    kavi->stream_format_a.bytes_per_second =
      audio->channels * (audio->bits / 8) * audio->samples_per_second;
    kavi->stream_format_a.block_align = audio->channels * (audio->bits / 8);
    kavi->stream_format_a.bits_per_sample = audio->bits;
    kavi->stream_format_a.size = 0;
  }

  write_chars_bin(out, "RIFF", 4);
  write_int(out, 0);
  write_chars_bin(out, "AVI ", 4);

  // kavi->marker=ftell(kavi->out);
  //write_avi_header_chunk(out, &kavi->avi_header, &kavi->stream_header_v, &kavi->stream_format_v);
  write_avi_header_chunk(kavi);

  write_chars_bin(out, "LIST", 4);
  kavi->marker = ftell(out);
  write_int(out, 0);
  write_chars_bin(out, "movi", 4);

  kavi->offsets_len = 1024;
  kavi->offsets = malloc(kavi->offsets_len * sizeof(int));
  kavi->offsets_ptr = 0;

  return kavi;
}

void kavi_set_framerate(struct kavi_t *kavi, float fps)
{
  kavi->stream_header_v.data_rate = fps;
  kavi->avi_header.time_delay = (int)(1000000.0 / fps);
}

void kavi_set_codec(struct kavi_t *kavi, const char *fourcc)
{
  memcpy(kavi->stream_header_v.codec,fourcc, 4);
  kavi->stream_format_v.compression_type=
    ((uint32_t)fourcc[3] << 24) |
    ((uint32_t)fourcc[2] << 16) |
    ((uint32_t)fourcc[1] << 8) |
    ((uint32_t)fourcc[0]);
}

void kavi_set_size(struct kavi_t *kavi, int width, int height)
{
  kavi->avi_header.data_rate = width * height * 3;
  kavi->avi_header.width = width;
  kavi->avi_header.height = height;
  kavi->avi_header.buffer_size = width * height * 3;
  kavi->stream_header_v.buffer_size = width * height * 3;
  kavi->stream_format_v.width = width;
  kavi->stream_format_v.height = height;
  kavi->stream_format_v.image_size = width * height * 3;
}

void kavi_add_frame(struct kavi_t *kavi, unsigned char *buffer, int len)
{
  int maxi_pad;  /* if your frame is raggin, give it some paddin' */
  int t;

  kavi->offset_count++;
  kavi->stream_header_v.data_length++;

  maxi_pad = len % 4;
  if (maxi_pad > 0) { maxi_pad = 4 - maxi_pad; }

  if (kavi->offset_count >= kavi->offsets_len)
  {
    kavi->offsets_len += 1024;
    kavi->offsets = realloc(kavi->offsets,kavi->offsets_len * sizeof(int));
  }

  kavi->offsets[kavi->offsets_ptr++] = len + maxi_pad;

  write_chars_bin(kavi->out,"00dc", 4);
  write_int(kavi->out, len + maxi_pad);

  t=fwrite(buffer, 1, len,kavi->out);

  for (t = 0; t < maxi_pad; t++) { putc(0,kavi->out); }
}

void kavi_add_audio(struct kavi_t *kavi, unsigned char *buffer, int len)
{
  int maxi_pad;  /* incase audio bleeds over the 4 byte boundary  */
  int t;

  kavi->offset_count++;

  maxi_pad=len % 4;
  if (maxi_pad > 0) { maxi_pad = 4 - maxi_pad; }

  if (kavi->offset_count >= kavi->offsets_len)
  {
    kavi->offsets_len += 1024;
    kavi->offsets = realloc(kavi->offsets, kavi->offsets_len * sizeof(int));
  }

  kavi->offsets[kavi->offsets_ptr++] = (len + maxi_pad) | 0x80000000;

  write_chars_bin(kavi->out, "01wb", 4);
  write_int(kavi->out, len + maxi_pad);

  t = fwrite(buffer, 1, len, kavi->out);

  for (t = 0; t < maxi_pad; t++) { putc(0, kavi->out); }

  kavi->stream_header_a.data_length += len+maxi_pad;
}

void kavi_close(struct kavi_t *kavi)
{
  long t;

  t = ftell(kavi->out);
  fseek(kavi->out, kavi->marker, SEEK_SET);
  write_int(kavi->out, t - kavi->marker - 4);
  fseek(kavi->out, t, SEEK_SET);

  write_index(kavi->out, kavi->offset_count, kavi->offsets);

  free(kavi->offsets);

  // Reset some avi header fields.
  kavi->avi_header.number_of_frames = kavi->stream_header_v.data_length;

  t = ftell(kavi->out);
  fseek(kavi->out, 12, SEEK_SET);
  write_avi_header_chunk(kavi);
  fseek(kavi->out, t ,SEEK_SET);

  t = ftell(kavi->out);
  fseek(kavi->out, 4, SEEK_SET);
  write_int(kavi->out, t - 8);
  fseek(kavi->out,t,SEEK_SET);

  if (kavi->stream_format_v.palette != NULL)
  {
    free(kavi->stream_format_v.palette);
  }

  fclose(kavi->out);
  free(kavi);
}


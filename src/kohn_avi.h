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

#ifndef KOHN_AVI_H
#define KOHN_AVI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

struct kohn_avi_audio_t
{
  int channels;
  int bits;
  int samples_per_second;
};

struct kohn_avi_header_t
{
  int time_delay;
  int data_rate;
  int reserved;
  int flags;
  int number_of_frames;
  int initial_frames;
  int data_streams;
  int buffer_size;
  int width;
  int height;
  int time_scale;
  int playback_data_rate;
  int starting_time;
  int data_length;
};

struct kohn_stream_header_t
{
  char data_type[5];
  char codec[5];
  int flags;
  int priority;
  int initial_frames;
  int time_scale;
  int data_rate;
  int start_time;
  int data_length;
  int buffer_size;
  int quality;
  int sample_size;
};

struct kohn_stream_format_v_t
{
  uint32_t header_size;
  uint32_t width;
  uint32_t height;
  uint16_t int num_planes;
  uint16_t int bits_per_pixel;
  uint32_t compression_type;
  uint32_t image_size;
  uint32_t x_pels_per_meter;
  uint32_t y_pels_per_meter;
  uint32_t colors_used;
  uint32_t colors_important;
  uint32_t *palette;
  uint32_t palette_count;
};

struct kohn_stream_format_a_t
{
  uint16_t format_type;
  uint16_t channels;
  uint32_t sample_rate;
  uint32_t bytes_per_second;
  uint16_t block_align;
  uint16_t bits_per_sample;
  uint16_t size;
};

struct kohn_avi_index_t
{
  int id;
  int flags;
  int offset;
  int length;
};

struct kavi_t
{
  FILE *out;
  struct kohn_avi_header_t avi_header;
  struct kohn_stream_header_t stream_header_v;
  struct kohn_stream_format_v_t stream_format_v;
  struct kohn_stream_header_t stream_header_a;
  struct kohn_stream_format_a_t stream_format_a;
  long marker;   /* movi marker */
  int offsets_ptr;
  int offsets_len;
  long offsets_start;
  uint32_t *offsets;
  int offset_count;
};

struct kavi_t *kavi_open(char *filename, int width, int height, char *fourcc, int fps, struct kohn_avi_audio_t *audio);
void kavi_add_frame(struct kavi_t *kavi, uint8_t *buffer, int len);
void kavi_add_audio(struct kavi_t *kavi, uint8_t *buffer, int len);
void kavi_close(struct kavi_t *kavi);

// If needed, these functions can be called before closing the file to
// change the framerate, codec, size. Note: AVI can only have a single
// frame rate, codec, size for the whole file so this affects anything
// recorded before these functions are called. These were added so
// libkohn_rtsp.so could open the file first, open the RTSP next, and
// later reset framerate, codec, and size without losing data.
void kavi_set_framerate(struct kavi_t *kavi, float fps);
void kavi_set_codec(struct kavi_t *kavi, const char *fourcc);
void kavi_set_size(struct kavi_t *kavi, int width, int height);

#ifdef __cplusplus
}
#endif

#endif


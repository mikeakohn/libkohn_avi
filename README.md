# libkohn_avi

Small library for creating AVI files along with a command line tool called
readavi for dumping AVI file structures as text for debugging.

Website: https://www.mikekohn.net/file_formats/libkohn_avi.php

This is some pretty old code that I pulled out of my old svn repo
and cleaned up a little bit. It still needs a bit more cleanup, but
it's been quite useful still.

There are some samples how to use the library in samples/ which
include a program that can grab frames from an Axis camera for creating
an MJPEG video and jpeg2avi which can read a series of jpeg's off
from the local filesystem to build an MJPEG.

The main library has the function calls:

    struct kavi_t *kavi_open(
      char *filename,
      int width,
      int height,
      char *fourcc,
      int fps,
      struct kohn_avi_audio_t *audio);

    void kavi_add_frame(struct kavi_t *kavi, uint8_t *buffer, int len);
    void kavi_add_audio(struct kavi_t *kavi, uint8_t *buffer, int len);
    void kavi_close(struct kavi_t *kavi);

Along with:

    // If needed, these functions can be called before closing the file to
    // change the framerate, codec, size. Note: AVI can only have a single
    // frame rate, codec, size for the whole file so this affects anything
    // recorded before these functions are called. These were added so
    // libkohn_rtsp.so could open the file first, open the RTSP next, and
    // later reset framerate, codec, and size without losing data.
    void kavi_set_framerate(struct kavi_t *kavi, float fps);
    void kavi_set_codec(struct kavi_t *kavi, const char *fourcc);
    void kavi_set_size(struct kavi_t *kavi, int width, int height);



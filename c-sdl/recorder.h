#ifndef RECORDER_H
#define RECORDER_H

#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>

#include <SDL.h>


void putPixel(SDL_Surface *screen, int x, int y, Uint32 pixel);

Uint32 getPixel(SDL_Surface *screen, int x, int y);

void av_init(char *filename, FILE **f, int codec_id, int width, int height, int fps,
          AVCodec **codec, AVCodecContext **c, AVFrame **frame);

void av_encode_frame(SDL_Surface *screen, int i, FILE **f, 
         AVCodec **codec, AVCodecContext **c, AVFrame **frame);

void av_close(FILE **f, AVCodec **codec, 
                  AVCodecContext **c, AVFrame **frame);

#endif

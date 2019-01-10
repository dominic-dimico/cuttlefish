#include "recorder.h"


void putPixel(SDL_Surface *screen, int x, int y, Uint32 pixel) {
    Uint8 *p = (Uint8*)screen->pixels 
             + y * screen->pitch 
             + x * screen->format->BytesPerPixel;
    *(Uint32 *)p = pixel;
}


Uint32 getPixel(SDL_Surface *screen, int x, int y) {
    Uint8 *p = (Uint8*)screen->pixels 
             + y * screen->pitch 
             + x * screen->format->BytesPerPixel;
    return *(Uint32 *)p;
}


void av_init(char *filename, FILE **f, int codec_id, int width, int height, int fps,
          AVCodec **codec, AVCodecContext **c, AVFrame **frame) {

    *f = fopen(filename, "wb");
    if (!*f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    /* find the video encoder **/
    *codec = avcodec_find_encoder(codec_id);
    if (!*codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    *c = avcodec_alloc_context3(*codec);
    if (!*c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    (*c)->bit_rate = 400000;
    (*c)->width    = width;
    (*c)->height   = height;
    (*c)->time_base = (AVRational){1,fps};
    (*c)->gop_size = 10;
    (*c)->max_b_frames = 1;
    (*c)->pix_fmt = AV_PIX_FMT_YUV420P;

    /* open it **/
    if (avcodec_open2(*c, *codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    *frame = av_frame_alloc();
    if (!*frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    (*frame)->format = (*c)->pix_fmt;
    (*frame)->width  = (*c)->width;
    (*frame)->height = (*c)->height;

    int ret = av_image_alloc((*frame)->data, (*frame)->linesize, 
                         (*c)->width, (*c)->height, (*c)->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }

}

void av_encode_frame(SDL_Surface *screen, int i, FILE **f, 
         AVCodec **codec, AVCodecContext **c, AVFrame **frame) {
 
    AVPacket pkt;

    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    fflush(stdout);

    int x, y;

     (*frame)->pts = i;
     Uint32 pixel;
     Uint8 R, G, B;
     Uint8 Y, U, V;

    for (y = 0; y < (*c)->height; y++) {
        for (x = 0; x < (*c)->width; x++) {
            pixel = getPixel(screen, x, y);
            R = (0xFF&(pixel>>16));
            G = (0xFF&(pixel>>8));
            B = (0xFF&pixel);
            Y = .299*R + .587*G + .114*B;
            (*frame)->data[0][y * (*frame)->linesize[0] + x] = Y;
        }
     }

    for (y = 0; y < (*c)->height/2; y++) {
        for (x = 0; x < (*c)->width/2; x++) {
            pixel = getPixel(screen, x, y);
            R = ((0x00FF0000&pixel)>>16);
            G = ((0x0000FF00&pixel)>>8);
            B = ((0x000000FF&pixel));
            Y = .299*R + .587*G + .114*B;
            U = .492*(B-Y)+0x7F;
            V = .877*(R-Y)+0x7F;
            (*frame)->data[1][y * (*frame)->linesize[1] + x] = U;
            (*frame)->data[2][y * (*frame)->linesize[2] + x] = V;
        }
        //printf("RGBYUV %d %d %d %d %d %d\n ", R, G, B, Y, U, V);
     }


     int got_output = 0;
     int ret = avcodec_encode_video2(*c, &pkt, *frame, &got_output);
     if (ret < 0) {
        fprintf(stderr, "Error encoding frame\n");
     }

     if (got_output) {
        //printf("Write frame %3d (size=%5d)\n", i, pkt.size);
        fwrite(pkt.data, 1, pkt.size, *f);
        av_packet_unref(&pkt);
     }

}

void av_close(FILE **f, AVCodec **codec, 
                  AVCodecContext **c, AVFrame **frame) {

    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    fwrite(endcode, 1, sizeof(endcode), *f);

    fclose(*f);

    avcodec_close(*c);
    av_free(*c);
    av_freep(&(*frame)->data[0]);
    av_frame_free(&*frame);
    printf("\n");

}


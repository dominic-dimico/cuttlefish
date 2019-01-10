#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>

#include "recorder.h"

struct options {
 int height;
 int width;
 int seconds;
 int spirals;
 int thickness;
 int framerate;
 int range;
 bool invert;
 bool inward;
 bool record;
 bool display;
 float sway;
 float period;
 float accel;
 float swayaccel;
 char* filename;
};

void usage(char *progname) {
  printf(" -h    --height \n         the window height\n");
  printf(" -w    --width \n         the window width\n");
  printf(" -n    --seconds \n         the number of seconds to run/record\n");
  printf(" -c    --spirals\n         the number of spirals\n");
  printf(" -r    --range\n          range of the sway\n");
  printf(" -i    --invert \n         invert black and white colors\n");
  printf(" -d    --inward \n         inward direction\n");
  printf(" -s    --sway \n         the period of horizontal sway\n");
  printf(" -R    --record\n         record the animation\n");
  printf(" -D    --nodisplay\n         don't display the animation\n");
  printf(" -p    --period \n         the period of circle formation\n");
  printf(" -t    --thickness \n         the thickness of the spirals\n");
  printf(" -a    --accel \n         acceleration of the period\n");
  printf(" -x    --swayaccel \n         acceleration of the sway period\n");
  printf(" -f    --filename \n         name of file to save to (if recording)\n");
  printf(" -F    --framerate\n         frames per second\n");
}

void test(struct options *opts) {
  printf(" height: %d\n", opts->height);
  printf(" width: %d\n", opts->width);
  printf(" seconds: %d\n", opts->seconds);
  printf(" spirals: %d\n", opts->spirals);
  printf(" range: %d\n", opts->range);
  printf(" invert: %d\n", opts->invert);
  printf(" inward: %d\n", opts->inward);
  printf(" record: %d\n", opts->record);
  printf(" display: %d\n", opts->display);
  printf(" sway: %f\n", opts->sway);
  printf(" period: %f\n", opts->period);
  printf(" thickness: %d\n", opts->thickness);
  printf(" accel: %f\n", opts->accel);
  printf(" swayaccel: %f\n", opts->swayaccel);
  printf(" filename: %s\n", opts->filename);
  printf(" framerate: %d\n", opts->framerate);
}


int sideof(int j) {
  if (j%2 == 0) return  j/2;
  else          return -j/2;
}


int main(int argc, char **argv) {

 int width = 1920;
 int height = 1200;
 int seconds = 3;
 int spirals = 10;
 int thickness = 1;
 int framerate = 48;
 int range = width/8;
 bool invert = 0;
 bool inward = 0;
 bool record = 0;
 bool display = 1;
 float sway = 2;
 float period = 1;
 float accel = 0;
 float swayaccel = 0;
 char* filename = calloc(1, 256);
 strcpy(filename, "output.mp4"); 

 static const struct option long_options[] = {
  {"height", required_argument, 0, 'h'},
  {"width", required_argument, 0, 'w'},
  {"seconds", required_argument, 0, 'n'},
  {"spirals", required_argument, 0, 'c'},
  {"range", required_argument, 0, 'R'},
  {"invert", no_argument, 0, 'i'},
  {"inward", no_argument, 0, 'd'},
  {"record", no_argument, 0, 'r'},
  {"display", no_argument, 0, 'D'},
  {"sway", required_argument, 0, 's'},
  {"period", required_argument, 0, 'p'},
  {"thickness", required_argument, 0, 't'},
  {"accel", required_argument, 0, 'a'},
  {"swayaccel", required_argument, 0, 'x'},
  {"filename", required_argument, 0, 'f'},
  {"framerate", required_argument, 0, 'F'}
 };

 int ch=0;
 int optdex=0;
 while ((ch = getopt_long (argc, argv, "h:w:n:c:F:RDidr:s:p:t:a:x:f:", long_options, &optdex)) != -1)
  switch (ch) {
   case 'h':
    height = atoi(optarg);
    break;
   case 'w':
    width = atoi(optarg);
    break;
   case 'n':
    seconds = atoi(optarg);
    break;
   case 'c':
    spirals = atoi(optarg);
    break;
   case 'F':
    framerate = atoi(optarg);
    break;
   case 'R':
    record = 1;
    break;
   case 'D':
    display = 0;
    break;
   case 'r':
    range = atoi(optarg);
    break;
   case 'd':
    inward = 1;
    break;
   case 'i':
    invert = 1;
    break;
   case 's':
    sway = atof(optarg);
    break;
   case 'p':
    period = atof(optarg);
    break;
   case 't':
    thickness = atoi(optarg);
    break;
   case 'a':
    accel = atof(optarg);
    break;
   case 'x':
    swayaccel = atof(optarg);
    break;
   case 'f':
    strcpy(filename, optarg);
    break;
   case '?':
    usage(argv[0]);
    exit(1);
   default:
    exit(1);
  }

 struct options *opts = (struct options *) malloc(sizeof(struct options));
 opts->height = height;
 opts->width = width;
 opts->seconds = seconds;
 opts->spirals = spirals;
 opts->record = record;
 opts->range = range;
 opts->inward = inward;
 opts->display = display;
 opts->invert = invert;
 opts->sway = sway;
 opts->period = period;
 opts->thickness = thickness;
 opts->accel = accel;
 opts->swayaccel = swayaccel;
 opts->filename = filename;
 opts->framerate = framerate;

 test(opts);


 if ((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) {
   printf("Could not initialize SDL: %s.\n", SDL_GetError());
   exit(-1);
 }
 SDL_Surface* screen = SDL_SetVideoMode(opts->width, opts->height, 32, SDL_DOUBLEBUF);
 SDL_WM_SetCaption("Circles", NULL);

 avcodec_register_all();
 int codec_id;

 FILE           **f      = malloc(sizeof(FILE *)); 
 AVCodec        **codec  = malloc(sizeof(AVCodec *)); 
 AVCodecContext **c      = malloc(sizeof(AVCodecContext *)); 
 AVFrame        **frame  = malloc(sizeof(AVFrame *));
 
 if (opts->record)
   av_init(opts->filename, f, AV_CODEC_ID_MPEG4, 
           opts->width, opts->height, opts->framerate, 
           codec, c, frame);
 
 int    x=0, y=0;
 int    i=0, j=0;
 int    fr=0;

 float  t=0;
 float max_radius = opts->width/2 + opts->height/2;
 float dif_radius = max_radius/opts->spirals;
 float dt         = 1.0/(float)opts->framerate;
 float ddt        = opts->accel;

 float r=0, R=0, da=0, p=0;

 Uint32 pixel = 0xFFFFFFFF;
 if (opts->invert)
   pixel = ~pixel;
 if (opts->sway > 0)
   max_radius += opts->width/2;

 printf("\n");
 bool flag = true;
 while (t<opts->seconds)
 {

   t  += dt;

/*
   if (flag && t >= (9*60+10.500)) {
     pixel = ~pixel;
     opts->inward = 1;
     opts->period = .5;
     opts->accel  = 0;
     opts->thickness = 10;
     flag = false;
   }
   */
 
   printf("Progress: %2.2f\r", 100.0*t/(opts->seconds));
   fflush(stdout);

   if (opts->accel != 0) {
     opts->period += opts->accel;
     if (opts->period < 0) {
       opts->period = 0;
       opts->accel = 0;
     }
   }

   if (opts->swayaccel != 0) {
     opts->sway += opts->swayaccel;
     if (opts->sway < 0) {
       opts->sway = 0;
       opts->swayaccel = 0;
     }
   }


   if (opts->inward) {
     r -= (M_PI/opts->period);
     if (r <= 0) r=dif_radius;
   } else {
     r += (M_PI/opts->period);
     if (r >= dif_radius) r=0;
   }
 

   R = sqrt(opts->height*opts->height + opts->width*opts->width);
   da = (1.0/R);
   float factor = (opts->spirals+1) * (opts->spirals+1);
   float maxA = opts->spirals*2*M_PI;

   //#pragma omp parallel for
   for (j=0; j<opts->thickness && j<=opts->thickness; j++)
   {

       //archimedes
       for (float a=0; a<maxA; a+=da)
       {
         x = a*R/factor*cos(a-t*opts->period)+sideof(j);
         if (opts->sway > 0)
           x += (opts->range)    *cos(2*M_PI*t/(opts->sway)) 
             +  (opts->range/4.0)*cos(cos(2*M_PI*t/(opts->sway)*(factor+a/maxA)/factor));
         y = a*R/factor*sin(a-t*opts->period)+sideof(j);
         if (abs(x) < opts->width/2 && abs(y) < opts->height/2) {
           putPixel(screen, x+opts->width/2, y+opts->height/2, pixel);
        }
       }


   }


 
   if (opts->record)
     av_encode_frame(screen, fr++, f, codec, c, frame);

   if (opts->display) {
     SDL_Flip(screen);
     SDL_Delay(1000.0/(float)opts->framerate);
   }

   SDL_FillRect(screen, NULL, ~pixel);
 
 }

 if (opts->record) {
   av_close(f, codec, c, frame);
   free(filename);
 }

 return 0;


}

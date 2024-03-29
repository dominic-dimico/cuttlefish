#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "recorder.h"

#define WIDTH    800
#define HEIGHT   600


struct options {
 int height;
 int width;
 int seconds;
 int tracers;
 int thickness;
 int framerate;
 float range;
 bool record;
 bool display;
 float period;
 float accel;
 float tracetime;
 char* filename;
};


void usage(char *progname) {
  printf(" -h    --height \n         the window height\n");
  printf(" -w    --width \n         the window width\n");
  printf(" -n    --seconds \n         the number of seconds to run/record\n");
  printf(" -c    --tracers\n         the number of tracers\n");
  printf(" -r    --range\n          range of the sway\n");
  printf(" -R    --record\n         record the animation\n");
  printf(" -D    --nodisplay\n         don't display the animation\n");
  printf(" -p    --period \n         the period of circle formation\n");
  printf(" -t    --thickness \n         the thickness of the tracers\n");
  printf(" -a    --accel \n         acceleration of the period\n");
  printf(" -x    --tracetime \n         time between tracers\n");
  printf(" -f    --filename \n         name of file to save to (if recording)\n");
  printf(" -F    --framerate\n         frames per second\n");
}


void test(struct options *opts) {
  printf(" height: %d\n", opts->height);
  printf(" width: %d\n", opts->width);
  printf(" seconds: %d\n", opts->seconds);
  printf(" tracers: %d\n", opts->tracers);
  printf(" range: %f\n", opts->range);
  printf(" record: %d\n", opts->record);
  printf(" display: %d\n", opts->display);
  printf(" period: %f\n", opts->period);
  printf(" thickness: %d\n", opts->thickness);
  printf(" accel: %f\n", opts->accel);
  printf(" tracetime: %f\n", opts->tracetime);
  printf(" filename: %s\n", opts->filename);
  printf(" framerate: %d\n", opts->framerate);
}



void drawCircle(SDL_Surface *screen, Uint16 x0, Uint16 y0, double r, int j)
{
    double t;
    Uint16 x, y;
    double limit = 2*M_PI;
    double unit  = (limit)/720;

    /*
    Uint8 *pix  = (Uint8*)screen->pixels;
    int   pitch = screen->pitch;
    Uint8 bpp   = screen->format->BytesPerPixel;
    */

    Uint32 color = 0x00FFFFFF - j*0x00222222 + j*0x22000000;
    int i;
    for (i=0; i<=2; i++)
      for (t=0; t<limit; t+=unit)
      {
        x = x0 + r*cos(t) + i;
        y = y0 + r*sin(t) + i;
        putPixel(screen, x, y, ~color);
        /*
        *(Uint32 *) (pix
                  + (int)(y)*pitch 
                  + (int)(x)*bpp
                  ) = 0x00FFFFFF;
                  */
      }

}

void drawLine(SDL_Surface *screen, Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, int j)
{ 

    /*
    Uint8  *p   = (Uint8*)screen->pixels;
    int   pitch = screen->pitch;
    Uint8 bpp   = screen->format->BytesPerPixel;
    */
    
    int i;
    double x = x1 - x0; 
    double y = y1 - y0; 
    double r = sqrt( x*x + y*y ); 
    double dx = x / r; 
    double dy = y / r; 
    x = x0; 
    y = y0; 

    // void putPixel(SDL_Surface *screen, int x, int y, Uint32 pixel);
    
    Uint32 color = 0x00FFFFFF - j*0x00222222 + j*0x22000000;

    //#pragma omp parallel for num_threads(4)
    for (i=0; i<r; i++) 
    { 
      putPixel(screen, x+=dx, y+=dy, ~color);
      /*
      *(Uint32 *) (p 
                + (int)(y+=dy)*pitch 
                + (int)(x+=dx)*bpp
                ) = 0x00FFFFFF;
      */
    }

}




void drawSpokes(SDL_Surface *screen, Uint16 x0, Uint16 y0, double r, double T, int j, int n) {
  double val = ( T/30.0 * 2 * M_PI );
  double valinc = n/(2*M_PI);
  for (int k=0; k<n; k++) {
    Uint16 x1 = x0+(r)*cos(val+valinc);
    Uint16 y1 = y0+(r)*sin(val+valinc);
    Uint16 x2 = x0-(r)*cos(val+valinc);
    Uint16 y2 = y0-(r)*sin(val+valinc);
    drawLine(screen, x1, y1, x2, y2, j);
  }
}


void watchface(SDL_Surface *screen, struct options *opts, Uint16 x0, Uint16 y0, double r, double T, int j)
{
  double val = ( T/3600.0 * 2 * M_PI );

  // Draw the minute hand
  Uint16 x = x0+(r/1.5)*cos(val);
  Uint16 y = y0+(r/1.5)*sin(val);
  drawLine(screen, x0-1, y0-1, x-1, y-1, j-1);
  drawLine(screen, x0, y0, x, y, j);
  drawLine(screen, x0+1, y0+1, x+1, y+1, j+1);

  // Draw the second hand
  val = ( T/60.0 * 2 * M_PI);
  x = x0+r*cos(val);
  y = y0+r*sin(val);
  drawLine(screen, x0, y0, x, y, j-1);
  drawLine(screen, x0+1, y0+1, x+1, y+1, j);

  //void drawLine(SDL_Surface *screen, Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, int j)
}





int main(int argc, char **argv) {

 int width = 1280;
 int height = 720;
 int seconds = 6;
 int tracers = 0;
 int thickness = 1;
 int framerate = 48;
 float range = 1;
 bool record = 0;
 bool display = 1;
 float period = 1;
 float accel = 0;
 float tracetime = 0.15;
 char* filename = calloc(1, 256);
 strcpy(filename, "output.mp4"); 

 static const struct option long_options[] = {
  {"height", required_argument, 0, 'h'},
  {"width", required_argument, 0, 'w'},
  {"seconds", required_argument, 0, 'n'},
  {"tracers", required_argument, 0, 'T'},
  {"range", required_argument, 0, 'R'},
  {"record", no_argument, 0, 'r'},
  {"display", no_argument, 0, 'D'},
  {"period", required_argument, 0, 'p'},
  {"thickness", required_argument, 0, 't'},
  {"accel", required_argument, 0, 'a'},
  {"tracetime", required_argument, 0, 'x'},
  {"filename", required_argument, 0, 'f'},
  {"framerate", required_argument, 0, 'F'}
 };

 int ch=0;
 int optdex=0;
 while ((ch = getopt_long (argc, argv, "h:w:n:T:F:RDr:s:p:t:a:x:f:", long_options, &optdex)) != -1)
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
   case 'T':
    tracers = atoi(optarg);
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
    tracetime = atof(optarg);
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
 opts->tracers = tracers;
 opts->record = record;
 opts->range = range;
 opts->display = display;
 opts->period = period;
 opts->thickness = thickness;
 opts->accel = accel;
 opts->tracetime = tracetime;
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
 

 Uint32 pixel = 0xFFFFFFFF;

  double dt = 1.0/(double)opts->framerate;
  double T = 0, t = 0;
  double x, y;
  double r = 30;
  int i, j;
  int fr=0;
  int flag = 0;

  while (T<opts->seconds)
  {
 
    T  += dt;
  
    printf("Progress: %2.2f\r", 100.0*T/(opts->seconds));
    fflush(stdout);
 
    if (opts->accel != 0) {
      opts->period += opts->accel;
      if (opts->period < 0) {
        opts->period = 0;
        opts->accel = 0;
      }
    }
 

   /*
   if ( fr > 48*13 ) {
       if (rand()%24 == 0) {
         dt *= -1;
       }
    }
    */

   //#pragma omp parallel for
   for (j=opts->tracers; j>=0; j--) {

     t = T - j*opts->tracetime;

     x =    opts->width/2 + 300.0*cos(t*opts->period);
     y = 3*opts->height/4 + 100.0*sin(t*opts->period)*sin(t*opts->period);

     //#pragma omp parallel for
     for (i=0; i<opts->thickness; i++)
     {
        drawCircle(screen, x, y, r+i, j);
     }

      watchface(screen, opts, x, y, r, t*opts->framerate, j);

      y -= r*sin(t)*sin(t) + r*cos(t)*cos(t);

      drawLine(screen, opts->width/2, opts->height/6, x, y, j);

  }

   if (opts->record)
     av_encode_frame(screen, fr, f, codec, c, frame);
   fr++;

   if (opts->display) {
     SDL_Flip(screen);
     SDL_Delay(1000.0/(float)opts->framerate);
   }

   //SDL_FillRect(screen, NULL, ~pixel);
   SDL_FillRect(screen, NULL, pixel);

  } 


  
  if (opts->record) {
   av_close(f, codec, c, frame);
   free(filename);
  }




}

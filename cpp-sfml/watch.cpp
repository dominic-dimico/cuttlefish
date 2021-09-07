#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <cmath>
#include <SFML/Graphics.hpp>



struct options {
 bool invert;
 int fps;
 int height;
 int width;
 int duration;
 float range;
 float radius;
 float length;
 float period;
 float accel;
};


void usage(char *progname) {
  printf("usage: %s [OPTIONS]\n\n", progname);
  printf(" -i    --invert \n         invert color scheme\n\n");
  printf(" -n    --fps \n         frames per second\n\n");
  printf(" -h    --height \n         height of window\n\n");
  printf(" -w    --width \n         width of window\n\n");
  printf(" -d    --duration \n         duration of play in seconds\n\n");
  printf(" -R    --range \n         range of the swing\n\n");
  printf(" -r    --radius \n         radius of the watch\n\n");
  printf(" -l    --length \n         length of the watch string\n\n");
  printf(" -p    --period \n         period of the motion\n\n");
  printf(" -a    --accel \n         acceleration of the period\n\n");
  printf(" -accel    --accel \n         À_›þ\n\n");
}


void test(struct options *opts) {
  printf(" invert: %d\n", opts->invert);
  printf(" fps: %d\n", opts->fps);
  printf(" height: %d\n", opts->height);
  printf(" width: %d\n", opts->width);
  printf(" duration: %d\n", opts->duration);
  printf(" range: %f\n", opts->range);
  printf(" radius: %f\n", opts->radius);
  printf(" length: %f\n", opts->length);
  printf(" period: %f\n", opts->period);
  printf(" accel: %f\n", opts->accel);
}


int main(int argc, char **argv) {

 bool invert = 0;
 int fps = 30;
 int height = 768;
 int width = 1280;
 int duration = 5;
 float range = 20;
 float radius = 40;
 float length = 500;
 float period = 10.0;
 float accel = 0;

 static const struct option long_options[] = {
  {"invert", no_argument, 0, 'i'},
  {"fps", required_argument, 0, 'f'},
  {"height", required_argument, 0, 'h'},
  {"width", required_argument, 0, 'w'},
  {"duration", required_argument, 0, 'd'},
  {"range", required_argument, 0, 'R'},
  {"radius", required_argument, 0, 'r'},
  {"length", required_argument, 0, 'l'},
  {"period", required_argument, 0, 'p'},
  {"accel", required_argument, 0, 'a'},
 };


 int c=0;
 int optdex=0;
 while ((c = getopt_long (argc, argv, "if:h:w:d:R:r:l:p:a:", long_options, &optdex)) != -1)
  switch (c) {
   case 'i':
    invert = 1;
    break;
   case 'n':
    fps = atoi(optarg);
    break;
   case 'h':
    height = atoi(optarg);
    break;
   case 'w':
    width = atoi(optarg);
    break;
   case 'd':
    duration = atoi(optarg);
    break;
   case 'R':
    range = atof(optarg);
    break;
   case 'r':
    radius = atof(optarg);
    break;
   case 'l':
    length = atof(optarg);
    break;
   case 'p':
    period = atof(optarg);
    break;
   case 'a':
    accel = atof(optarg);
    break;
   case '?':
    usage(argv[0]);
    exit(1);
   default:
    exit(1);
  }


 struct options *opts = (struct options *) malloc(sizeof(struct options));
 opts->invert = invert;
 opts->fps = fps;
 opts->height = height;
 opts->width = width;
 opts->duration = duration;
 opts->range = range;
 opts->radius = radius;
 opts->length = length;
 opts->period = period;
 opts->accel = accel;

 test(opts);

 sf::RenderWindow window(sf::VideoMode(width, height), "Operation Pocketwatch!");


 float chainx = width/2;
 float chainy = height/8;

 /*
  * setPosition defines the actual origin (for rotation, etc)
  * setOrigin defines the position from origin
  */

 sf::CircleShape watch(radius);
 watch.setFillColor(sf::Color::White);
 watch.setPosition(chainx-radius, chainy-radius);
 
 int buttonsize = radius/5.0;
 sf::CircleShape button(buttonsize);
 button.setFillColor(sf::Color::Black);
 button.setPosition(chainx-radius-buttonsize, chainy-radius-buttonsize);

 sf::Vector2f minutev(6, .7*radius);
 sf::RectangleShape minutehand(minutev);
 minutehand.setFillColor(sf::Color::Black);

 sf::Vector2f secondv(4, radius);
 sf::RectangleShape secondhand(secondv);
 secondhand.setFillColor(sf::Color::Black);

 sf::Vector2f chainv(2, length);
 sf::RectangleShape chain(chainv);
 chain.setFillColor(sf::Color::White);
 chain.setPosition(chainx, chainy);

 int n, frames;
 frames = fps * duration;

 float angle; 
 sf::Time naptime = sf::seconds(1.0/(float)fps);
 sf::Vector2f pos;

 sf::Vector2u windowSize = window.getSize();
 sf::Texture texture;
 texture.create(windowSize.x, windowSize.y);
 sf::Image screenshot;

 char buffer[128];


 for (n=0; n<frames; n++) {

       angle = 45.0 * cos((float)n/period);


       chain.setRotation(angle);

       watch.setPosition(
         (chainx-    radius) +    (length- 1.0*radius)*  -(sin( angle/((length+radius)/10.0))),
         (chainy-    radius) +    (length- 0.0*radius)*   (cos( angle/((length+radius)/10.0)))
       );

       pos = watch.getPosition();
       button.setPosition(pos.x+radius-buttonsize, pos.y+radius-buttonsize);

       minutehand.setPosition(pos.x+radius, pos.y+radius);
       minutehand.setRotation(n*10);

       secondhand.setPosition(pos.x+radius, pos.y+radius);
       secondhand.setRotation(n*20);

       //sf::sleep(naptime);
       
       sf::Transform what (
          sin(angle),  cos(angle), -sin(angle),
          cos(angle), -sin(angle), -cos(angle),
          sin(angle),  cos(angle),  sin(angle)
       );

       window.clear();
       window.draw(chain);
       window.draw(watch);
       window.draw(minutehand);
       window.draw(secondhand);
       window.draw(button);
       //window.display();

       texture.update(window);
       screenshot = texture.copyToImage();
       sprintf(buffer, "frames/%08d.jpg", n);
       screenshot.saveToFile(buffer);

 }

 return 0;

}

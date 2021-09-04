#!/usr/bin/python

import cv2
import numpy as np
import math
from cv2 import VideoWriter, VideoWriter_fourcc
from tqdm import tqdm
import threading

# idea: make configurable function z(x, y, t, o) which is used to graph a
# parametric equation (with o as options)

global width
global height

width   = 1280
height  = 720
fps     = 24
seconds = 10
radius  = 50
velocity = 10 
paint_h = int(height/2)


fourcc = VideoWriter_fourcc(*'MP42')
video  = VideoWriter('./output.avi', fourcc, fps, (width, height))


def polar(x, y, o):
    X = x - o["width"]/2;
    Y = y - o["height"]/2;
    if (X==0):
       if Y>0: a = math.pi/2.0
       else:   a = 3*math.pi/2.0
    else: a = math.atan(Y/X);
    r = X*X + Y*Y;
    return (r, a);


def maxradius(o):
    X = o["width"]/2;
    Y = o["height"]/2;
    r = X*X + Y*Y;
    return r;


def cartesian(x, y, o):
    X = x - width/2;
    Y = y - height/2;
    return (X, Y);


o = {
  "height": height,
  "width":  width,
  "fps":  fps
}


global R;
R = maxradius(o);


# Can use eval() to evaluate any expression
# Also compiler.compile() to speed up eval()
def zed(x, y, t, o):
    (x, y) = cartesian(x, y, o);
    (r, a) = polar(x, y, o);
    return 0xFF * r * math.cos(a);


frame = np.zeros(
   shape=(height, width, 3),
   dtype=np.uint8
);


#for f in tqdm(range(seconds*fps)):
#    t = f / float(fps);
#    for y in range(0, height):
#        for x in range(0, width):
#            z = zed(x, y, t, o);
#            frame[y][x] = (z, z, z);
#    video.write(frame);
#video.release()
 


numthreads = 2;
global frames;
frames = np.zeros(
  shape=(numthreads, height, width, 3),
  dtype=np.uint8
);
lock = threading.Lock();

def calculate_frame(f, i, o):
    height = o["height"];
    width = o["width"];
    t = (f+i)/o["fps"];
    frame = np.zeros(
      shape=(height, width, 3),
      dtype=np.uint8
    );
    for y in range(0, height):
        for x in range(0, width):
            z = zed(x, y, t, o);
            frame[y][x] = (z, z, z);
    lock.acquire();
    frames[i] = frame;
    lock.release();


its = seconds*fps/numthreads;
for it in tqdm(range(its)):
    f = it*numthreads;
    ts = []
    for i in range(0, numthreads):
        th = threading.Thread(target=calculate_frame, args=(f, i, o));
        ts.append(th);
        th.start();
    for i in range(0, numthreads): 
        ts[i].join();
    for i in range(0, numthreads): 
        video.write(frames[i]);
video.release();

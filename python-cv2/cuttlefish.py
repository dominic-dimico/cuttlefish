#!/usr/bin/python

import getopt
import sys

def opts():
 try:
  opts, args = getopt.getopt(
    args=sys.argv[1:], 
    shortopts="h:w:t:r:n:l:p:s:a:b:z:diqo:m:c:", longopts=["height=","width=","seconds=","range=","tracers=","lagtime=","period=","swayperiod=","acceleration=","swayaccel=","size=","direction","invert","quiet","output=","method=","custom="]);
 except getopt.GetoptError as err:
  print str(err)
  usage()
  sys.exit(2)
 for option, value in opts:
  if option == "-v":
     verbose = True
  elif option in ("-h", "--height"):
   height = value
  elif option in ("-w", "--width"):
   width = value
  elif option in ("-t", "--seconds"):
   seconds = value
  elif option in ("-r", "--range"):
   range = value
  elif option in ("-n", "--tracers"):
   tracers = value
  elif option in ("-l", "--lagtime"):
   lagtime = value
  elif option in ("-p", "--period"):
   period = value
  elif option in ("-s", "--swayperiod"):
   swayperiod = value
  elif option in ("-a", "--acceleration"):
   acceleration = value
  elif option in ("-b", "--swayaccel"):
   swayaccel = value
  elif option in ("-z", "--size"):
   size = value
  elif option in ("-d", "--direction"):
   direction = value
  elif option in ("-i", "--invert"):
   invert = value
  elif option in ("-q", "--quiet"):
   quiet = value
  elif option in ("-o", "--output"):
   output = value
  elif option in ("-m", "--method"):
   method = value
  elif option in ("-c", "--custom"):
   custom = value
  else:
    assert False, "unhandled option"

 print swayaccel;

if __name__ == "main":
  opts();

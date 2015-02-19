#!/usr/bin/env python

import liblo, sys, time, math

# send all messages to port 1234 on the local machine
try:
    target = liblo.Address(12345)
except liblo.AddressError as err:
    print(err)
    sys.exit()

# send message "/foo/message1" with int, float and string arguments
liblo.send(target, "/load", 0, "SerenadeStrHi.wav")
liblo.send(target, "/loop", 0, 1)
liblo.send(target, "/play", 0)

# Now loop and send continuous messages
pitch=1.0
top = 2.0
bottom = 0.0
step=0
while True:
    pitch = (-1*math.sin(step)*(top-bottom))+(top-bottom)
    step += 0.2
    liblo.send(target, "/pitch", 0, pitch)
    time.sleep(0.1)


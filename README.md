Sonic Bike Audio Engine
============

An audio playback mechanism written with openframeworks. Uses OSC, multichannel audio. Devised for the Sonic Bike project. www.sonicbikes.net

Currently Tested and working with:

- OpenFrameworks v0.8.4 linux64 release
- Beaglebone black
- Arch Linux

Utalises the ofxJSON addon from: https://github.com/jefftimesten/ofxJSON<br />
Be sure to download and add to OF addon directory: <br />

    $ cd pathtoopenframeworks/addons 
    $ git clone https://github.com/jefftimesten/ofxJSON.git

Api
=====================
load [int channel] [string filename]
Example osc. Load a soundfile in channel 0: "/load 0 asoundfile.wav"

unload [int channel]<br />
Example osc. Free memory in channel 0: "/unload 0"

setaudodir [string directorypath/] <br />
Example osc. Set the audio directiry: "/setaudodir /home/user/Desktop/sounds"

play [int channel]
Example osc. Play channel 0 from start to end: "/play 0"

stop [int channel]
Example osc. Stop channel 0 playing: "/stop 0"

loop [int channel] [int loopon(1) loopoff(0)]
Example osc. Set channel 0 to loop when its played next: "/loop 0 1"

loopplay [int channel]
Example osc. Start playing channel 0 in a loop: "/loopplay 0 1"

superlooper [int channel, float position(0.0-1.0), float length(seconds)] 
Example OSC. Loop a sound in channel 0 from postion 0.2 for 1.5 seconds: "/superlooper 0 0.2 1.5"

superpitch [int channel, string("u","d"), float speed(0.1-100 seconds) float max(0.0-20.0), float min(0.0-20.0)] 
Example OSC. Loop a sound in channel 0 from postion 0.2 for 1.5 seconds: "/superlooper 0 0.2 1.5"

pause [int channel] [int start(1) | stop(0)]
Example osc. Start playing channel 0: "/pause 0 1"

fadein [int channel] [float volumeIncrement] [float everyNseconds]:<br />
example osc. Fadein channel 0: "/fadein 0 0.1 0.25" 

fadeout [int channel] [float volumeIncrement] [float everyNseconds]:<br />
example osc. Fadeout channel 0: "/fadeout 0 0.1 0.25"

setframerate [int openframeworksFramerate]<br />
Set framerate: /setframerate 60

Build instrctions
====================
make run

License
=====================

Embedded system for Kaffe Matthews 
Copyright (C) 2014 Tom Keene

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see http://www.gnu.org/licenses/.

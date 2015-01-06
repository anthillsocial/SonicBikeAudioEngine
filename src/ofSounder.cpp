/* BicrophonicsV1 embedded system for Kaffe Matthews 
-- Copyright (C) 2014 Tom Keene
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ofSounder.h"
// try: http://forum.openframeworks.cc/t/tonic-a-refreshingly-crisp-c-audio-patching-library/12923/12
ofSounder::ofSounder(string _soundfile)
{   
    // Setup some base variables
	soundfile = _soundfile;

    // And get the initial sound going
	mysound.loadSound(soundfile);
	mysound.setVolume(0.75f);
    mysound.setMultiPlay(false);
    mysound.play();
}


void ofSounder::update(){
 /* 
    mysound.getIsPlaying()
    mysound.setPlayer()
    mysound.setPositionMS()
  */
}

void ofSounder::load(string soundfile){
    // TODO: Seems to bug where unload sounds doesn't work & load needs to be called twice.
    //       Soooo... this hacky fix will do for now but should be properly sorted.
   	mysound.loadSound(soundfile);
	mysound.loadSound(soundfile);
	cout << "attempted to load: " << soundfile << "\n";
}

void ofSounder::play(){
    mysound.play();
}

void ofSounder::stop(){
    mysound.stop();
}

void ofSounder::pause(bool paused){
    mysound.setPaused(paused);
}

void ofSounder::setSpeed(float speed){
    mysound.setSpeed(speed);
}

void ofSounder::setPan(float pan){
    mysound.setPan(pan);
}

void ofSounder::setVolume(float vol){
    mysound.setVolume(vol);
}

void ofSounder::setLoop(bool loop){
    mysound.setLoop(loop);
}

void ofSounder::setPosition(float pos){
    mysound.setPosition(pos);
}

void ofSounder::setMultiPlay(bool multi){
    mysound.setMultiPlay(multi);   
}

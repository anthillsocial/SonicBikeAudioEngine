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
ofSounder::ofSounder()
{   
    // Setup some base variables
    masschange = false;
    myvol = 1.0f;
	mysound.setVolume(myvol);
    mysound.setMultiPlay(false);
    soundfile = "none";
    state = "none";
    myfade = 0.0f;
    timepassed = ofGetElapsedTimeMicros(); // 1000000 microsecs = 1 second
    timer = 1000000; // Perform an action every second
}


void ofSounder::update(){
	// A timeer to perform accurate fades
	int currenttime = ofGetElapsedTimeMicros(); // 1000000 microsecs = 1 second
	int elapsedtime = currenttime-timepassed;
	bool tick = false;
	if(elapsedtime>=timer){
		timepassed = ofGetElapsedTimeMicros();
		tick = true;
	}
	// Check if we need to fade in or out
	if(state=="fadein" && tick){
		myvol = myvol+myfade;
		//cout << myvol << "\n";
		if(myvol<1.0){
			mysound.setVolume(myvol);
		}else{
			mysound.setVolume(1.0f);
			state = "playing";
		}
	}
	// Fadeout
	else if(state=="fadeout" && tick){
		myvol = myvol-myfade;
		//cout << myvol << "\n";
		if(myvol>0.0){
			mysound.setVolume(myvol);  
		}else{
			mysound.setVolume(0.0f);
			state = "playing";
			myvol=0.0f;
		}
	}
    //mysound.setPlayer()
    //mysound.setPositionMS()
}

void ofSounder::load(string newsoundfile, bool stream){
    // TODO: Seems to bug where unload sounds doesn't work & load needs to be called twice.
    //       Soooo... this hacky fix will do for now but should be properly sorted.
    //	     Need to check: ../../../libs/openFrameworks/sound/ofOpenALSoundPlayer.cpp
    soundfile = newsoundfile;  
	if( ! mysound.loadSound(soundfile, stream) ){
        mysound.loadSound(soundfile); 
	}
	cout << "Attempted to load: " << soundfile << "\n";
}

void ofSounder::unload(){
	mysound.unloadSound();
	state = "none";
	cout << "Unloaded: " << soundfile << "\n";
}

void ofSounder::play(){
	state = "playing";
    mysound.play();
}

void ofSounder::stop(){
	state = "stop";
    mysound.stop();
}

void ofSounder::pause(bool paused){
	if(paused){
		state = "pause1"; 
	}else{
		state = "pause0";
	}
    mysound.setPaused(paused);
}

void ofSounder::setSpeed(float speed){
	state = "speed";  
    mysound.setSpeed(speed);
}

void ofSounder::fadein(float fade, float time){
	timer = int(time*1000000);
	state = "fadein";
	myvol = 0.0f;
	mysound.setPaused(false);
    myfade = fade;
}

void ofSounder::fadeout(float fade, float time){
	timer = int(time*1000000);
	state = "fadeout";  
    myfade = fade;
}

void ofSounder::setPan(float pan){
	state = "pan";  
    mysound.setPan(pan);
}

void ofSounder::setVolume(float vol){  
	state = "volume";  
    mysound.setVolume(vol);
}

void ofSounder::setLoop(bool loop){
	state = "loop";  
    mysound.setLoop(loop);
}

void ofSounder::setPosition(float pos){
	state = "positon";  
    mysound.setPosition(pos);
}

void ofSounder::setMultiPlay(bool multi){
	state = "multiplay";  
    mysound.setMultiPlay(multi);   
}
void ofSounder::setMassChange(bool change){
	masschange = change;   
}


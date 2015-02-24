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
    masspitch = false;
	massvolume = false;
	masssuperlooper = false;
	masssuperpitch = false;
    massstartpoint = false;
    fadervol = 1.0f;
	mysound.setVolume(fadervol);
    mysound.setMultiPlay(false);
    soundfile = "none";
    state = "none";
    // Fader
    faderstate = "none";
    faderfade = 0.0f;
    fadertimepassed = ofGetElapsedTimeMicros(); // 1000000 microsecs = 1 second
    fadertimer = 1000000; // Perform an action every second
    // Superlooper
    superloopercommand = "off";
    superlooperlen = 0.0f;
    superlooperpos = 0.0f;
    superloopertimepassed = 0;
    // Super pitch
    superpitchcommand = "off";
    superpitchspeed = 0.0f;
    superpitchinc = 0.0f;
    superpitchmax = 0.0f;
    superpitchmin = 0.0f;
    superpitchtimepassed = 0;
    superpitchpitch = 1.0;
}


void ofSounder::update(){
	int currenttime = ofGetElapsedTimeMicros(); // 1000000 microsecs = 1 second
	
	// Perform smooth fades
	int faderelapsedtime = currenttime-fadertimepassed;
	bool fadertick = false;
	if(faderelapsedtime>=fadertimer){
		fadertimepassed = ofGetElapsedTimeMicros();
		fadertick = true;
	}
	// Check if we need to fade in or out
	if(faderstate=="fadein" && fadertick){
		fadervol = fadervol+faderfade;
		//cout << myvol << "\n";
		if(fadervol<1.0){
			mysound.setVolume(fadervol);
		}else{
			mysound.setVolume(1.0f);
			faderstate = "playing";
		}
	}
	// Fadeout
	else if(faderstate=="fadeout" && fadertick){
		fadervol = fadervol-faderfade;
		//cout << myvol << "\n";
		if(fadervol>0.0){
			mysound.setVolume(fadervol);  
		}else{
			mysound.setVolume(0.0f);
			faderstate = "playing";
			fadervol=0.0f;
		}
	}

	// Set super looper
	if(superloopercommand=="on"){   
	    int superlooperelapsedtime = currenttime-superloopertimepassed; 
        if(superlooperelapsedtime>=superlooperlen){
            superloopertimepassed = ofGetElapsedTimeMicros();  
            mysound.setPosition(superlooperpos); 
        }
    }

	// Set super pitch
	if(superpitchcommand!="off"){   
	    int superpitchelapsedtime = currenttime-superpitchtimepassed; 
        if(superpitchelapsedtime>=superpitchspeed){
            superpitchtimepassed = ofGetElapsedTimeMicros();  
            if(superpitchcommand=="up"){  
                superpitchpitch = superpitchpitch+superpitchinc;
            }else{
                superpitchpitch = superpitchpitch-superpitchinc;
            }
            // Make sure we keep iun range
            if(superpitchpitch > superpitchmax){
                superpitchpitch = superpitchmax;
            }else if(superpitchpitch < superpitchmin){
                superpitchpitch = superpitchmin;
            }
            // Now set the pitch
            mysound.setSpeed(superpitchpitch); 
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
        mysound.loadSound(soundfile, stream); 
	}
	//cout << "Attempted to load: " << soundfile << "\n";
}

void ofSounder::unload(){
	mysound.unloadSound();
	state = "none";
    //	cout << "Unloaded: " << soundfile << "\n";
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
	fadertimer = int(time*1000000);
	faderstate = "fadein";
	fadervol = 0.0f;
	mysound.setPaused(false);
    faderfade = fade;
}

void ofSounder::fadeout(float fade, float time){
	fadertimer = int(time*1000000);
	faderstate = "fadeout";  
    faderfade = fade;
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
void ofSounder::setSuperLooper(string command, float pos, float len){
	superloopercommand = command; // on off  
    superlooperpos = pos;
    superlooperlen = len*1000000.0;
}
void ofSounder::setSuperPitch(string command, float inc, float speed, float max, float min){
    superpitchcommand = command; // up down off
    superpitchinc = inc; 
    superpitchspeed = speed*1000000.0;     // how quickly to change  
    superpitchmax = max;
    superpitchmin = min;
}
void ofSounder::setMultiPlay(bool multi){
	state = "multiplay";  
    mysound.setMultiPlay(multi);   
}
void ofSounder::setMassChange(bool change, string param){
    // pitch,volume,superlooper,superpitch, startpoint]";
    if(param=="pitch"){
	    masspitch = change; 
	}else if(param=="volume"){
        massvolume = change;
    }else if(param=="superlooper"){
        masssuperlooper = change;
    }else if(param=="superpitch"){
        masssuperpitch = change;
    }else if(param=="startpoint"){
        massstartpoint = change; 
    }
}


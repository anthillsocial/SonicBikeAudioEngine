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
 // Check the sound is loaded, if not attempt to load again
 /* 
    mysound.getIsPlaying()
    mysound.setPaused()
    mysound.setPlayer()
    mysound.setPositionMS()
  */
}

void ofSounder::load(string soundfile){
    // TODO: Check the soundfile exists
    // TODO: Sometimes doesn't load: check error messages
   	mysound.loadSound(soundfile);
	cout << "attempted to load: " << soundfile << "\n";
}

void ofSounder::play(){
    mysound.play();
}

void ofSounder::stop(){
    mysound.stop();
}

void ofSounder::setspeed(float speed){
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







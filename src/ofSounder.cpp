#include "ofSounder.h"
// try: http://forum.openframeworks.cc/t/tonic-a-refreshingly-crisp-c-audio-patching-library/12923/12
ofSounder::ofSounder(string _soundfile)
{   
    // Setup some base variables
	speedX = ofRandom(-1, 1);
	speedY = ofRandom(-1, 1);	
	soundfile = _soundfile;
    // And get the sound going
	mysound.loadSound(soundfile);
	mysound.setVolume(0.75f);
    mysound.setMultiPlay(false);
    mysound.play();
}


void ofSounder::update(){

 /* 
    mysound.setMultiPlay(false);
    mysound.setLoop()
    mysound.getIsPlaying()
    mysound.setPan()
    mysound.setPaused()
    mysound.setPlayer()
    mysound.setPosition()
    mysound.setPositionMS()
    mysound.setSpeed()
    mysound.setVolume()
    mysound.stop()
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




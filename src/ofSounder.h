#ifndef _OF_BALL		// by using this if statement you prevent the class to be called more than once - in other words what we're saying here is 
#define _OF_BALL		//if the class has NOT been defined then define it
					
#include "ofMain.h"

class ofSounder {
	  
   public:   

		// methods
		void update();
        void load(string soundfile);
        void play();
        void stop();
        void setSpeed(float speed);
        void setPan(float pan);
        void setVolume(float vol);
        void setLoop(bool loop);
        void setPosition(float pos);
        void setMultiPlay(bool multi);

		// constructor
		ofSounder(string soundfile);

		// variables
        string soundfile;
        ofSoundPlayer  mysound;

	private:
 
   
};					

#endif 

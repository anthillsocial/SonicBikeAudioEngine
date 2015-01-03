#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofSounder.h"
#include "ofxOsc.h"

// The config file
#define CONFIG "config.json"

// listen on port 12345
#define PORT 12345

class ofApp : public ofBaseApp{

	public:
	    // Base methods
		void setup();
		void update();
        
        //------------------------OSC control
		ofxOscReceiver receiver;
        // ofImage receivedImage; // TODO: Placeholder: Enable audo file transmission

    private:
        //------------------------Json congif loader
        ofxJSONElement result;
        //------------------------Sound control objects
        ofSounder** mySounder;  
        int nSounders;
};

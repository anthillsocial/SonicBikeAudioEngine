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
 		string ofTkSystem(const string& command);

        //------------------------OSC control
        string audiodirectory;
        string startupsound; 
		ofxOscReceiver receiver;
        // ofImage receivedImage; // TODO: Placeholder: Enable audo file transmission

    private:
        //------------------------Json congif loader
        ofxJSONElement result;
        ofxJSONElement altresult; 
        //------------------------Sound control objects
        ofSounder** mySounder;  
        int nChannels;
};

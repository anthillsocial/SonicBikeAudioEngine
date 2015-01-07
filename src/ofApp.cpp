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

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Load and parse the config file
    bool parsingSuccessful = result.open(CONFIG);
    if (parsingSuccessful){
        ofLogNotice("Loaded config") << result.getRawString();
    }else{
         ofLogError("Config: ")  << "Failed to parse JSON";
    }
    // Check if the altconfig file exists, and load it if it does
    string altconfig = result["altconfig"].asString();
    bool altParsingSuccessful = altresult.open(altconfig);
    if (altParsingSuccessful){
        ofLogNotice("Loaded altconfig") << altresult.getRawString();
        result = altresult;
    }else{
        ofLogNotice("Config: ")  << "No altconfig: " << result["altconfig"];
        ofLogNotice("Config: ")  << "Using default: " << CONFIG;
    }
    // Setup base variables
    if(result["debug"].asString() ==  "true"){
        ofLogToFile(result["audio_log"].asString(), true);
    }else{
        ofSetLogLevel(OF_LOG_ERROR);
    }
    ofLogNotice("STARTUP: "+ofGetTimestampString("%w %e %b %H:%M:%S%A" ));
    int channels = result["audio_channels"].asDouble();
    audiodirectory = result["audio_path"].asString();
    startupsound = audiodirectory+'/'+result["audio_startup"].asString(); 

    // Initialise some sound control objects
	mySounder = new ofSounder*[channels];
	for (int i = 0; i < channels; i++){                            
	    mySounder[i] = new ofSounder(startupsound);                              
	}

	// OSC Vars: listen on the given port
	ofLogNotice("OSC LISTEN ON PORT: ") << PORT;
	receiver.setup(PORT);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Update all the sound objects
    for (int i = 0; i < nSounders; i++){
        mySounder[i]->update();
    }

	// Update the sound playing system:
	ofSoundUpdate();

    // OSC check for waiting messages
	while(receiver.hasWaitingMessages()){
		
		// Get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);

		// Load a soundfile
		if(m.getAddress() == "/load"){
			int channel = m.getArgAsInt32(0);
			string soundfile = audiodirectory+'/'+m.getArgAsString(1);
	        mySounder[channel]->load(soundfile);
			ofLogNotice("osc:") << "/load [" << channel << "] " << soundfile;
		}
	    
	    // Play soundfile
		else if(m.getAddress() == "/play"){
			int channel = m.getArgAsInt32(0);
	        mySounder[channel]->play();
			ofLogNotice("osc:") << "/play [" << channel << "] ";
		}
    	
    	// Stop soundfile
		else if(m.getAddress() == "/stop"){
			int channel = m.getArgAsInt32(0);
	        mySounder[channel]->stop();
			ofLogNotice("osc:") << "/stop [" << channel << "] ";
		}

        // Pause soundfile
		else if(m.getAddress() == "/pause"){
			int channel = m.getArgAsInt32(0);	
			int paused = m.getArgAsInt32(1);
	        mySounder[channel]->pause(paused);
			ofLogNotice("osc:") << "/play [" << channel << "] ";
		}


    	// Set the pitch of a channel
    	else if(m.getAddress() == "/pitch"){
			int channel = m.getArgAsInt32(0);
	        float speed = m.getArgAsFloat(1);
	        mySounder[channel]->setSpeed(speed);
			ofLogNotice("osc:") << "/pitch [" << channel << "] pitch: " << speed;
        }

 	    // Set the volume of a channel
    	else if(m.getAddress() == "/volume"){
			int channel = m.getArgAsInt32(0);
	        float vol = m.getArgAsFloat(1);
	        mySounder[channel]->setVolume(vol);
			ofLogNotice("osc:") << "/volume [" << channel << "] vol: " << vol;
        }

	    // Set the pan of a channel
    	else if(m.getAddress() == "/pan"){
			int channel = m.getArgAsInt32(0);
	        float pan = m.getArgAsFloat(1);
	        mySounder[channel]->setPan(pan);
			ofLogNotice("osc:") << "/pan [" << channel << "] pan: " << pan;
        }
	    
	    // Set the loop of a channel
	    // TODO: Deal with the bool properly   
    	else if(m.getAddress() == "/loop"){
			int channel = m.getArgAsInt32(0);
	        int loop = m.getArgAsInt32(1);
	        mySounder[channel]->setLoop(loop);
			ofLogNotice("osc:") << "/loop [" << channel << "] loop: " << loop;
        }
	    
	    // Set the position of a channel
    	else if(m.getAddress() == "/position"){
			int channel = m.getArgAsInt32(0);
	        float pos = m.getArgAsFloat(1);
	        mySounder[channel]->setPosition(pos);
			ofLogNotice("osc:") << "/position [" << channel << "] pos: " <<  pos;
        }
	    
	    // Set the multiplay of a channel
	    // TODO: Deal with the bool properly
    	else if(m.getAddress() == "/multiplay"){
			int channel = m.getArgAsInt32(0);
	        int mp = m.getArgAsInt32(1);
	        mySounder[channel]->setMultiPlay(mp);
			ofLogNotice("osc:") << "/multiplay [" << channel << "] mp: " << mp << "\n";
        }

        // Check for a file being sent 
        // note: the size of the file depends on network buffer sizes 
        // if an file is too big the message won't come through  
        else if(m.getAddress() == "/file" ){
            ofBuffer buffer = m.getArgAsBlob(0);
            //receivedImage.loadImage(buffer); TODO: Enable audio transmission
        }

        // The message is unrecognised
		else{
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for(int i = 0; i < m.getNumArgs(); i++){
				// get the argument type
				msg_string += m.getArgTypeName(i);
				msg_string += ":";
				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msg_string += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msg_string += m.getArgAsString(i);
				}
				else{
					msg_string += "unknown";
				}
			}
			// print the message to stdout
			ofLogNotice("osc:") << msg_string;
		}

	}

}




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
        ofLogNotice("Config") << "Loaded default" ;// result.getRawString();
    }else{
        ofLogError("Config")  << "Failed to parse file";
    }
    // Check if the altconfig file exists, and load it if it does
    string altconfig = result["altconfig"].asString();
    ofSetLogLevel(OF_LOG_SILENT);
    bool altParsingSuccessful = altresult.open(altconfig);
    ofSetLogLevel(OF_LOG_NOTICE);
    if (altParsingSuccessful){
        ofLogNotice("Config") << "Loaded altconfig" ;//altresult.getRawString();
        result = altresult;
    }else{
        ofLogNotice("Config") << "No altconfig loaded (not found or error)" ; //altresult.getRawString();
    }
    // Setup base variables
    if(result["debug"].asString() ==  "true"){
        ofLogToFile(result["audio_log"].asString(), true);
    }else{
        ofSetLogLevel(OF_LOG_ERROR);
    }
    ofLogNotice("STARTUP: "+ofGetTimestampString("%w %e %b %H:%M:%S%A" ));
    nChannels = result["audio_channels"].asDouble();
    audiodirectory = result["audio_path"].asString();
    startupsound = audiodirectory+'/'+result["audio_startup"].asString(); 

    // Initialise some sound control objects
	mySounder = new ofSounder*[nChannels];
	for (int i = 0; i < nChannels; i++){                            
	    mySounder[i] = new ofSounder();
	}

	// OSC Vars: listen on the given port
	ofLogNotice("OSC LISTEN ON PORT: ") << PORT;
	receiver.setup(PORT);
}

//--------------------------------------------------------------
void ofApp::update(){
    // Grab the current log level
    ofLogLevel logLevel = ofGetLogLevel();  

    // Update all the sound objects
    for (int i = 0; i < nChannels; i++){
        mySounder[i]->update();
    }

	// Update the sound playing system:
	ofSoundUpdate();

    // OSC check for waiting messages
	while(receiver.hasWaitingMessages()){
		
		// Get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
				
		// Check we have been sent a valid channel
		int channel = -1;
		int args = m.getNumArgs();
		if(args>=1){
			if( m.getArgType(0) == OFXOSC_TYPE_INT32){
				channel = m.getArgAsInt32(0);
				if(channel>(nChannels-1)){
					ofLogNotice("osc error") << "channel [" <<channel<<"] out of range. max " << nChannels;
					channel = -1;
				}
			}
		}else{
			ofLogNotice("osc error") << "no channel defined as OFXOSC_TYPE_INT32";
		}

		// Load a soundfile
		if(m.getAddress() == "/load" && channel >=0){
			string soundfile = audiodirectory+'/'+m.getArgAsString(1);
			// Check if the file exists
			ofFile file (soundfile);   
			if(file.exists()){
				// Check we have enough memory to load it
				int filesize = file.getSize(); ofLogNotice("filesize") << filesize;
				int free = ofToInt(ofSystem("free | grep Mem | awk '{print $4}'")); ofLogNotice("free") << free;
				float filesizemb = float(filesize)/1000.0/1000.0; ofLogNotice("filesizemb") << filesizemb;
				float freemb = float(free)/1000.0; ofLogNotice("freemb") << freemb;
				float availmb = freemb-filesizemb; ofLogNotice("availmb") << availmb;
				if(filesizemb > availmb){
					ofLogNotice("osc error") << "Not enough memory (" << freemb << "mb) to load file (" << filesizemb << "mb) | /load [" << channel << "]" << soundfile;
				}else{
					// Silence error TODO: Fix hack where audio class needs to load twice
					ofLogNotice("start to load")
					//ofSetLogLevel(OF_LOG_SILENT);
	        		mySounder[channel]->load(soundfile);
	        		//ofSetLogLevel(logLevel);
					ofLogNotice("osc") << "/load [" << channel << "] " << soundfile << " | freemem: " << freemb << "mb filesize: " << filesizemb << "mb memavail: " << availmb << "mb";
				}
			}else{
				ofLogNotice("osc error") << "File doesn't exist \"/load [" << channel << "] " << soundfile; 
			}
			file.close();  
		}	    
	    // Unload a soundfile
		else if(m.getAddress() == "/unload" && channel>=0){
		    mySounder[channel]->unload();
			ofLogNotice("osc") << "/unload [" << channel << "]";
		}
	    // Play soundfile
		else if(m.getAddress() == "/play" && channel>=0){
	        mySounder[channel]->play();
			ofLogNotice("osc") << "/play [" << channel << "] ";
		}
    	// Stop soundfile
		else if(m.getAddress() == "/stop" && channel>=0){
	        mySounder[channel]->stop();
			ofLogNotice("osc") << "/stop [" << channel << "] ";
		}
        // Pause soundfile
		else if(m.getAddress() == "/pause" && channel>=0){
			if(m.getArgType(1) == OFXOSC_TYPE_INT32){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
				int paused = m.getArgAsInt32(1);
	        	mySounder[channel]->pause(paused);
				ofLogNotice("osc") << "/pause [" << channel << "] ";
			}else{
				ofLogNotice("osc error") << "/pause [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_INT32";
			}
		}
    	// Set the pitch of a channel
    	else if(m.getAddress() == "/pitch" && channel>=0){
	    	if(m.getArgType(1) == OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING 
	    		float speed = m.getArgAsFloat(1);
	        	mySounder[channel]->setSpeed(speed);
				ofLogNotice("osc") << "/pitch [" << channel << "] pitch: " << speed;
        	}else{
				ofLogNotice("osc error") << "/pitch [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
 	    // Set the volume of a channel
    	else if(m.getAddress() == "/volume" && channel>=0){
	        if(m.getArgType(1) == OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING 
	        	float vol = m.getArgAsFloat(1);
	        	mySounder[channel]->setVolume(vol);
				ofLogNotice("osc") << "/volume [" << channel << "] vol: " << vol;
        	}else{
				ofLogNotice("osc error") << "/volume [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
	    // Set the pan of a channel
    	else if(m.getAddress() == "/pan" && channel>=0){
	        if(m.getArgType(1) == OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
	        	float pan = m.getArgAsFloat(1);
	        	mySounder[channel]->setPan(pan);
				ofLogNotice("osc") << "/pan [" << channel << "] pan: " << pan;
        	}else{
				ofLogNotice("osc error") << "/pan [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
	    // Set the loop of a channel
    	else if(m.getAddress() == "/loop" && channel>=0){
	        if(m.getArgType(1) == OFXOSC_TYPE_INT32){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING 
	        	int loop = m.getArgAsInt32(1);
	        	mySounder[channel]->setLoop(loop);
				ofLogNotice("osc") << "/loop [" << channel << "] loop: " << loop;
        	}else{
				ofLogNotice("osc error") << "/loop [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_INT32";
        	}
        }
        // Set the loop of a channel
    	else if(m.getAddress() == "/loopplay" && channel>=0){
	        mySounder[channel]->setLoop(1);
	        mySounder[channel]->play();
			ofLogNotice("osc") << "/loopplay [" << channel << "] loop: ";
        }
	    // Set the position of a channel
    	else if(m.getAddress() == "/position" && channel>=0){
	        if(m.getArgType(1) == OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
	        	float pos = m.getArgAsFloat(1);
	        	mySounder[channel]->setPosition(pos);
				ofLogNotice("osc") << "/position [" << channel << "] pos: " <<  pos;
        	}else{
				ofLogNotice("osc error") << "/position [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
	    // Set the multiplay of a channel
    	else if(m.getAddress() == "/multiplay" && channel>=0){
	        if(m.getArgType(1) == OFXOSC_TYPE_INT32){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING 	
	        	int mp = m.getArgAsInt32(1);
	        	mySounder[channel]->setMultiPlay(mp);
				ofLogNotice("osc") << "/multiplay [" << channel << "] mp: " << mp << "\n";
        	}else{
				ofLogNotice("osc error") << "/multiplay [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_INT32";
        	}
        }
        // Check for a file being sent 
        // note: the size of the file depends on network buffer sizes 
        // if an file is too big the message won't come through  
        //else if(m.getAddress() == "/file" ){
        //    ofBuffer buffer = m.getArgAsBlob(0);
        //    //receivedImage.loadImage(buffer); TODO: Enable audio transmission
        //}
        // The message is unrecognised
		else if (channel >=0){
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
			ofLogNotice("osc error") << msg_string;
		}

	}

}


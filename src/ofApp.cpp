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
    canstream = false; // Can we stream rather than load in memory?
    if(result["debug"].asString() ==  "true"){
        ofLogToFile(result["audio_log"].asString(), true);
    }else{
        ofSetLogLevel(OF_LOG_ERROR);
    }
    ofLogNotice("STARTUP: "+ofGetTimestampString("%w %e %b %H:%M:%S%A" ));
    nChannels = result["audio_channels"].asDouble();
    audiodirectory = result["audio_path"].asString();
    
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
			string soundfile = audiodirectory+m.getArgAsString(1);
			delete mySounder[channel];
			mySounder[channel] = new ofSounder();
			// Check if the file exists
			ofFile file (soundfile);   
			if(file.exists()){ 
				// Check we have enough memory to load it
				int filesize = file.getSize(); //ofLogNotice("filesize") << filesize;
				int inc = 0;
				int free = 0;
				bool gotfree = false;
				// The system call sometimes doesn't work, so try it a few times
				while (gotfree==false){
					free = ofToInt(ofTkSystem("free | grep Mem | awk '{print $4}'")); 
					ofLogNotice("free") << free;
					if(inc>10 || free > 0){
						gotfree = true;
					}
					inc++;
				}
				float filesizemb = float(filesize)/1000.0/1000.0; //ofLogNotice("filesizemb") << filesizemb;
				float freemb = float(free)/1000.0; //ofLogNotice("freemb") << freemb;
				float availmb = freemb-filesizemb; //ofLogNotice("availmb") << availmb;
				if( (filesizemb*3) > availmb){
					ofLogNotice("memoryIssue") << "Load as stream. Info (" << freemb << "mb) toLoadFile (" << filesizemb << "mb) | /load [" << channel << "]" << m.getArgAsString(1);
				    // Could try streaming the file?
				    if(canstream){
				        mySounder[channel]->load(soundfile, true);
				    }
				}else{
					// Silence error TODO: Fix hack where audio class needs to load twice
					//ofLogNotice("start to load");
					ofSetLogLevel(OF_LOG_SILENT);
	        		mySounder[channel]->load(soundfile, false);
	        		ofSetLogLevel(logLevel);
					ofLogNotice("mem") 	<< "freemem: " << freemb << "mb filesize: " << filesizemb << "mb memavail: " << availmb << "mb";
				}
				ofLogNotice("osc") << "/load [" << channel << "] " << m.getArgAsString(1);
			}else{
				ofLogNotice("osc error") << "File doesn't exist \"/load [" << channel << "] " << soundfile; 
			}
			file.close();  
		}	    
	    // Unload a soundfile
		else if(m.getAddress() == "/unload" && channel>=0){
		    mySounder[channel]->unload();
		    string sfile = mySounder[channel]->soundfile;
		    delete mySounder[channel];
		    mySounder[channel] = new ofSounder();
			ofLogNotice("osc") << "/unload [" << channel << "] " << sfile;
		}
		// Clear all the memory
		else if(m.getAddress() == "/clear"){  
			exit();
		}
		// Set the audio directory
		else if(m.getAddress() == "/setaudiodir"){  
		    if(m.getArgType(0) == OFXOSC_TYPE_STRING){
			    audiodirectory = m.getArgAsString(0);
			    ofLogNotice("osc") << "/setaudiodir " << audiodirectory;
		    }else{
                ofLogNotice("oscerror") << "/setaudiodir NOTSTRING";
		    }
		}
        // Set a directory for random files to be selected from
		else if(m.getAddress() == "/setrandomdir"){  
			if(m.getArgType(0) == OFXOSC_TYPE_STRING){
			    randomdirectory = m.getArgAsString(0);
			    ofLogNotice("osc") << "/setrandomdir " << randomdirectory;
			}else{
                ofLogNotice("oscerror") << "/setrandomdir NOTSTRING";
			}
		}
	    // Set if we can stream or not
		else if(m.getAddress() == "/setcanstream"){  
            if(m.getArgType(0)==OFXOSC_TYPE_INT32){
			    int resp = m.getArgAsInt32(0);
			    if(resp==1){
			        canstream = true;
			    }else{
                    canstream = false; 
			    }
			    ofLogNotice("osc") << "/setcanstream " << resp;
			}else{
			    ofLogNotice("oscerror") << "/setcanstream NOTINT";
            }
		}
	    // Play soundfile
		else if(m.getAddress() == "/play" && channel>=0){
	        mySounder[channel]->play();
			ofLogNotice("osc") << "/play [" << channel << "] " << mySounder[channel]->soundfile;
		}
    	// Stop soundfile
		else if(m.getAddress() == "/stop" && channel>=0){
	        mySounder[channel]->stop();
			ofLogNotice("osc") << "/stop [" << channel << "] "  << mySounder[channel]->soundfile;
		}
        // Pause soundfile
		else if(m.getAddress() == "/pause" && channel>=0){
			if(m.getArgType(1) == OFXOSC_TYPE_INT32){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
				int paused = m.getArgAsInt32(1);
	        	mySounder[channel]->pause(paused);
				ofLogNotice("osc") << "/pause [" << channel << "] " << paused << mySounder[channel]->soundfile;
			}else{
				ofLogNotice("osc error") << "/pause [" << channel << "] "  << mySounder[channel]->soundfile << " | var 1 not recognised as OFXOSC_TYPE_INT32";
			}
		}
    	// Set the pitch of a channel
    	else if(m.getAddress() == "/pitch" && channel>=0){
	    	if(m.getArgType(1) == OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING 
	    		float speed = m.getArgAsFloat(1);
	        	mySounder[channel]->setSpeed(speed);
				ofLogNotice("osc") << "/pitch [" << channel << "] pitch: " << speed << " | "  << mySounder[channel]->soundfile;
        	}else{
				ofLogNotice("osc error") << "/pitch [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
		// Set the fadein of a channel
    	else if(m.getAddress() == "/fadein" && channel>=0){
	    	if(m.getArgType(1) == OFXOSC_TYPE_FLOAT && m.getArgType(2) == OFXOSC_TYPE_FLOAT ){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING 
	    		float fade = m.getArgAsFloat(1);
	        	float timer = m.getArgAsFloat(2); 
	        	mySounder[channel]->fadein(fade, timer);
				ofLogNotice("osc") << "/fadein [" << channel << "] fadein: " << fade << " | "  << mySounder[channel]->soundfile;
        	}else{
				ofLogNotice("osc error") << "/fadein [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
		// Set the fadeout of a channel
    	else if(m.getAddress() == "/fadeout" && channel>=0){
	    	if(m.getArgType(1) == OFXOSC_TYPE_FLOAT && m.getArgType(2) == OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING 
	    		float fade = m.getArgAsFloat(1);
	    		float timer = m.getArgAsFloat(2); 
	        	mySounder[channel]->fadeout(fade, timer);
				ofLogNotice("osc") << "/fadeout [" << channel << "] fadeout: " << fade << " timer: " << timer << " | "  << mySounder[channel]->soundfile;
        	}else{
				ofLogNotice("osc error") << "/fadeout [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
 	    // Set the volume of a channel
    	else if(m.getAddress() == "/volume" && channel>=0){
	        if(m.getArgType(1) == OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING 
	        	float vol = m.getArgAsFloat(1);
	        	mySounder[channel]->setVolume(vol);
				ofLogNotice("osc") << "/volume [" << channel << "] vol: " << vol << " "  << mySounder[channel]->soundfile;
        	}else{
				ofLogNotice("osc error") << "/volume [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
	    // Set the pan of a channel
    	else if(m.getAddress() == "/pan" && channel>=0){
	        if(m.getArgType(1) == OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
	        	float pan = m.getArgAsFloat(1);
	        	mySounder[channel]->setPan(pan);
				ofLogNotice("osc") << "/pan [" << channel << "] pan: " << pan << " "  << mySounder[channel]->soundfile;
        	}else{
				ofLogNotice("osc error") << "/pan [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
	    // Set the loop of a channel
    	else if(m.getAddress() == "/loop" && channel>=0){
	        if(m.getArgType(1) == OFXOSC_TYPE_INT32){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING 
	        	int loop = m.getArgAsInt32(1);
	        	mySounder[channel]->setLoop(loop);
				ofLogNotice("osc") << "/loop [" << channel << "] loop: " << loop << " " << mySounder[channel]->soundfile;
        	}else{
				ofLogNotice("osc error") << "/loop [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_INT32";
        	}
        }
        // Set the loop of a channel
    	else if(m.getAddress() == "/loopplay" && channel>=0){
	        mySounder[channel]->setLoop(1);
	        mySounder[channel]->play();
			ofLogNotice("osc") << "/loopplay [" << channel << "] (loop then play)" << mySounder[channel]->soundfile;
        }
	    // Set the position of a channel
    	else if(m.getAddress() == "/position" && channel>=0){
	        if(m.getArgType(1) == OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
	        	float pos = m.getArgAsFloat(1);
	        	mySounder[channel]->setPosition(pos);
				ofLogNotice("osc") << "/position [" << channel << "] pos: " <<  pos << " " << mySounder[channel]->soundfile;
        	}else{
				ofLogNotice("osc error") << "/position [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
        // Set the superlooper of a channel
    	else if(m.getAddress() == "/superlooper" && channel>=0){
	        if(m.getArgType(1) == OFXOSC_TYPE_STRING &&  m.getArgType(2) == OFXOSC_TYPE_FLOAT && m.getArgType(3) == OFXOSC_TYPE_FLOAT ){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
	        	string command = m.getArgAsString(1); 
	        	float pos = m.getArgAsFloat(2);
	        	float len = m.getArgAsFloat(3);
	        	mySounder[channel]->setSuperLooper(command, pos, len);
				ofLogNotice("osc") << "/superlooper [" << channel << "] command(on|off):" << command << " pos: " <<  pos << " len: " << len << " " << mySounder[channel]->soundfile;
        	}else{
				ofLogNotice("osc error") << "/superlooper [" << channel << "] " << "| vars not recognised as: string(on|off) float float";
        	}
        }
        // Set all channels marked masschange to superloop
    	else if(m.getAddress() == "/masssuperlooper"){
	        if(m.getArgType(0)==OFXOSC_TYPE_STRING &&  
	           m.getArgType(1)==OFXOSC_TYPE_FLOAT && 
	           m.getArgType(2)==OFXOSC_TYPE_FLOAT ){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
	        	string command = m.getArgAsString(0); 
	        	float pos = m.getArgAsFloat(1);
	        	float len = m.getArgAsFloat(2);
                for (int i = 0; i < nChannels; i++){
	        	    if(mySounder[i]->masschange == true){
                        mySounder[i]->setSuperLooper(command, pos, len);
	        	    }
	        	}
				ofLogNotice("osc") << "/masssuperlooper command(on|off):" << command << " pos: " <<  pos << " len: " << len;
        	}else{
				ofLogNotice("osc error") << "/masssuperlooper | vars not recognised as: string(on|off) float float";
        	}
        }

        // Set the superPitch of a channel
    	else if(m.getAddress() == "/superpitch" && channel>=0){
	        if(m.getArgType(1) == OFXOSC_TYPE_STRING && m.getArgType(2)==OFXOSC_TYPE_FLOAT && m.getArgType(3)==OFXOSC_TYPE_FLOAT && m.getArgType(4) == OFXOSC_TYPE_FLOAT && OFXOSC_TYPE_FLOAT && m.getArgType(5) ){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
	        	string command = m.getArgAsString(1);
	        	float inc = m.getArgAsFloat(2);  
	        	float speed = m.getArgAsFloat(3);
	        	float max = m.getArgAsFloat(4);
	        	float min = m.getArgAsFloat(5);
	        	mySounder[channel]->setSuperPitch(command, inc, speed, max, min);
				ofLogNotice("osc") << "/superpitch [" << channel << "] command: " <<  command << " inc:" << inc << " speed: " << speed << " max: " << max << " min:" << min << " " << mySounder[channel]->soundfile;
        	}else{
				ofLogNotice("osc error") << "/superpitch [" << channel << "] " << "| vars not recognised as: string,float,float";
        	}
        }
        // Set the mass superPitch of a channel
    	else if(m.getAddress() == "/masssuperpitch"){
	        ofLogNotice("osc")  << "Got masssuperpitch"; 
	        if(m.getArgType(0)==OFXOSC_TYPE_STRING &&
	           m.getArgType(1)==OFXOSC_TYPE_FLOAT && 
	           m.getArgType(2)==OFXOSC_TYPE_FLOAT && 
	           m.getArgType(3)==OFXOSC_TYPE_FLOAT && 
	           m.getArgType(4)==OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
	        	string command = m.getArgAsString(0);
	        	float inc = 0.2; //m.getArgAsFloat(1);  
	        	float speed = 0.2; //m.getArgAsFloat(2);
	        	float max = 19.5; //m.getArgAsFloat(3);
	        	float min = 0.5; //m.getArgAsFloat(4);
    	        for (int i = 0; i < nChannels; i++){
	        	    if(mySounder[i]->masschange == true){
                        mySounder[i]->setSuperPitch(command, inc, speed, max, min);
	        	    }
	        	}
				ofLogNotice("osc") << "/masssuperpitch command:" <<  command << " inc:" << inc << " speed:" << speed << " max:" << max << " min:" << min;
        	}else{
				ofLogNotice("osc error") << "/masssuperpitch | vars not recognised as: string float float float float";
        	}
        }
	    // Set the pitch of all channels if the 'masschange' variable has been set to 1
    	else if(m.getAddress() == "/masspitch"){
	        if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
	        	float pitch = m.getArgAsFloat(0);
	        	for (int i = 0; i < nChannels; i++){
	        	    if(mySounder[i]->masschange == true){
						//ofLogNotice("massvchangeTrue:") << i;
                        mySounder[i]->setSpeed(pitch);
	        	    }
	        	}
				ofLogNotice("osc") << "/masspitch " <<  pitch;
        	}else{
				ofLogNotice("osc error") << "/masspitch | var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
	    // Set the volume of all channels if the 'masschange' variable has been set to 1
    	else if(m.getAddress() == "/massvolume"){
	        if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
	        	float vol = m.getArgAsFloat(0);
	        	for (int i = 0; i < nChannels; i++){
	        	    if(mySounder[i]->masschange == true){
                        mySounder[i]->setVolume(vol);
	        	    }
	        	}
				ofLogNotice("osc") << "/massvolume " <<  vol;
        	}else{
				ofLogNotice("osc error") << "/massvolume | var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
        // Set the superlooper if the 'masschange' variable has been set to 1
    	else if(m.getAddress() == "/masslooper"){
	        if(m.getArgType(0) == OFXOSC_TYPE_FLOAT){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
	        	float vol = m.getArgAsFloat(0);
	        	for (int i = 0; i < nChannels; i++){
	        	    if(mySounder[i]->masschange == true){
                        mySounder[i]->setVolume(vol);
	        	    }
	        	}
				ofLogNotice("osc") << "/masslooper " <<  vol;
        	}else{
				ofLogNotice("osc error") << "/masslooper | var 1 not recognised as OFXOSC_TYPE_FLOAT";
        	}
        }
        // Can this channel be changed as part of a 'masspitch' or 'massposition' command
    	else if(m.getAddress() == "/masschange"  && channel>=0){
	        if(m.getArgType(0) == OFXOSC_TYPE_INT32){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING
	        	int change = m.getArgAsInt32(1);
	        	if(change==1){
	        	    mySounder[channel]->setMassChange(true);
	        	}else{
                    mySounder[channel]->setMassChange(false);
	        	}
				ofLogNotice("osc") << "/masschange [" << channel << "] change: " <<  change;
        	}else{
				ofLogNotice("osc error") << "/masschange [" << channel << "] " << "| var 1 not recognised as OFXOSC_TYPE_INT32";
        	}
        }
	    // Set the multiplay of a channel
    	else if(m.getAddress() == "/multiplay" && channel>=0){
	        if(m.getArgType(1) == OFXOSC_TYPE_INT32){ // OFXOSC_TYPE_INT32 OFXOSC_TYPE_FLOAT OFXOSC_TYPE_STRING 	
	        	int mp = m.getArgAsInt32(1);
	        	mySounder[channel]->setMultiPlay(mp);
				ofLogNotice("osc") << "/multiplay [" << channel << "] mp: " << mp << "" <<  mySounder[channel]->soundfile;
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

// cleanup on exit
void ofApp::exit(){
	// Update all the sound objects
 	for (int i = 0; i < nChannels; i++){
 		delete mySounder[i];
 	}
 	ofLogNotice("oss") << "Exited SonicBikeAudioEngine";
}

// an exact replica of ofSystem to experiemnt with. wasnt working on ARM
string ofApp::ofTkSystem(const string& command){
	FILE * ret = NULL;
#ifdef TARGET_WIN32
	ret = _popen(command.c_str(),"r");
#else 
	ret = popen(command.c_str(),"r");
#endif	
	string strret;
	int c;

	if (ret == NULL){
		ofLogError("ofUtils") << "ofSystem(): error opening return file for command \"" << command  << "\"";
	}else{
		c = fgetc (ret);
		while (c != EOF) {
			//ofLogNotice("ofUtils") << "'" << c << "'";
			strret += c;
			c = fgetc (ret);
		}
#ifdef TARGET_WIN32
		_pclose (ret);
#else
		pclose (ret);
#endif
	}

	return strret;
}

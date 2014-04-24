#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Load and parse the config file
    bool parsingSuccessful = result.open(CONFIG);
    if (parsingSuccessful){
        ofLogNotice("Loaded config:") << result.getRawString();
    }else{
         ofLogError("Config: ")  << "Failed to parse JSON" << endl;
    }
    int channels = result["audiochannels"].asDouble();
    audiodirectory = result["audiodirectory"].asString();
    startupsound = audiodirectory+'/'+result["startupsound"].asString(); 
    cout << "CHANNELS: " << channels << "\n";
    cout << "STARTUP SOUND: " << startupsound << "\n";

    // Initialise some sound control objects
	mySounder = new ofSounder*[channels];
	for (int i = 0; i < channels; i++){                            
	    mySounder[i] = new ofSounder(startupsound);                              
	}

	// OSC Vars: listen on the given port
	cout << "OSC LISTEN ON PORT: " << PORT << "\n";
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
			cout << "osc: /load [" << channel << "] " << soundfile << "\n";
		}
	    
	    // Play soundfile
		else if(m.getAddress() == "/play"){
			int channel = m.getArgAsInt32(0);
	        mySounder[channel]->play();
			cout << "osc: /play [" << channel << "] " << "\n";
		}
    	
    	// Set the pitch of a channel
    	else if(m.getAddress() == "/set/pitch"){
			int channel = m.getArgAsInt32(0);
	        float speed = m.getArgAsFloat(1);
	        mySounder[channel]->setSpeed(speed);
			cout << "osc: /set/pitch [" << channel << "] " << "\n";
        }

 	    // Set the volume of a channel
    	else if(m.getAddress() == "/set/volume"){
			int channel = m.getArgAsInt32(0);
	        float vol = m.getArgAsFloat(1);
	        mySounder[channel]->setVolume(vol);
			cout << "osc: /set/volume [" << channel << "] " << "\n";
        }

	    // Set the pan of a channel
    	else if(m.getAddress() == "/set/pan"){
			int channel = m.getArgAsInt32(0);
	        float pan = m.getArgAsFloat(1);
	        mySounder[channel]->setPan(pan);
			cout << "osc: /set/pan [" << channel << "] " << "\n";
        }
	    
	    // Set the loop of a channel
	    // TODO: Deal with the bool properly   
    	else if(m.getAddress() == "/set/loop"){
			int channel = m.getArgAsInt32(0);
	        float loop = m.getArgAsInt32(1);
	        mySounder[channel]->setLoop(loop);
			cout << "osc: /set/loop [" << channel << "] " << "\n";
        }
	    
	    // Set the position of a channel
    	else if(m.getAddress() == "/set/position"){
			int channel = m.getArgAsInt32(0);
	        float pos = m.getArgAsInt32(1);
	        mySounder[channel]->setPosition(pos);
			cout << "osc: /set/position [" << channel << "] " << "\n";
        }
	    
	    // Set the multiplay of a channel
	    // TODO: Deal with the bool properly
    	else if(m.getAddress() == "/set/multiplay"){
			int channel = m.getArgAsInt32(0);
	        float mp = m.getArgAsInt32(1);
	        mySounder[channel]->setMultiPlay(mp);
			cout << "osc: /set/multiplay [" << channel << "] " << "\n";
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
			cout << "osc: " << msg_string << "\n";
		}

	}

}




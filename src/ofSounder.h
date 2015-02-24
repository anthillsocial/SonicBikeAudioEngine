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

#ifndef _OF_BALL		// by using this if statement you prevent the class to be called more than once - in other words what we're saying here is 
#define _OF_BALL		// if the class has NOT been defined then define it
					
#include "ofMain.h"

class ofSounder {
	  
   public:   

		// methods
		void update();
        void load(string newsoundfile, bool stream);
        void unload();
        void play();
        void stop();
        void fadeout(float fade, float time);
        void fadein(float fade, float time);
        void pause(bool paused);
        void setSpeed(float speed);
        void setPan(float pan);
        void setVolume(float vol);
        void setLoop(bool loop);
        void setPosition(float pos);
        void setMultiPlay(bool multi);
        void setMassChange(bool change, string param);   
        void setSuperLooper(string command, float pos, float len);
        void setSuperPitch(string command, float inc, float speed, float max, float min);

		// constructor
		ofSounder();

		// variables
        string soundfile;
        string state; 
        ofSoundPlayer  mysound;
        bool loaded;
        bool masschange;
        bool masspitch;
		bool massvolume;
		bool masssuperlooper;
		bool masssuperpitch;
		bool massstartpoint; 

		string faderstate; 
		float faderfade;
		float fadervol;
		int fadertimepassed;
		int fadertimer;

		string superloopercommand;
		float superlooperpos;
		float superlooperlen; 
        float superloopertimepassed;

        string superpitchcommand;
		float superpitchspeed; 
		float superpitchinc;
        float superpitchmax;
        float superpitchmin;
        float superpitchtimepassed; 
        float superpitchpitch;

	private:
 
   
};					

#endif 

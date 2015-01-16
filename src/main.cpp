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
//#include "ofMain.h"
// Commented out ofMain.h and copied it contents here so we can selectivly load
// #pragma once

//--------------------------
// utils
#include "ofConstants.h"
#include "ofFileUtils.h"
#include "ofLog.h"
#include "ofSystemUtils.h"
#include "ofThread.h"
#include "ofURLFileLoader.h"
#include "ofUtils.h"

//--------------------------
// types
#include "ofBaseTypes.h"
#include "ofTypes.h"
//#include "ofColor.h"
//#include "ofPoint.h"
//#include "ofRectangle.h"
//#include "ofXml.h"
#include "ofParameter.h"
#include "ofParameterGroup.h"

//--------------------------
// math
#include "ofMath.h"
#include "ofVectorMath.h"

//--------------------------
// communication
//#if !defined( TARGET_OF_IOS ) & !defined(TARGET_ANDROID)
//	#include "ofSerial.h"
//	#include "ofArduino.h"
//#endif

//--------------------------
// gl
#include "ofFbo.h"
#include "ofGLRenderer.h"
#include "ofGLUtils.h"
//#include "ofLight.h"
//#include "ofMaterial.h"
//#include "ofShader.h"
//#include "ofTexture.h"
//#include "ofVbo.h"
//#include "ofVboMesh.h"
//#include "ofGLProgrammableRenderer.h"
//#include "ofGLRenderer.h"

//--------------------------
// graphics
//#if !defined( TARGET_OF_IOS ) & !defined(TARGET_ANDROID)
//#include "ofCairoRenderer.h"
//#endif
//#include "ofGraphics.h"
//#include "ofImage.h"
//#include "ofPath.h"
//#include "ofPixels.h"
//#include "ofPolyline.h"
//#include "ofRendererCollection.h"
//#include "ofTessellator.h"
//#include "ofTrueTypeFont.h"

//--------------------------
// app
#include "ofBaseApp.h"
#include "ofAppRunner.h"

//--------------------------
// audio
#include "ofSoundStream.h"
#include "ofSoundPlayer.h"

//--------------------------
// video
//#include "ofVideoGrabber.h"
//#include "ofVideoPlayer.h"

//--------------------------
// events
#include "ofEvents.h"

//--------------------------
// 3d
//#include "of3dUtils.h"
//#include "ofCamera.h"
//#include "ofEasyCam.h"
//#include "ofMesh.h"
//#include "ofNode.h"

// Return to normal
#include "ofApp.h"
#include "ofAppNoWindow.h"

//========================================================================
int main( ){
    ofAppNoWindow w;
    ofSetupOpenGL(&w,13,3, OF_WINDOW);
    ofRunApp( new ofApp());
}

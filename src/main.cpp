#include "ofMain.h"
#include "ofApp.h"
#include "ofAppNoWindow.h"

//========================================================================
int main( ){
    ofAppNoWindow w;
    ofSetupOpenGL(&w,1024,768, OF_WINDOW);
    ofRunApp( new ofApp());
}

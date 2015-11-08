#pragma once

#include "ofMain.h"
#include "ofxFilterLibrary.h"
#include "ofxCUPS.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    int                         _currentFilter;
    vector<AbstractFilter *>    _filters;
    ofVideoGrabber              _video;
    ofxCUPS printer;
    
    
    ofPixels pix;
    
    ofTrueTypeFont  contdown;
    ofFbo m_fbo;
    ofFbo m_fboFrame;
    ofImage m_imageOutput;
    float timer, time,timingContdown;
    
    string path;
    
    bool m_shoot;
    
    void setupArduino();
    
    void updateArduino();
    
    char		bytesRead[3];				// data from serial, we will be trying to read 3
    char		bytesReadString[4];			// a string needs a null terminator, so we need 3 + 1 bytes
    int			nBytesRead;					// how much did we read?
    int			nTimesRead;					// how many times did we read?
    float		readTime;					// when did we last read?
    
    ofSerial	serial;
    
    ofVec2f frameSize;
    
    vector<ofImage> frames;
    
    int frameIndex;
    
    ofSoundPlayer  synth;
    
};

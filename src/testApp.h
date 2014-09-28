#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxKinectUsers.h"

#include "ofxXmlSettings.h"

class testApp : public ofBaseApp {

    public:
    
        void setup();
        void update();
        void draw();
        void loadSettings(string a_sFileName);

        void keyPressed  (int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void windowResized(int w, int h);
        
        ofxKinectUsers	inter;
        ofxXmlSettings m_xmlSettings;
        
        void	inPose(ofxUser & user);
        void	inGesture(ofxUser & user);
        void	userIn(ofxUser & user);
        void	userMove(ofxUser & user);
        void	userOut(ofxUser & user);
	
    private:
    
        bool	m_bDisplayInterfaceDebug;
        bool    m_bFullscreen;
};

#endif
    
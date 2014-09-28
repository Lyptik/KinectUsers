#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    	
    string sSettingsFileName = "settings.xml";
    
    loadSettings(sSettingsFileName);
    inter.setup(sSettingsFileName);
    
    ofEnableAlphaBlending();

    if(m_bFullscreen)
        ofToggleFullscreen();
    
	ofAddListener(inter.inPose,this,&testApp::inPose);
	ofAddListener(inter.inGesture,this,&testApp::inGesture);
	ofAddListener(inter.userIn,this,&testApp::userIn);
	ofAddListener(inter.userMove,this,&testApp::userMove);
	ofAddListener(inter.userOut,this,&testApp::userOut);
}

//--------------------------------------------------------------
void testApp::update() {
	inter.update();
}

//--------------------------------------------------------------
void testApp::loadSettings(string a_sFileName) {

    if(!m_xmlSettings.loadFile(a_sFileName)) {
        cerr << "[app] Could not load " + a_sFileName + " ! (not found or not formated correctly)" << endl;
        ofExit(2);
    }

    m_xmlSettings.pushTag("display");
    m_bFullscreen = (bool) m_xmlSettings.getValue("init:fullscreen",false);
    m_bDisplayInterfaceDebug = (bool) m_xmlSettings.getValue("init:displayDebugInterface", true);
    m_xmlSettings.popTag();
}

//--------------------------------------------------------------
void testApp::draw() {
	ofSetColor(255, 255, 255);
	
	if (m_bDisplayInterfaceDebug)
    {
		inter.debugDraw();
    } else
    {
        ofSetColor(0);
        ofDrawBitmapString("'d' Display interface\n'f' Full screen\n'1' Prespective 0\n'2' Perspective 2\n'3' Perspective 3\n",20, 20);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {

	switch(key){
            
		case 'd':
        case 'D':
			m_bDisplayInterfaceDebug=!m_bDisplayInterfaceDebug;
			break;
        case 'f':
        case 'F':
            ofToggleFullscreen();
            break;
		case '1':
			inter.setPerspective(0);
			break;
		case '2':
			inter.setPerspective(1);
			break;
		case '3':
			inter.setPerspective(2);
			break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	if (m_bDisplayInterfaceDebug) {
		inter.mouseDragged(x,y,button);
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
    if (m_bDisplayInterfaceDebug) {
		inter.mouseReleased(x,y,button);
	}
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void testApp::inPose(ofxUser & user) {
	// Debug
    //cout << "INPOSE" << endl;
}

//--------------------------------------------------------------
void testApp::inGesture(ofxUser & user) {
	// Debug
    //cout << "INGESTURE" << endl;
}

//--------------------------------------------------------------
void testApp::userIn(ofxUser & user) {
	// Debug
    //cout << "USERIN" << endl;
}

//--------------------------------------------------------------
void testApp::userMove(ofxUser & user) {
	// Debug
    //cout << "USERMOVE" << endl;
}

//--------------------------------------------------------------
void testApp::userOut(ofxUser & user) {
	// Debug
    //cout << "USEROUT" << endl;
}

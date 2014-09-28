/*
 *  ofxKinectUsers.h
 *  ofxKinectUsers
 *
 *  Created by Patricio Gonzalez Vivo on 17/03/11.
 *  Copyright 2011 www.PatricioGonzalezVivo.com All rights reserved.
 *
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 */

#include "ofxKinectUsers.h"

#define SMOOTHVALUE 3 // john

/////////////////////////////////////////////////////////////////// SETUP
//--------------------------------------------------------------------------------------------

// Some default initial configuration flags
ofxKinectUsers::ofxKinectUsers(){
	nPerspective = -1;
	setMasking(false);
    bPreviousPose = false;
    bPreviousGesture = false;
    for(int i  = 0 ; i < MAX_USERS ; i++)
        lbPreviousDetectState.push_back(false);
}

// Setup and initiation of managers and configuration list
void ofxKinectUsers::setup(string _configFile){
    
    configFile = _configFile;					// xml file use to config everything

	openni.setup(users, _configFile);			// openNI Init
    
    loadOscSettings(configFile);                // Init OSC connection
    
	loadPerspectives(configFile);				// Loading Perspectives
    
    loadOscSettings(configFile);                // Laoding osc
    
    loadPointsToSend(configFile);				// Loading Skeleton jooints to send
    
	loadDetectionArea(configFile);				// Loading Detection Area
	
	gestures.setup(users,configFile, 
				   &defaultPerspective);		// Loading Gestures
	
	hands.setup(users,configFile,perspectives);	// Loading Hand Manager for sending TUIO positions of the hands
    for(int i = 0 ; i < MAX_USERS; i++)
    {        
		users[i].setup(i, &defaultPerspective);	// All users resive an id and default perspective
        
        // vector which gather the sum of each joint
        users[i].setSumList(l_sName.size(), *new ofVec3f()); // john
        
        // give the list of joint name to ofxUser
        users[i].setJointNameList(l_sName); // john
        
        // vector which gather the sum of each joint
       // users[i].setWindowValue(l_sName.size(), SMOOTHVALUE);
        
        // vector which gather the table of SMOOTHVALUE by joint
        for(int t = 0 ; t < l_sName.size() ; t++) // john
        {
            vector<ofVec3f> valueListWindow;
            for(int cpt = 0 ; cpt < SMOOTHVALUE ; cpt++)
            {
                ofVec3f indexValue = *new ofVec3f();
                indexValue.x = 0;
                indexValue.y = 0;
                indexValue.z = 0;
                valueListWindow.push_back(indexValue);
            }
            users[i].l_fValuesWindow.push_back(valueListWindow);
        }
        
        // vector which permit to commant the value list
        vector<int> indexWindow;
        for(int cpt = 0 ; cpt < l_sName.size() ; cpt++)
            indexWindow.push_back(0);
        users[i].setIndexWindow(indexWindow);
    }
}

void ofxKinectUsers::loadPerspectives(string filePath){

	ofxXmlSettings XML;
	cout << "Loading perspectives: " << filePath;
	
	if (XML.loadFile(filePath)){
		cout << " [ OK ]" << endl;
		
		XML.pushTag("PERSPECTIVES");
		for (int i = 0; i < XML.getNumTags("PERSPECTIVE"); i++){
			XML.pushTag("PERSPECTIVE", i);
			
			Perspective p;
			p.name = XML.getValue("NAME","LATERAL");
			ofMatrix4x4 m;
			p.mat = m;
			
			cout << " Loading perspective " << p.name << " y ";
			for (int j = 0; j < XML.getNumTags("SCALE"); j++){
				XML.pushTag("SCALE", j);
				p.mat.scale(XML.getValue("X",1.0),XML.getValue("Y",1.0),XML.getValue("Z",1.0));
				XML.popTag();
				cout << " SCALE ";
			}
			
			for (int j = 0; j < XML.getNumTags("ROTATE"); j++){
				XML.pushTag("ROTATE", j);
				p.mat.rotate(XML.getValue("ANGLE",90.0),XML.getValue("X",0.0),XML.getValue("Y",0.0),XML.getValue("Z",0.0));
				XML.popTag();
				cout << " ROTATE ";
			}
			
			for (int j = 0; j < XML.getNumTags("TRANSLATE"); j++){
				XML.pushTag("TRANSLATE", j);
				p.mat.translate(XML.getValue("X",0.0),XML.getValue("Y",0.0),XML.getValue("Z",0.0));
				XML.popTag();
				cout << " TRANSLATE ";
			}
			
			perspectives.push_back(p);
			cout << " [OK] " << endl;
			XML.popTag();
		}
		
		if (XML.tagExists("DEFAULT")){
			setPerspective(XML.getValue("DEFAULT", "FRONT"));
		}
		
		XML.popTag();
	} else
		cout << " [ FAIL ]" << endl;
}

void ofxKinectUsers::loadDetectionArea(string filePath){
	ofxXmlSettings XML;
	cout << "Loading detection area set up: " << filePath;
	
	if (XML.loadFile(filePath)){
		cout << " [ OK ]" << endl;
		
		XML.pushTag("AREA");
		
		XML.pushTag("MIN");
		ofVec3f minV;
		minV.set(XML.getValue("X",0.0),XML.getValue("Y",0.0),XML.getValue("Z",0.0));
		XML.popTag();
		
		XML.pushTag("MAX");
		ofVec3f maxV;
		maxV.set(XML.getValue("X",640.0),XML.getValue("Y",480.0),XML.getValue("Z",6000.0));
		XML.popTag();
		
		dArea.set(minV,maxV);
		
		XML.popTag();
	} else
		cout << " [ FAIL ]" << endl;
}

void ofxKinectUsers::saveDetectionArea(string filePath){
	ofxXmlSettings XML;
	cout << "Saving detection area: " << filePath;
	
	if (XML.loadFile(filePath)){
		XML.pushTag("AREA");
		
		XML.setValue("MIN:X", dArea.min.x);
		XML.setValue("MIN:Y", dArea.min.y);
		XML.setValue("MIN:Z", dArea.min.z);
		
		XML.setValue("MAX:X", dArea.max.x);
		XML.setValue("MAX:Y", dArea.max.y);
		XML.setValue("MAX:Z", dArea.max.z);
		
		XML.popTag();
		XML.saveFile(filePath);
		cout << " [ SAVED ]" << endl;
	} else
		cout << " [ FAIL ]" << endl;
}

void ofxKinectUsers::setPerspective(int _n){
	if(( _n < perspectives.size() ) && (_n > -1) ) { 
		defaultPerspective = perspectives[_n]; 
		nPerspective = _n;
	}
}

void ofxKinectUsers::setPerspective(string _name){
	for (int i = 0 ; i < perspectives.size() ; i++){
		if (perspectives[i].name == _name){
			defaultPerspective = perspectives[i];
			nPerspective = i;
			return;
		}
	}
}

void ofxKinectUsers::loadOscSettings(string filePath) { // john
	ofxXmlSettings XML;
	cout << "Loading networks Settings: " << filePath;
	
	if (XML.loadFile(filePath)){
		cout << " [ OK ]" << endl;
		
		XML.pushTag("osc");
        m_iPort = XML.getValue("out:port",7000);
        m_sHost = XML.getValue("out:remoteHost","localhost");
		XML.popTag();
	} else
		cout << " [ FAIL ]" << endl;

    sender.setup(m_sHost, m_iPort); 
}

void ofxKinectUsers::loadPointsToSend(string filePath){ // john
	ofxXmlSettings XML;
	cout << "Loading Points to Send by OSC Settings: " << filePath;
	
	if (XML.loadFile(filePath)){
		cout << " [ OK ]" << endl;
        
		XML.pushTag("POINTS");
        int nbPointsTag = XML.getNumTags("POINT");
        
		for (int i = 0; i < nbPointsTag ; i++){
			XML.pushTag("POINT", i);
            
			string joint = XML.getValue("NAME","head");
            string sendTF = XML.getValue("SEND","TRUE");
			l_sName.push_back(joint);
            l_bSend.push_back(sendTF);
            
			XML.popTag();
		}
		XML.popTag();
	} else
		cout << " [ FAIL ]" << endl;
    
    sender.setup(m_sHost, m_iPort);
}

/////////////////////////////////////////////////////////////////// UPDATE
//--------------------------------------------------------------------------------------------

void ofxKinectUsers::update(){
	openni.update();
	ofxUser activeUser;
	nUsers = nBones = 0;
	float verticalNorm = 0;
	
    int i = 0;
    bool findActiveUser = false;
    users[i].update(); // update the first user which compulsory exists
    
    while(!findActiveUser && i < MAX_USERS){
        if ( users[i].isIni ){
			if ( dArea.check3D( users[i].centroid ) ){
				if ( !users[i].isActive ){
					users[i].isActive = true;
					users[i].isIdle	= false;
					users[i].idleTime = 0;
					ofNotifyEvent(userIn, users[i],this);
                    sendOscBoolMsg("/player0/detect",1); //&
				} else
					if ((users[i].velF != 0) )//|| (users[i].bonesVelF != 0))
						ofNotifyEvent(userMove, users[i],this);
				findActiveUser = true;
                
				if ( !users[i].isIdle ){
					nUsers++;
					verticalNorm += users[i].centroid.y;
					if ( users[i].bones ){
						nBones++;
						
						gestures.update(i);
						hands.update(i);
						
                        sendOscBoolMsg("/player0/skeleton", 1); //&
                        
						if (users[i].inPose)
							ofNotifyEvent(inPose, users[i],this);
						
						if (users[i].inGesture)
							ofNotifyEvent(inGesture, users[i],this);
					}
				}
			} else {
				if (users[i].isActive){
					users[i].isActive = false;
					users[i].isIdle	= true;
					users[i].bones = false;
					ofNotifyEvent(userOut, users[i],this);
                    
                    // search if other user exists inside the active area
                    bool findOtherActiveUser = false;
                    for(int y = 0 ; y < MAX_USERS ; y++)
                        if( y != i )
                            if(users[y].isActive)
                                findOtherActiveUser = true;
                    
                    // if not, then you can disable the skeleton and the detect
                    if(!findOtherActiveUser)
                    {
                        sendOscBoolMsg("/player0/detect",0); //&
                        sendOscBoolMsg("/player0/skeleton",0); //&
                    }
				}
			}
		}
        i++;
    }
    
    // Send user information by OSC
    if(findActiveUser)
    {
        if( (i-1) != 0 )
            users[i-1].update();
        this->sendOsc(users[i-1]); //&
	}
    hands.send();
}

///////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------

void ofxKinectUsers::mouseDragged(int _x, int _y, int _button){
	if (_button == 0){
		if (ofDist(_x, _y, dArea.min.x, dArea.min.y) <= 30){
			dArea.min.x = _x;
			dArea.min.y = _y;
			dArea.update();
			//saveDetectionArea(configFile); // dac TOKILL ?
		} else if (ofDist(_x, _y, dArea.max.x, dArea.max.y) <= 30){
			dArea.max.x = _x;
			dArea.max.y = _y;
			dArea.update();
			//saveDetectionArea(configFile); // dac TOKILL ?
		}
	} else if (_button == 2){
		if ( (_x >= dArea.min.x-20) && (_x <= dArea.min.x+20) ){
			dArea.min.z = ofMap(_y, dArea.min.y, dArea.max.y, 0, 6000, true);
			dArea.update();
			saveDetectionArea(configFile); // dac TOKILL ?
		} else if ( (_x >= dArea.max.x-20) && (_x <= dArea.max.x+20) ){
			dArea.max.z = ofMap(_y, dArea.min.y, dArea.max.y, 0, 6000, true);;
			dArea.update();
			//saveDetectionArea(configFile); // dac TOKILL ?
		} 
	} 
}

void ofxKinectUsers::mouseReleased(int x, int y, int button) {

    if(button == 0) {
        if(ofDist(x, y, dArea.min.x, dArea.min.y) <= 30 || ofDist(x, y, dArea.max.x, dArea.max.y) <= 30)
            saveDetectionArea(configFile);
    } else {
        if(button == 2) {
            if(( (x >= dArea.min.x-20) && (x <= dArea.min.x+20) ) || ( (x >= dArea.max.x-20) && (x <= dArea.max.x+20) ))
                saveDetectionArea(configFile);                
        }
    }
}

void ofxKinectUsers::mousePressed(int x, int y, int button) {
	mouseDragged(x, y, button);
}

/////////////////////////////////////////////////////////////////// DRAW
//--------------------------------------------------------------------------------------------
void ofxKinectUsers::debugDraw(){
	ofSetColor(0,255);
	ofRect(0, 0, 970,550);
	ofSetColor(255, 200);
	openni.depthGenerator.draw(0,0,640,480);
	
	// -------------------- Border
	ofNoFill();
	ofSetLineWidth(2);
	ofRect(0, 0, 640, 480);
	// -------------------- Detection Area
	ofSetColor(255,100);
	dArea.draw();
	// -------------------- Area Buttons
	ofRect(dArea.min.x-20, dArea.min.y-20, 40, 40);
	ofRect(dArea.max.x-20, dArea.max.y-20, 40, 40);
	// -------------------- Data: Fps, Users, Bones, accelerometers & Tilt Angle
	ofSetColor(200,255);
	ofDrawBitmapString("fps: "+ofToString(ofGetFrameRate(), 2), 5, 495);
	ofDrawBitmapString("Users: " + ofToString(nUsers), 105, 495);
	ofDrawBitmapString("Bones: " + ofToString(nBones), 205, 495);
	if (hands.isActive)
		ofDrawBitmapString("Sending TUIO cursors to " + hands.tuio.localHost + " : " + ofToString(hands.tuio.TUIOPort), 305, 495);
#ifdef KINECT_HARDWARE
	ofPoint acc = openni.hardware.getAccelerometers();
	ofDrawBitmapString("X: " + ofToString(acc.x,3),  5, 525);
	ofDrawBitmapString("Y: " + ofToString(acc.y,3), 105,525);
	ofDrawBitmapString("Z: " + ofToString(acc.z,3), 205,525);
	ofDrawBitmapString("Angle: " + ofToString(openni.hardware.getTiltAngle(),3), 315, 525);
#endif
	
	// -------------------- Perspectives
	float pHeight[perspectives.size()];	// Set the Y position for arrange the perpective one on the buttom of the other
	for (int j = 0; j < perspectives.size(); j++){
		Area area;		// Total Perspective Area look at that perspective
		area.set(ofVec3f(0,0,0),ofVec3f(640,480,6000));
		area.transform(perspectives[j].mat);
		
		Area pDArea;	// Selected Area in that perspective
		pDArea = dArea;
		pDArea.transform(perspectives[j].mat);
		
		if (j + 1 <= perspectives.size()) {
			float h = area.max.y + 15;
			pHeight[j+1] = pHeight[j] + h*0.2 + 15;
		}
		
		ofPushMatrix();
		
		ofTranslate(640+15,pHeight[j], 0);
		
		if (nPerspective == j){
			ofSetColor(255,255);
			ofDrawBitmapString(" -> Gestures & Poses Manager", 50, 10);
		} else 
			ofSetColor(200,255);
		
		ofDrawBitmapString(perspectives[j].name, 5, 10);
		ofTranslate(0, 15, 0);
		ofScale(0.1,0.1,0.1);
		area.draw();
		
		ofSetColor(255,100);
		pDArea.draw();
		
		ofPopMatrix();
	}
	//-------------------- TUIO output window
	if (hands.isActive){
		ofSetColor(200,255);
		ofDrawBitmapString("TUIO cursors from " + hands.perspective.name + " view", 
						   640 + 15, pHeight[perspectives.size()-1] + 50 - 5);
		hands.drawOutPut(640 + 15, pHeight[perspectives.size()-1] + 50, 
						 300 , 480 - pHeight[perspectives.size()-1] - 50);
	}
	ofFill();
	
	// ---------------------------------------- BLOBS y ESKELETOS
	for(int i = 0 ; i < MAX_USERS; i++){
		if (users[i].isActive){
			ofSetLineWidth(2);
			ofSetColor(255, 0, 0);
			ofPushMatrix();
			ofTranslate(users[i].centroid.x,users[i].centroid.y);
			ofPoint end = users[i].vel * users[i].velF;
			ofLine(0, 0, end.x, end.y);
			ofPopMatrix();
            
            if (users[i].bones){
                
				// Draw Bones
				ofSetLineWidth(1);
				ofSetColor(0, 255, 0, 255);
				users[i].bonesPoints.draw();
				ofSetColor(0, 255, 100, 100);
				users[i].area.draw();
				
				// Draw Perspectives
				for (int j = 0; j < perspectives.size(); j++){
					Bones temp;
					temp = users[i].bonesPoints;
					temp.transform(perspectives[j].mat);
					
					Area area;
					area = users[i].area;
					area.transform(perspectives[j].mat);
					
					ofPushMatrix();
					ofTranslate(640+15, pHeight[j] + 15 , 0);
					ofScale(0.1,0.1,0.1);
					
					if (nPerspective == j)
						ofSetColor(0,255, 0, 255);
					else 
						ofSetColor(0, 200, 100, 200);
					temp.draw();
					
					if (nPerspective == j)
						ofSetColor(0,255, 0, 100);
					else 
						ofSetColor(0, 200, 100, 100);
					area.draw();
					ofPopMatrix();
				}
				
				// Draw Velocity Vectors
				ofSetLineWidth(2);
				ofSetColor(255, 0, 0);
				for (int j = 0; j < 15; j++){
					ofPushMatrix();
					ofTranslate(users[i].bonesPoints.bone[j]->x,users[i].bonesPoints.bone[j]->y);
					ofLine(0, 0, users[i].bonesVel.bone[j]->x, users[i].bonesVel.bone[j]->y);
					ofPopMatrix();
				}
                
			} else {
				ofSetLineWidth(1);
                
				for (int j = 0; j < perspectives.size(); j++){
					ofVec3f pos = users[i].centroid;
					pos = pos * perspectives[j].mat;
						
					// Draw Users with no Bones
					ofPushMatrix();
					ofTranslate(640+15, pHeight[j] + 15 , 0);
					ofScale(0.1,0.1,0.1);
					if (nPerspective == j)
						ofSetColor(0,255, 0, 150);
					else 
						ofSetColor(0, 200, 100, 150);
					ofCircle(pos.x-50,pos.y-50, 50);
					ofPopMatrix();
				}
			}
		}
	}
	
	// Draw Gestures
	gestures.debugDraw();
	
	// Draw the Areas for the hand traking Hands 
	hands.drawHandAreas();
	
	ofFill();
}


//--------------------------------------------------------------
void ofxKinectUsers::sendOsc(ofxUser user) { // john
    ofxOscMessage  mStatesDetect, mStatesSkeleton, mPose, mGesture;
    
    string player = "/player0";
    int nbJoint = l_sName.size();
    string addr;
    for(int i = 0 ; i < nbJoint ; i++)
    {
        ofxOscMessage mPos, mSpeed, mAcc;
        
        
        if(l_bSend.at(i) == "TRUE")
        {
            //ofVec3f vjointPos = user.averageFilter( i, l_sName.at(i) );
            
            ofVec3f vjointPos = (user.jointToSend(user.bonesPoints, l_sName.at(i)));
            vjointPos.x = ofNormalize(vjointPos.x, 0, 640);
            vjointPos.y = ofNormalize(vjointPos.y, 0, 480);
            vjointPos.z = ofNormalize(vjointPos.z, 0, 10000);
            
            addr = player + "/" + l_sName[i] + "/pos";
            mPos.setAddress(addr);
            mPos.addFloatArg(vjointPos.x);
            mPos.addFloatArg(vjointPos.y);
            mPos.addFloatArg(vjointPos.z);
            sender.sendMessage(mPos);
            
            ofVec3f vjointVel = user.jointToSend(user.bonesVel, l_sName.at(i));
            vjointVel.x = ofMap(vjointVel.x, -50, 50, -1, 1, true);
            vjointVel.y = ofMap(vjointVel.y, -50, 50, -1, 1, true);
            vjointVel.z = ofMap(vjointVel.z, -150, 150, -1, 1, true);
            /*vjointVel = (user.jointToSend(user.bonesVel, l_sName.at(i))).normalize();*/
            mSpeed.setAddress(player + "/" + l_sName[i] + "/speed");
            mSpeed.addFloatArg(vjointVel.x);
            mSpeed.addFloatArg(vjointVel.y);
            mSpeed.addFloatArg(vjointVel.z);
            sender.sendMessage(mSpeed);
            
            ofVec3f vjointAcc;
            vjointAcc = user.jointToSend(user.bonesAcc, l_sName.at(i));
            vjointAcc.x = ofMap(vjointAcc.x, -50, 50, -1, 1, true);
            vjointAcc.y = ofMap(vjointAcc.y, -50, 50, -1, 1, true);
            vjointAcc.z = ofMap(vjointAcc.z, -150, 150, -1, 1, true);
            mAcc.setAddress(player + "/" + l_sName[i] + "/acc");
            mAcc.addFloatArg(vjointAcc.x);
            mAcc.addFloatArg(vjointAcc.y);
            mAcc.addFloatArg(vjointAcc.z);
            sender.sendMessage(mAcc);
        }
    }
    
    // POSE
    if(user.inPose)
    {
        mPose.setAddress(player +  "/pose");
        mPose.addStringArg(ofToString(user.pose));
        sender.sendMessage(mPose);
        bPreviousPose = true;
    }
    else
    {
        if(bPreviousPose == true)
        {
            mGesture.setAddress(player +  "/pose");
            mGesture.addStringArg(" ");
            sender.sendMessage(mGesture);
            bPreviousPose = false;
        }
    }
    
    // GESTURE
    if(user.inGesture)
    {
        mGesture.setAddress(player +  "/gesture");
        mGesture.addStringArg(ofToString(user.gesture));
        sender.sendMessage(mGesture);
        bPreviousGesture = true;
    }
    else 
    {
        if(bPreviousGesture == true)
        {
            mGesture.setAddress(player +  "/gesture");
            mGesture.addStringArg(" ");
            sender.sendMessage(mGesture);
            bPreviousGesture = false;
        }
    }
}

void ofxKinectUsers::sendOscBoolMsg(string address, int value) { // john
    ofxOscMessage mBool;
    
    mBool.setAddress(address);
    mBool.addIntArg(value);
    sender.sendMessage(mBool);
}



/*
 *  ofxUser.h
 *  ofxKinectUsers
 *
 *  Created by Patricio Gonzalez Vivo on 17/03/11.
 *  Copyright 2011 www.PatricioGonzalezVivo.com All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 */

#ifndef OFXUSER
#define OFXUSER

#include "ofMain.h"
#include "ofxKTools.h"

class ofxUser{
public:
	
	unsigned char *	maskPixels;
	ofVec3f centroid, vel;
	float	velF;
	
	int		userId;
    int		idleTime;
	bool	isIni, isActive, isIdle;
	
	bool	bones;
	//------------------------------ bones
	Bones	bonesPoints, bonesVel, bonesAcc;
	BonesF	bonesVelF, bonesAccF;
	Area	area;
	Arms	arms;			
	
	string	pose, gesture;
	int		poseTimer, gestureTimer;
	bool	inPose,gestureInit,gestureComp, inGesture;
	
	float	head,palms;
	int		jumpCounter, applauseCounter;
	
    vector<ofVec3f>   l_iSum; // sum for average - smoothing
    vector< vector< ofVec3f > >   l_fValuesWindow; // sum for average - smoothing
    vector<int> indexWindow; // john
    
	//------------------------------- Hands
	bool	left, right;
	int		leftId, rightId;
    
	ofxUser();
	
	void setup(int _userId, Perspective * _perspective);
    void setSumList(int nbJoint, ofVec3f valueVect3f); // john
    void setJointNameList(vector<string> l_sName); // john
    void setIndexWindow(vector<int> indexWindow); // john
    void update();
	
	ofVec3f getPerspectiveCentroid(){return centroid * perspective->mat;};
	ofVec3f getPerspectiveVel(){return vel * perspective->mat;};
	//------------------------------- bones
	Bones	getPerspectiveBones(){return bonesPoints * perspective->mat;};
	Bones	getPerspectiveBonesVel(){return bonesVel * perspective->mat;};
	Area	getPerspectiveArea(){return area * perspective->mat;};

    ofVec3f         averageFilter(int iJointIndex, string jointName, Bones bonesType); // john
    ofVec3f         jointToSend(Bones lJoints, string jointName ); // john
private:
	Perspective *perspective;
	
	ofVec3f lastCentroid;
	Bones	bonesLastPoints, bonesLastVel;
	
    vector<string> l_sName; // john
    
	void reIni();

};
#endif

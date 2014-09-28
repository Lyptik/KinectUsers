/*
 *  ofxUser.cpp
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

#define SMOOTHVALUE 3 // john

#include "ofxUser.h"

ofxUser::ofxUser(){
	userId	= 0;
	isIni	= false;
	isIdle  = true;
	isActive = false;
	bones	= false;
}

void ofxUser::reIni(){
	isIni	= true;
	isIdle	= true;
	idleTime = 0;
	
	bones = false;
	bonesPoints.set(0, 0, 0);
	bonesVel.set(0, 0, 0);
	bonesLastPoints.set(0, 0, 0);
	
	centroid.set(0,0,0);
	lastCentroid.set(0,0,0);
	vel.set(0,0,0);
}

void ofxUser::setup(int _userId, Perspective * _perspective){
	userId	= _userId;
	perspective = _perspective;
	
	reIni();
}

void ofxUser::update(){
	if(isIni){
		vel			= centroid - lastCentroid;
		
		velF	= vel.length();
		vel		= vel.getNormalized();
		
		lastCentroid = centroid;
		
		if (isActive){
			if(bones){
                // calculate velocity
                //bonesPoints.normalize();
               // bonesLastPoints.normalize();
                
                // AVERAGE POINT TO SMOOTH IT
                for(int i = 0 ; i < l_fValuesWindow.size() ; i++) // john
                    *bonesPoints.bone[i] = averageFilter( i , l_sName[i], bonesPoints );

				bonesVel	= bonesPoints - bonesLastPoints;
               // for(int i = 0 ; i < l_fValuesWindow.size() ; i++)
                 //   *bonesVel.bone[i] = averageFilter( i , l_sName[i], bonesVel );
                
				bonesVelF = bonesVel.length();
				//bonesVel.normalize();
		
				bonesLastPoints = bonesPoints;
	
                // calculate acceleration
                bonesAcc	= bonesVel - bonesLastVel; // john
                
				bonesAccF = bonesAcc.length(); // john
                
				bonesLastVel = bonesVel; // john
                
				ofVec3f min, max;
				min.set(1000,1000,0);
				max.set(0,0,10000);	
		
				for(int i = 0 ; i < 15 ; i++)
					if( bonesPoints.bone[i]->x < min.x)
						min.x = bonesPoints.bone[i]->x;
		
				for(int i = 0 ; i < 15 ; i++)
					if( bonesPoints.bone[i]->y < min.y)
						min.y = bonesPoints.bone[i]->y;
		
				for(int i = 0 ; i < 15 ; i++)
					if( bonesPoints.bone[i]->z > min.z)
						min.z = bonesPoints.bone[i]->z;
		
				for(int i = 0 ; i < 15 ; i++)
					if( bonesPoints.bone[i]->x > max.x)
						max.x = bonesPoints.bone[i]->x;
			
				for(int i = 0 ; i < 15 ; i++)
					if( bonesPoints.bone[i]->y > max.y)
						max.y = bonesPoints.bone[i]->y;
		
				for(int i = 0 ; i < 15 ; i++)
					if( bonesPoints.bone[i]->z < max.z)
						max.z = bonesPoints.bone[i]->z;
		
				area.set(min,max);
			}
		}
		
		if ( velF > 0.5 ){
			isIdle = false;
			idleTime == 0;
		} else {
			isIdle = true;
			idleTime++;
		}

		if (idleTime > 100)
			isActive = bones = false;
	}
}

void ofxUser::setSumList(int nbJoint, ofVec3f valueVect3f) // john
{
    for(int i = 0 ; i < nbJoint ; i++)
        l_iSum.push_back(valueVect3f);
}

void ofxUser::setIndexWindow(vector<int> indexWindow) // john
{
    this->indexWindow = indexWindow;
}

void ofxUser::setJointNameList(vector<string> l_sJointName) // john
{
    this->l_sName = l_sJointName;
}

ofVec3f ofxUser::jointToSend(Bones lJoints, string jointName ) // john
{
    ofVec3f vjoint;
    
    // HEAD
    if(jointName == "head")
        vjoint = lJoints.head;
    
    // NECK
    else if(jointName == "neck")
        vjoint = lJoints.neck;
    
    // TORSO
    else if(jointName == "torso")
        vjoint = lJoints.torso;
    
    // LEFT_SHOULDER
    else if(jointName == "shoulder_left")
        vjoint = lJoints.left_shoulder;
    
    // LEFT_ELBOW
    else if(jointName == "elbow_left")
        vjoint = lJoints.left_elbow;
    
    // LEFT_HAND
    else if(jointName == "hand_left")
        vjoint = lJoints.left_hand;
    
    // LEFT_HIP
    else if(jointName == "hip_left")
        vjoint = lJoints.left_hip;
    
    // LEFT_KNEE
    else if(jointName == "knee_left")
        vjoint = lJoints.left_knee;
    
    // LEFT_FOOT
    else if(jointName == "foot_left")
        vjoint = lJoints.left_foot;
    
    // RIGHT_SHOULDER
    else if(jointName == "shoulder_right")
        vjoint = lJoints.right_shoulder;
    
    // RIGHT_ELBOW
    else if(jointName == "elbow_right")
        vjoint = lJoints.right_elbow;
    
    // RIGHT_HAND
    else if(jointName == "hand_right")
        vjoint = lJoints.right_hand;
    
    // RIGHT_HIP
    else if(jointName == "hip_right")
        vjoint = lJoints.right_hip;
    
    // RIGHT_KNEE
    else if(jointName == "knee_right")
        vjoint = lJoints.right_knee;
    
    // RIGHT_FOOT
    else if(jointName == "foot_right")
        vjoint = lJoints.right_foot;
    
    return vjoint;
}

ofVec3f ofxUser::averageFilter( int iJointIndex, string jointName, Bones bonesType) // john
{
    // average computation
    this->l_iSum[iJointIndex] -= (this->l_fValuesWindow[iJointIndex][indexWindow[iJointIndex]]) ; /* subtract value falling off */
    
    ofVec3f currentpoint = jointToSend(bonesType, jointName ); 
    
    this->l_iSum[iJointIndex]  +=  currentpoint; /* add new value */
    
    this->l_fValuesWindow[iJointIndex][indexWindow[iJointIndex]] = currentpoint;/* save new value so it can be subtracted later */

    if(++indexWindow[iJointIndex] == SMOOTHVALUE) /* increment joint index */
        indexWindow[iJointIndex] = 0;
    
    return this->l_iSum[iJointIndex]/SMOOTHVALUE;
    
}

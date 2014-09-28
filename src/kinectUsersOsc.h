/*
 *  kinectUsersOsc.h
 *  Created by David Chanel on 11/24/12.
 *
 */

#ifndef KINECT_USERS_OSC
#define KINECT_USERS_OSC

#include "ofxOsc.h"

class kinectUsersOsc {
    
    public:

        kinectUsersOsc();
        ~kinectUsersOsc();
    
    private:
    
        ofxOscSender m_oOscOut;
};

#endif

#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

#include <signal.h>

void signalHandler(int a_iSignum)
{
    switch(a_iSignum)
    {
        case 2 :
            cout << "[main] Caught SIGINT (" << a_iSignum << ") or ctrl+c... Exiting..." << endl;
            ofExit(0);
            break;
            
        case 3 :
            cout << "[main] Caught SIGQUIT (" << a_iSignum << ")... Exiting..." << endl;
            ofExit(0);
            break;
            
        case 15 :
            cout << "[main] Caught SIGTERM (" << a_iSignum << ")... Exiting..." << endl;
            ofExit(0);
            break;
            
        default :
            cerr << "[main] Caught signal " << a_iSignum << " !" << endl;
            break;
    }
}

//========================================================================
int main(){
    
    
    #ifdef __APPLE__
        // This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle
        // ----------------------------------------------------------------------------
        char path[2048];
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
        if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, 2048))
        {
            std::cerr << "Error : couldn't get bundle path resources" << std::endl;
        }
        CFRelease(resourcesURL);
        
        //int result = chdir(path); // do not move path because we are still using openframeworks stuff
        string resourcePath = ofToString(path);
        resourcePath.append("/");
        std::cout << "Current Path: " << resourcePath << std::endl;
        ofSetDataPathRoot(resourcePath); // openframeworks way to append string
    #endif
    
    
    // Catch signal to exit cleanly app when ctrl+c or other signals
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
    signal(SIGTERM, signalHandler);

    ofAppGlutWindow window;
	//ofSetupOpenGL(&window, 1400,800, OF_WINDOW);			// <-------- setup the GL context
	ofSetupOpenGL(&window, 970, 550, OF_WINDOW);
	
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

}

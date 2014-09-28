Additionnal feature from patriciogonzalez ofxKinectusers fork :

- Transformed ofxKinectUsers into Kinectusers (means there is no add-on but one app containing everything)
- Version with Xtion working
- Latest OpenNI1 libs updated
- Added Osc streaming for Nite skeleton
- Added Xcode Project to make it working with of 007 OSX 10.6
- Added Xcode Project to make it working with of 0.8.4 OSX 10.6 (on a 10.9 sys) (note that you have to create an enclosing dir)
- Moved ressources and Xml into .app

Addons : OfxOpenNI1 from gameoverhack (use git tag to revert to V1)

Warning : Provided as is, lots of features are still experimental and can contain bug

TODO : 

- Is not dynamic ! (find a way to load the dynamic libs from inside .app which is not working)

Still one problem with dynamic libs system, here are some insights :

We added the path to the resource folder in the main
We create the Resource directory inside the .app
We copy the settings file in the Resources directory from the data directory

DONE BUT USELESS
We copy all the dynamic libs from the data directory to the MacOS directory inside the .app
>> We still have to keep the data file next to the binary to make it run

TODO
Tell xcode that it should look into the .app/MacOS instead of the data to look for the dynamic libs

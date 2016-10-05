#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
//	ofSetupOpenGL(1440,850,OF_WINDOW);			// <-------- setup the GL context
//
//	// this kicks off the running of my app
//	// can be OF_WINDOW or OF_FULLSCREEN
//	// pass in width and height too:
//	ofRunApp(new ofApp());
    
    
    ofGLFWWindowSettings mainSettings;
    mainSettings.width = 1440;
    mainSettings.height = 450;
    mainSettings.setPosition(ofVec2f(0,0));
    mainSettings.windowMode = OF_WINDOW;
    mainSettings.resizable = true;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(mainSettings);
    
    ofGLFWWindowSettings prevSettings;
    prevSettings.width = 300;
    prevSettings.height = 250;
    prevSettings.setPosition(ofVec2f(0, mainSettings.height));
    prevSettings.windowMode = OF_WINDOW;
    prevSettings.resizable = true;
    prevSettings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> prevWindow = ofCreateWindow(prevSettings);
    
//    shared_ptr<previewWindow> monitorApp(new previewWindow);
    shared_ptr<ofApp> mainApp(new ofApp);
    ofAddListener(prevWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow);
    ofAddListener(prevWindow->events().keyPressed, mainApp.get(), &ofApp::keyPressedOnSecondWindow);
    
//    monitorApp->setMainApp(mainApp);
//    mainApp->setMonitor(monitorApp);
    
//    ofRunApp(prevWindow, monitorApp);
    ofRunApp(mainWindow, mainApp);
    
    ofRunMainLoop();
}

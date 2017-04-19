#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main(){
    
    ofGLFWWindowSettings mainSettings;
    mainSettings.width = 1965;
    mainSettings.height = 1010;
    mainSettings.setPosition(ofVec2f(0,0));
    mainSettings.windowMode = OF_WINDOW;
    mainSettings.resizable = true;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(mainSettings);
    
    ofGLFWWindowSettings prevSettings;
    prevSettings.width = 300;
    prevSettings.height = 250;
    prevSettings.setPosition(ofVec2f(mainSettings.width+100, 0));
    prevSettings.windowMode = OF_WINDOW;
    prevSettings.resizable = true;
    prevSettings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> prevWindow = ofCreateWindow(prevSettings);
    
//    shared_ptr<previewWindow> monitorApp(new previewWindow);
    shared_ptr<ofApp> mainApp(new ofApp);
    ofAddListener(prevWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow);
    ofAddListener(prevWindow->events().keyPressed, mainApp.get(), &ofApp::keyPressedOnSecondWindow);
    
    ofRunApp(mainWindow, mainApp);
    
    ofRunMainLoop();
}

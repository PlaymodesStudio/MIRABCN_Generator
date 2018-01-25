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
    mainSettings.setGLVersion(4,1);
    ofCreateWindow(mainSettings);
    
//    shared_ptr<ofApp> mainApp(new ofApp);
////    ofAddListener(prevWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow);
////    ofAddListener(prevWindow->events().mouseMoved, mainApp.get(), &ofApp::mouseMovedSecondWindow);
////    ofAddListener(prevWindow->events().mousePressed, mainApp.get(), &ofApp::mousePressedSecondWindow);
////    ofAddListener(prevWindow->events().mouseReleased, mainApp.get(), &ofApp::mouseReleasedSecondWindow);
////    ofAddListener(prevWindow->events().mouseDragged, mainApp.get(), &ofApp::mouseDraggedSecondWindow);
//    
//    ofRunApp(mainWindow, mainApp);
//    
//    ofRunMainLoop();
    
    ofRunApp(new ofApp());
}

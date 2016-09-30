#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    
    //Set the FrameRate to be 40, that is the frame rate of the Pixel Bars
    ofSetFrameRate(40);
    
    //Initialize our pixelNum var to have all the pixels in one bar
    pixelNum = PIXEL_X_BAR;
    
    //Setup the generator of waves and pass it the numbers of items it will have
    singleGenerator.setIndexCount(pixelNum);
    singleGenerator.setup();
    
    waveControl.setup(COL_BARS, ROW_BARS);
    
    
    //Initialize the calculator of index modifications in delaying the info calulated by the singleGenerator
    delayControler.setIndexCount(NUM_BARS);
    delayControler.setup();
    
    
    //Initialize our vector that stores the information of the oscilators
    infoVec.resize(pixelNum, 0);
    
    //Initlize our syphon and specify the name
    syphonServer.setName("MIRABCN_Generator");
    
    //Allocation of the texture, and modify to show correctly the discrete pixels
    pixelContent.allocate(NUM_BARS, PIXEL_X_BAR, GL_RGB);
    pixelContent.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    waveGrid.allocate(COL_BARS, ROW_BARS, GL_RGB);
    waveGrid.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    //Setup of the phasor, wich controls the oscilator generator
    phasor.setup();
    
    
    //Paramters binding
    paramsControl.bindPhasorParams(phasor.getParameterGroup());
    paramsControl.bindOscilatorParams(singleGenerator.getParameterGroup());
    paramsControl.bindDelayParams(delayControler.getParameterGroup());
    paramsControl.setup();
    
    guiWidth = paramsControl.getGuiWidth();
}

//--------------------------------------------------------------
void ofApp::update(){
    paramsControl.update();
    
    //Phasor updates automatically at audio rate
    
    vector<vector<float>> phase_offset =  waveControl.computeWave(waveGrid, phasor.getPhasor());
    
    for(int i = 0; i < COL_BARS ; i++){
        for (int j = 0; j < ROW_BARS ; j++){
            //Calculation of the oscilators for each element, with phasor info
            singleGenerator.computeFunc(infoVec.data(), phasor.getPhasor(), phase_offset[j][i]);
            waveControl.computeOutTex(pixelContent, infoVec, ofVec2f(i, j));
        }
    }
    
    //Fill the fbo with the information in infoVec, and delaying it and modifing with it's controls
    //delayControler.applyDelayToTexture(pixelContent, infoVec, phasor.getParameterGroup().getFloat("BPM"));
    
    //Pass texture to syphon
    syphonServer.publishTexture(&pixelContent.getTexture());
}

//--------------------------------------------------------------
void ofApp::draw(){
    
}

void ofApp::drawSecondWindow(ofEventArgs &args){
    ofBackground(0);
    ofSetColor(255);
    int contentWidth = ofGetWidth();
    //Draw the fbo
    pixelContent.getTexture().draw(0, 0, contentWidth/2, 5*ofGetHeight()/11);
    
    waveGrid.getTexture().draw((contentWidth/2), 0, contentWidth/2, 5*ofGetHeight()/11);
    
    //Draw the Bars
    float wid = (float)contentWidth/pixelNum;
    float hei = 5*ofGetHeight()/11;
    for(int i = 0; i < pixelNum; i++)
        ofDrawRectangle((i*wid), (1-infoVec[i])*hei+hei, wid, infoVec[i]*hei);
    
    
    //draw the phasor evolution
    ofDrawTriangle(0, ofGetHeight(), ofGetWidth(), ofGetHeight(), ofGetWidth(), 10*ofGetHeight()/11);
    ofSetColor(127);
    ofDrawRectangle(((float)contentWidth * phasor.getPhasor()), 10*ofGetHeight()/11, 5, ofGetHeight()/11);
    
    //Draw the framerate
    ofSetColor(255, 0,0);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, ofGetHeight()-10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
//    if(isdigit(key))
//        paramsControl.loadPreset(key-48);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

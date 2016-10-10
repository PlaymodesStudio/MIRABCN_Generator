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
    singleGenerator.setup(1);
    
    
    waveControl.setup(COL_BARS, ROW_BARS, 1);
    
    
    //Initialize our vector that stores the information of the oscilators
    infoVec.resize(pixelNum, 0);
    
    //Initlize our syphon and specify the name
    syphonServer.setName("MIRABCN_Generator");
    
    //Allocation of the texture, and modify to show correctly the discrete pixels
    pixelContent.allocate(NUM_BARS, PIXEL_X_BAR, GL_RGB);
    pixelContent.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    waveGrid.allocate(COL_BARS, ROW_BARS, GL_RGB);
    waveGrid.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    waveLinear.allocate(COL_BARS*ROW_BARS, 255, GL_RGB);
    waveLinear.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    //Setup of the phasors, wich controls the oscilator generator and other parameters
    phasors.resize(2);
    for(int i=0; i<phasors.size(); i++)
        phasors[i].setup(i+1);
    
    paramsControl.createGuiFromParams(phasors[0].getParameterGroup());
    paramsControl.createGuiFromParams(phasors[1].getParameterGroup());
    paramsControl.createGuiFromParams(singleGenerator.getParameterGroup());
    paramsControl.createGuiFromParams(waveControl.getParameterGroup());
    paramsControl.createGuiFromParams(waveControl.getGeneratorParameterGroup());
    

    paramsControl.setup();
    
    soundStream.setup(this, 0, 2, 44100, 512, 4);
}

//--------------------------------------------------------------
void ofApp::update(){
    paramsControl.update();
    
    //Phasor updates automatically at audio rate
    
    vector<vector<float>> phase_offset =  waveControl.computeWave(waveGrid, waveLinear, phasors[1].getPhasor());
    
    float update_Phasor = phasors[0].getPhasor();
    for(int i = 0; i < COL_BARS ; i++){
        for (int j = 0; j < ROW_BARS ; j++){
            //Calculation of the oscilators for each element, with phasor info
            singleGenerator.computeFunc(infoVec.data(), update_Phasor, phase_offset[j][i]);
            waveControl.computeOutTex(pixelContent, infoVec, ofVec2f(i, j));
        }
    }
    
    singleGenerator.computeFunc(infoVec.data(), update_Phasor);

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
    pixelContent.getTexture().draw(0, 0, contentWidth/2, ofGetHeight()/3);
    
    waveGrid.getTexture().draw((contentWidth/2), 0, contentWidth/2, ofGetHeight()/3);
    
    //Draw the Bars
    float wid = (float)contentWidth/pixelNum;
    float hei = ofGetHeight()/3;
    for(int i = 0; i < pixelNum; i++)
        ofDrawRectangle((i*wid), (1-infoVec[i])*hei+hei, wid, infoVec[i]*hei);
    
    //Draw the Bars2
    waveLinear.getTexture().draw(0, 2*ofGetHeight()/3, ofGetWidth(), ofGetHeight()/3);
    
    
    //Draw the framerate
    ofSetColor(255, 0,0);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, ofGetHeight()-10);
}

void ofApp::keyPressedOnSecondWindow(ofKeyEventArgs & args){
    if(args.key == ' ')
        waveControl.togglePreviewTexture();
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    for(auto phasor : phasors)
        phasor.audioIn(input, bufferSize, nChannels);
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

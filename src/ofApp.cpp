#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    logBuffer = make_shared<bufferLoggerChannel>();
    ofSetLoggerChannel((shared_ptr<ofBaseLoggerChannel>)logBuffer);
    
    //Set the FrameRate to be 40, that is the frame rate of the Pixel Bars
    ofSetFrameRate(40);
    
    //Initialize our pixelNum var to have all the pixels in one bar
    pixelNum = PIXEL_X_BAR;
    
    //Setup the generator of waves and pass it the numbers of items it will have
    singleGenerator.setIndexCount(pixelNum);
    singleGenerator.setup(1);
    
    //Function that generates the wave(2D Wave) that modifies the bank of oscillators
    waveControl.setup(COL_BARS, ROW_BARS, 1);
    
    masterModule.setup(1);
    
    //Initialize our vector that stores the information of the oscilators
    infoVec.resize(pixelNum, 0);
    
    bankDatas.resize(COL_BARS*NUM_BARS, infoVec);
    
    //Initlize our syphon and specify the name
    syphonServer.setName("MIRABCN_Generator");
    
    //Allocation of the fbo's, and modify the texture to show correctly the discrete pixels
    //bank of oscillators
    pixelContent.allocate(NUM_BARS, PIXEL_X_BAR, GL_RGB);
    pixelContent.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    //2D wave in grid arrangement
    waveGrid.allocate(COL_BARS, ROW_BARS, GL_RGB);
    waveGrid.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    //2D Wave in linear arrangement
    waveLinear.allocate(COL_BARS*ROW_BARS, 255, GL_RGB);
    waveLinear.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    //Setup of the phasors, wich controls the oscilator generator and other parameters
    phasors.resize(2);
    for(int i=0; i<phasors.size(); i++)
        phasors[i].setup(i+1);
    
    //Take the parameter groups and pass them to create the gui.
    //ParamsControl can handle a gui creation with a parameterGroup that has all the parameters needed to be modified
    //Briefly what it does is:
    // - ofParameter<int> and ofParameter<float> -> creates a slider
    // - ofParameter<bool> -> creates a toogle
    // - ofParameter<string> -> creates a textInputField;
    // - ofParameter<string> whose name ends with "_label" -> creates a label
    // - ofParameter<int> and ofParameter<string> inside a ofParameterGroup -> creates a dropdown list with elements in ofParameter<string> delimeted by "-|-"
    paramsControl.createGuiFromParams(phasors[0].getParameterGroup());
    paramsControl.createGuiFromParams(singleGenerator.getParameterGroup());
    paramsControl.createGuiFromParams(phasors[1].getParameterGroup());
    paramsControl.createGuiFromParams(waveControl.getParameterGroup());
    paramsControl.createGuiFromParams(waveControl.getGeneratorParameterGroup());
    paramsControl.createGuiFromParams(masterModule.getParameterGroup());
    
    //Create main gui, and add listeners when all guis are created
    paramsControl.setup();
    
    //Setup the soundStream so we can use the audio rate called function "audioIn" to update the phasor and have it better synced
    soundStream.setup(this, 0, 2, 44100, 512, 4);
    
    outputCurve.setup();
    outputCurve.useMouse(true);
    outputCurve.load("responseCurve.yml");
    
    curvePos = ofPoint(600, 250);
    curveDragger = ofRectangle(curvePos.x, curvePos.y-20, 255, 20);
    outputCurve.notifyEvents(true);
    masterModule.setCurve(outputCurve);
    
    ofAddListener(outputCurve.curHoverUpdate, this, &ofApp::outputCurveListener);
}

//--------------------------------------------------------------
void ofApp::update(){
    //Update paramsControl, becouse it also handles osc and midi
    paramsControl.update();
    
    waveLinear.begin();
    ofSetColor(0);
    ofDrawRectangle(0, 0, waveLinear.getWidth(), waveLinear.getHeight());
    waveLinear.end();

    
    //Phasor updates automatically at audio rate, but we need to take the value for this update so the phasor is the same along all the time update is being called
    float update_Phasor = phasors[0].getPhasor();
    
    //We get the values that we will use to modulate our bank of oscillators, we get them as a matrix to get them clear so we understand it like the space it has to be installed
    vector<vector<float>> modValues =  waveControl.computeWave(phasors[1].getPhasor());
    
    
    //We iterate for each column, and we compute it's value;
    for(int i = 0; i < COL_BARS ; i++){
        for (int j = 0; j < ROW_BARS ; j++){
            int index = waveControl.getIndexFromPosition(j, i);
            //Calculation of the oscilators for each element, with phasor info and modulation info
            singleGenerator.computeFunc(bankDatas[index].data(), update_Phasor, modValues[j][i]);
            //We use this indo to fill the output texture
            masterModule.computeOutTex(pixelContent, bankDatas[index], index);
            masterModule.computeWaveTex(waveGrid, modValues[j][i], ofPoint(i, j));
            masterModule.computeLinWaveTex(waveLinear, modValues[j][i], index);
        }
    }
    
    //We compute one more time without modifiers to have a representation what tha bank of oscillators originaly was
    singleGenerator.computeFunc(infoVec.data(), update_Phasor);

    //Pass texture to syphon
    syphonServer.publishTexture(&pixelContent.getTexture());
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    if(masterModule.drawCurve()){
        ofSetColor(ofColor::grey);
        ofDrawRectangle(curveDragger);
        ofSetColor(ofColor::red);
        ofDrawBitmapString("DRAG ME", curvePos.x + 75, curvePos.y - 5);
        outputCurve.draw(curvePos.x, curvePos.y);
    }
}

void ofApp::exit(){
    outputCurve.save("responseCurve.yml");
    paramsControl.saveGuiArrangement();
}

void ofApp::drawSecondWindow(ofEventArgs &args){
    //This functions is the implementation of the draw event on second window (preview window)
    
    ofBackground(0);
    ofSetColor(255);
    int contentWidth = 2*ofGetWidth()/3;
    //Draw the fbo
    pixelContent.getTexture().draw(0, 0, contentWidth, ofGetHeight()/3);
    ofPushStyle();
    ofSetColor(ofColor::indianRed);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(0, 0, contentWidth, ofGetHeight()/3);
    ofPopStyle();
    
    waveGrid.getTexture().draw(contentWidth, 0, ofGetWidth()-contentWidth, ofGetHeight()/3);
    ofPushStyle();
    ofSetColor(ofColor::indianRed);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(contentWidth, 0, ofGetWidth()-contentWidth, ofGetHeight()/3);
    ofPopStyle();
    
    //Draw the Bars
    float wid = (float)contentWidth/pixelNum;
    float hei = ofGetHeight()/3;
    for(int i = 0; i < pixelNum; i++)
        ofDrawRectangle((i*wid), (1-infoVec[i])*hei+hei, wid, infoVec[i]*hei);
    ofPushStyle();
    ofSetColor(ofColor::indianRed);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(0, ofGetHeight()/3, contentWidth, ofGetHeight()/3);
    ofPopStyle();
    
    
    //Draw the Bars2
    waveLinear.getTexture().draw(0, 2*ofGetHeight()/3, contentWidth, ofGetHeight()/3);
    ofPushStyle();
    ofSetColor(ofColor::indianRed);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(0, 2*ofGetHeight()/3, contentWidth, ofGetHeight()/3);
    ofPopStyle();
    
    //Draw another time the grid
    //waveGrid.getTexture().draw(contentWidth, 2*ofGetHeight()/3, ofGetWidth()-contentWidth, ofGetHeight()/3);
    
    
    //Draw notifiers
    ofRectangle debugRectangle(contentWidth, ofGetHeight()/3, ofGetWidth()-contentWidth, 2*ofGetHeight()/3);
    
    while(logBuffer->getSize()*15 > 2*ofGetHeight()/3) logBuffer->eraseLastLine();
    
    for (int i = 0; i < logBuffer->getSize(); i++){
        string line = logBuffer->getLine(i);
        ofDrawBitmapString(line, debugRectangle.x, debugRectangle.y + (15*(i+1)));
    }
    
    
    //Draw the framerate
    ofSetColor(255, 0,0);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, ofGetHeight()-10);
}

void ofApp::keyPressedOnSecondWindow(ofKeyEventArgs & args){
    
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    for(auto phasor : phasors)
        phasor.audioIn(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(isDragging){
        curvePos = (curvePos+(ofPoint(x, y)-curveDraggerPrevPos));
        curveDragger = ofRectangle(curvePos.x, curvePos.y-20, 255, 20);
        curveDraggerPrevPos = ofPoint(x, y);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if(curveDragger.inside(x, y) && masterModule.drawCurve()){
        curveDraggerPrevPos = ofPoint(x, y);
        isDragging = true;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    isDragging = false;
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

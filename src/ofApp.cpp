#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    logBuffer = make_shared<bufferLoggerChannel>();
    ofSetLoggerChannel((shared_ptr<ofBaseLoggerChannel>)logBuffer);
    
    //Set the FrameRate to be 40, that is the frame rate of the Pixel Bars
    ofSetFrameRate(60);
    
    bpmControl::getInstance().setup();
    
    masterModule.setup(1);
    
    
    //Setup of the phasors, wich controls the oscilator generator and other parameters
    for(int i=0; i<1; i++)
        phasors.push_back(make_shared<phasorClass>(i+1));
    
    
    paramsControl = &parametersControl::getInstance();

    
    oscBankGroup = new oscillatorBankGroup(PIXEL_X_BAR, NUM_BARS);
    oscillators.push_back(make_shared<oscillatorBank>(NUM_BARS, true, 1));
    colorModule = new colorApplier(NUM_BARS);
    senderModule = new senderManager();
    preview = new waveScope(logBuffer, 3);
    //Create main gui, and add listeners when all guis are created
    paramsControl->setup();
    
    //Setup the soundStream so we can use the audio rate called function "audioIn" to update the phasor and have it better synced
    soundStream.setup(this, 0, 2, 44100, 512, 4);
    
   
    
    ofAddListener(paramsControl->createNewModule, this, &ofApp::newModuleListener);
    ofAddListener(paramsControl->destroyModule, this, &ofApp::deleteModuleListener);
}

void ofApp::newModuleListener(pair<moduleType, ofPoint> &info){
    switch (info.first) {
        case phasor_module:
            phasors.push_back(make_shared<phasorClass>(phasors.size()+1, info.second));
            break;
        case oscillator_module:
            monoOscillator.push_back(make_shared<baseOscillator>(monoOscillator.size()+1, true, info.second));
            break;
        case oscillatorBank_module:
        {
            int nOscillators = ofToInt(ofSystemTextBoxDialog("How many oscillators?"));
            oscillators.push_back(make_shared<oscillatorBank>(nOscillators, true, oscillators.size()+1, info.second));
            break;
        }
//        case oscillatorBankGroup_module:
//        {
//            vector<string> bankGroupSizeInfo = ofSplitString(ofSystemTextBoxDialog("Bank group size, expressed as 161x12"), "x");
//            if(bankGroupSizeInfo.size() == 2)
//                oscBankGroup.push_back(new oscillatorBankGroup(ofToInt(bankGroupSizeInfo[0]), ofToInt(bankGroupSizeInfo[1])));
//            else
//                ofSystemAlertDialog("Wrong entered info");
//        }
        default:
            break;
    }
}

void ofApp::deleteModuleListener(string &moduleName){
    int id;
    if(ofSplitString(moduleName, " ").size() > 0)
        id = ofToInt(ofSplitString(moduleName, " ").back());
    else
        id = -1;
    
    ofStringReplace(moduleName, " " + ofToString(id), "");
    ofLog()<<"Module Type: " << moduleName << "ID: " << id;
    
    if(moduleName == "phasor"){
        phasors.erase(phasors.begin()+id-1);
    }
    else if(moduleName == "oscillator"){
        monoOscillator.erase(monoOscillator.begin()+id-1);
    }
    else if(moduleName == "oscillatorBank"){
        oscillators.erase(oscillators.begin()+id-1);
    }
    else if(moduleName == "colorAppier"){
        delete colorModule;
    }
    else if(moduleName == "waveScope"){
        delete preview;
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
    //Phasor updates automatically at audio rate, but we need to take the value for this update so the phasor is the same along all the time update is being called
    //this has to be done in another thread?? Does not work in another thread becouse the last step is syphon send and can not be done in a thread that is not the main thread becouse opengl calls must run on the same thread
    for(auto &phasor : phasors)
        phasor->getPhasor();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
}

void ofApp::exit(){
    paramsControl->saveGuiArrangement();
}

void ofApp::drawSecondWindow(ofEventArgs &args){
    preview->draw();
}

void ofApp::keyPressedOnSecondWindow(ofKeyEventArgs & args){
    
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    for(auto phasor : phasors)
        phasor->audioIn(input, bufferSize, nChannels);
    
    bpmControl::getInstance().audioIn(input, bufferSize, nChannels);
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
    if(dragInfo.files.size() == 1){
        ofXml xml;
        if(xml.load(dragInfo.files[0])){
            if(xml.getName() == "GeneratorConfig"){
                
            }
        }
    }
}

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //Turn off quit app on ESC press.
    ofSetEscapeQuitsApp(false);
    
    ofBuffer buffer = ofBufferFromFile("lastOpenedFile.txt");
    string path = *buffer.getLines().begin();
    auto result = ofSystemLoadDialog("Select Generator File", false, path);
    if(ofSplitString(result.getPath(), ".").back() != "generator"){
        ofSystemAlertDialog(".generator file needed");
        ofExit();
    }
    
    ofBuffer buf;
    buf.append(result.getPath());
    ofBufferToFile("lastOpenedFile.txt", buf);

    
    ofBackground(0);
    logBuffer = make_shared<bufferLoggerChannel>();
    ofSetLoggerChannel((shared_ptr<ofBaseLoggerChannel>)logBuffer);
    
    //Set the FrameRate to be 40, that is the frame rate of the Pixel Bars
    ofSetFrameRate(60);
    
    paramsControl = &parametersControl::getInstance();
    
    bpmControl::getInstance().setup();
    
    ofXml xml;
    if(xml.load(result.getPath())){
        if(xml.exists("GeneratorConfig")){
            xml.setTo("GeneratorConfig");
            string name = xml.getValue("Name");
            int width = xml.getIntValue("Width");
            int height = xml.getIntValue("Height");
            bool invert = xml.getBoolValue("Invert");
            
            ofSetWindowTitle(name + " " + ofToString(width)+ "x" + ofToString(height));
            
            phasors.push_back(new phasorClass(1));
            oscBankGroup = new oscillatorBankGroup(height, width);
            oscillators.push_back(new oscillatorBank(width, true, 1));
            colorModule = new colorApplier(width);
            senderModule = new senderManager(invert);
            preview = new waveScope(logBuffer, 3);
            //Create main gui, and add listeners when all guis are created
            paramsControl->setup();
        }
    }
    
    //Setup the soundStream so we can use the audio rate called function "audioIn" to update the phasor and have it better synced
    soundStream.setup(this, 0, 2, 44100, 512, 4);
    
   
    
    ofAddListener(paramsControl->createNewModule, this, &ofApp::newModuleListener);
    ofAddListener(paramsControl->destroyModule, this, &ofApp::deleteModuleListener);
    
    ofAddListener(paramsControl->nextFrameEvent, this, &ofApp::nextFrameListener);
}

void ofApp::newModuleListener(pair<moduleType, ofPoint> &info){
    switch (info.first) {
        case phasor_module:{
            bool foundNullElementInVector = false;
            for (int i = 0; (i < phasors.size() && !foundNullElementInVector) ; i++){
                if(phasors[i] == nullptr){
                    phasors[i] = new phasorClass(i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                phasors.push_back(new phasorClass(phasors.size()+1, info.second));
            break;
        }
        case oscillator_module:{
            bool foundNullElementInVector = false;
            for (int i = 0; (i < monoOscillator.size() && !foundNullElementInVector) ; i++){
                if(monoOscillator[i] == nullptr){
                    monoOscillator[i] = new baseOscillator(i+1, true, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                monoOscillator.push_back(new baseOscillator(monoOscillator.size()+1, true, info.second));
            break;
        }
        case oscillatorBank_module:
        {
            int nOscillators = info.second.z;
            if(nOscillators == 0) nOscillators = ofToInt(ofSystemTextBoxDialog("How many oscillators?"));
            info.second.z = 0;
            
            bool foundNullElementInVector = false;
            for (int i = 0; (i < oscillators.size() && !foundNullElementInVector) ; i++){
                if(oscillators[i] == nullptr){
                    oscillators[i] = new oscillatorBank(nOscillators, true, i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                oscillators.push_back(new oscillatorBank(nOscillators, true, oscillators.size()+1, info.second));
            break;
        }
        case waveScope_module:
        {
            int nScopes = info.second.z;
            if(nScopes == 0) nScopes = ofToInt(ofSystemTextBoxDialog("How many Scopes?"));
            info.second.z = 0;
            if(preview != nullptr) delete preview;
            preview = new waveScope(logBuffer, nScopes, info.second);
            break;
        }
        case colorApplier_module:
        {
            colorModule = new colorApplier(NUM_BARS);
        }
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
        delete phasors[id-1];
        phasors[id-1] = nullptr;
    }
    else if(moduleName == "oscillator"){
        delete monoOscillator[id-1];
        monoOscillator[id-1] = nullptr;
    }
    else if(moduleName == "oscillatorBank"){
        delete oscillators[id-1];
        oscillators[id-1] = nullptr;
    }
    else if(moduleName == "colorAppier"){
        delete colorModule;
    }
    else if(moduleName == "waveScope"){
        delete preview;
        preview = nullptr;
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    //Phasor updates automatically at audio rate, but we need to take the value for this update so the phasor is the same along all the time update is being called
    //this has to be done in another thread?? Does not work in another thread becouse the last step is syphon send and can not be done in a thread that is not the main thread becouse opengl calls must run on the same thread
    for(auto &phasor : phasors){
        if(phasor != nullptr)
            phasor->getPhasor();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
//    ofDrawBitmapString(ofToString(sharedResources::getFilePath()), 20, 20);
}

void ofApp::exit(){
    paramsControl->saveGuiArrangement();
}

void ofApp::drawSecondWindow(ofEventArgs &args){
    if(preview != nullptr) preview->draw();
}

void ofApp::keyPressedOnSecondWindow(ofKeyEventArgs & args){
    
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    for(auto phasor : phasors){
        if(phasor != nullptr)
            phasor->audioIn(input, bufferSize, nChannels);
    }
    
    bpmControl::getInstance().audioIn(input, bufferSize, nChannels);
}

void ofApp::nextFrameListener(){
    for(auto phasor : phasors){
        if(phasor != nullptr)
            phasor->nextFrame();
    }
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
//    if(dragInfo.files.size() == 1){
//        ofXml xml;
//        if(xml.load(dragInfo.files[0])){
//            if(xml.exists("GeneratorConfig")){
//                xml.setTo("GeneratorConfig");
//                ofLog()<<xml.getValue("Name");
//                int width = xml.getIntValue("Width");
//                int height = xml.getIntValue("Height");
//                bool invert = xml.getBoolValue("Invert");
//                
//                
//                phasors.push_back(make_shared<phasorClass>(1));
//                oscBankGroup = new oscillatorBankGroup(height, width);
//                oscillators.push_back(make_shared<oscillatorBank>(width, true, 1));
//                colorModule = new colorApplier(width);
//                senderModule = new senderManager();
//                preview = new waveScope(logBuffer, 3);
//                //Create main gui, and add listeners when all guis are created
//                paramsControl->setup();
//            }
//        }
//    }
}

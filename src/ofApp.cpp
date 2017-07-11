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
    
    //Set the FrameRate to be 40, that is the frame rate of artnet
    ofSetFrameRate(60);
    
    paramsControl = &parametersControl::getInstance();
    
    bpmControl::getInstance().setup();
    
    ofXml xml;
    if(xml.load(result.getPath())){
        if(xml.exists("GeneratorConfig")){
            xml.setTo("GeneratorConfig");
            string name = xml.getValue("Name");
            width = xml.getIntValue("Width");
            height = xml.getIntValue("Height");
            bool invert = xml.getBoolValue("InvertTexture");
            float bpm = xml.getFloatValue("BPM");
            if(bpm == 0) bpm = 120;
            
            hasColorApplier = xml.getBoolValue("Color");
            int previewGroupSize = xml.getIntValue("GroupScopes");
            int previewBankSize = xml.getIntValue("BankScopes");
            
        
            ofSetWindowTitle(name + " " + ofToString(width)+ "x" + ofToString(height));
            
            phasors.push_back(new phasorClass(1));
            oscBankGroups.push_back(new oscillatorBankGroup(height, width, 1));
            oscillators.push_back(new oscillatorBank(width, true, 1));
            if(hasColorApplier)
                colorModule = new colorApplier();
            senderModule = new senderManager(invert);
            preview = new waveScope(logBuffer, previewGroupSize, previewBankSize);
            //Create main gui, and add listeners when all guis are created
            paramsControl->setup();
            paramsControl->setGlobalBPM(bpm);
        }
    }
    
    //Setup the soundStream so we can use the audio rate called function "audioIn" to update the phasor and have it better synced
    soundStream.setup(this, 0, 2, 44100, 512, 4);
    
   
    
    ofAddListener(paramsControl->createNewModule, this, &ofApp::newModuleListener);
    ofAddListener(paramsControl->destroyModule, this, &ofApp::deleteModuleListener);
    ofAddListener(paramsControl->nextFrameEvent, this, &ofApp::nextFrameListener);
}

void ofApp::newModuleListener(pair<string, ofPoint> &info){
    vector<string> moduleName = ofSplitString(info.first, " ");
    string moduleTypeName = moduleName[0];
    
    if(moduleTypeName == "phasor"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < phasors.size() && !foundNullElementInVector) ; i++){
                if(phasors[i] == nullptr){
                    phasors[i] = new phasorClass(i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                phasors.push_back(new phasorClass(phasors.size()+1, info.second));
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(phasors.size() <= id-1)
                phasors.push_back(nullptr);
            phasors[id-1] = new phasorClass(id, info.second);
        }
    }
    else if(moduleTypeName == "oscillator"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < monoOscillator.size() && !foundNullElementInVector) ; i++){
                if(monoOscillator[i] == nullptr){
                    monoOscillator[i] = new baseOscillator(i+1, true, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                monoOscillator.push_back(new baseOscillator(monoOscillator.size()+1, true, info.second));
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(monoOscillator.size() <= id-1)
                monoOscillator.push_back(nullptr);
            monoOscillator[id-1] = new baseOscillator(id, true, info.second);
        }
    }
    
    else if(moduleTypeName == "oscillatorBank"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < oscillators.size() && !foundNullElementInVector) ; i++){
                if(oscillators[i] == nullptr){
                    oscillators[i] = new oscillatorBank(width, true, i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                oscillators.push_back(new oscillatorBank(width, true, oscillators.size()+1, info.second));
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(oscillators.size() <= id-1)
                oscillators.push_back(nullptr);
            oscillators[id-1] = new oscillatorBank(width, true, id, info.second);
        }
    }
    
    else if(moduleTypeName == "oscillatorGroup"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < oscBankGroups.size() && !foundNullElementInVector) ; i++){
                if(oscBankGroups[i] == nullptr){
                    oscBankGroups[i] = new oscillatorBankGroup(height, width, i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                oscBankGroups.push_back(new oscillatorBankGroup(height, width, oscBankGroups.size()+1, info.second));
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(oscBankGroups.size() <= id-1)
                oscBankGroups.push_back(nullptr);
            oscBankGroups[id-1] = new oscillatorBankGroup(height, width, id, info.second);
        }
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
    else if(moduleName == "oscillatorGroup"){
        delete oscBankGroups[id-1];
        oscBankGroups[id-1] = nullptr;
    }
    else if(moduleName == "colorAppier"){
        delete colorModule;
        colorModule = nullptr;
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
    for(int i = phasors.size()-1 ; i >= 0 ; i--){
        if(phasors[i] != nullptr)
            phasors[i]->getPhasor();
    }
}

#pragma mark -------- Second window events --------

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
//    ofDrawBitmapString(ofToString(sharedResources::getFilePath()), 20, 20);
}

void ofApp::exit(){
    paramsControl->saveGuiArrangement();
    paramsControl->saveMidiMapping();
}

void ofApp::drawSecondWindow(ofEventArgs &args){
    if(preview != nullptr) preview->draw();
}

void ofApp::mouseMovedSecondWindow(ofMouseEventArgs &a){
    if(preview != nullptr)
        preview->mouseMoved(a);
}

void ofApp::mousePressedSecondWindow(ofMouseEventArgs &a){
    if(preview != nullptr)
        preview->mousePressed(a);
}

void ofApp::mouseReleasedSecondWindow(ofMouseEventArgs &a){
    if(preview != nullptr)
        preview->mouseReleased(a);
}

void ofApp::mouseDraggedSecondWindow(ofMouseEventArgs &a){
    if(preview != nullptr)
        preview->mouseDragged(a);
}


//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    for(int i = phasors.size()-1 ; i >= 0 ; i--){
        if(phasors[i] != nullptr)
            phasors[i]->audioIn(input, bufferSize, nChannels);
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

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //Turn off quit app on ESC press.
    ofSetEscapeQuitsApp(false);
    
    ofBuffer buffer = ofBufferFromFile("lastOpenedFile.txt");
    string path = *buffer.getLines().begin();
    auto result = ofSystemLoadDialog("Select Generator File", false, path);
    if(ofSplitString(result.getPath(), ".").back() != "generator"){
//        ofSystemAlertDialog(".generator file needed");
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
            
            float bpm = xml.getFloatValue("BPM");
            if(bpm == 0) bpm = 120;
            
            hasColorApplier = xml.getIntValue("Color");
            int previewGroupSize = xml.getIntValue("GroupScopes");
            int previewBankSize = xml.getIntValue("BankScopes");
            
           
        
            ofSetWindowTitle(name + " " + ofToString(width)+ "x" + ofToString(height));
            
            phasors.push_back(new phasorClass(1));
            oscBankGroups.push_back(new oscillatorBankGroup(height, width, 1));
            oscillatorBanks.push_back(new oscillatorBank(width, true, 1));
            //envelopeGens.push_back(new envelopeGenerator(1, width));
            for(int i = 0 ; i < hasColorApplier ; i++)
                colorModules.push_back(new colorApplier(i+1));
            
            int numSyphonServers = xml.getIntValue("SyphonSenders");
            for(int i = 0; i < numSyphonServers; i++){
                bool invert = xml.getBoolValue("SyphonSender"+ ofToString(i+1) + "InvertTexture");
                string grayName = xml.getValue("SyphonSender"+ ofToString(i+1) + "GrayName");
                string colorName = xml.getValue("SyphonSender"+ ofToString(i+1) + "ColorName");
                senderModules.push_back(new senderManager(i+1, invert, grayName, colorName));
            }
            
            preview = new waveScope(logBuffer, hasColorApplier, previewBankSize);
            converters.push_back(new typeConverter<vector<float>, vector<vector<float>>>(1, ofPoint(700,500)));
            expressionOps.push_back(new expressionOperator<vector<float>>(1, 2, ofPoint(800, 500)));
            new midiGateIn();
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
            for (int i = 0; (i < oscillators.size() && !foundNullElementInVector) ; i++){
                if(oscillators[i] == nullptr){
                    oscillators[i] = new baseOscillator(i+1, true, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                oscillators.push_back(new baseOscillator(oscillators.size()+1, true, info.second));
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(oscillators.size() <= id-1)
                oscillators.push_back(nullptr);
            oscillators[id-1] = new baseOscillator(id, true, info.second);
        }
    }
    
    else if(moduleTypeName == "oscillatorBank"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < oscillatorBanks.size() && !foundNullElementInVector) ; i++){
                if(oscillatorBanks[i] == nullptr){
                    int nOscillators = ofToInt(ofSystemTextBoxDialog("How many Oscillators? Width is " + ofToString(width) + ", height is " + ofToString(height)));
                    oscillatorBanks[i] = new oscillatorBank(nOscillators, true, i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector){
                int nOscillators = ofToInt(ofSystemTextBoxDialog("How many Oscillators?"));
                oscillatorBanks.push_back(new oscillatorBank(nOscillators, true, oscillatorBanks.size()+1, info.second));
            }
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(oscillatorBanks.size() <= id-1)
                oscillatorBanks.push_back(nullptr);
            int nOscillators = info.second.z;
            info.second.z = 0;
            oscillatorBanks[id-1] = new oscillatorBank(nOscillators, true, id, info.second);
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
    
    else if(moduleTypeName == "envelopeGenerator"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < envelopeGens.size() && !foundNullElementInVector) ; i++){
                if(envelopeGens[i] == nullptr){
                    envelopeGens[i] = new envelopeGenerator(i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                envelopeGens.push_back(new envelopeGenerator(envelopeGens.size()+1, info.second));
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(envelopeGens.size() <= id-1)
                envelopeGens.push_back(nullptr);
            envelopeGens[id-1] = new envelopeGenerator(id, info.second);
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
        delete oscillators[id-1];
        oscillators[id-1] = nullptr;
    }
    else if(moduleName == "oscillatorBank"){
        delete oscillatorBanks[id-1];
        oscillatorBanks[id-1] = nullptr;
    }
    else if(moduleName == "oscillatorGroup"){
        delete oscBankGroups[id-1];
        oscBankGroups[id-1] = nullptr;
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
    if(paramsControl != nullptr){
        paramsControl->saveGuiArrangement();
        paramsControl->saveMidiMapping();
    }
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

#include "ofApp.h"
#include "chartresTextureUnifier.h"
#include "dataRecorder.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //Turn off quit app on ESC press.
    ofSetEscapeQuitsApp(false);
    configured = false;
    
    ofBuffer buffer = ofBufferFromFile("lastOpenedFile.txt");
    string path = *buffer.getLines().begin();
    auto result = ofSystemLoadDialog("Select Generator File", false, path);
    if(ofSplitString(result.getPath(), ".").back() != "generator"){
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
    
    ofRectangle prevWinRect = ofRectangle(-1, -1, 100, 100);
    
    bpmControl::getInstance().setup();
    
    ofXml xml;
    if(xml.load(result.getPath())){
        if(xml.exists("GeneratorConfig")){
            xml.setTo("GeneratorConfig");
            string name = xml.getValue("Name");
            
            int numBankGroups = xml.getIntValue("BankGroupsNum");
            for(int i = 0 ; i < numBankGroups; i++){
                int groupWidth = xml.getIntValue("BankGroup" + ofToString(i+1) + "Width");
                int groupHeight = xml.getIntValue("BankGroup" + ofToString(i+1) + "Height");
                oscBankGroups.push_back(new oscillatorBankGroup(groupHeight, groupWidth, oscBankGroups.size()+1));
            }
            
            float bpm = xml.getFloatValue("BPM");
            if(bpm == 0) bpm = 120;
            
            int frameRate = xml.getIntValue("FrameRate");
            ofSetFrameRate(frameRate);
            
            hasColorApplier = xml.getIntValue("Color");
            int previewGroupSize = xml.getIntValue("GroupScopes");
            int previewColorSize = xml.getIntValue("GroupColorScopes");
            int previewBankSize = xml.getIntValue("BankScopes");
            
            ofSetWindowTitle(name);
            
            for(int i = 0 ; i < hasColorApplier ; i++)
                colorModules.push_back(new colorApplier(i+1));
            
            int numSyphonServers = xml.getIntValue("SyphonSenders");
            for(int i = 0; i < numSyphonServers; i++){
                bool invert = xml.getBoolValue("SyphonSender"+ ofToString(i+1) + "InvertTexture");
                string grayName = xml.getValue("SyphonSender"+ ofToString(i+1) + "GrayName");
                string colorName = xml.getValue("SyphonSender"+ ofToString(i+1) + "ColorName");
                senderModules.push_back(new senderManager(i+1, invert, grayName, colorName));
            }
            
            auto mainPos = ofSplitString(xml.getValue("MainWindowPos"), "_");
            if(mainPos.size() == 2){
                ofSetWindowPosition(ofToInt(mainPos[0]), ofToInt(mainPos[1]));
            }
            
            auto mainSize = ofSplitString(xml.getValue("MainWindowSize"), "_");
            if(mainSize.size() == 2){
                ofSetWindowShape(ofToInt(mainSize[0]), ofToInt(mainSize[1]));
            }
            
            auto prevPos = ofSplitString(xml.getValue("PrevWindowPos"), "_");
            if(prevPos.size() == 2){
                prevWinRect.setPosition(ofToInt(prevPos[0]), ofToInt(prevPos[1]));
            }
            
            auto prevSize = ofSplitString(xml.getValue("PrevWindowSize"), "_");
            if(prevSize.size() == 2){
                prevWinRect.setSize(ofToInt(prevSize[0]), ofToInt(prevSize[1]));
            }
            if(xml.getBoolValue("AudioControls"))
                audioControl = new audioEngineController();
            
            if(xml.getBoolValue("OscInput")){
                new oscInput();
            }
            int audioChanels = xml.getIntValue("AudioAnalyzer");
            if(audioChanels > 0){
                audioAnalysis = new audioAnalyzer(audioChanels);
                //Setup the soundStream so we can use the audio rate called function "audioIn" to update the phasor and have it better synced
                soundStream.setup(this, 0, audioChanels, 44100, 512, 4);
            }else{
                //Setup the soundStream so we can use the audio rate called function "audioIn" to update the phasor and have it better synced
                soundStream.setup(this, 0, 2, 44100, 512, 4);
            }
            
            if(xml.getBoolValue("TextureUnifier")){
                new chartresTextureUnifier();
            }
            if(xml.getBoolValue("DataRecorder")){
                new dataRecorder();
            }
            
            preview = new waveScope(logBuffer, previewGroupSize, previewColorSize, previewBankSize);
//            converters.push_back(new typeConverter<vector<float>, vector<vector<float>>>(1, ofPoint(700,500)));
            //Create main gui, and add listeners when all guis are created
            paramsControl->setup();
            paramsControl->setGlobalBPM(bpm);
            
            configured = true;
        }
    }
    
    if(!configured) ofExit();
    
    
    preview->activateSeparateWindow(prevWinRect);
    
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
                    int nOscillators = ofToInt(ofSystemTextBoxDialog("How many Oscillators?"));
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
//        if(moduleName.size() < 2){
//            bool foundNullElementInVector = false;
//            for (int i = 0; (i < oscBankGroups.size() && !foundNullElementInVector) ; i++){
//                if(oscBankGroups[i] == nullptr){
//                    oscBankGroups[i] = new oscillatorBankGroup(height, width, i+1, info.second);
//                    foundNullElementInVector = true;
//                }
//            }
//            if(!foundNullElementInVector)
//                oscBankGroups.push_back(new oscillatorBankGroup(height, width, oscBankGroups.size()+1, info.second));
//        }
//        else{
//            int id = ofToInt(moduleName[1]);
//            while(oscBankGroups.size() <= id-1)
//                oscBankGroups.push_back(nullptr);
//            oscBankGroups[id-1] = new oscillatorBankGroup(height, width, id, info.second);
//        }
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
    
    else if(moduleTypeName == "midiGateIn"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < midiGateIns.size() && !foundNullElementInVector) ; i++){
                if(midiGateIns[i] == nullptr){
                    midiGateIns[i] = new midiGateIn(i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                midiGateIns.push_back(new midiGateIn(midiGateIns.size()+1, info.second));
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(midiGateIns.size() <= id-1)
                midiGateIns.push_back(nullptr);
            midiGateIns[id-1] = new midiGateIn(id, info.second);
        }
    }
    
    else if(moduleTypeName == "delta"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < deltaCalculators.size() && !foundNullElementInVector) ; i++){
                if(deltaCalculators[i] == nullptr){
                    deltaCalculators[i] = new delta(i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                deltaCalculators.push_back(new delta(deltaCalculators.size()+1, info.second));
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(deltaCalculators.size() <= id-1)
                deltaCalculators.push_back(nullptr);
            deltaCalculators[id-1] = new delta(id, info.second);
        }
    }
    else if(moduleTypeName == "expressionOperator"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < expressionOps.size() && !foundNullElementInVector) ; i++){
                if(expressionOps[i] == nullptr){
                    int nInputs = ofToInt(ofSystemTextBoxDialog("How many Inputs?"));
                    if(nInputs > 0 && nInputs < 4)
                        expressionOps[i] = new expressionOperator<vector<float>>(i+1, nInputs, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector){
                int nInputs = ofToInt(ofSystemTextBoxDialog("How many Inputs?"));
                if(nInputs > 0 && nInputs < 4)
                    expressionOps.push_back(new expressionOperator<vector<float>>(expressionOps.size()+1, nInputs,info.second));
            }
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(expressionOps.size() <= id-1)
                expressionOps.push_back(nullptr);
            int nInputs = info.second.z;
            info.second.z = 0;
            if(nInputs > 0 && nInputs < 4)
                expressionOps[id-1] = new expressionOperator<vector<float>>(id, nInputs, info.second);
        }
    }
    else if(moduleTypeName == "mapper"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < mappers.size() && !foundNullElementInVector) ; i++){
                if(mappers[i] == nullptr){
                    mappers[i] = new mapper(i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                mappers.push_back(new mapper(mappers.size()+1, info.second));
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(mappers.size() <= id-1)
                mappers.push_back(nullptr);
            mappers[id-1] = new mapper(id, info.second);
        }
    }
    else if(moduleTypeName == "vectorMapper"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < vecMappers.size() && !foundNullElementInVector) ; i++){
                if(vecMappers[i] == nullptr){
                    vecMappers[i] = new vectorMapper(i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                vecMappers.push_back(new vectorMapper(vecMappers.size()+1, info.second));
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(vecMappers.size() <= id-1)
                vecMappers.push_back(nullptr);
            vecMappers[id-1] = new vectorMapper(id, info.second);
        }
    }
    else if(moduleTypeName == "manualOscillatorBank"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < manualOscBanks.size() && !foundNullElementInVector) ; i++){
                if(manualOscBanks[i] == nullptr){
                    int nOscillators = ofToInt(ofSystemTextBoxDialog("How many Oscillators?"));
                    if(nOscillators > 0)
                        manualOscBanks[i] = new manualOscillatorBank(nOscillators, i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector){
                int nOscillators = ofToInt(ofSystemTextBoxDialog("How many Oscillators?"));
                if(nOscillators > 0)
                    manualOscBanks.push_back(new manualOscillatorBank(nOscillators, manualOscBanks.size()+1, info.second));
            }
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(manualOscBanks.size() <= id-1)
                manualOscBanks.push_back(nullptr);
            int nOscillators = info.second.z;
            info.second.z = 0;
            if(nOscillators > 0)
                manualOscBanks[id-1] = new manualOscillatorBank(nOscillators, id, info.second);
        }
    }
    else if(moduleTypeName == "typeConverter"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < converters.size() && !foundNullElementInVector) ; i++){
                if(converters[i] == nullptr){
                    int type = ofToInt(ofSystemTextBoxDialog("Choose TypeConverter Type: \n 1. float to vector<float> \n 2. vector<float> to vector<vector<float>> \n 3. vector<float> to float \n 4. vector<vector<float>> to vector<float>"));
                    switch(static_cast<converterTypes>(type)){
                        case CONVERT_FLOAT_TO_VECFLOAT:
                            converters[i] = new typeConverter<float, vector<float>>(i+1, info.second);
                            break;
                        case CONVERT_VECFLOAT_TO_FLOAT:
                            converters[i] = new typeConverter<vector<float>, float>(i+1, info.second);
                            break;
                        case CONVERT_VECFLOAT_TO_VECVECFLOAT:
                            converters[i] = new typeConverter<vector<float>, vector<vector<float>>>(i+1, info.second);
                            break;
                        case CONVERT_VECVECFLOAT_TO_VECFLOAT:
                            converters[i] = new typeConverter<vector<vector<float>>, vector<float>>(i+1, info.second);
                            break;
                        default:
                            break;
                            
                    }
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector){
                int type = ofToInt(ofSystemTextBoxDialog("Choose TypeConverter Type: \n 1. float to vector<float> \n 2. vector<float> to vector<vector<float>> \n 3. vector<float> to float \n 4. vector<vector<float>> to vector<float>"));
                switch(static_cast<converterTypes>(type)){
                    case CONVERT_FLOAT_TO_VECFLOAT:
                        converters.push_back(new typeConverter<float, vector<float>>(converters.size()+1, info.second));
                        break;
                    case CONVERT_VECFLOAT_TO_FLOAT:
                        converters.push_back(new typeConverter<vector<float>, float>(converters.size()+1, info.second));
                        break;
                    case CONVERT_VECFLOAT_TO_VECVECFLOAT:
                        converters.push_back(new typeConverter<vector<float>, vector<vector<float>>>(converters.size()+1, info.second));
                        break;
                    case CONVERT_VECVECFLOAT_TO_VECFLOAT:
                        converters.push_back(new typeConverter<vector<vector<float>>, vector<float>>(converters.size()+1, info.second));
                        break;
                    default:
                        break;
                }
            }
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(converters.size() <= id-1)
                converters.push_back(nullptr);
            int type = info.second.z;
            info.second.z = 0;
            switch(static_cast<converterTypes>(type)){
                case CONVERT_FLOAT_TO_VECFLOAT:
                    converters[id-1] = new typeConverter<float, vector<float>>(id, info.second);
                    break;
                case CONVERT_VECFLOAT_TO_FLOAT:
                    converters[id-1] = new typeConverter<vector<float>, float>(id, info.second);
                    break;
                case CONVERT_VECFLOAT_TO_VECVECFLOAT:
                    converters[id-1] = new typeConverter<vector<float>, vector<vector<float>>>(id, info.second);
                    break;
                case CONVERT_VECVECFLOAT_TO_VECFLOAT:
                    converters[id-1] = new typeConverter<vector<vector<float>>, vector<float>>(id, info.second);
                    break;
                default:
                    break;
            }
        }
    }
    else if(moduleTypeName == "vectorGetter"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < vectorGetters.size() && !foundNullElementInVector) ; i++){
                if(vectorGetters[i] == nullptr){
                    vectorGetters[i] = new vectorGetter(i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector)
                vectorGetters.push_back(new vectorGetter(vectorGetters.size()+1, info.second));
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(vectorGetters.size() <= id-1)
                vectorGetters.push_back(nullptr);
            vectorGetters[id-1] = new vectorGetter(id, info.second);
        }
    }
    else if(moduleTypeName == "vectorChain"){
        if(moduleName.size() < 2){
            bool foundNullElementInVector = false;
            for (int i = 0; (i < vectorChains.size() && !foundNullElementInVector) ; i++){
                if(vectorChains[i] == nullptr){
                    int nInputs = ofToInt(ofSystemTextBoxDialog("How many Inputs?"));
                    if(nInputs > 0)
                        vectorChains[i] = new vectorChain(nInputs, i+1, info.second);
                    foundNullElementInVector = true;
                }
            }
            if(!foundNullElementInVector){
                int nInputs = ofToInt(ofSystemTextBoxDialog("How many Inputs?"));
                if(nInputs > 0)
                    vectorChains.push_back(new vectorChain(nInputs, vectorChains.size()+1, info.second));
            }
        }
        else{
            int id = ofToInt(moduleName[1]);
            while(vectorChains.size() <= id-1)
                vectorChains.push_back(nullptr);
            int nInputs = info.second.z;
            info.second.z = 0;
            if(nInputs > 0)
                vectorChains[id-1] = new vectorChain(nInputs, id, info.second);
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
    else if(moduleName == "envelopeGenerator"){
//        delete envelopeGens[id-1];
//        envelopeGens[id-1] = nullptr;
    }
    else if(moduleName == "midiGateIn"){
        delete midiGateIns[id-1];
        midiGateIns[id-1] = nullptr;
    }
    else if(moduleName == "delta"){
        delete deltaCalculators[id-1];
        deltaCalculators[id-1] = nullptr;
    }
    else if(moduleName == "expressionOperator"){
        delete expressionOps[id-1];
        expressionOps[id-1] = nullptr;
    }
    else if(moduleName == "mapper"){
        delete mappers[id-1];
        mappers[id-1] = nullptr;
    }
    else if(moduleName == "vectorMapper"){
        delete vecMappers[id-1];
        vecMappers[id-1] = nullptr;
    }
    else if(moduleName == "manualOscillatorBank"){
        delete manualOscBanks[id-1];
        manualOscBanks[id-1] = nullptr;
    }
    else if(moduleName == "typeConverter"){
        delete converters[id-1];
        converters[id-1] = nullptr;
    }
    else if(moduleName == "vectorGetter"){
        delete vectorGetters[id-1];
        vectorGetters[id-1] = nullptr;
    }
    else if(moduleName == "vectorChain"){
        delete vectorChains[id-1];
        vectorChains[id-1] = nullptr;
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

//--------------------------------------------------------------
void ofApp::draw(){
    
}

void ofApp::exit(){
    if(configured){
        if(paramsControl != nullptr){
            paramsControl->saveGuiArrangement();
        }
        
        ofBuffer buffer = ofBufferFromFile("lastOpenedFile.txt");
        string path = *buffer.getLines().begin();
        
        ofXml xml;
        if(xml.load(path)){
            if(xml.exists("GeneratorConfig")){
                xml.setTo("GeneratorConfig");
                xml.setValue("MainWindowPos", ofToString(ofGetWindowPositionX()) + "_" + ofToString(ofGetWindowPositionX()));
                xml.setValue("MainWindowSize", ofToString(ofGetWindowSize().x) + "_" + ofToString(ofGetWindowSize().y));
                xml.setValue("PrevWindowPos", ofToString(preview->getWindow()->getWindowPosition().x) + "_" + ofToString(preview->getWindow()->getWindowPosition().y));
                xml.setValue("PrevWindowSize", ofToString(preview->getWindow()->getWidth()) + "_" + ofToString(preview->getWindow()->getHeight()));
            }
        }
        xml.save(path);
    }
}


//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    for(int i = phasors.size()-1 ; i >= 0 ; i--){
        if(phasors[i] != nullptr)
            phasors[i]->audioIn(input, bufferSize, nChannels);
    }
    if(audioAnalysis != nullptr) audioAnalysis->audioIn(input, bufferSize, nChannels);
    
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

//
//  parametersControl.cpp
//  DGTL_Generator
//
//  Created by Eduard Frigola on 03/08/16.
//
//

#include "parametersControl.h"
#include <random>


parametersControl::~parametersControl(){
    datGuis.clear();
    parameterGroups.clear();
    connections.clear();
}

void parametersControl::createGuiFromParams(ofParameterGroup *paramGroup, ofColor guiColor, ofPoint pos){
    
    ofxDatGuiLog::quiet();
    ofxDatGui::setAssetPath("");
    
    //Put parameterGroup into vector
    parameterGroups.push_back(paramGroup);
    
    if(ofStringTimesInString(paramGroup->getName(), "phasor") != 0 && paramGroup->getFloat("BPM").getName() == "BPM" && datGui != nullptr)
        paramGroup->getFloat("BPM") = datGui->getSlider("Global BPM")->getValue();
    
    shared_ptr<ofxDatGui> tempDatGui = make_shared<ofxDatGui>();
    tempDatGui->setAutoDraw(false);
    
    tempDatGui->setTransformMatrix(transformMatrix);
    
    ofxDatGuiTheme* theme = new ofxDatGuiThemeCharcoal;
    theme->color.slider.fill = guiColor;
    theme->color.textInput.text = guiColor;
    theme->color.icons = guiColor;
    tempDatGui->setTheme(theme);
    
    tempDatGui->setWidth(290);
    tempDatGui->addHeader(paramGroup->getName());
    tempDatGui->addFooter();
    if(pos == ofPoint(-1, -1)){
        if(datGuis.size() == 0)
            tempDatGui->setPosition(0, 0);
        else
            tempDatGui->setPosition((datGuis[datGuis.size()-1]->getPosition().x + datGuis[datGuis.size()-1]->getWidth()) + 20, 0);
    }else{
        tempDatGui->setPosition(pos.x, pos.y);
    }
    
    for(int i=0 ; i<paramGroup->size(); i++){
        ofAbstractParameter &absParam = paramGroup->get(i);
        if(absParam.type() == typeid(ofParameter<float>).name())
            tempDatGui->addSlider(paramGroup->getFloat(i));
        else if(absParam.type() == typeid(ofParameter<int>).name())
            tempDatGui->addSlider(paramGroup->getInt(i));
        else if(absParam.type() == typeid(ofParameter<bool>).name())
            tempDatGui->addToggle(paramGroup->getName(i))->setChecked(paramGroup->getBool(i).get());
        else if(absParam.type() == typeid(ofParameter<string>).name()){
            if(ofSplitString(absParam.getName(), "_").size() > 1)
                tempDatGui->addLabel(ofSplitString(absParam.getName(), "_")[0]);
            else
                tempDatGui->addTextInput(absParam.getName(), absParam.cast<string>());
        }
        else if(absParam.type() == typeid(ofParameter<ofColor>).name())
            tempDatGui->addColorPicker(paramGroup->getName(i), ofColor::white);
        else if(absParam.type() == typeid(ofParameterGroup).name()){
            tempDatGui->addLabel(paramGroup->getGroup(i).getName());
            tempDatGui->addDropdown(paramGroup->getGroup(i).getName(), ofSplitString(paramGroup->getGroup(i).getString(0), "-|-"))->select(paramGroup->getGroup(i).getInt(1));
        }else{
            tempDatGui->addLabel(absParam.getName());
        }
    }
    
    //GUIS EVENT LISTERNERS
    tempDatGui->onButtonEvent(this, &parametersControl::onGuiButtonEvent);
    tempDatGui->onToggleEvent(this, &parametersControl::onGuiToggleEvent);
    tempDatGui->onDropdownEvent(this, &parametersControl::onGuiDropdownEvent);
    //        gui->onSliderEvent(this, &parametersControl::onGuiSliderEvent);
    tempDatGui->onTextInputEvent(this, &parametersControl::onGuiTextInputEvent);
    tempDatGui->onColorPickerEvent(this, &parametersControl::onGuiColorPickerEvent);
    tempDatGui->onRightClickEvent(this, &parametersControl::onGuiRightClickEvent);

    
    //OF PARAMETERS LISTERENRS
    ofAddListener(paramGroup->parameterChangedE(), this, &parametersControl::listenerFunction);
    
    datGuis.push_back(tempDatGui);
}

void parametersControl::setup(){
    
    arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    
    auto createCursorFromImage = [](string filename)->GLFWcursor*{
        ofImage img;
        img.load("Cursors/" + filename);
        GLFWimage glfwImg;
        glfwImg.height = img.getHeight();
        glfwImg.width = img.getWidth();
        glfwImg.pixels = img.getPixels().getData();
        return glfwCreateCursor(&glfwImg, 0, 0);
    };
    
    openedHandCursor = createCursorFromImage("openedHand.png");
    closedHandCursor = createCursorFromImage("closedHand.png");
    arrowListCursor = createCursorFromImage("arrowList.png");
    zoomInCursor = createCursorFromImage("zoomIn.png");
    zoomOutCursor = createCursorFromImage("zoomOut.png");
    crossCursor = createCursorFromImage("cross.png");
    
    ofAddListener(ofEvents().update, this, &parametersControl::update);
    ofAddListener(ofEvents().draw, this, &parametersControl::draw);
    
    ofRegisterKeyEvents(this);
    ofRegisterMouseEvents(this);
    
    
    //DatGui
    
    ofxDatGuiLog::quiet();
    ofxDatGui::setAssetPath("");
    
    datGui = new ofxDatGui();
    datGui->setAutoDraw(false);
    
    mainGuiTheme = new ofxDatGuiThemeCharcoal;
    ofColor randColor =  ofColor::indianRed;
    mainGuiTheme->color.slider.fill = randColor;
    mainGuiTheme->color.textInput.text = randColor;
    mainGuiTheme->color.icons = randColor;
    datGui->setTheme(mainGuiTheme);//, true);
    
    datGui->setWidth(290);
    datGui->addHeader("Presets Control");
    datGui->addFooter();

    datGui->addButton("Global Reset Phase");
    datGui->addSlider("Global BPM", 0, 999, 120);
    
    //Preset Control
    ofDirectory dir;
    vector<string> banks;
    dir.open("Presets");
    for(int i = 0; i < dir.listDir() ; i++)
        banks.push_back(dir.getName(i));
    banks.push_back(" -- NEW BANK -- ");
    bankSelect = datGui->addDropdown("Bank Select", banks);
    bankSelect->select(0);
    datGui->addLabel("<== Presets List ==>")->setStripe(ofColor::red, 10);
    
    presetsList = datGui->addScrollView("test", 10);
    
    loadBank();
    
    datGui->addTextInput("New Preset");
    
    
    datGui->addToggle("BPM Sync")->setChecked(false);
    //datGui->addToggle("Automatic Preset");
    //datGui->addButton("Reload Sequence");
    datGui->addSlider(fadeTime.set("Fade Time", 1, 0, 10));
//    datGui->addSlider(presetChangeBeatsPeriod.set("Beats Period", 4, 1, 120));
    
    datGui->setPosition(ofxDatGuiAnchor::BOTTOM_LEFT);

    
    //ControlGui Events
    datGui->onButtonEvent(this, &parametersControl::onGuiButtonEvent);
    datGui->onToggleEvent(this, &parametersControl::onGuiToggleEvent);
    datGui->onSliderEvent(this, &parametersControl::onGuiSliderEvent);
    datGui->onDropdownEvent(this, &parametersControl::onGuiDropdownEvent);
    datGui->onScrollViewEvent(this, &parametersControl::onGuiScrollViewEvent);
    datGui->onTextInputEvent(this, &parametersControl::newPresetListener);

    
    //OSC
    ofLog() << "OSC Info:";
    ofXml xml;
    if(xml.load("OSCConfig.xml")){
        string host = xml.getValue("host");
        if(host != ""){
            oscReceiver.setup(xml.getIntValue("ReceiverPort"));
            oscSender.setup(host, xml.getIntValue("SenderPort"));
            ofLog() << "Listening on port " << 12345;
            ofLog() << "Sending to host: " << host << " on port: " << 54321;
        }else
            ofLog() << "NO OSC";
    }
    
    //MIDI
    ofLog() << "MIDI Info:";
    ofxMidiOut::listPorts();
    if(midiOut.openPort("BCF2000")){
        midiIn.openPort("BCF2000");
        midiIn.addListener(this);
    }
    
    
    autoPreset = false;
    
    datGui->getButton("Reload Sequence")->setBackgroundColor(loadPresetsSequence() ? ofColor::green : ofColor::red);
    
    Tweenzor::init();
    
    beatTracker = &bpmControl::getInstance();
    
    
    ///POP UP MENuS
    popUpMenu = new ofxDatGui();
    popUpMenu->setVisible(false);
    popUpMenu->setPosition(-1, -1);
    ofColor randColor2 =  ofColor::indianRed;
//    theme->color.slider.fill = randColor2;
//    theme->color.textInput.text = randColor2;
//    theme->color.icons = randColor2;
    popUpMenu->setTheme(mainGuiTheme);
    popUpMenu->addDropdown("Choose module", {"Phasor", "Oscillator", "Oscillator Bank", "ColorApplier", "WaveScope"})->expand();
    
    popUpMenu->onDropdownEvent(this, &parametersControl::newModuleListener);
}


void parametersControl::update(ofEventArgs &args){
    
    datGui->update(args);
    for(auto gui : datGuis)
        gui->update(args);
        
    Tweenzor::update(ofGetElapsedTimeMillis());
    
    while(oscReceiver.hasWaitingMessages()){
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);
        
        vector<string> splitAddress = ofSplitString(m.getAddress(), "/");
        if(splitAddress.size() >= 2){
            ofStringReplace(splitAddress[1], "_", " ");
            if(splitAddress[1] == "presetLoad"){
                loadPreset(m.getArgAsString(0), m.getArgAsString(1));
            }else if(splitAddress[1] == "presetSave"){
                savePreset(m.getArgAsString(0), m.getArgAsString(1));
            }else if(splitAddress[1] == "phaseReset"){
                for(auto groupParam : parameterGroups){
                    if(ofStringTimesInString(groupParam->getName(), "phasor") != 0)
                        groupParam->getBool("Reset Phase") = 0;
                }
            }else if(splitAddress[1] == "nextFrame"){
                ofNotifyEvent(nextFrameEvent, this);
            }else{
                for(auto groupParam : parameterGroups){
                    if(groupParam->getName() == splitAddress[1]){
                        ofAbstractParameter &absParam = groupParam->get(splitAddress[2]);
                        if(absParam.type() == typeid(ofParameter<float>).name()){
                            ofParameter<float> castedParam = absParam.cast<float>();
                            castedParam = ofMap(m.getArgAsFloat(0), 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                        }else if(absParam.type() == typeid(ofParameter<int>).name()){
                            ofParameter<int> castedParam = absParam.cast<int>();
                            castedParam = ofMap(m.getArgAsFloat(0), 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                        }else if(absParam.type() == typeid(ofParameter<bool>).name())
                            groupParam->getBool(splitAddress[2]) = m.getArgAsBool(0);
                        else if(absParam.type() == typeid(ofParameter<string>).name())
                            groupParam->getString(splitAddress[2]) = m.getArgAsString(0);
                        else
                            groupParam->getGroup(splitAddress[2]).getInt(1) = m.getArgAsInt(0); //DropDown
                    }
                }
            }
        }
    }
    
    if(isFading){
        ofxOscMessage m;
        m.setAddress("presetFade");
        m.addFloatArg(parameterGroups[parameterGroups.size()-1]->getFloat("Master Fader").get());
        oscSender.sendMessage(m);
    }
    
    //TODO: Reimplement MIDI
//    //MIDI - Process the midi deque
//    while(midiMessages.size() > 0){
//        int parameterNum = midiMessages[0].control;
//        int parameterVal = midiMessages[0].value;
//        
//        midiMessages.pop_front();
//        
//        //get the grup in each iteration
//        ofParameterGroup groupParam;
//        if(parameterNum > phasorParams.size()-1){
//            if(parameterNum > (phasorParams.size() + oscilatorParams.size() -1 )){
//                if(parameterNum > (phasorParams.size() + oscilatorParams.size() + delayParams.size() -1))
//                    return;
//                parameterNum -= (phasorParams.size() + oscilatorParams.size());
//                groupParam = delayParams;
//            }
//            else{
//                parameterNum -= phasorParams.size();
//                groupParam = oscilatorParams;
//            }
//        }
//        else{
//            groupParam = phasorParams;
//        }
//        
//        //Iterate for all parameters in parametergroup and look for the type of the parameter
//        ofAbstractParameter &absParam = groupParam->get(parameterNum);
//        if(absParam.type() == typeid(ofParameter<float>).name()){
//            //Cast it
//            ofParameter<float> castedParam = absParam.cast<float>();
//            
//            //get the value of that parameter and map it
//            castedParam = (ofMap(parameterVal, 0, 127, castedParam.getMin(), castedParam.getMax(), true));
//        }
//        else if(absParam.type() == typeid(ofParameter<int>).name()){
//            ofParameter<int> castedParam = absParam.cast<int>();
//            int range = castedParam.getMax()-castedParam.getMin();
//            if(range < 128)
//                castedParam = ofMap(parameterVal, 0, ((int)(128/(range))*range), castedParam.getMin(), castedParam.getMax(), true);
//            else
//                castedParam = ofMap(parameterVal, 0, range/ceil((float)range/(float)128), castedParam.getMin(), castedParam.getMax(), true);
//            
//        }
//        else if(absParam.type() == typeid(ofParameter<bool>).name()){
//            ofParameter<bool> castedParam = absParam.cast<bool>();
//            
//            //get the value of that parameter and map it
//            castedParam.set(parameterVal >= 64 ? true : false);
//        }
//    }
    
    //Auto preset
    if(randomPresetsArrange.size()>0 && autoPreset && (ofGetElapsedTimef()-presetChangedTimeStamp) > periodTime){
        presetChangedTimeStamp = presetChangedTimeStamp+periodTime;
        int index = randomPresetsArrange[presetChangeCounter];
        loadPresetWithFade(presetNumbersAndBanks.at(index).first, presetNumbersAndBanks.at(index).second);
        periodTime = presetsTime[index];
        presetChangeCounter++;
        if(presetChangeCounter >= randomPresetsArrange.size()){
            presetChangeCounter = 0;
            mt19937 g(static_cast<uint32_t>(time(0)));
            shuffle(randomPresetsArrange.begin(), randomPresetsArrange.end(), g);
        }
    }
    
    if(newBpm != 0){
        setGlobalBPM(newBpm);
        newBpm = 0;
    }

}

void parametersControl::draw(ofEventArgs &args){
    ofPushMatrix();
    ofMultMatrix(transformMatrix);
    ofPushStyle();
     ofSetColor(ofColor::white);
    ofSetLineWidth(2);
    for(auto connection : connections){
        ofPath path = connection->getPath();
        if(connection->getMin() != 0 || connection->getMax() != 1)
            path.setColor(ofColor::navajoWhite);
        path.draw();
    }
    ofPopStyle();
    ofPopMatrix();
    
    
    datGui->draw(args);
    for(auto gui : datGuis)
        gui->draw(args);
    
    if(ofGetKeyPressed('r')){
        ofPopStyle();
        ofSetColor(255, 0, 0, 50);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        ofPopStyle();
    }
}

void parametersControl::saveGuiArrangement(){
    /*
    //xml.load("Preset_"+ofToString(presetNum)+".xml");
    xml.clear();
    
    //Root Element
    xml.addChild("GUIARRANGEMENT");
    
    // now we set our current element so we're on the right
    // element, so when add new nodes, they are still inside
    //the root element
    xml.setTo("GUIARRANGEMENT");
    
    xml.addChild("GuiWindow");
    xml.setTo("GuiWindow");
    xml.addValue("x", guiWindow->getWindowPosition().x);
    xml.addValue("y", guiWindow->getWindowPosition().y);
    xml.addValue("width", guiWindow->getWindowSize().x);
    xml.addValue("height", guiWindow->getWindowSize().y);
    xml.setToParent();
    
    xml.addChild("PrevWindow");
    xml.setTo("PrevWindow");
    xml.addValue("x", prevWindow->getWindowPosition().x);
    xml.addValue("y", prevWindow->getWindowPosition().y);
    xml.addValue("width", prevWindow->getWindowSize().x);
    xml.addValue("height", prevWindow->getWindowSize().y);
    xml.setToParent();
    
    xml.addChild("Main");
    xml.setTo("Main");
    xml.addValue("x", ofToString(datGui->getPosition().x));
    xml.addValue("y", ofToString(datGui->getPosition().y));
    xml.setToParent();
    
    for ( int i = 0; i < datGuis.size() ; i++){
        xml.addChild("Gui_" + ofToString(i));
        xml.setTo("Gui_" + ofToString(i));
        xml.addValue("x", ofToString(datGuis[i]->getPosition().x));
        xml.addValue("y", ofToString(datGuis[i]->getPosition().y));
        xml.setToParent();
    }
    xml.save("GuiArrangement_"+bankSelect->getSelected()->getName()+".xml");
    */
}

void parametersControl::loadGuiArrangement(){
    //Test if there is no problem with the file
    /*if(!xml.load("GuiArrangement_"+bankSelect->getSelected()->getName()+".xml"))
        return;
    
    if(xml.exists("GuiWindow")){
        xml.setTo("GuiWindow");
        guiWindow->setWindowPosition(xml.getIntValue("x"), xml.getIntValue("y"));
        guiWindow->setWindowShape(xml.getIntValue("width"), xml.getIntValue("height"));
        xml.setToParent();
    }
    
    if(xml.exists("PrevWindow")){
        xml.setTo("PrevWindow");
        prevWindow->setWindowPosition(xml.getIntValue("x"), xml.getIntValue("y"));
        prevWindow->setWindowShape(xml.getIntValue("width"), xml.getIntValue("height"));
        xml.setToParent();
    }

    if(xml.exists("Main")){
        xml.setTo("Main");
        datGui->setPosition(xml.getIntValue("x"), xml.getIntValue("y"));
        xml.setToParent();
    }

    for ( int i = 0; i < datGuis.size() ; i++){
        if(xml.exists("Gui_" + ofToString(i))){
            xml.setTo("Gui_" + ofToString(i));
            datGuis[i]->setPosition(xml.getIntValue("x"), xml.getIntValue("y"));
            xml.setToParent();
        }
    }
    
    ofLog()<<"Load Arrangement_" + bankSelect->getSelected()->getName();
     */
}

void parametersControl::loadBank(){
    string bankName = bankSelect->getSelected()->getName();
    
    ofDirectory dir;
    vector<pair<int, string>> presets;
    dir.open("Presets/" + bankName);
    if(!dir.exists())
        dir.createDirectory("Presets/" + bankName);
    dir.sort();
    int numPresets = dir.listDir();
    ofLog() << "Dir size: " << ofToString(numPresets);
    for ( int i = 0 ; i < numPresets; i++)
        presets.push_back(pair<int, string>(ofToInt(ofSplitString(dir.getName(i), "|")[0]), ofSplitString(dir.getName(i), ".")[0]));
    
    std::sort(presets.begin(), presets.end(), [](pair<int, string> &left, pair<int, string> &right) {
        return left.first< right.first;
    });
    
    presetsList->clear();
    
    for(auto preset : presets)
        presetsList->add(preset.second);
}

void parametersControl::changePresetLabelHighliht(ofxDatGuiButton *presetToHighlight){
    if(oldPresetButton != nullptr) oldPresetButton->setTheme(mainGuiTheme);
    presetToHighlight->setLabelColor(ofColor::red);
    oldPresetButton = presetToHighlight;
}


void parametersControl::setGlobalBPM(float bpm){
    datGui->getSlider("Global BPM")->setValue(bpm);
    for(auto groupParam : parameterGroups){
        if(ofStringTimesInString(groupParam->getName(), "phasor") != 0 && groupParam->getFloat("BPM").getName() == "BPM")
            groupParam->getFloat("BPM") = bpm;
    }
}

bool parametersControl::loadPresetsSequence(){
    // this is our buffer to stroe the text data
    ofBuffer buffer = ofBufferFromFile("PresetsSequencing.txt");
    
    if(buffer.size()) {
        presetNumbersAndBanks.clear();
        presetsTime.clear();
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            string line = *it;
            
            // make sure its not a empty line
            if(!line.empty()){
                vector<string> splitedStr = ofSplitString(line,"-");
                pair<int, string> tempPair;
                tempPair.first = ofToInt(splitedStr[0]);
                tempPair.second = splitedStr[1];
                presetNumbersAndBanks.push_back(tempPair);
                presetsTime.push_back(ofToInt(splitedStr[2]));
            }
        }
    }
    else{
        return false;
    }
    
    if(presetNumbersAndBanks.size() != presetsTime.size()){
        presetNumbersAndBanks.clear();
        presetsTime.clear();
        return false;
    }
    
    randomPresetsArrange.clear();
    for(int i = 0 ; i < presetNumbersAndBanks.size(); i++)
        randomPresetsArrange.push_back(i);
    
    presetChangeCounter = 0;
    presetChangedTimeStamp = ofGetElapsedTimef();
    
    mt19937 g(static_cast<uint32_t>(time(0)));
    shuffle(randomPresetsArrange.begin(), randomPresetsArrange.end(), g);
    
    return true;
}

void parametersControl::savePreset(string presetName, string bank){
    //xml.load("Preset_"+ofToString(presetNum)+".xml");
    xml.clear();
    
    //Root Element
    xml.addChild("PRESET");
    
    // now we set our current element so we're on the right
    // element, so when add new nodes, they are still inside
    //the root element
    xml.setTo("PRESET");
    
    xml.addChild("DYNAMIC_NODES");
    xml.setTo("DYNAMIC_NODES");
    
    vector<ofPoint> toCreatePhasors;
    vector<ofPoint> toCreateOscillators;
    vector<ofPoint> toCreateOscillatorBanks;
    int toCreateWaveScope = 0;
    ofPoint toCreateColorApplier = ofPoint(-1, -1);
    
    for(int i = 0; i < parameterGroups.size() ; i++){
        string moduleName = ofSplitString(parameterGroups[i]->getName(), " ")[0];
        if(moduleName == "phasor"){
            toCreatePhasors.push_back(datGuis[i]->getPosition());
        }
        else if(moduleName == "oscillator"){
            toCreateOscillators.push_back(datGuis[i]->getPosition());
        }
        else if(moduleName == "oscillatorBank"){
            toCreateOscillatorBanks.push_back(datGuis[i]->getPosition());
            toCreateOscillatorBanks.back().z = parameterGroups[i]->getFloat("Num Waves").getMax();
        }
        else if(moduleName == "waveScope"){
            while(datGuis[i]->getLabel("Osc Bank "+ofToString(toCreateWaveScope))->getName() != "X")
                toCreateWaveScope++;
        }
        else if(moduleName == "colorApplier"){
            toCreateColorApplier = datGuis[i]->getPosition();
        }
    }
    
    //save Phasors
    for(int i = 0; i < toCreatePhasors.size() ; i++){
        xml.addValue("phasor_"+ofToString(i), ofToString(toCreatePhasors[i].x)+"_"+ofToString(toCreatePhasors[i].y));
    }
    
    //save Oscillators
    for(int i = 0; i < toCreateOscillators.size() ; i++){
        xml.addValue("oscillator_"+ofToString(i), ofToString(toCreateOscillators[i].x)+"_"+ofToString(toCreateOscillators[i].y));
    }
    
    //save OscillatorBanks
    for(int i = 0; i < toCreateOscillatorBanks.size() ; i++){
        xml.addValue("oscillatorBank_"+ofToString(i), ofToString(toCreateOscillatorBanks[i].x)+"_"+ofToString(toCreateOscillatorBanks[i].y)+"_"+ofToString(toCreateOscillatorBanks[i].z));
    }
    xml.addValue("waveScope", ofToString(toCreateWaveScope));
    xml.addValue("colorApplier", ofToString(toCreateColorApplier.x)+"_"+ofToString(toCreateColorApplier.y));
    
    
    xml.setToParent();
    
    
    
    //Iterate for all the parameterGroups
    for (auto groupParam : parameterGroups){
        //set XML structure to parameterGroup
        string noSpacesGroupName = groupParam->getName();
        if((ofSplitString(noSpacesGroupName, " ")[0] != "waveScope" && ofSplitString(noSpacesGroupName, " ")[0] != "senderManager")){
            ofStringReplace(noSpacesGroupName, " ", "_");
            xml.addChild(noSpacesGroupName);
            xml.setTo(noSpacesGroupName);
            
            //Iterate for all parameters in parametergroup and look for the type of the parameter
            for (int j = 0; j < groupParam->size() ; j++){
                ofAbstractParameter &absParam = groupParam->get(j);
                if(absParam.type() == typeid(ofParameter<float>).name()){
                    //Cast it
                    ofParameter<float> castedParam = absParam.cast<float>();
                    
                    //Replace blank spaces with underscore
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    
                    //add the value of that parameter into xml
                    xml.addValue(noSpaces, ofMap(castedParam.get(), castedParam.getMin(), castedParam.getMax(), 0, 1, true));
                }
                else if(absParam.type() == typeid(ofParameter<int>).name()){
                    ofParameter<int> castedParam = absParam.cast<int>();
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    xml.addValue(noSpaces, ofMap(castedParam.get(), castedParam.getMin(), castedParam.getMax(), 0, 1, true));
                }
                else if(absParam.type() == typeid(ofParameter<bool>).name()){
                    ofParameter<bool> castedParam = absParam.cast<bool>();
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    xml.addValue(noSpaces, castedParam.get());
                }
                else if(absParam.type() == typeid(ofParameter<string>).name()){
                    ofParameter<string> castedParam = absParam.cast<string>();
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    xml.addValue(noSpaces, castedParam.get());
                }
                else if(absParam.type() == typeid(ofParameter<ofColor>).name()){
                    ofParameter<ofColor> castedParam = absParam.cast<ofColor>();
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    xml.addValue(noSpaces, ofToString((int)castedParam.get().r) + "-" + ofToString((int)castedParam.get().g) + "-" + ofToString((int)castedParam.get().b));
                }
                else if(absParam.type() == typeid(ofParameterGroup).name()){
                    string noSpaces = groupParam->getGroup(j).getName();
                    ofStringReplace(noSpaces, " ", "_");
                    xml.addValue(noSpaces, groupParam->getGroup(j).getInt(1).get());
                }
            }
            xml.setToParent();
        }
    }
    
    
    xml.addChild("CONNECTIONS");
    xml.setTo("CONNECTIONS");
    for(int i = 0; i < connections.size(); i++){
        vector<string> groupNames = connections[i]->getSourceParameter()->getGroupHierarchyNames();
        xml.addValue("connection_" + ofToString(i) + "_source", groupNames[0] + "-|-" + groupNames[1]);
        
        groupNames = connections[i]->getSinkParameter()->getGroupHierarchyNames();
        xml.addValue("connection_" + ofToString(i) + "_sink", groupNames[0] + "-|-" + groupNames[1]);
        
        float min = connections[i]->getMin();
        float max = connections[i]->getMax();
        xml.addValue("connection_" + ofToString(i) + "_minmax", ofToString(min) + "-|-" + ofToString(max));
    }
    
    ofLog() <<"Save " << presetName;
    xml.save("Presets/" + bank + "/" + presetName + ".xml");
    
    ofxOscMessage m;
    m.setAddress("presetSave");
    m.addStringArg(presetName);
    m.addStringArg(bank);
    oscSender.sendMessage(m);
}

void parametersControl::loadPreset(string presetName, string bank){
    //Test if there is no problem with the file
    
    bool isColorLoaded = false;
    
    if(!xml.load("Presets/" + bank + "/" + presetName + ".xml"))
        return;
    
    
    //Get the dinamic modules that have to be created or updated
    vector<ofPoint> toCreatePhasors;
    vector<ofPoint> toCreateOscillators;
    vector<ofPoint> toCreateOscillatorBanks;
    int toCreateWaveScope = 0;
    ofPoint waveScopePosition = ofPoint(-1, -1);
    ofPoint toCreateColorApplier = ofPoint(-1, -1);
    
    if(xml.exists("DYNAMIC_NODES")){
        xml.setTo("DYNAMIC_NODES");
        //getPhasors;
        int i = 0;
        while(xml.getValue("phasor_"+ofToString(i)) != ""){
            vector<string> strPoint = ofSplitString(xml.getValue("phasor_"+ofToString(i)), "_");
            toCreatePhasors.push_back(ofPoint(ofToInt(strPoint[0]), ofToInt(strPoint[1])));
            i++;
        }
        //getOscillators;
        i = 0;
        while(xml.getValue("oscillator_"+ofToString(i)) != ""){
            vector<string> strPoint = ofSplitString(xml.getValue("oscillator_"+ofToString(i)), "_");
            toCreateOscillators.push_back(ofPoint(ofToInt(strPoint[0]), ofToInt(strPoint[1])));
            i++;
        }
        //getOscillatorBanks;
        i = 0;
        while(xml.getValue("oscillatorBank_"+ofToString(i)) != ""){
            vector<string> strPoint = ofSplitString(xml.getValue("oscillatorBank_"+ofToString(i)), "_");
            if(strPoint.size() == 2)
                toCreateOscillatorBanks.push_back(ofPoint(ofToInt(strPoint[0]), ofToInt(strPoint[1])));
            else if(strPoint.size() == 3)
                toCreateOscillatorBanks.push_back(ofPoint(ofToInt(strPoint[0]), ofToInt(strPoint[1]), ofToInt(strPoint[2])));
            i++;
        }
        toCreateWaveScope = ofToInt(xml.getValue("waveScope"));
        vector<string> strPoint = ofSplitString(xml.getValue("colorApplier"), "_");
        toCreateColorApplier = ofPoint(ofToInt(strPoint[0]), ofToInt(strPoint[1]));
    }
    
    xml.setToParent();
    
    bool newModulesCreated = false;
    
    connections.clear();
    
    //Iterate for all the parameterGroups
    for (int i = 0; i < parameterGroups.size();){
        auto &groupParam = parameterGroups[i];
        bool isDestroyed = false;
        
        //Move, edit or destroy nodes
        string moduleName = ofSplitString(groupParam->getName(), " ")[0];
        if(!newModulesCreated){
            if(moduleName == "phasor"){
                int id = ofToInt(ofSplitString(groupParam->getName(), " ")[1]);
                if(toCreatePhasors.size() < id){
                    //destroy node
                    isDestroyed = true;
                }else{
                    //move to new position
                    datGuis[i]->setPosition(toCreatePhasors[id-1].x, toCreatePhasors[id-1].y);
                    toCreatePhasors[id-1] = ofPoint(-1, -1);
                }
            }
            else if(moduleName == "oscillator"){
                int id = ofToInt(ofSplitString(groupParam->getName(), " ")[1]);
                if(toCreateOscillators.size() < id){
                    //destroy node
                    isDestroyed = true;
                }else{
                    //move to new position
                    datGuis[i]->setPosition(toCreateOscillators[id-1].x, toCreateOscillators[id-1].y);
                    toCreateOscillators[id-1] = ofPoint(-1, -1);
                }
            }
            else if(moduleName == "oscillatorBank"){
                int id = ofToInt(ofSplitString(groupParam->getName(), " ")[1]);
                if(toCreateOscillatorBanks.size() < id){
                    //destroy node
                    isDestroyed = true;
                }else if(toCreateOscillatorBanks[id-1].z !=0 && toCreateOscillatorBanks[id-1].z != groupParam->getFloat("Num Waves").getMax()){
                    //destroy node
                    isDestroyed = true;
                }else{
                    //move to new position
                    datGuis[i]->setPosition(toCreateOscillatorBanks[id-1].x, toCreateOscillatorBanks[id-1].y);
                    toCreateOscillatorBanks[id-1] = ofPoint(-1, -1);
                }
            }
            else if(moduleName == "waveScope"){
                int actualWaveScopeSize = 0;
                while(datGuis[i]->getLabel("Osc Bank "+ofToString(actualWaveScopeSize))->getName() != "X")
                    actualWaveScopeSize++;
                if(actualWaveScopeSize != toCreateWaveScope)
                    isDestroyed = true;
                else
                    toCreateWaveScope = 0;
                
                waveScopePosition = datGuis[i]->getPosition();
            }
            else if(moduleName == "colorApplier"){
                isDestroyed = toCreateColorApplier == ofPoint(-1, -1) ? true : false;
                toCreateColorApplier = ofPoint(-1, -1);
            }
        }
        
        //if we have to destroy the module we do it
        if(isDestroyed){
            destroyModuleAndConnections(i);
        }
        else{
            if(moduleName != "waveScope" && moduleName != "colorApplier"){
                //Put xml in the place of the parametergroup
                string noSpacesGroupName = groupParam->getName();
                ofStringReplace(noSpacesGroupName, " ", "_");
                if(xml.exists(noSpacesGroupName)){
                    xml.setTo(noSpacesGroupName);
                    
                    //Iterate for all parameters in parametergroup and look for the type of the parameter
                    for (int j = 0; j < groupParam->size() ; j++){
                        ofAbstractParameter &absParam = groupParam->get(j);
                        if(absParam.type() == typeid(ofParameter<float>).name()){
                            //Cast it
                            ofParameter<float> castedParam = absParam.cast<float>();
                            
                            //Replace blank spaces with underscore
                            string noSpaces = castedParam.getName();
                            ofStringReplace(noSpaces, " ", "_");
                            
                            //get the value of that parameter if it's not bpm, we don't want to lose sync
                            if(castedParam.getName() != "BPM" && xml.exists(noSpaces)  && !ofStringTimesInString(groupParam->getName(), "master"))
                                castedParam = ofMap(xml.getFloatValue(noSpaces), 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                        }
                        else if(absParam.type() == typeid(ofParameter<int>).name()){
                            ofParameter<int> castedParam = absParam.cast<int>();
                            string noSpaces = castedParam.getName();
                            ofStringReplace(noSpaces, " ", "_");
                            if(xml.exists(noSpaces) && !ofStringTimesInString(groupParam->getName(), "master"))
                                castedParam = round(ofMap(xml.getFloatValue(noSpaces), 0, 1, castedParam.getMin(), castedParam.getMax(), true));
                        }
                        else if(absParam.type() == typeid(ofParameter<bool>).name()){
                            ofParameter<bool> castedParam = absParam.cast<bool>();
                            string noSpaces = castedParam.getName();
                            ofStringReplace(noSpaces, " ", "_");
                            if(xml.exists(noSpaces) && !ofStringTimesInString(groupParam->getName(), "master"))
                                castedParam = xml.getValue(noSpaces, castedParam.get());
                        }
                        else if(absParam.type() == typeid(ofParameter<string>).name()){
                            ofParameter<string> castedParam = absParam.cast<string>();
                            string noSpaces = castedParam.getName();
                            ofStringReplace(noSpaces, " ", "_");
                            if(xml.exists(noSpaces) && !ofStringTimesInString(groupParam->getName(), "master"))
                                castedParam.set(xml.getValue(noSpaces, castedParam.get()));
                        }
                        else if(absParam.type() == typeid(ofParameter<ofColor>).name()){
                            ofParameter<ofColor> castedParam = absParam.cast<ofColor>();
                            string noSpaces = castedParam.getName();
                            ofStringReplace(noSpaces, " ", "_");
                            if(xml.exists(noSpaces)){
                                vector<string> colors = ofSplitString(xml.getValue(noSpaces), "-");
                                ofColor_<unsigned char> color = ofColor(ofToInt(colors[0]), ofToInt(colors[1]), ofToInt(colors[2]));
                                //TODO: hack, arreglar
                                groupParam->getInt("R Channel") = ofToInt(colors[0]);
                                groupParam->getInt("G Channel") = ofToInt(colors[1]);
                                groupParam->getInt("B Channel") = ofToInt(colors[2]);
                                isColorLoaded = true;
                            }
                        }
                        else if(absParam.type() == typeid(ofParameterGroup).name()){
                            string noSpaces = groupParam->getGroup(j).getName();
                            ofStringReplace(noSpaces, " ", "_");
                            if(xml.exists(noSpaces) && !ofStringTimesInString(groupParam->getName(), "master"))
                                groupParam->getGroup(j).getInt(1) = xml.getValue(noSpaces, groupParam->getGroup(j).getInt(1));
                        }
                    }
                    //Jump one label before in xml structure
                    xml.setToParent();
                    //reset Phasor
                    if(ofSplitString(groupParam->getName(), " ")[0] == "phasor")
                        groupParam->getBool("Reset Phase") = false;
                }
            }
            
            //increase if it is not destroyed
            i++;
            
        }
        if(i == parameterGroups.size() && !newModulesCreated){
            //create Phasors
            for(int i = 0; i < toCreatePhasors.size() ; i++){
                if(toCreatePhasors[i] != ofPoint(-1, -1)){
                    pair<moduleType, ofPoint> pairToSend;
                    pairToSend.first = phasor_module;
                    pairToSend.second = toCreatePhasors[i];
                    ofNotifyEvent(createNewModule, pairToSend, this);
                }
            }
            toCreatePhasors.clear();
            
            //create Oscillators
            for(int i = 0; i < toCreateOscillators.size() ; i++){
                if(toCreateOscillators[i] != ofPoint(-1, -1)){
                    pair<moduleType, ofPoint> pairToSend;
                    pairToSend.first = oscillator_module;
                    pairToSend.second = toCreateOscillators[i];
                    ofNotifyEvent(createNewModule, pairToSend, this);
                }
            }
            
            //create OscillatorBanks
            for(int i = 0; i < toCreateOscillatorBanks.size() ; i++){
                if(toCreateOscillatorBanks[i] != ofPoint(-1, -1)){
                    pair<moduleType, ofPoint> pairToSend;
                    pairToSend.first = oscillatorBank_module;
                    pairToSend.second = toCreateOscillatorBanks[i];
                    ofNotifyEvent(createNewModule, pairToSend, this);
                }
            }
            
            if(toCreateWaveScope != 0){
                pair<moduleType, ofPoint> pairToSend;
                pairToSend.first = waveScope_module;
                pairToSend.second =  waveScopePosition;
                pairToSend.second.z = toCreateWaveScope;
                ofNotifyEvent(createNewModule, pairToSend, this);
            }
            
            if(toCreateColorApplier != ofPoint(-1, -1)){
                pair<moduleType, ofPoint> pairToSend;
                pairToSend.first = colorApplier_module;
                pairToSend.second = toCreateColorApplier;
                ofNotifyEvent(createNewModule, pairToSend, this);
            }
            newModulesCreated = true;
        }
    }
    
//    xml.setToParent();
    
//    connections.clear();
    
    if(xml.exists("CONNECTIONS")){
        xml.setTo("CONNECTIONS");
        int i = 0;
        while(xml.getValue("connection_" + ofToString(i) + "_source") != ""){
            vector<string> sourceInfo = ofSplitString(xml.getValue("connection_" + ofToString(i) + "_source"), "-|-");
            vector<string> sinkInfo = ofSplitString(xml.getValue("connection_" + ofToString(i) + "_sink"), "-|-");
            vector<string> minMaxInfo = ofSplitString(xml.getValue("connection_" + ofToString(i) + "_minmax"), "-|-");
            if(minMaxInfo.size() != 2) minMaxInfo = {"0", "1"};
            ofStringReplace(sourceInfo[0], "_", " ");
            ofStringReplace(sinkInfo[0], "_", " ");
            ofStringReplace(sourceInfo[1], "_", " ");
            ofStringReplace(sinkInfo[1], "_", " ");
            for(int j = 0; j < parameterGroups.size(); j++){
                if(parameterGroups[j]->getName() == sourceInfo[0]){
                    connections.push_back(make_shared<nodeConnection>(datGuis[j]->getComponent(sourceInfo[1]), datGuis[j], &parameterGroups[j]->get(sourceInfo[1])));
                    ofAddListener(connections.back()->destroyEvent, this, &parametersControl::destroyedConnection);
                }
            }
            for(int j = 0; j < parameterGroups.size(); j++){
                if(parameterGroups[j]->getName() == sinkInfo[0]){
                    connections.back()->connectTo(datGuis[j]->getComponent(sinkInfo[1]), datGuis[j], &parameterGroups[j]->get(sinkInfo[1]));
                    connections.back()->setMin(ofToFloat(minMaxInfo[0]));
                    connections.back()->setMax(ofToFloat(minMaxInfo[1]));
                }
            }
            i++;
        }
    }
    
    
    for(auto paramGroup : parameterGroups){
        string moduleName = ofSplitString(paramGroup->getName(), " ")[0];
        if((moduleName == "oscillatorBank") ||( moduleName == "oscillator")){
            //If there is a rand wave that does not have a phasor atach we need to trigger the random creation, that means sending a value less that the value that actually is there, becouse we have previously assigned a value, we need to send a value less than the actual value for the oscillators to compute a new random.
            if(paramGroup->getGroup("Wave Select").getInt(1) == 6)
                paramGroup->getFloat("Phasor In") = paramGroup->getFloat("Phasor In") - 0.01;
            else
                paramGroup->getFloat("Phasor In") = paramGroup->getFloat("Phasor In");
        }
    }
    
    
    ofLog()<<"Load " << presetName;
    
    ofxOscMessage m;
    m.setAddress("presetLoad");
    m.addStringArg(presetName);
    m.addStringArg(bank);
    oscSender.sendMessage(m);
    
    for(auto groupParam : parameterGroups){
        if(ofStringTimesInString(groupParam->getName(), "phasor") != 0)
            groupParam->getBool("Reset_Phase") = 0;
    }
}

void parametersControl::loadPresetWithFade(int presetNum, string bank){
//    ofXml xml2;
//    if(xml2.load("Preset_"+ofToString(presetNum)+"_"+bank+".xml")){
//        presetToLoad = presetNum;
//        bankToLoad = bank;
//        ofxOscMessage m;
//        m.setAddress("bankSelect");
//        m.addStringArg(bank);
//        oscSender.sendMessage(m);
//        Tweenzor::add((float*)&parameterGroups[parameterGroups.size()-1]->getFloat("Master Fader").get(), parameterGroups[parameterGroups.size()-1]->getFloat("Master Fader").get(), 0.0f, 0.0f, fadeTime);
//        Tweenzor::addCompleteListener(Tweenzor::getTween((float*)&parameterGroups[parameterGroups.size()-1]->getFloat("Master Fader").get()), this, &parametersControl::loadPresetWhenFadeOutCompletes);
//        isFading = true;
//    }
}

void parametersControl::loadPresetWhenFadeOutCompletes(float *arg){
//    if(*arg == 0){
//        loadPreset(presetToLoad, bankToLoad);
//        Tweenzor::add((float*)&parameterGroups[parameterGroups.size()-1]->getFloat("Master Fader").get(), 0.0f, 1.0f, 0.0f, fadeTime);
//         Tweenzor::addCompleteListener(Tweenzor::getTween((float*)&parameterGroups[parameterGroups.size()-1]->getFloat("Master Fader").get()), this, &parametersControl::loadPresetWhenFadeOutCompletes);
//    }
//    else if(*arg == 1.0f){
//        isFading = false;
//    }
}

void parametersControl::onGuiButtonEvent(ofxDatGuiButtonEvent e){
    if(datGui->getButton(e.target->getName())){
        if(ofStringTimesInString(e.target->getName(), "Global")){
            string nameNoGlobal = e.target->getName();
            ofStringReplace(nameNoGlobal, "Global ", "");
            for(auto groupParam : parameterGroups){
                if(ofStringTimesInString(groupParam->getName(), "phasor") != 0)
                    groupParam->getBool(nameNoGlobal) = 0;
            }
        }else if(e.target->getName() == "Reload Sequence"){
            e.target->setBackgroundColor(loadPresetsSequence() ? ofColor(0,50,0) : ofColor(50,0,0));
        }
    }
}
void parametersControl::onGuiToggleEvent(ofxDatGuiToggleEvent e){
    for (int i=0; i < datGuis.size() ; i++){
        if(datGuis[i]->getToggle(e.target->getName()) == e.target)
            parameterGroups[i]->getBool(e.target->getName()) = e.target->getChecked();
    }
    if(e.target->getName() == "Automatic Preset"){
        autoPreset = e.checked;
        presetChangedTimeStamp = ofGetElapsedTimef();
        srand(time(0));
        random_shuffle(randomPresetsArrange.begin(), randomPresetsArrange.end());
    }else if(e.target->getName() == "BPM Sync"){
        if(e.checked)
            ofAddListener(beatTracker->bpmChanged, this, &parametersControl::bpmChangedListener);
        else
            ofRemoveListener(beatTracker->bpmChanged, this, &parametersControl::bpmChangedListener);
        
    }
}

void parametersControl::onGuiDropdownEvent(ofxDatGuiDropdownEvent e){
    if(e.target == bankSelect){
        oldPresetButton = nullptr;
        if(e.child == bankSelect->getNumOptions()-1){
            bankSelect->addOption("Bank " + ofGetTimestampString(), bankSelect->getNumOptions()-1);
            bankSelect->select(bankSelect->getNumOptions()-2);
        }
        loadBank();
    }
    else{
        for (int i=0; i < datGuis.size() ; i++){
            if(datGuis[i]->getDropdown(e.target->getName()) == e.target){
                parameterGroups[i]->getGroup(e.target->getName()).getInt(1) = e.child;
                //            if(datGuis[i]->getHeight() > ofGetHeight())
                //                ofSetWindowShape(ofGetWidth(), datGuis[i]->getHeight());
            }
        }
    }
}


void parametersControl::onGuiSliderEvent(ofxDatGuiSliderEvent e){
    if(datGui->getSlider(e.target->getName())->getName() == e.target->getName()){
        string nameNoGlobal = e.target->getName();
        ofStringReplace(nameNoGlobal, "Global ", "");
        for(auto groupParam : parameterGroups){
            if(ofStringTimesInString(groupParam->getName(), "phasor") != 0 && groupParam->getFloat(nameNoGlobal).getName() == nameNoGlobal)
                groupParam->getFloat(nameNoGlobal) = e.value;
        }
    }
    if(e.target->getName() == "Beats Period")
        periodTime = e.value / datGui->getSlider("Global BPM")->getValue() * 60.;
}

void parametersControl::onGuiTextInputEvent(ofxDatGuiTextInputEvent e){
    for (int i=0; i < datGuis.size() ; i++){
        if(datGuis[i]->getTextInput(e.target->getName()) == e.target)
            parameterGroups[i]->getString(e.target->getName()) = e.text;
    }
}

void parametersControl::onGuiColorPickerEvent(ofxDatGuiColorPickerEvent e){
    for (int i=0; i < datGuis.size() ; i++){
        if(datGuis[i]->getColorPicker(e.target->getName()) == e.target)
            parameterGroups[i]->getColor(e.target->getName()) = e.color;
    }
}

void parametersControl::onGuiRightClickEvent(ofxDatGuiRightClickEvent e){
    if(e.down == 1){
        for (int i=0; i < datGuis.size() ; i++){
            if(datGuis[i]->getComponent(e.target->getType(), e.target->getName()) == e.target){
                ofAbstractParameter &parameter = parameterGroups[i]->get(e.target->getName());
                bool foundParameter = false;
                for(int j = 0 ; j < connections.size() ; j++){
                    if(connections[j]->getSinkParameter() == &parameter){
                        swap(connections[j], connections.back());
                        connections.back()->disconnect();
                        foundParameter = true;
                        break;
                    }
                }
                if(!foundParameter){
                    connections.push_back(make_shared<nodeConnection>(e.target, datGuis[i], &parameter));
                    ofAddListener(connections.back()->destroyEvent, this, &parametersControl::destroyedConnection);
                }
            }
        }
    }else if(connections.size() > 0){
        for (int i=0; i < datGuis.size() ; i++){
            if(datGuis[i]->getComponent(e.target->getType(), e.target->getName()) == e.target
               && !connections.back()->closedLine
               && connections.back()->getSourceParameter() != &parameterGroups[i]->get(e.target->getName())){
                connections.back()->connectTo(e.target, datGuis[i], &parameterGroups[i]->get(e.target->getName()));
                for(int i = 0; i<connections.size()-1 ; i++){
                    if(connections.back()->getSinkParameter() == connections[i]->getSourceParameter() ||
                       connections.back()->getSinkParameter() == connections[i]->getSinkParameter() ||
                       connections.back()->getSourceParameter() == connections[i]->getSinkParameter())
                        connections.erase(connections.begin()+i);
                }
            }
        }
    }
}

void parametersControl::onGuiScrollViewEvent(ofxDatGuiScrollViewEvent e){
    if(ofGetKeyPressed(OF_KEY_SHIFT)){
        changePresetLabelHighliht(e.target);
        savePreset(e.target->getName(), bankSelect->getSelected()->getName());
    }else{
        changePresetLabelHighliht(e.target);
        loadPreset(e.target->getName(), bankSelect->getSelected()->getName());
        if(autoPreset)
            presetChangedTimeStamp = ofGetElapsedTimef();
    }
}

void parametersControl::newModuleListener(ofxDatGuiDropdownEvent e){
    pair<moduleType, ofPoint> pairToSend;
    pairToSend.first = static_cast<moduleType>(e.child + 1);
    ofVec4f transformedPos = popUpMenu->getPosition();
    transformedPos -= transformMatrix.getTranslation();
    transformedPos = transformMatrix.getInverse().postMult(transformedPos);
    pairToSend.second = transformedPos;
    if(pairToSend.first == waveScope_module){
        for(int i = 0; i < datGuis.size(); i++)
            if(datGuis[i]->getHeader()->getName() == "waveScope") destroyModuleAndConnections(i);
    }
    ofNotifyEvent(createNewModule, pairToSend, this);
    
    popUpMenu->setVisible(false);
    popUpMenu->setPosition(-1, -1);
    ofxDatGuiDropdown* drop = popUpMenu->getDropdown("Choose module");
    drop->setLabel("Choose module");
}

void parametersControl::newPresetListener(ofxDatGuiTextInputEvent e){
    if(e.text != ""){
        string newPresetName;
        if(presetsList->getNumItems() != 0){
            string lastPreset = presetsList->get(presetsList->getNumItems()-1)->getName();
            newPresetName = ofToString(ofToInt(ofSplitString(lastPreset, "|")[0])+1) + "|" + e.text;
        }else
            newPresetName = "1|" + e.text;
        
        presetsList->add(newPresetName);
        changePresetLabelHighliht(presetsList->get(presetsList->getNumItems()-1));
        savePreset(newPresetName, bankSelect->getSelected()->getName());
        
        e.text = "";
    }
}



#pragma mark --Keyboard and mouse Listenerrs--

void parametersControl::keyPressed(ofKeyEventArgs &e){
    if(e.key == OF_KEY_COMMAND){
        commandPressed = true;
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), arrowListCursor);
    }
    else if(e.key == OF_KEY_ESC){
        popUpMenu->setVisible(false);
        for(auto connection : connections){
            connection->toggleGui(false);
        }
    }
    else if(e.key == ' '  && !canvasDragging){
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), openedHandCursor);
        canvasDragging = true;
    }
        
}

void parametersControl::keyReleased(ofKeyEventArgs &e){
    if(e.key == OF_KEY_COMMAND){
        commandPressed = false;
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), arrowCursor);
    }
    else if(e.key == ' '){
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), arrowCursor);
        canvasDragging = false;
    }
}

void parametersControl::mouseMoved(ofMouseEventArgs &e){
    
}

void parametersControl::mouseDragged(ofMouseEventArgs &e){
    ofVec4f transformedPos = e;
    transformedPos -= transformMatrix.getTranslation();
    transformedPos = transformMatrix.getInverse().postMult(transformedPos);
    if(e.button == 2 && connections.size() > 0){
        if(!connections.back()->closedLine){
            connections.back()->moveLine(transformedPos);
        }
    }else if(e.button == 0 && canvasDragging){
        transformMatrix.translate(e - dragCanvasInitialPoint);
        for(auto &gui : datGuis)
            gui->setTransformMatrix(transformMatrix);//gui->setTransformMatrix(ofMatrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
        dragCanvasInitialPoint = e;
    }
}

void parametersControl::mousePressed(ofMouseEventArgs &e){
    ofVec4f transformedPos = e;
    transformedPos -= transformMatrix.getTranslation();
    transformedPos = transformMatrix.getInverse().postMult(transformedPos);
    if(commandPressed){
       if(e.button == 0){
        popUpMenu->setPosition(e.x, e.y);
        popUpMenu->setVisible(true);
        popUpMenu->getDropdown("Choose module")->expand();
       }
       else if(e.button == 1){
           transformMatrix.translate(-transformedPos * (1-transformMatrix.getScale()));
           transformMatrix = ofMatrix4x4::newTranslationMatrix(transformMatrix.getTranslation());
           for(auto &gui : datGuis)
               gui->setTransformMatrix(transformMatrix);//gui->setTransformMatrix(ofMatrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
       }
    }if(canvasDragging){
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), closedHandCursor);
        dragCanvasInitialPoint = e;
    }if(ofGetKeyPressed(OF_KEY_CONTROL)){
        bool cablePressed = false;
        for(auto connection : connections){
            if(connection->hitTest(transformedPos) && !cablePressed){
                connection->toggleGui(true, e);
                cablePressed = true;
            }
            else
                connection->toggleGui(false);
        }
    }if(ofGetKeyPressed('r')){
        for(int i = 0; i<datGuis.size(); i++){
            if(datGuis[i]->hitTest(e)
               && datGuis[i]->getHeader()->getName() != "oscillatorGroup"
               && datGuis[i]->getHeader()->getName() != "senderManager"){
                destroyModuleAndConnections(i);
            }
        }
    }
}

void parametersControl::mouseReleased(ofMouseEventArgs &e){
    if(e.button == 2 && connections.size() > 0){
        if(!connections.back()->closedLine)
            connections.pop_back();
    }
    if(canvasDragging){
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), openedHandCursor);
    }
}

void parametersControl::mouseScrolled(ofMouseEventArgs &e){
    ofVec4f transformedPos = e;
    transformedPos -= transformMatrix.getTranslation();
    transformedPos = transformMatrix.getInverse().postMult(transformedPos);
    if(commandPressed){
        transformMatrix.translate(transformedPos * transformMatrix.getScale()*e.scrollY/100);
        transformMatrix.glScale(ofVec3f(1-(e.scrollY/100), 1-(e.scrollY/100), 1));
        for(auto &gui : datGuis)
            gui->setTransformMatrix(transformMatrix);//gui->setTransformMatrix(ofMatrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
        if(e.scrollY < 0)
            glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), zoomInCursor);
        else
            glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), zoomOutCursor);
    }
}

void parametersControl::mouseEntered(ofMouseEventArgs &e){
    
}

void parametersControl::mouseExited(ofMouseEventArgs &e){
    
}


#pragma mark --parameter listenerrs--
void parametersControl::bpmChangedListener(float &bpm){
    newBpm = bpm;
}

void parametersControl::listenerFunction(ofAbstractParameter& e){
    int position = 0;
    vector<shared_ptr<nodeConnection>> validConnections;
    
    if(e.getName() == "Phasor Monitor"){
        //return;
    }
    
    auto parent = e.getGroupHierarchyNames()[0];
    int parentIndex = 0;
    ofStringReplace(parent, "_", " ");
    for(int i = 0; i<parameterGroups.size(); i++){
        if(parameterGroups[i]->getName() == parent){
            parentIndex = i;
            break;
        }
    }
    
    //ParameterBinding
    for(auto &connection : connections){
        if(connection != nullptr){
            ofAbstractParameter* possibleSource = connection->getSourceParameter();
            if(possibleSource == &e && connection->closedLine){
                validConnections.push_back(connection);
            }
        }
    }
    
    //Midi and to gui
    float normalizedVal = 0;
    if(e.type() == typeid(ofParameter<float>).name()){
        ofParameter<float> castedParam = e.cast<float>();
        normalizedVal = ofMap(castedParam, castedParam.getMin(), castedParam.getMax(), 0, 1);
        if(castedParam.getName() == "BPM")
            periodTime = presetChangeBeatsPeriod / castedParam * 60.;
        
        for(auto validConnection : validConnections)
            setFromNormalizedValue(validConnection->getSinkParameter(), ofMap(normalizedVal, 0, 1, validConnection->getMin(), validConnection->getMax()));
        
    }
    else if(e.type() == typeid(ofParameter<int>).name()){
        ofParameter<int> castedParam = e.cast<int>();
        normalizedVal = ofMap(castedParam, castedParam.getMin(), castedParam.getMax(), 0, 1);
        
        if(ofStringTimesInString(castedParam.getName(), "Select") == 1){
            datGuis[parentIndex]->getDropdown(castedParam.getName())->select(castedParam);
        }
        
        for(auto validConnection : validConnections)
            setFromNormalizedValue(validConnection->getSinkParameter(), ofMap(normalizedVal, 0, 1, validConnection->getMin(), validConnection->getMax()));
    }
    else if(e.type() == typeid(ofParameter<bool>).name()){
        ofParameter<bool> castedParam = e.cast<bool>();
        normalizedVal = castedParam ? 1 : 0;
        
        //Update to datGuis
        datGuis[parentIndex]->getToggle(castedParam.getName())->setChecked(normalizedVal);
        
        for(auto validConnection : validConnections)
            setFromNormalizedValue(validConnection->getSinkParameter(), ofMap(normalizedVal, 0, 1, validConnection->getMin(), validConnection->getMax()));
    }
    else if(e.type() == typeid(ofParameter<string>).name()){
        ofParameter<string> castedParam = e.cast<string>();
        
        datGuis[parentIndex]->getTextInput(castedParam.getName())->setTextWithoutEvent(castedParam);
        
        for(auto validConnection : validConnections)
            setFromNormalizedValue(validConnection->getSinkParameter(), ofMap(normalizedVal, 0, 1, validConnection->getMin(), validConnection->getMax()));
    }
    else if(e.type() == typeid(ofParameter<ofColor>).name()){
        ofParameter<ofColor> castedParam = e.cast<ofColor>();
        
        datGuis[parentIndex]->getColorPicker(castedParam.getName())->setColor(castedParam);
    }else if(e.type() == typeid(ofParameterGroup).name()){
        ofParameter<int> castedParam = parameterGroups[parentIndex]->getGroup(e.getName()).getInt(1);
        datGuis[parentIndex]->getDropdown(e.getName())->select(castedParam);
        
        normalizedVal = ofMap(castedParam, castedParam.getMin(), castedParam.getMax(), 0, 1);
        
        for(auto validConnection : validConnections)
            setFromNormalizedValue(validConnection->getSinkParameter(), ofMap(normalizedVal, 0, 1, validConnection->getMin(), validConnection->getMax()));
    }else{
        for(auto &validConnection : validConnections)
            setFromSameTypeValue(validConnection);
    }
}

void parametersControl::newMidiMessage(ofxMidiMessage &eventArgs){
    //Save all midi messages into a que;
    midiMessages.push_front(eventArgs);
}

void parametersControl::setFromNormalizedValue(ofAbstractParameter* e, float v){
    if(e->type() == typeid(ofParameter<float>).name()){
        ofParameter<float> castedParam = e->cast<float>();
        castedParam.set(ofMap(v, 0, 1, castedParam.getMin(), castedParam.getMax()));
    }
    else if(e->type() == typeid(ofParameter<int>).name()){
        ofParameter<int> castedParam = e->cast<int>();
        castedParam.set(ofMap(v, 0, 1, castedParam.getMin(), castedParam.getMax()));
    }
    else if(e->type() == typeid(ofParameter<bool>).name()){
        ofParameter<bool> castedParam = e->cast<bool>();
        castedParam = v >= 0.5 ? true : false;
    }
    else if(e->type() == typeid(ofParameter<ofColor>).name()){
        ofParameter<ofColor> castedParam = e->cast<ofColor>();
        castedParam.set(ofColor::fromHsb(v*255, 255, 255));
    }
    else if(e->type() == typeid(ofParameterGroup).name()){
        ofParameterGroup adoptiveGroup;
        adoptiveGroup.add(*e);
        ofParameterGroup nestedGroup = adoptiveGroup.getGroup(0);
        ofParameter<int> castedParam = nestedGroup.getInt(1);
        castedParam.set(ofMap(v, 0, 1, castedParam.getMin(), castedParam.getMax()));
    }
}

void parametersControl::setFromSameTypeValue(shared_ptr<nodeConnection> connection){
    ofAbstractParameter* source = connection->getSourceParameter();
    ofAbstractParameter* sink = connection->getSinkParameter();
    if(source->type() == sink->type()){
        if(source->type() == typeid(ofParameter<vector<float>>).name()){
            vector<float> tempVec = source->cast<vector<float>>();
            for (auto &pos : tempVec)
                pos = ofMap(pos, 0, 1, connection->getMin(), connection->getMax());
            sink->cast<vector<float>>() = tempVec;
        }
        else if(source->type() == typeid(ofParameter<vector<vector<float>>>).name()){
            vector<vector<float>> tempVec = source->cast<vector<vector<float>>>();
            for (auto &pos : tempVec){
                for(auto &pos2 : pos)
                    pos2 = ofMap(pos2, 0, 1, connection->getMin(), connection->getMax());
            }
            sink->cast<vector<vector<float>>>() = tempVec;
        }
        else if(source->type() == typeid(ofParameter<vector<int>>).name()){
            vector<int> tempVec = source->cast<vector<int>>();
            for (auto &pos : tempVec)
                pos = ofMap(pos, 0, 1, connection->getMin(), connection->getMax());
            sink->cast<vector<int>>() = tempVec;
        }
        else if(source->type() == typeid(ofParameter<vector<vector<int>>>).name()){
            vector<vector<int>> tempVec = source->cast<vector<vector<int>>>();
            for (auto &pos : tempVec){
                for(auto &pos2 : pos)
                    pos2 = ofMap(pos2, 0, 1, connection->getMin(), connection->getMax());
            }
            sink->cast<vector<vector<int>>>() = tempVec;
        }
        else if(source->type() == typeid(ofParameter<vector<vector<ofColor>>>).name()){
            vector<vector<ofColor>> tempVec = source->cast<vector<vector<ofColor>>>();
            for (auto &pos : tempVec){
                for(auto &pos2 : pos)
                    pos2.setHue(ofMap(pos2.getHue(), 0, 1, connection->getMin(), connection->getMax()));
            }
            sink->cast<vector<vector<ofColor>>>() = tempVec;
        }
    }else{
        for(auto &connection : connections){
            if(connection->getSourceParameter() == source && connection->getSinkParameter() == sink){
                connections.erase(remove(connections.begin(), connections.end(), connection));
                break;
            }
        }
    }
        
}

void parametersControl::destroyModuleAndConnections(int index){
    for(int j = 0; j < connections.size();){
        auto &connection = connections[j];
        if(connection->getParentGuis(0) == datGuis[index] || connection->getParentGuis(1) == datGuis[index])
            connections.erase(remove(connections.begin(), connections.end(), connection));
        else
            j++;
    }
    datGuis.erase(datGuis.begin()+index);
    string moduleName = parameterGroups[index]->getName();
    ofNotifyEvent(destroyModule, moduleName, this);
    parameterGroups.erase(parameterGroups.begin()+index);
}

void parametersControl::destroyedConnection(ofAbstractParameter &disconnectedParameter){
    for(auto paramGroup : parameterGroups){
        if(&paramGroup->get(disconnectedParameter.getName()) == &disconnectedParameter){
            if(ofStringTimesInString(disconnectedParameter.getName(), "Vector")){
                string paramNameWithoutVector = disconnectedParameter.getName();
                paramNameWithoutVector.erase(paramNameWithoutVector.end() - 7, paramNameWithoutVector.end());
                if(paramGroup->get(paramNameWithoutVector).type() == typeid(ofParameter<float>).name())
                    paramGroup->getFloat(paramNameWithoutVector) = paramGroup->getFloat(paramNameWithoutVector);
                else if(paramGroup->get(paramNameWithoutVector).type() == typeid(ofParameter<int>).name())
                    paramGroup->getInt(paramNameWithoutVector) = paramGroup->getInt(paramNameWithoutVector);
                else if(paramGroup->get(paramNameWithoutVector).type() == typeid(ofParameter<bool>).name())
                    paramGroup->getBool(paramNameWithoutVector) = paramGroup->getBool(paramNameWithoutVector);
                else if(paramGroup->get(paramNameWithoutVector).type() == typeid(ofParameterGroup).name())
                    paramGroup->getGroup(paramGroup->get(paramNameWithoutVector).getName()).getInt(1) = paramGroup->getGroup(paramGroup->get(paramNameWithoutVector).getName()).getInt(1);
            }
            break;
        }
    }
}

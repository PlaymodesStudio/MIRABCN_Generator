//
//  parametersControl.cpp
//  DGTL_Generator
//
//  Created by Eduard Frigola on 03/08/16.
//
//

#include "parametersControl.h"

void parametersControl::createGuiFromParams(ofParameterGroup paramGroup){
    
    ofxDatGuiLog::quiet();
    ofxDatGui::setAssetPath("");
    
    //Put parameterGroup into vector
    parameterGroups.push_back(paramGroup);
        
    ofxDatGui* tempDatGui = new ofxDatGui();
    tempDatGui->setWidth(290);
//    tempDatGui->setAssetPath("");
    tempDatGui->addHeader(paramGroup.getName());
    tempDatGui->addFooter();
    if(datGuis.size() == 0)
        tempDatGui->setPosition(0, 0);
    else
        tempDatGui->setPosition((datGuis[datGuis.size()-1]->getPosition().x + datGuis[datGuis.size()-1]->getWidth()), 0);
    
    for(int i=0 ; i<paramGroup.size(); i++){
        ofAbstractParameter &absParam = paramGroup.get(i);
        if(absParam.type() == typeid(ofParameter<float>).name())
            tempDatGui->addSlider(paramGroup.getFloat(i));
        else if(absParam.type() == typeid(ofParameter<int>).name())
            tempDatGui->addSlider(paramGroup.getInt(i));
        else if(absParam.type() == typeid(ofParameter<bool>).name())
            tempDatGui->addToggle(paramGroup.getName(i))->setChecked(paramGroup.getBool(i).get());
        else if(absParam.type() == typeid(ofParameter<string>).name()){
            if(ofSplitString(absParam.getName(), "_").size() > 1)
                tempDatGui->addLabel(ofSplitString(absParam.getName(), "_")[0]);
            else
                tempDatGui->addTextInput(absParam.getName())->setText(absParam.cast<string>());
        }
        else{
            tempDatGui->addLabel(paramGroup.getGroup(i).getName());
            tempDatGui->addDropdown(paramGroup.getGroup(i).getName(), ofSplitString(paramGroup.getGroup(i).getString(0), "-|-"))->select(paramGroup.getGroup(i).getInt(1));
        }
    }
    datGuis.push_back(tempDatGui);
}

void parametersControl::setup(){
    //DatGui
    
    ofxDatGuiLog::quiet();
    ofxDatGui::setAssetPath("");
    
    datGui = new ofxDatGui();
    datGui->setWidth(290);
    datGui->addHeader("Pressets Control");
    datGui->addFooter();

    datGui->addButton("Global Reset Phase");
    datGui->addSlider("Global BPM", 0, 999, 120);
    
    //Preset Control
    bankSelect = datGui->addDropdown("Bank Select", {"edu", "eloi", "santi"});
    presetMatrix = datGui->addMatrix("Presets", NUM_PRESETS, true);
    presetMatrix->setRadioMode(true);
    presetMatrix->setOpacity(.75);
    
    presetMatrix->onMatrixEvent(this, &parametersControl::onGuiMatrixEvent);
    
    
    datGui->addToggle("Automatic Preset");
    datGui->addSlider(presetChangeBeatsPeriod.set("Beats Period", 4, 1, 120));
    
    datGui->setPosition(ofxDatGuiAnchor::BOTTOM_LEFT);
    
    //ControlGui Events
    datGui->onButtonEvent(this, &parametersControl::onGuiButtonEvent);
    datGui->onToggleEvent(this, &parametersControl::onGuiToggleEvent);
    datGui->onSliderEvent(this, &parametersControl::onGuiSliderEvent);

    //GUIS EVENT LISTERNERS
    for(auto gui : datGuis){
        gui->onButtonEvent(this, &parametersControl::onGuiButtonEvent);
        gui->onToggleEvent(this, &parametersControl::onGuiToggleEvent);
        gui->onDropdownEvent(this, &parametersControl::onGuiDropdownEvent);
//        gui->onSliderEvent(this, &parametersControl::onGuiSliderEvent);
        gui->onTextInputEvent(this, &parametersControl::onGuiTextInputEvent);
    }
    
    //OF PARAMETERS LISTERENRS
    for(auto paramGroup : parameterGroups){
        ofAddListener(paramGroup.parameterChangedE(), this, &parametersControl::listenerFunction);
    }
    
    //OSC
    oscReceiver.setup(12345);
    string host = "192.168.1.39";
    oscSender.setup(host, 54321);
    
    ofLog() << "OSC Info:";
    ofLog() << "Listening on port " << 12345;
    ofLog() << "Sending to host " << host << " on port " << 54321;
  
    
    
    //MIDI
    ofLog() << "MIDI Info:";
    ofxMidiOut::listPorts();
    if(midiOut.openPort("BCF2000")){
        midiIn.openPort("BCF2000");
        midiIn.addListener(this);
    }
    
    
//    loadPreset(1);
    presetChangeCounter = 0;
    presetChangedTimeStamp = ofGetElapsedTimef();
    periodTime = presetChangeBeatsPeriod / parameterGroups[0].getFloat("BPM") * 60.;
    for(int i = 0 ; i < NUM_PRESETS ; i++)
        randomPresetsArrange.push_back(i+1);
    
    random_shuffle(randomPresetsArrange.begin(), randomPresetsArrange.end());
}


void parametersControl::update(){
    while(oscReceiver.hasWaitingMessages()){
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);
        
        vector<string> splitAddress = ofSplitString(m.getAddress(), "/");
        if(splitAddress.size() >= 2){
            ofStringReplace(splitAddress[1], "_", " ");
            if(splitAddress[1] == "presetLoad"){
                presetMatrix->setSelected({m.getArgAsInt(0)});
                loadPreset(m.getArgAsInt(0), m.getArgAsString(1));
            }else if(splitAddress[1] == "presetSave"){
                savePreset(m.getArgAsInt(0), m.getArgAsString(1));
            }else if(splitAddress[1] == "phaseReset"){
                for(auto groupParam : parameterGroups){
                    if(ofStringTimesInString(groupParam.getName(), "phasor") != 0)
                        groupParam.getBool("Reset Phase") = 0;
                }
            }else{
                for(auto groupParam : parameterGroups){
                    if(groupParam.getName() == splitAddress[1]){
                        ofAbstractParameter &absParam = groupParam.get(splitAddress[2]);
                        if(absParam.type() == typeid(ofParameter<float>).name()){
                            ofParameter<float> castedParam = absParam.cast<float>();
                            castedParam = ofMap(m.getArgAsFloat(0), 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                        }else if(absParam.type() == typeid(ofParameter<int>).name()){
                            ofParameter<int> castedParam = absParam.cast<int>();
                            castedParam = ofMap(m.getArgAsFloat(0), 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                        }else if(absParam.type() == typeid(ofParameter<bool>).name())
                            groupParam.getBool(splitAddress[2]) = m.getArgAsBool(0);
                        else if(absParam.type() == typeid(ofParameter<string>).name())
                            groupParam.getString(splitAddress[2]) = m.getArgAsString(0);
                        else
                            groupParam.getGroup(splitAddress[2]).getInt(1) = m.getArgAsInt(0); //DropDown
                    }
                }
            }
        }
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
//        ofAbstractParameter &absParam = groupParam.get(parameterNum);
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
    if(autoPreset && (ofGetElapsedTimef()-presetChangedTimeStamp) > periodTime){
        presetChangedTimeStamp = presetChangedTimeStamp+periodTime;
        loadPreset(randomPresetsArrange.at(presetChangeCounter), bankSelect->getSelected()->getName());
        presetChangeCounter++;
        if(presetChangeCounter >= NUM_PRESETS)
            presetChangeCounter = 0;
    }
}


void parametersControl::savePreset(int presetNum, string bank){
    //xml.load("Preset_"+ofToString(presetNum)+".xml");
    xml.clear();
    
    //Root Element
    xml.addChild("PRESET");
    
    // now we set our current element so we're on the right
    // element, so when add new nodes, they are still inside
    //the root element
    xml.setTo("PRESET");
    
    //Iterate for all the three parameterGroups
    for (auto groupParam : parameterGroups){
        //set XML structure to parameterGroup
        string noSpacesGroupName = groupParam.getName();
        ofStringReplace(noSpacesGroupName, " ", "_");
        xml.addChild(noSpacesGroupName);
        xml.setTo(noSpacesGroupName);
        
        //Iterate for all parameters in parametergroup and look for the type of the parameter
        for (int j = 0; j < groupParam.size() ; j++){
            ofAbstractParameter &absParam = groupParam.get(j);
            if(absParam.type() == typeid(ofParameter<float>).name()){
                //Cast it
                ofParameter<float> castedParam = absParam.cast<float>();
                
                //Replace blank spaces with underscore
                string noSpaces = castedParam.getName();
                ofStringReplace(noSpaces, " ", "_");
                
                //add the value of that parameter into xml
                xml.addValue(noSpaces, castedParam.get());
            }
            else if(absParam.type() == typeid(ofParameter<int>).name()){
                ofParameter<int> castedParam = absParam.cast<int>();
                string noSpaces = castedParam.getName();
                ofStringReplace(noSpaces, " ", "_");
                xml.addValue(noSpaces, castedParam.get());
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
            else{
                string noSpaces = groupParam.getGroup(j).getName();
                ofStringReplace(noSpaces, " ", "_");
                xml.addValue(noSpaces, groupParam.getGroup(j).getInt(1).get());
            }
        }
        xml.setToParent();
    }
    ofLog() <<"Save Preset_" << presetNum<< "_" << bank;
    xml.save("Preset_"+ofToString(presetNum)+"_"+bank+".xml");
    
    ofxOscMessage m;
    m.setAddress("presetSave");
    m.addIntArg(presetNum);
    m.addStringArg(bank);
    oscSender.sendMessage(m);
}

void parametersControl::loadPreset(int presetNum, string bank){
    //Test if there is no problem with the file
    if(!xml.load("Preset_"+ofToString(presetNum)+"_"+bank+".xml"))
        return;
    
    //Iterate for all the parameterGroups
    for (auto groupParam : parameterGroups){
        //Put xml in the place of the parametergroup
        string noSpacesGroupName = groupParam.getName();
        ofStringReplace(noSpacesGroupName, " ", "_");
        if(xml.exists(noSpacesGroupName)){
            xml.setTo(noSpacesGroupName);
            
            //Iterate for all parameters in parametergroup and look for the type of the parameter
            for (int j = 0; j < groupParam.size() ; j++){
                ofAbstractParameter &absParam = groupParam.get(j);
                if(absParam.type() == typeid(ofParameter<float>).name()){
                    //Cast it
                    ofParameter<float> castedParam = absParam.cast<float>();
                    
                    //Replace blank spaces with underscore
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    
                    //get the value of that parameter if it's not bpm, we don't want to lose sync
                    if(castedParam.getName() != "BPM" && xml.exists(noSpaces))
                        castedParam = xml.getValue(noSpaces, castedParam.get());
                }
                else if(absParam.type() == typeid(ofParameter<int>).name()){
                    ofParameter<int> castedParam = absParam.cast<int>();
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    if(xml.exists(noSpaces))
                        castedParam = xml.getValue(noSpaces, castedParam.get());
                }
                else if(absParam.type() == typeid(ofParameter<bool>).name()){
                    ofParameter<bool> castedParam = absParam.cast<bool>();
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    if(xml.exists(noSpaces))
                        castedParam = xml.getValue(noSpaces, castedParam.get());
                }
                else if(absParam.type() == typeid(ofParameter<string>).name()){
                    ofParameter<string> castedParam = absParam.cast<string>();
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    if(xml.exists(noSpaces))
                        castedParam.set(xml.getValue(noSpaces, castedParam.get()));
                }
                else{
                    string noSpaces = groupParam.getGroup(j).getName();
                    ofStringReplace(noSpaces, " ", "_");
                    if(xml.exists(noSpaces))
                        groupParam.getGroup(j).getInt(1) = xml.getValue(noSpaces, groupParam.getGroup(j).getInt(1));
                }
            }
            //Jump one label before in xml structure
            xml.setToParent();
            //reset Phasor
            if(ofSplitString(groupParam.getName(), " ")[0] == "phasor")
                groupParam.getBool("Reset Phase") = false;
        }
    }
    ofLog()<<"Load Preset_" << presetNum<< "_" << bank;
    vector<int> tempVec;
    tempVec.push_back(presetNum-1);
    presetMatrix->setSelected(tempVec);
    
    ofxOscMessage m;
    m.setAddress("presetLoad");
    m.addIntArg(presetNum);
    m.addStringArg(bank);
    oscSender.sendMessage(m);
    
}

void parametersControl::onGuiButtonEvent(ofxDatGuiButtonEvent e){
    if(datGui->getButton(e.target->getName())){
        string nameNoGlobal = e.target->getName();
        ofStringReplace(nameNoGlobal, "Global ", "");
        for(auto groupParam : parameterGroups){
            if(ofStringTimesInString(groupParam.getName(), "phasor") != 0)
            groupParam.getBool(nameNoGlobal) = 0;
        }
    }
}
void parametersControl::onGuiToggleEvent(ofxDatGuiToggleEvent e){
    for (int i=0; i < datGuis.size() ; i++){
        if(datGuis[i]->getToggle(e.target->getName()) == e.target)
            parameterGroups[i].getBool(e.target->getName()) = e.target->getChecked();
    }
    if(e.target->getName() == "Automatic Preset")
        autoPreset = e.checked;
}

void parametersControl::onGuiDropdownEvent(ofxDatGuiDropdownEvent e){
    for (int i=0; i < datGuis.size() ; i++){
        if(datGuis[i]->getDropdown(e.target->getName()) == e.target)
            parameterGroups[i].getGroup(e.target->getName()).getInt(1) = e.child;
    }
}

void parametersControl::onGuiMatrixEvent(ofxDatGuiMatrixEvent e){
    if(ofGetKeyPressed(OF_KEY_SHIFT))
        savePreset(e.child+1, bankSelect->getSelected()->getName());
    else{
        loadPreset(e.child+1, bankSelect->getSelected()->getName());
        if(autoPreset)
            presetChangedTimeStamp = ofGetElapsedTimef();
    }
}

void parametersControl::onGuiSliderEvent(ofxDatGuiSliderEvent e){
    if(datGui->getSlider(e.target->getName())->getName() == e.target->getName()){
        string nameNoGlobal = e.target->getName();
        ofStringReplace(nameNoGlobal, "Global ", "");
        for(auto groupParam : parameterGroups){
            if(ofStringTimesInString(groupParam.getName(), "phasor") != 0 && groupParam.getFloat(nameNoGlobal).getName() == nameNoGlobal)
                groupParam.getFloat(nameNoGlobal) = e.value;
        }
    }
    if(e.target->getName() == "Beats Period")
        periodTime = e.value / datGui->getSlider("Global BPM")->getValue() * 60.;
}

void parametersControl::onGuiTextInputEvent(ofxDatGuiTextInputEvent e){
    for (int i=0; i < datGuis.size() ; i++){
        if(datGuis[i]->getTextInput(e.target->getName()) == e.target)
            parameterGroups[i].getString(e.target->getName()) = e.text;
    }
}


void parametersControl::listenerFunction(ofAbstractParameter& e){
    int position = 0;
    
    
    
    if(e.getName() == "Phasor Monitor"){
        return;
    }
//        for(auto stri : e.getGroupHierarchyNames())
//            cout<<stri<<"-";
//
//        cout<<endl;
    
    
    auto parent = e.getGroupHierarchyNames()[0];
    int parentIndex = 0;
    ofStringReplace(parent, "_", " ");
    for(int i = 0; i<parameterGroups.size(); i++){
        if(parameterGroups[i].getName() == parent)
            parentIndex = i;
    }
    
    int normalizedVal = 0;
    if(e.type() == typeid(ofParameter<float>).name()){
        ofParameter<float> castedParam = e.cast<float>();
        normalizedVal = ofMap(castedParam, castedParam.getMin(), castedParam.getMax(), 0, 127);
        position = parameterGroups[parentIndex].getPosition(e.getName());
        position += parentIndex*20;
        
        if(castedParam.getName() == "BPM")
            periodTime = presetChangeBeatsPeriod / castedParam * 60.;
    }
    else if(e.type() == typeid(ofParameter<int>).name()){
        ofParameter<int> castedParam = e.cast<int>();
        int range = castedParam.getMax()-castedParam.getMin();
        if(range < 128)
            normalizedVal = ofMap(castedParam, castedParam.getMin(), castedParam.getMax(), 0, ((int)(128/(range))*range));
        else
            normalizedVal = ofMap(castedParam, castedParam.getMin(), castedParam.getMax(), 0, range/ceil((float)range/(float)128));
        position = parameterGroups[parentIndex].getPosition(e.getName());
        position += parentIndex*20;
        
        if(ofStringTimesInString(castedParam.getName(), "Select") == 1){
            datGuis[parentIndex]->getDropdown(castedParam.getName())->select(castedParam);
        }
    }
    else if(e.type() == typeid(ofParameter<bool>).name()){
        ofParameter<bool> castedParam = e.cast<bool>();
        normalizedVal = castedParam ? 127 : 0;
        position = parameterGroups[parentIndex].getPosition(e.getName());
        position += parentIndex*20;
        
        //Update to datGuis
        datGuis[parentIndex]->getToggle(castedParam.getName())->setChecked(normalizedVal);
    }
    else if(e.type() == typeid(ofParameter<string>).name()){
        ofParameter<string> castedParam = e.cast<string>();
        position = -1;
        
        datGuis[parentIndex]->getTextInput(castedParam.getName())->setText(castedParam);
    }
    
    
    
    if(position != -1)
        midiOut.sendControlChange(1, position, normalizedVal);
    
//    cout<<"Para Change: "<< e.getName() << " |pos: " << position << " |val: " << e  << " |MIDI: " << normalizedVal << endl;
}

void parametersControl::newMidiMessage(ofxMidiMessage &eventArgs){
    //Save all midi messages into a que;
    midiMessages.push_front(eventArgs);
}
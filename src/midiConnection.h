//
//  midiConnection.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 05/06/2017.
//
//

#ifndef midiConnection_h
#define midiConnection_h

#include "ofMain.h"

template<typename T>
class midiConnection{
public:
    midiConnection(){};
    midiConnection(ofParameter<T>* p){
        bindedParameter = p;
        midiListenMode = true;
        toggle = false;
    }
    
    void assignMidiControl(string device, int channel, int control){
        midiDevice = device;
        channelNumber = channel;
        controlNumber = control;
        midiListenMode = false;
        if(ofGetKeyPressed(OF_KEY_COMMAND))
            toggle = true;
        *bindedParameter = *bindedParameter;
    }
    
    int sendValue(){
        int toMidiVal = 0;
        if(bindedParameter->type() == typeid(ofParameter<int>).name()){
            int range = bindedParameter->getMax()-bindedParameter->getMin();
            //TODO: Review, map is from 0 127 not 0 1;
            if(range < 128)
                toMidiVal = ofMap(*bindedParameter, bindedParameter->getMin(), bindedParameter->getMax(), 0, ((int)(128/(range))*range));
            else
                toMidiVal = ofMap(*bindedParameter, bindedParameter->getMin(), bindedParameter->getMax(), 0, range/ceil((float)range/(float)128));
            
        }
        else if(bindedParameter->type() == typeid(ofParameter<float>).name()){
            toMidiVal = ofMap(*bindedParameter, bindedParameter->getMin(), bindedParameter->getMax(), 0, 127);
        }
        else if(bindedParameter->type() == typeid(ofParameter<bool>).name()){
            if(!toggle)
                toMidiVal = (bindedParameter == 0)? 0 : 127;
        }
        return toMidiVal;
    }
    void setValue(int midiValue){
        if(bindedParameter->type() == typeid(ofParameter<int>).name()){
            int range = bindedParameter->getMax()-bindedParameter->getMin();
            if(range < 128)
                *bindedParameter = ofMap(midiValue, 0, ((int)(128/(range))*range), bindedParameter->getMin(), bindedParameter->getMax(), true);
            else
                *bindedParameter = ofMap(midiValue, 0, range/ceil((float)range/(float)128), bindedParameter->getMin(), bindedParameter->getMax(), true);
            
        }
        else if(bindedParameter->type() == typeid(ofParameter<float>).name()){
            *bindedParameter = (ofMap(midiValue, 0, 127, bindedParameter->getMin(), bindedParameter->getMax(), true));
        }
        else if(bindedParameter->type() == typeid(ofParameter<bool>).name()){
            if(toggle && midiValue == 127)
                *bindedParameter = !*bindedParameter;
            else if(!toggle)
                *bindedParameter = (midiValue == 0)? false : true;
        }
    }
    
    ofParameter<T>* getParameter(){return bindedParameter;};
    bool isListening(){return midiListenMode;};
    string getDevice(){return midiDevice;};
    int getChannel(){return channelNumber;};
    int getControl(){return controlNumber;};
    bool isToggle(){return toggle;};
    void setToggle(bool b){toggle = b;};
    
private:
    ofParameter<T>* bindedParameter;
    bool midiListenMode;
    string midiDevice;
    int channelNumber;
    int controlNumber;
    bool toggle;
};

#endif /* midiConnection_h */

//
//  midiConnection.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola Bagué on 21/02/2018.
//

#include "midiConnection.h"

template<>
int midiConnection<int>::sendValue(){
    int toMidiVal = 0;
    int range = bindedParameter->getMax()-bindedParameter->getMin();
    //TODO: Review, map is from 0 127 not 0 1;
    if(range < 128)
        toMidiVal = ofMap(*bindedParameter, bindedParameter->getMin(), bindedParameter->getMax(), 0, ((int)(128/(range))*range));
    else
        toMidiVal = ofMap(*bindedParameter, bindedParameter->getMin(), bindedParameter->getMax(), 0, range/ceil((float)range/(float)128));
    return toMidiVal;
};

template<>
void midiConnection<int>::setValue(int midiValue){
    int range = bindedParameter->getMax()-bindedParameter->getMin();
    if(range < 128)
        *bindedParameter = ofMap(midiValue, 0, ((int)(128/(range))*range), bindedParameter->getMin(), bindedParameter->getMax(), true);
    else
        *bindedParameter = ofMap(midiValue, 0, range/ceil((float)range/(float)128), bindedParameter->getMin(), bindedParameter->getMax(), true);
}

template<>
int midiConnection<float>::sendValue(){
    return ofMap(*bindedParameter, bindedParameter->getMin(), bindedParameter->getMax(), 0, 127);
}

template<>
void midiConnection<float>::setValue(int midiValue){
    *bindedParameter = (ofMap(midiValue, 0, 127, bindedParameter->getMin(), bindedParameter->getMax(), true));
}

template<>
int midiConnection<bool>::sendValue(){
    return (bindedParameter == 0)? 0 : 127;
}

template<>
void midiConnection<bool>::setValue(int midiValue){
    if(toggle && midiValue == 127)
        *bindedParameter = !*bindedParameter;
    else if(!toggle)
        *bindedParameter = (midiValue == 0)? false : true;
}

template<>
int midiConnection<vector<float>>::sendValue(){
     return ofMap(bindedParameter->get()[0], bindedParameter->getMin()[0], bindedParameter->getMax()[0], 0, 127);
}

template<>
void midiConnection<vector<float>>::setValue(int midiValue){
    vector<float> vecFloat;
    vecFloat.resize(1);
    vecFloat[0] = ofMap(midiValue, 0, 127, bindedParameter->getMin()[0], bindedParameter->getMax()[0], true);
    bindedParameter->set(vecFloat);
}



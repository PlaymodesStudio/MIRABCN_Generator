//
//  bpmControl.cpp
//  DGTL_Generator
//
//  Created by Eduard Frigola on 12/08/16.
//
//

#include "bpmControl.h"

void bpmControl::setup(){
    // setup beat object
    beat.setup();
    //beat.setup("default", 2 * bufferSize, bufferSize, samplerate);
    oldBpm = 0;
}

void bpmControl::audioIn(float * input, int bufferSize, int nChannels){
    // compute beat location
    beat.audioIn(input, bufferSize, nChannels);
    if(oldBpm != beat.bpm)
        ofNotifyEvent(bpmChanged, beat.bpm, this);
    oldBpm = beat.bpm;
}

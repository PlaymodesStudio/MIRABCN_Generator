//
//  speakerPowerCalculator.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 27/09/2017.
//
//

#include "speakerPowerCalculator.h"
#include "ofSoundUtils.h"

speakerPowerCalculator::speakerPowerCalculator(int lenght){
    width = 2;
    height = lenght;
    
    parameters = new ofParameterGroup();
    parameters->setName("speakerPowerCalculator");
    parameters->add(input.set("Input", {{}}));
    parameters->add(output.set("Output", {{}}));
    
    input.addListener(this, &speakerPowerCalculator::inputListener);
    widthManualWindowing.resize(2);
}

void speakerPowerCalculator::inputListener(vector<vector<ofColor>> &in){
    int w = in.size();
    int h = in[0].size();
    
    if(w != widthManualWindowing.size()){
        //left
        widthManualWindowing[0].resize(w, 0);
        //right
        widthManualWindowing[1].resize(w, 0);
        for(int i = -w/2; i < w/2; i++){
            float left;
            float right;
            ofStereoVolumes(1, i/(w/2), widthManualWindowing[0][i], widthManualWindowing[1][i]);
        }
    }
}


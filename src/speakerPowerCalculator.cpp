//
//  speakerPowerCalculator.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 27/09/2017.
//
//

#include "speakerPowerCalculator.h"
#include "ofSoundUtils.h"
#include "parametersControl.h"

speakerPowerCalculator::speakerPowerCalculator(int lenght){
    width = 2;
    height = lenght;
    
    parameters = new ofParameterGroup();
    parameters->setName("speakerPowerCalculator");
    parameters->add(input.set("Input", {{}}));
//    parameters->add(computeFunc.set("ComputeFunc", 0, 0, 1));
    parametersControl::addDropdownToParameterGroupFromParameter(parameters, "Function", {"Sum", "Max", "Mean", "Min"}, computeFunc);
    parameters->add(output.set("Output", {{}}));
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::green);
    input.addListener(this, &speakerPowerCalculator::inputListener);
    widthManualWindowing.resize(2);
}

void speakerPowerCalculator::inputListener(vector<vector<ofColor>> &in){
    int h = in.size();
    int w = in[0].size();
    
    int widthToPan = w/3;
    if(widthToPan != widthManualWindowing[0].size()){
        //left
        widthManualWindowing[0].resize(widthToPan, 0);
        //right
        widthManualWindowing[1].resize(widthToPan, 0);
        for(int i = 0; i < widthToPan; i++){
            float panPos = ((float)(i + 0.5)-((float)widthToPan/2.)) / ((float)widthToPan/2.);
            ofStereoVolumes(1, panPos, widthManualWindowing[0][i], widthManualWindowing[1][i]);
        }
    }
    
    if(h != heightManualWindowing.size()){
        heightManualWindowing.resize(h);
        //Hamming window
        float alpha = 0.54;
        float beta = 1 - alpha;
        for (int i = 0; i < h; i++){
            heightManualWindowing[i] = alpha-beta * cos((2*PI*i)/(h-1));
        }
    }
    
    //computeStereo
    vector<vector<float>> stereoSplit;
    stereoSplit.resize(width, vector<float>(h));
    
    for(int i = 0; i < h; i++){
        //iterate for each arch
        switch(computeFunc){
            case 0: //sum;
            {
                float sumL = 0;
                float sumR = 0;
                for(int j = 0; j < widthToPan; j++){
                    //iterate each point in arch
                    sumL = sumL + (in[i][j].getBrightness()/255.);
                }
                for(int j = widthToPan; j < widthToPan*2; j++){
                    //iterate each point in arch
                    sumL = sumL + (in[i][j].getBrightness()/255.)*widthManualWindowing[0][j-widthToPan];
                    sumR = sumR + (in[i][j].getBrightness()/255.)*widthManualWindowing[1][j-widthToPan];
                }
                for(int j = widthToPan*2; j < w; j++){
                    //iterate each point in arch
                    sumR = sumR + (in[i][j].getBrightness()/255.);
                }
                stereoSplit[0][i] = ofClamp(sumL, 0, 1);
                stereoSplit[1][i] = ofClamp(sumR, 0, 1);
                break;
            }
            case 1: //max;
            {
                float maxL = 0;
                float maxR = 0;
                for(int j = 0; j < widthToPan; j++){
                    //iterate each point in arch
                    maxL = maxL > (in[i][j].getBrightness()/255.) ? maxL : (in[i][j].getBrightness()/255.);
                }
                for(int j = widthToPan; j < widthToPan*2; j++){
                    //iterate each point in arch
                    maxL = (maxL > ((in[i][j].getBrightness()/255.)*widthManualWindowing[0][j-widthToPan])) ? maxL : (in[i][j].getBrightness()/255.*widthManualWindowing[0][j-widthToPan]);
                    maxR = (maxR > ((in[i][j].getBrightness()/255.)*widthManualWindowing[1][j-widthToPan])) ? maxR : (in[i][j].getBrightness()/255.*widthManualWindowing[1][j-widthToPan]);
                }
                for(int j = widthToPan*2; j < w; j++){
                    //iterate each point in arch
                    maxR = maxR > (in[i][j].getBrightness()/255.) ? maxR : (in[i][j].getBrightness()/255.);
                }
                stereoSplit[0][i] = maxL;
                stereoSplit[1][i] = maxR;
                break;
            }
            case 2: //mean;
            {
                float sumL = 0;
                float sumR = 0;
                for(int j = 0; j < widthToPan; j++){
                    //iterate each point in arch
                    sumL = sumL + (in[i][j].getBrightness()/255.);
                }
                for(int j = widthToPan; j < widthToPan*2; j++){
                    //iterate each point in arch
                    sumL = sumL + (in[i][j].getBrightness()/255.)*widthManualWindowing[0][j-widthToPan];
                    sumR = sumR + (in[i][j].getBrightness()/255.)*widthManualWindowing[1][j-widthToPan];
                }
                for(int j = widthToPan*2; j < w; j++){
                    //iterate each point in arch
                    sumR = sumR + (in[i][j].getBrightness()/255.);
                }
                stereoSplit[0][i] = sumL/(float)w;
                stereoSplit[1][i] = sumR/(float)w;
                break;
            }
        }
    }
    
    vector<vector<float>>   lenghtShrink;
    lenghtShrink.resize(width, vector<float>(height));
    
    for(int i = 0; i < width; i++){
        switch (computeFunc) {
            case 0:{ //Sum
                for(int j = 1; j < h; j += 4){
                    int initialVal = max(0, (int)ceil((j+.5) - h/2));
                    int finalVal = min(h-1, (int)floor((j+.5) + h/2));
                    float sum = 0;
                    for(int k = initialVal; k <= finalVal; k++){
                        int manualWindowIndex = (h/2);
                        if(k < (j+.5)) manualWindowIndex -= ceil(abs((float)k - ((float)j+.5)));
                        else manualWindowIndex += floor((float)k - ((float)j+.5));
                        sum = sum + (stereoSplit[i][k] * heightManualWindowing[manualWindowIndex]);
                    }
                    lenghtShrink[i][(j-1)/4] = ofClamp(sum, 0, 1);
                }
                break;
            }
            case 1:{ //Max
                for(int j = 1; j < h; j += 4){
                    int initialVal = max(0, (int)ceil((j+.5) - h/2));
                    int finalVal = min(h-1, (int)floor((j+.5) + h/2));
                    float max = 0;
                    for(int k = initialVal; k <= finalVal; k++){
                        int manualWindowIndex = (h/2);
                        if(k < (j+.5)) manualWindowIndex -= ceil(abs((float)k - (j+.5)));
                        else manualWindowIndex += floor((float)k - (j+.5));
                        float actualVal = (stereoSplit[i][k] * heightManualWindowing[manualWindowIndex]);
                        if(actualVal > max) max = actualVal;
                    }
                    lenghtShrink[i][(j-1)/4] = max;
                }
                break;
            }
            case 2:{ //Mean
                for(int j = 1; j < h; j += 4){
                    int initialVal = max(0, (int)ceil((j+.5) - h/2));
                    int finalVal = min(h-1, (int)floor((j+.5) + h/2));
                    float sum = 0;
                    for(int k = initialVal; k <= finalVal; k++){
                        int manualWindowIndex = (h/2);
                        if(k < (j+.5)) manualWindowIndex -= ceil(abs((float)k - ((float)j+.5)));
                        else manualWindowIndex += floor((float)k - ((float)j+.5));
                        sum = sum + (stereoSplit[i][k] * heightManualWindowing[manualWindowIndex]);
                    }
                    lenghtShrink[i][(j-1)/4] = (float)sum / float(abs(finalVal-initialVal));
                }
                break;
            }
            default:
                break;
        }
    }


    parameters->get("Output").cast<vector<vector<float>>>() = lenghtShrink;
}


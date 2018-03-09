//
//  valueSmoother.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 18/09/2017.
//
//

#ifndef valueSmoother_h
#define valueSmoother_h

#include "ofMain.h"
#include "parametersControl.h"

class valueSmoother{
public:
    valueSmoother(int id_, ofPoint pos){
        parameters = new ofParameterGroup();
        parameters->setName("valueSmoother " + ofToString(id_));
        
        parameters->add(input.set("Input", 0, 0, 1));
        parameters->add(smoothing.set("Smoothing", 0.5, 0, 1));
        parameters->add(tension.set("Tension", 0, -1, 1));
        parameters->add(output.set("Output", 0, 0, 1));
        
        parametersControl::getInstance().createGuiFromParams(parameters, ofColor::green, pos);
        
        input.addListener(this, &valueSmoother::inputListener);
        previousValue = -1;
    }
    
private:
    void inputListener(float &f){
        if(previousValue == -1) previousValue = f;
        float newSmoothing = smoothing;
        if(tension > 0)
            newSmoothing = ofClamp(smoothing * (1 - (abs(previousValue - f) * tension)), 0, 1);
        else if(tension < 0)
            newSmoothing = ofClamp(smoothing * (1 - ((1 - abs(previousValue - f)) * abs(tension))), 0, 1);
        
        float smoothedValue = (newSmoothing * previousValue) + ((1 - newSmoothing) * f);
        parameters->getFloat("Output") = smoothedValue;
        previousValue = smoothedValue;
    }
    
    ofParameterGroup *parameters;
    ofParameter<float>  input;
    ofParameter<float>  smoothing;
    ofParameter<float>  tension;
    ofParameter<float>  output;
    float previousValue;
};

class vectorValueSmoother{
public:
    vectorValueSmoother(int id_, ofPoint pos){
        parameters = new ofParameterGroup();
        parameters->setName("vectorValueSmoother " + ofToString(id_));
        
        parameters->add(input.set("Input",{}));
        parameters->add(smoothing.set("Smoothing", 0.5, 0, 1));
        parameters->add(output.set("Output", {0}, {0}, {1}));
        
        parametersControl::getInstance().createGuiFromParams(parameters, ofColor::green, pos);
        
        input.addListener(this, &vectorValueSmoother::inputListener);
    }
    
private:
    void inputListener(vector<float> &vf){
        if(previousValue.size() != vf.size()) previousValue = vf;
        vector<float> vecCopy = vf;
        
        for(int i = 0; i < vecCopy.size(); i++){
            float newSmoothing = smoothing;
            if(tension > 0)
                newSmoothing = ofClamp(smoothing * (1 - (abs(previousValue[i] - vf[i]) * tension)), 0, 1);
            else if(tension < 0)
                newSmoothing = ofClamp(smoothing * (1 - ((1 - abs(previousValue[i] - vf[i])) * abs(tension))), 0, 1);
            
            vecCopy[i] = (newSmoothing * previousValue[i]) + ((1 - newSmoothing) * vf[i]);
        }
        parameters->get("Output").cast<vector<float>>() = vecCopy;
        previousValue = vecCopy;
    }
    
    ofParameterGroup *parameters;
    ofParameter<vector<float>>  input;
    ofParameter<float>  smoothing;
    ofParameter<float>  tension;
    ofParameter<vector<float>>  output;
    vector<float> previousValue;
};



#endif /* valueSmoother_h */

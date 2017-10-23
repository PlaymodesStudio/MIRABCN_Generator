//
//  bankDimensionCombinator.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 18/10/2017.
//
//

#include "bankDimensionCombinator.h"
#include "parametersControl.h"

bankDimensionCombinator::bankDimensionCombinator(){
    parameters = new ofParameterGroup();
    parameters->setName("bankDimensionCombinator");
    
    parameters->add(x_input.set("Input X", {}));
    parameters->add(y_input.set("Input Y", {}));
    parametersControl::addDropdownToParameterGroupFromParameter(parameters, "Operation", {"+", "x", "-", "mean"}, operationSelector);
    
    parameters->add(output.set("Output", {}));
    
    x_input.addListener(this, &bankDimensionCombinator::paramListener);
    parametersControl::getInstance().createGuiFromParams(parameters);
}

void bankDimensionCombinator::paramListener(vector<float> &vf){
//    int lin_size = vf.size();
//    if(y_input.get().size() == lin_size){
//        vector<float> tempOut(lin_size);
//        int width = 0, height = 0;
//        if(pow(int(sqrt(lin_size)), 2) ==  lin_size){
//            width = sqrt(lin_size);
//            height = sqrt(lin_size);
//        }
//        
//        int width_i = 0, height_i = 0;
//        for(int i = 0; i < lin_size; i++){
//            height_i = floor(i/width);
//            width_i = i - (height_i * width);
//            tempOut[i] = x_input.get()[width_i] + y_input.get()[height_i];
//        }
//        parameters->get("Output").cast<vector<float>>() = tempOut;
//    }
    
    
//    int width = 0, height = 0;
//    if(pow(int(sqrt(lin_size)), 2) ==  lin_size){
//        width = sqrt(lin_size);
//        height = sqrt(lin_size);
//    }
//    else{
//        for(int i = floor(sqrt(lin_size)); i < 0; i++){
//            if((lin_size/i) == int(lin_size/i)){
//                width = i;
//                height = lin_size/i;
//                break;
//            }
//        }
//    }
    
    

    int width = x_input.get().size();
    int height = y_input.get().size();
    int lin_size = width * height;
    vector<float> tempOut(lin_size);
    if(width != 0 && height != 0){
        int width_i = 0, height_i = 0;
        for(int i = 0; i < lin_size; i++){
            height_i = floor(i/width);
            width_i = i - (height_i * width);
            switch(operationSelector){
                case 0:
                    tempOut[i] = ofClamp(x_input.get()[width_i] + y_input.get()[height_i],0,1);
                    break;
                case 1:
                    tempOut[i] = ofClamp(x_input.get()[width_i] * y_input.get()[height_i],0,1);
                    break;
                case 2:
                    tempOut[i] = ofClamp(x_input.get()[width_i] - y_input.get()[height_i],0,1);
                    break;
                case 3:
                    tempOut[i] = ofClamp(x_input.get()[width_i]*.5 + y_input.get()[height_i]*.5, 0, 1);
                    break;
            }
        }
        parameters->get("Output").cast<vector<float>>() = tempOut;
    }
    
}

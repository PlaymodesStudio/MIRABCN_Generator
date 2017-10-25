//
//  SubDimensionCombinator.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 25/10/2017.
//
//

#include "SubDimensionCombinator.h"
#include "parametersControl.h"

subDimensionCombinator::subDimensionCombinator(){
    parameters = new ofParameterGroup();
    parameters->setName("subDimensionCombinator");
    
    parameters->add(original.set("Original", {}));
    parameters->add(subDimension.set("Sub Dimension", {}));
    
    parameters->add(output.set("Output", {}));
    
    original.addListener(this, &subDimensionCombinator::paramListener);
    parametersControl::getInstance().createGuiFromParams(parameters);
}

void subDimensionCombinator::paramListener(vector<float> &vf){
    int originSize = original.get().size();
    int subDimSize = subDimension.get().size();
    if(originSize == 0 || subDimSize == 0) return;
    if(originSize < subDimSize){
        if(subDimSize % originSize == 0){
            vector<float> tempOut(subDimSize, 0);
            int dimStep = subDimSize / originSize;
            for(int i = 0 ; i < subDimSize; i++){
                int dimStep_ind = ceil(i / dimStep);
                tempOut[i] = original.get()[dimStep_ind] * subDimension.get()[i];
            }
            parameters->get("Output").cast<vector<float>>() = tempOut;
        }
    }
    else{
        if(originSize % subDimSize == 0){
            int size = subDimSize * originSize;
            vector<float> tempOut(size);
            for(int i = 0 ; i < size; i++){
                int dimStep_ind = ceil(i / subDimSize);
                tempOut[i] = original.get()[dimStep_ind] * subDimension.get()[i % subDimSize];
            }
            parameters->get("Output").cast<vector<float>>() = tempOut;
        }
    }
    
}

//
//  typeConverter.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 13/07/2017.
//
//

#include "typeConverter.h"

template<>
void typeConverter<vector<float>, vector<vector<float>>>::sourceListener(vector<float> &s){
    vector<vector<float>> toSend = {s};
    parameters->get("Dest").cast<vector<vector<float>>>() = toSend;
};

template<>
void typeConverter<float, vector<float>>::sourceListener(float &s){
    vector<float> toSend = {s};
    parameters->get("Dest").cast<vector<float>>() = toSend;
};


template<>
void typeConverter<vector<vector<float>>, vector<float>>::sourceListener(vector<vector<float>> &s){
    vector<float> toSend = s[0];
    parameters->get("Dest").cast<vector<float>>() = toSend;
};

template<>
void typeConverter<vector<float>, float>::sourceListener(vector<float> &s){
    float toSend = s[0];
    parameters->get("Dest").cast<float>() = toSend;
};

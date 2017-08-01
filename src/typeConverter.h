//
//  typeConverter.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 13/07/2017.
//
//

#ifndef typeConverter_h
#define typeConverter_h

#include "ofMain.h"
#include "parametersControl.h"

class genericTypeConverter{
public:
    genericTypeConverter(){};
    ~genericTypeConverter(){};
};

template <typename Tsource,typename Tdest>
class typeConverter : public genericTypeConverter{
public:
    typeConverter(int id, ofPoint pos = ofPoint(-1, -1)){
        parameters = new ofParameterGroup();
        parameters->setName("typeConverter " + ofToString(id));
        
        sourceTypeName = "Source - ";
        if(typeid(Tsource).name() == typeid(float).name()){
            sourceTypeName += "float";
        }
        else if(typeid(Tsource).name() == typeid(vector<float>).name()){
            sourceTypeName += "vector float";
        }
        else if(typeid(Tsource).name() == typeid(vector<vector<float>>).name()){
            sourceTypeName += "vector vector float";
        }
        else{
            sourceTypeName += "unknown";
        }
        
        destTypeName = "Dest - ";
        if(typeid(Tdest).name() == typeid(float).name()){
            destTypeName += "float";
        }
        else if(typeid(Tdest).name() == typeid(vector<float>).name()){
            destTypeName += "vector float";
        }
        else if(typeid(Tdest).name() == typeid(vector<vector<float>>).name()){
            destTypeName += "vector vector float";
        }
        else{
            destTypeName += "unknown";
        }
        
        parameters->add(source.set(sourceTypeName, Tsource()));
        parameters->add(dest.set(destTypeName, Tdest()));
        
        source.addListener(this, &typeConverter::sourceListener);
        
        parametersControl::getInstance().createGuiFromParams(parameters, ofColor::yellow, pos);
    };
    ~typeConverter(){};
    
private:
    void sourceListener(Tsource &s){
//        if(typeid(Tsource).name() == typeid(typename Tdest::value_type).name()){
//            Tdest toSend = {source};
//            parameters->get(destTypeName).template cast<Tdest>() = toSend;
//        }else if(typeid(typename Tsource::value_type).name() == typeid(Tdest).name()){
//            Tdest toSend = source.get()[0];
//            parameters->get(destTypeName).template cast<Tdest>() = toSend;
//        }
        if(typeid(Tsource).name() == typeid(vector<float>).name() && typeid(Tdest).name() == typeid(vector<vector<float>>).name()){
            vector<vector<float>> toSend = {source};
            parameters->get("Dest").cast<Tdest>() = toSend;
        }
    }
    
    ofParameterGroup*    parameters;
    ofParameter<Tsource>    source;
    ofParameter<Tdest>      dest;
    
    string sourceTypeName;
    string destTypeName;
};

#endif /* typeConverter_h */

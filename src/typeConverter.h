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


//#define CONVERT_FLOAT_TO_VECFLOAT = 1;
//#define CONVERT_VECFLOAT_TO_VECVECFLOAT = 2;
//#define CONVERT_VECFLOAT_TO_FLOAT = 3;
//#define CONVERT_VECVECFLOAT_TO_VECFLOAT = 4;

enum converterTypes{
    CONVERT_FLOAT_TO_VECFLOAT = 1,
    CONVERT_VECFLOAT_TO_VECVECFLOAT = 2,
    CONVERT_VECFLOAT_TO_FLOAT = 3,
    CONVERT_VECVECFLOAT_TO_VECFLOAT = 4
};


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
        
        sourceTypeName = "";
        if(typeid(Tsource).name() == typeid(float).name()){
            sourceTypeName += "float";
        }
        else if(typeid(Tsource).name() == typeid(vector<float>).name()){
            sourceTypeName += "vector<float>";
        }
        else if(typeid(Tsource).name() == typeid(vector<vector<float>>).name()){
            sourceTypeName += "vector<vector<float>>";
        }
        else{
            sourceTypeName += "unknown";
        }
        
        destTypeName = "";
        if(typeid(Tdest).name() == typeid(float).name()){
            destTypeName += "float";
        }
        else if(typeid(Tdest).name() == typeid(vector<float>).name()){
            destTypeName += "vector<float>";
        }
        else if(typeid(Tdest).name() == typeid(vector<vector<float>>).name()){
            destTypeName += "vector<vector<float>>";
        }
        else{
            destTypeName += "unknown";
        }
        
        int type = 0;
        if(sourceTypeName == "float" && destTypeName == "vector<float>") type = 1;
        else if(sourceTypeName == "vector<float>" && destTypeName == "vector<vector<float>>") type = 2;
        else if(sourceTypeName == "vector<float>" && destTypeName == "float") type = 3;
        else if(sourceTypeName == "vector<vector<float>>" && destTypeName == "vector<float>") type = 4;
        
        ofParameter<string> label;
        parameters->add(label.set(ofToString(type) + ". " + sourceTypeName + " to " + destTypeName + "_label", ""));
        parameters->add(source.set("Source", Tsource()));
        parameters->add(dest.set("Dest", Tdest()));
        
        source.addListener(this, &typeConverter::sourceListener);
        
        parametersControl::getInstance().createGuiFromParams(parameters, ofColor::yellow, pos);
    };
    ~typeConverter(){};
    
protected:
    void sourceListener(Tsource &s){
    };
    
    ofParameterGroup*    parameters;
    ofParameter<Tsource>    source;
    ofParameter<Tdest>      dest;
    
    string sourceTypeName;
    string destTypeName;
};



#endif /* typeConverter_h */

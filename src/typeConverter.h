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

template <typename Tsource,typename Tdest>
class typeConverter{
public:
    typeConverter(int id, ofPoint pos = ofPoint(-1, -1)){
        parameters = new ofParameterGroup();
        parameters->setName("typeConverter " + ofToString(id));
        
//        parameters->add(source.set("Source-" + ofToString(typeid(Tsource).name()), Tsource()));
//        parameters->add(dest.set("Dest-" + ofToString(typeid(Tdest).name()), Tdest()));
        
        parameters->add(source.set("Source", Tsource()));
        parameters->add(dest.set("Dest", Tdest()));
        
        source.addListener(this, &typeConverter::sourceListener);
        
        parametersControl::getInstance().createGuiFromParams(parameters, ofColor::yellow, pos);
    };
    ~typeConverter(){};
    
private:
    void sourceListener(Tsource &s){
        if(typeid(Tsource).name() == typeid(vector<float>).name() && typeid(Tdest).name() == typeid(vector<vector<float>>).name()){
            vector<vector<float>> toSend = {source};
            parameters->get("Dest").cast<Tdest>() = toSend;
        }
    }
    
    ofParameterGroup*    parameters;
    ofParameter<Tsource>    source;
    ofParameter<Tdest>      dest;
};

#endif /* typeConverter_h */

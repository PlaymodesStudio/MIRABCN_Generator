//
//  baseIndexer.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 09/01/2017.
//
//

#ifndef baseIndexer_h
#define baseIndexer_h

#include "ofMain.h"

class baseIndexer{
public:
    baseIndexer(int numIndexs);
    ~baseIndexer(){};
    
    ofParameter<float>  numWaves_Param; //Desphase Quantity
    ofParameter<bool>   invert_Param;
    ofParameter<int>    symmetry_Param;
    ofParameter<float>  indexRand_Param;
    ofParameter<int>    indexOffset_Param;
    ofParameter<int>    indexQuant_Param;
    ofParameter<float>  combination_Param;
    ofParameter<int>    modulo_Param;
    
protected:
    ofParameterGroup*   parameters;
    
    vector<float>       indexs;
    virtual void        newIndexs(){};
    
private:
    void parameterBoolListener(bool &b){recomputeIndexs();};
    void parameterFloatListener(float &f){recomputeIndexs();};
    void parameterIntListener(int &i){recomputeIndexs();};
    void recomputeIndexs();
    void indexRandChanged(float &val);
    
   
    
    int                 indexCount; //The max number you will get from index
    
    vector<int>         indexRand;
    int                 indexRand_Param_previous;
};

#endif /* baseIndexer_h */

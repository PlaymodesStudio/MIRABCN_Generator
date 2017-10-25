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
    ~baseIndexer(){
        delete parameters;
        if(reindexWindow != nullptr){
            reindexWindow->setWindowShouldClose();
        }
    };
    
    ofParameter<float>  numWaves_Param; //Desphase Quantity
    ofParameter<float>   indexInvert_Param;
    ofParameter<int>    symmetry_Param;
    ofParameter<float>  indexRand_Param;
    ofParameter<float>    indexOffset_Param;
    ofParameter<int>    indexQuant_Param;
    ofParameter<float>  combination_Param;
    ofParameter<int>    modulo_Param;
    ofParameter<bool>   manualReindex_Param;
    
    void draw(ofEventArgs &e);
    
    void mouseMoved(ofMouseEventArgs &a);
    void mousePressed(ofMouseEventArgs &a);
    void mouseReleased(ofMouseEventArgs &a);
    void mouseDragged(ofMouseEventArgs &a);
    
protected:
    ofParameterGroup*   parameters;
    
    vector<float>       indexs;
    virtual void        newIndexs(){};
    
    vector<vector<bool>> reindexGrid;
    
private:
    void drawManualReindex(bool &b);
    shared_ptr<ofAppBaseWindow> reindexWindow;
    ofRectangle                 reindexWindowRect;
    
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

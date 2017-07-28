//
//  baseIndexer.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 09/01/2017.
//
//

#include "baseIndexer.h"

baseIndexer::baseIndexer(int numIndexs){
    indexCount = numIndexs;
    indexs.resize(indexCount, 0);
    indexRand.resize(indexCount , 0);
    for(int i = 0; i < indexRand.size(); i++)
        indexRand[i] = i-((float)indexRand.size()/2.f);
    indexRand_Param_previous = 0;
    
    parameters = new ofParameterGroup;
    parameters->setName("Indexer");
    parameters->add(numWaves_Param.set("Num Waves", 1, 0, indexCount));
    parameters->add(indexInvert_Param.set("Index Invert", 0, 0, 1));
    parameters->add(symmetry_Param.set("Symmetry", 0, 0, 10));
    parameters->add(indexRand_Param.set("Index Random", 0, 0, 1));
    parameters->add(indexOffset_Param.set("Index Offset", 0, -indexCount/2, indexCount/2));
    parameters->add(indexQuant_Param.set("Index Quantization", indexCount, 1, indexCount));
    parameters->add(combination_Param.set("Index Combination", 0, 0, 1));
    parameters->add(modulo_Param.set("Index Modulo", indexCount, 1, indexCount));
    
    numWaves_Param.addListener(this, &baseIndexer::parameterFloatListener);
    indexInvert_Param.addListener(this, &baseIndexer::parameterFloatListener);
    symmetry_Param.addListener(this, &baseIndexer::parameterIntListener);
    indexRand_Param.addListener(this, &baseIndexer::parameterFloatListener);
    indexOffset_Param.addListener(this, &baseIndexer::parameterFloatListener);
    indexQuant_Param.addListener(this, &baseIndexer::parameterIntListener);
    combination_Param.addListener(this, &baseIndexer::parameterFloatListener);
    modulo_Param.addListener(this, &baseIndexer::parameterIntListener);
    
    indexRand_Param.addListener(this, &baseIndexer::indexRandChanged);

    
    recomputeIndexs();
}

void baseIndexer::recomputeIndexs(){
    for (int i = 0; i < indexCount ; i++){
        int index = i;
        
        //QUANTIZE
        int newNumOfPixels = indexQuant_Param;
        
        index = floor(index/((float)indexCount/(float)newNumOfPixels));
        
        
        while(symmetry_Param > newNumOfPixels-1)
            symmetry_Param--;
        
        bool odd = false;
        if((int)((index)/(newNumOfPixels/(symmetry_Param+1)))%2 == 1 ) odd = true;
        
        
        //SYMMETRY santi
        int veusSym = newNumOfPixels/(symmetry_Param+1);
        index = veusSym-abs((((int)(index/veusSym)%2) * veusSym)-(index%veusSym));
        
        
        if(newNumOfPixels % 2 == 0)
            index += odd ? 1 : 0;
        
        
        //INVERSE
        //Fisrt we invert the index to simulate the wave goes from left to right, inverting indexes, if we want to invertit we don't do this calc
        int nonInvertIndex = index-1;
        int invertedIndex = ((float)indexCount-(float)index);
        index = indexInvert_Param*invertedIndex + (1-indexInvert_Param)*nonInvertIndex;
        
        //random
        index += indexRand[index-1]*indexRand_Param;
        index %= indexCount;
        if(index < 0)
            index += indexCount;
        
        
        //COMB
        index = abs(((index%2)*indexCount*combination_Param)-index);
        
        //Modulo
        if(modulo_Param != modulo_Param.getMax())
            index %= modulo_Param;
        
        
        int shifted_i = i + round(indexOffset_Param);
        if(shifted_i < 0) shifted_i += indexCount;
        shifted_i %= indexCount;
        indexs[shifted_i] = (((float)index/(float)newNumOfPixels))*numWaves_Param;
    }
    newIndexs();
}

void baseIndexer::indexRandChanged(float &val){
    if(indexRand_Param_previous == 0)
        random_shuffle(indexRand.begin(), indexRand.end());
    indexRand_Param_previous = val;
}

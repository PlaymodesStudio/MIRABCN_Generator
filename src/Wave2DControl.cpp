//
//  Wave2DControl.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 28/09/16.
//
//

#include "Wave2DControl.h"

void Wave2DControl::setup(int _width, int _height){
//    parameters.setName("delay");
//    parameters.add(delay_frames.set("Delay", 1, 0, 40));
//    parameters.add(invert_Param.set("Invert Delay", false));
//    parameters.add(symmetry_Param.set("Symmetry", 0, 0, 10));
//    parameters.add(comb_Param.set("Combination", 0, 0, 1));
//    parameters.add(delay_sixteenth.set("Delay Sixteenth", 0, 0, 16));
//    grid.resize(height, vector<int> (width, 0));
    width = _width;
    height = _height;
    
    grid.resize(height);
    for( auto &row : grid)
        row.resize(width, 0);
    for(int i=0 ; i<height ; i++){
        for(int j=0; j<width; j++){
            pair<ofVec2f, float> tempPair;
            tempPair.first = ofVec2f(j, i);
            tempPair.second = float((width*i)+j) * float(255) / float(height*width);
            cout<<tempPair.second<<endl;
            barInfo_Pos.push_back(tempPair);
        }
    }
}


int Wave2DControl::computeFunc(int index){
    
//    //SYMMETRY santi
//    int veusSym = indexCount_Param/(symmetry_Param+1);
//    index = veusSym-abs((((int)(index/veusSym)%2) * veusSym)-(index%veusSym));
//    
//    index += symmetry_Param;
//    
//    //INVERSE
//    //Fisrt we invert the index to simulate the wave goes from left to right, inverting indexes, if we want to invertit we don't do this calc
//    if(!invert_Param)
//        index = ((float)indexCount_Param-(float)index+1);
//    
//    //QUANTIZE
//    //index = ceil(index/indexQuant_Param);
//    
//    //COMB
//    index = abs(((index%2)*indexCount_Param*comb_Param)-index);
//    
//    return index;
    
}


void Wave2DControl::applyDelayToTexture(ofFbo &fbo, vector<float> infoVec, float bpm, float phasor){
    
//    //Fill the buffer with the new info of the current frame
//    infoVecBuffer.push_front(infoVec);
//    
//    //Use the fbo to paint on it
//    fbo.begin();
//    
//    //first color to black, for fixing isues
//    int delay_frames_sixteenth = ((((float)delay_sixteenth/4)/bpm*60))*40;
//    cout<<delay_frames_sixteenth<< endl;
//    ofSetColor(0);
//    for(int j = 0 ; j < fbo.getHeight() ; j++){
//        //compute the index where there is the info we are interested
//        int delayIndex = (delay_frames+delay_frames_sixteenth)*(computeFunc(j));
//        
//        //If we want to acces a position that is not existing, get the last position
//        if(infoVecBuffer.size() <= delayIndex) delayIndex = 0;
//        
//        //Paint the square for each column
//        for (int i = 0; i < fbo.getWidth() ; i++){
//            ofSetColor(infoVecBuffer[delayIndex][i] * 255);
//            ofDrawRectangle(i, j, 1, 1);
//        }
//    }
//    fbo.end();
//    
//    
//    //If we have values that we will not use anymore, they are too far away, remove them
//    while(infoVecBuffer.size() > (delay_frames+delay_frames_sixteenth)*fbo.getHeight())
//        infoVecBuffer.pop_back();
    
    
    //Use the fbo to paint on it
    fbo.begin();
    ofSetColor(0);
    for(auto point : barInfo_Pos){
        //ofSetColor(point.second);
        float z = -cos(1*sqrt(pow(point.first.x-width/2,2)+pow(point.first.y-height/2+0.5,2))-(2*PI*phasor));
        ofSetColor(z*255);
        ofDrawRectangle(point.first.x, point.first.y, 1, 1);
    }
    fbo.end();
}
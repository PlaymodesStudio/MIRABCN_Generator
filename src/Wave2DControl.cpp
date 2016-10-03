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


vector<vector<float>> Wave2DControl::computeWave(ofFbo &waveTex, float phasor){
    //Use the fbo to paint on it
    waveTex.begin();
    ofSetColor(0);
    for(auto point : barInfo_Pos){
        float z= point.second/255;
        int t;
        //t = (2*PI*phasor);
        t = 0;
//        float z = -cos(3*sqrt(pow(point.first.x-width/2,2)+pow(point.first.y-height/2+0.5,2))-t);
        grid[point.first.y][point.first.x] = z;
        point.second = z;
        ofClamp(z, 0, 1);
        ofSetColor(z*255);
        ofDrawRectangle(point.first.x, point.first.y, 1, 1);
    }
    waveTex.end();
    return grid;
}

void Wave2DControl::computeOutTex(ofFbo &outTex, vector<float> infoVec, ofVec2f pos){
    outTex.begin();
    for(int i=0 ; i < barInfo_Pos.size() ; i++){
        auto &point = barInfo_Pos[i];
        if(point.first.x == pos.x && point.first.y == pos.y){
            for(int j = 0; j < infoVec.size(); j++){
                //TODO: Change the inversion type, forula and dropdown
                if(previewTex){
                    ofSetColor(ofClamp(infoVec[j]*255, 0, 255));
                    ofDrawRectangle(i,j, 1, 1);
                    ofSetColor(ofClamp((infoVec[infoVec.size()-j])*255, 0, 255));
                    ofDrawRectangle(i+barInfo_Pos.size(),j, 1, 1);
                }else{
                    ofSetColor(infoVec[j]*255);
                    ofDrawRectangle(2*i, j,1, 1);
                    ofSetColor((infoVec[infoVec.size()-j])*255);
                    ofDrawRectangle(2*i+1, j,1, 1);
                }
            }
        }
    }
    outTex.end();
}


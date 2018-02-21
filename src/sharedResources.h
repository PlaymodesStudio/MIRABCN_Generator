//
//  sharedResources.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#ifndef sharedResources_h
#define sharedResources_h

#include "ofMain.h"

class sharedResources{
public:
    
    sharedResources(){
        lastShaderTexturePositionUsed = -1;
    }
    ~sharedResources(){};
    
    static sharedResources &getInstance()
    {
        static sharedResources instance;
        return instance;
    }
    
    static void addDropdownToParameterGroupFromParameters(ofParameterGroup* parameterGroup, string name, vector<string> options, ofParameter<int> dropdownSelector){
        ofParameterGroup tempDropdown;
        tempDropdown.setName(name + " Select");
        string  tempStr;
        ofParameter<string> tempStrParam("Options");
        for(auto opt : options)
            tempStr += opt + "-|-";
        
        tempStr.erase(tempStr.end()-3, tempStr.end());
        tempStrParam.set(tempStr);
        
        tempDropdown.add(tempStrParam);
        tempDropdown.add(dropdownSelector.set(name +" Select", 0, 0, options.size()));
        parameterGroup->add(tempDropdown);
    }
    
    int getNextAvailableShaderTextureLocation(){
        return ++lastShaderTexturePositionUsed;
    }
    
private:
    uint lastShaderTexturePositionUsed = -1;
};




#endif /* sharedResources_h */

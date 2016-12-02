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
    
//    void createDropdownAndStringInput(string name, vector<string> options, ofParameter<string> textInput, ofParameter<int> dropDownSelector){
//        ofParameterGroup tempDropdown;
//        tempDropdown.setName(name + " Select");
//        string  tempStr;
//        ofParameter<string> tempStrParam("Options");
//        for(auto opt : options)
//            tempStr += opt + "-|-";
//        
//        tempStr.erase(tempStr.end()-3, tempStr.end());
//        tempStrParam.set(tempStr);
//        
//        
//        tempDropdown.add(tempStrParam);
//        tempDropdown.add(dropDownSelector.set(name +" Select", 0, 0, options.size()));
//        parameters.add(tempDropdown);
//        ofParameter<string> label("INSERT "+name+"_label", " ");
//        parameters.add(label);
//        parameters.add(textInput.set(name, options[0]));
//    }
    
    static void addDropdownToParameterGroupFromParameters(ofParameterGroup &parameterGroup, string name, vector<string> options, ofParameter<int> dropdownSelector){
        ofParameterGroup tempDropdown;
        parameterGroup.setName(name + " Select");
        string  tempStr;
        ofParameter<string> tempStrParam("Options");
        for(auto opt : options)
            tempStr += opt + "-|-";
        
        tempStr.erase(tempStr.end()-3, tempStr.end());
        tempStrParam.set(tempStr);
        
        tempDropdown.add(tempStrParam);
        tempDropdown.add(dropdownSelector.set(name +" Select", 0, 0, options.size()));
        parameterGroup.add(tempDropdown);
    }
};




#endif /* sharedResources_h */

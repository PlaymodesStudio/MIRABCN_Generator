//
//  colorApplier.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#include "colorApplier.h"
#include "sharedResources.h"

colorApplier::colorApplier(){
    
    parameters.setName("colorApplier 1");
    parameters.add(colorPickerParam[0].set("Color 1 Picker", ofColor::white));
    parameters.add(colorRParam[0].set("Color 1 R", 1, 0, 1));
    parameters.add(colorGParam[0].set("Color 1 G", 1, 0, 1));
    parameters.add(colorBParam[0].set("Color 1 B", 1, 0, 1));

    parameters.add(colorPickerParam[1].set("Color 2 Picker", ofColor::white));
    parameters.add(colorRParam[1].set("Color 2 R", 1, 0, 1));
    parameters.add(colorGParam[1].set("Color 2 G", 1, 0, 1));
    parameters.add(colorBParam[1].set("Color 2 B", 1, 0, 1));
    
    parameters.add(randomColorStepsParam.set("Rnd Color Steps", 4, 0, 255));
    sharedResources::addDropdownToParameterGroupFromParameters(parameters, "Rnd ChangeTypes", {"no", "on presset", "onTrigger"}, randomizeTypeColorParam);
    
//    parameters.add(indexs.set("Indexs", {0}));
//    parameters.add(colorizedValues.set("Output", {{ofColor::white}}));
    parametersControl::getInstance().createGuiFromParams(parameters);
}


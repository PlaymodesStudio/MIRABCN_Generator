//
//  expressionOperator.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 13/07/2017.
//
//

#ifndef expressionOperator_h
#define expressionOperator_h

#include "ofMain.h"
#include "ofxPython.h"
#include "parametersControl.h"

class abstractExpressionOperator{
public:
    abstractExpressionOperator(){};
    ~abstractExpressionOperator(){};
};

template <typename T>
class expressionOperator : public abstractExpressionOperator{
public:
    expressionOperator(int _id, int numInputs = 3, ofPoint pos = ofPoint(-1, -1)){
        abstractExpressionOperator();
        python.init();
        parameters = new ofParameterGroup();
        parameters->setName("expressionOperator " + ofToString(_id));
        
        parameters->add(expression.set("Expression_paragraph", "import math\nfrom openframeworks import * \nprint \"LOL\" \ndef compute(x1, x2, x3):\n    return x1+x2+x3"));
        inputs.resize(numInputs);
        floatInputs.resize(numInputs, 0);
        for(int i = 0; i< numInputs; i++){
            parameters->add(inputs[i].set("Input " + ofToString(i), T()));
//            expression_parser.addSymbol("x" + ofToString(i+1), floatInputs[i]);
        }
        parameters->add(output.set("Output", T()));
        
        expression.addListener(this, &expressionOperator::expressionChanged);
        inputs[0].addListener(this, &expressionOperator::inputReceived);
        
        parametersControl::getInstance().createGuiFromParams(parameters, ofColor::white, pos);
        
        
        python.executeString(expression.get());
    }
    
    
private:
    void inputReceived(T &val){
        int maxSize = 0;
        int maxIndexSize = 0;
        for(int i = 0; i < inputs.size(); i++){
            if(inputs[i].get().size() > maxSize){
                maxSize = inputs[i].get().size();
                maxIndexSize = i;
            }
        }
        vector<float> tempOut;
        tempOut.resize(maxSize);
        for(int i = 0; i < maxSize; i++){
            for(int j = 0; j < inputs.size(); j++){
                if(inputs[j].get().size() > i)
                    floatInputs[j] = inputs[j].get()[i];
                else
                    floatInputs[j] = 0;
            }
            auto result = python.getObject("compute")(ofxPythonObject::fromFloat(floatInputs[0]), ofxPythonObject::fromFloat(floatInputs[1]), ofxPythonObject::fromFloat(floatInputs[2]));
            tempOut[i] = ofToFloat(result.repr());
        }
        
        parameters->get("Output").cast<vector<float>>() = tempOut;
    }
    
    void expressionChanged(string &s){
            python.executeString(s);
    }
    
    ofParameterGroup *parameters;
    ofParameter<string> expression;
    vector<ofParameter<T>> inputs;
    ofParameter<T> output;
    
    //Variables for the function expression evaluator
    ofxPython python;
    vector<float>   floatInputs;
};

#endif /* expressionOperator_h */

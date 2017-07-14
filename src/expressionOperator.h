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
//#include "ofxExprtk.h"
#include "parametersControl.h"

class abstractExpressionOperator{
public:
    abstractExpressionOperator(){};
    ~abstractExpressionOperator(){};
};

template <typename T>
class expressionOperator : public abstractExpressionOperator{
public:
    expressionOperator(int _id, int numInputs, ofPoint pos = ofPoint(-1, -1)){
        abstractExpressionOperator();
        parameters = new ofParameterGroup();
        parameters->setName("expressionOperator " + ofToString(_id));
        
        parameters->add(expression.set("Expression", "x1"));
        inputs.resize(numInputs);
        floatInputs.resize(numInputs);
        for(int i = 0; i< numInputs; i++){
            parameters->add(inputs[i].set("Input " + ofToString(i), T()));
//            expression_parser.addSymbol("x" + ofToString(i+1), floatInputs[i]);
        }
        parameters->add(output.set("Output", T()));
        
        expression.addListener(this, &expressionOperator::expressionChanged);
        inputs[0].addListener(this, &expressionOperator::inputReceived);
        
        parametersControl::getInstance().createGuiFromParams(parameters, ofColor::white, pos);
        
//        expression_parser.registerSymbols();
//        expression_parser.compileExpression(expression);
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
            
//            tempOut[i] = expression_parser.evaluateExpression();
        }
        
        parameters->get("Output").cast<vector<float>>() = tempOut;
    }
    
    void expressionChanged(string &s){
//        if(!expression_parser.compileExpression(s)) ofLog() << "Can't Compile Expression: " << s;
    }
    
    ofParameterGroup *parameters;
    ofParameter<string> expression;
    vector<ofParameter<T>> inputs;
    ofParameter<T> output;
    
    //Variables for the function expression evaluator
//    ofxExprtk<float> expression_parser;
    vector<float>   floatInputs;
};

#endif /* expressionOperator_h */

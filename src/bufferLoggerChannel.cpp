//
//  bufferLoggerChannel.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 25/10/16.
//
//

#include "bufferLoggerChannel.h"

void bufferLoggerChannel::log(ofLogLevel level, const string &module, const string &message){
    messagesBuffer.push_back(message);
}

void bufferLoggerChannel::log(ofLogLevel level, const string &module, const char *format, ...){
    
}

void bufferLoggerChannel::log(ofLogLevel level, const string &module, const char *format, va_list args){
    
}

string bufferLoggerChannel::getLine(int i){
    if(i < messagesBuffer.size())
        return messagesBuffer[i];
    else
        return "";
}

void bufferLoggerChannel::eraseLastLine(){
    messagesBuffer.pop_front();
}

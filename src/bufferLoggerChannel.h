//
//  bufferLoggerChannel.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 25/10/16.
//
//

#ifndef bufferLoggerChannel_h
#define bufferLoggerChannel_h

#include "ofMain.h"

class bufferLoggerChannel : public ofBaseLoggerChannel{
public:
    /// \brief Destroy the channel.
    virtual ~bufferLoggerChannel(){};
    
    /// \brief Log a message.
    /// \param level The log level.
    /// \param module The target module.
    /// \param message The log message.
    virtual void log(ofLogLevel level, const string & module, const string & message);
    
    /// \brief Log a message.
    /// \param level The log level.
    /// \param module The target module.
    /// \param format The printf-style format string.
    virtual void log(ofLogLevel level, const string & module, const char* format, ...);
    
    /// \brief Log a message.
    /// \param level The log level.
    /// \param module The target module.
    /// \param format The printf-style format string.
    /// \param args the list of printf-style arguments.
    virtual void log(ofLogLevel level, const string & module, const char* format, va_list args);
    
    string getLine(int i);
    int getSize(){return messagesBuffer.size();};
    
    void eraseLastLine();
    
private:
    deque<string>  messagesBuffer;
};


#endif /* bufferLoggerChannel_h */

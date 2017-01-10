//
//  oscillatorBank.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 09/01/2017.
//
//

#ifndef oscillatorBank_h
#define oscillatorBank_h

#include "baseIndexer.h"

//This class will contain a set of oscillators and has to inherit the indexer class (or bank class)
class oscillatorBank : public baseIndexer{
public:
    oscillatorBank(int nOscillators);
    ~oscillatorBank(){};
private:
    
};

#endif /* oscillatorBank_h */

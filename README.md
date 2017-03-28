# MIRABCN_Generator

Evolved from [DGTL_Generator](https://github.com/PlaymodesStudio/DGTL_Generator), it is a multi oscillator to create lighting installations. Now it has become like a node editor. Work in progress...
You need at least openframeworks 0.9

## Addons
- [ofxDatGui_PM (redoEvents branch)](https://github.com/PlaymodesStudio/ofxDatGui_PM/tree/redoEvents)
- [ofxExprtk, (Playmodes fork)](https://github.com/PlaymodesStudio/ofxExprtk)
- [ofxMidi (included)]()
- [ofxOsc (included)]()
- [ofxSyphon](https://github.com/astellato/ofxSyphon)
- [ofxCurvesTool](https://github.com/charlesveasey/ofxCurvesTool)
- [ofxAubio](https://github.com/aubio/ofxAubio)
- [ofxTweenzor](https://github.com/NickHardeman/ofxTweenzor)


### Preview Window explanation
![Alt text](prevWindow.png?raw=true "Preview Window")

1. __OutTexture__, the texture that has the information that will be sent to pixels
2. __2dWave Grid__, it is the modulator wave represented in 2d, useful when using functions, mainly not used
3. __Oscillator Banks__, is a grup of scopes for the banks of oscillators
4. __Info__, Information exposed to user, driven by ofLogNotices()


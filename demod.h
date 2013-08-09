#pragma once
//GPL license and attributions are in gpl.h and terms are included in this file by reference
#include "gpl.h"
#include "global.h"

#include "QObject"
#include "signalprocessing.h"
#include <iostream>
#include <QString>
#include "filters/fir.h"
#include "filters/iir.h"
#include "demod/wfmdemod.h"
#include "demod/rdsdecode.h"

enum DEMODMODE {
    dmAM,
    dmSAM,
    dmFMN,
    dmFMMono,
    dmFMStereo,
    dmDSB,
    dmLSB,
    dmUSB,
    dmCWL,
    dmCWU,
    dmDIGL,
    dmDIGU,
	dmNONE
};

class Demod : public SignalProcessing
{
    //Note:  If moc complier is not called, delete Makefile so it can be regenerated
    Q_OBJECT

public:
    Demod(int _inputRate, int _inputWfmRate, int size);
	~Demod();

    CPX * ProcessBlock(CPX * in, int _numSamples);
    DEMODMODE DemodMode() const;
    void ResetDemod(); //Resets all data decoders, called after frequency change from receiver
    void SetDemodMode(DEMODMODE mode, int _sourceSampleRate, int _audioSampleRate);
	static DEMODMODE StringToMode(QString m);
	static QString ModeToString(DEMODMODE dm);

signals:
    //We can't call receiver->OutputData directly because we're running in a different thread
    //Signals and Slots solve the problem
    void OutputData(const char *);

private:
    DEMODMODE mode;
    
    //Testing
    CWFmDemod *wfmDemod;
    CRdsDecode rdsDecode;
    char rdsBuf[256]; //Formatted RDS string for output
    char rdsCallString[10];
    char rdsString[128]; //Max RDS string
    bool rdsUpdate; //true if we need to update display


    void FMDeemphasisFilter(int _bufSize, CPX *in, CPX *out);
    float fmDeemphasisAlpha;
    static const float usDeemphasisTime; //Use for US & Korea FM
    static const float intlDeemphasisTime;  //Use for international FM

    //CFir fmMonoLPFilter;
    CIir fmMonoLPFilter;
    CFir fmAudioLPFilter;
    CIir fmPilotNotchFilter;
    CIir fmPilotBPFilter;
    CFir hilbertFilter;

    //Used for FMW only for now
    int inputSampleRate; //Original signal coming in
    int inputWfmSampleRate; //Original signal coming in for Wfm (wider signal)

    int audioSampleRate; //Audio out

    float pllPhase;
    float samLockCurrent;
    float samLockPrevious;
    float samDc;
    float fmDCOffset;
	//Previous I/Q values, used in simpleFM
	float fmIPrev;
	float fmQPrev;
    
	//PLL variables
    float pllFrequency;
	float alpha,beta;

	//Moving averages for smooth am detector
	float amDc;
	float amSmooth;

    
	//SAM config
	float samLoLimit; //PLL range
	float samHiLimit;
	float samBandwidth;

	//FMN config
	float fmLoLimit;
	float fmHiLimit;
	float fmBandwidth;

    CPX PLL(CPX sig, float loLimit, float hiLimit, int _numSamples);

    void PllSAM(CPX * in, CPX * out, int _numSamples);
    void PllFMN(CPX * in, CPX * out, int _numSamples);
    void FMMono(CPX * in, CPX * out, int _numSamples);
    void FMStereo(CPX * in, CPX * out, int _numSamples);

	//Simple time domain demod algorithms for testing and comparison
    void SimpleAM(CPX *in, CPX *out, int _numSamples);
    void SimpleAMSmooth(CPX *in, CPX *out, int _numSamples);
    void SimpleUSB(CPX *in, CPX *out, int _numSamples);
    void SimpleLSB(CPX *in, CPX *out, int _numSamples);
    void SimplePhase(CPX *in, CPX *out, int _numSamples);
    void SimpleFM(CPX *in, CPX *out, int _numSamples);
    void SimpleFM2(CPX *in, CPX *out, int _numSamples);

};

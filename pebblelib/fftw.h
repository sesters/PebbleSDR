#ifndef FFTW_H
#define FFTW_H
//GPL license and attributions are in gpl.h and terms are included in this file by reference
#include "gpl.h"

#include "fft.h"
#include "cpx.h"
#include "../fftw-3.3.5/api/fftw3.h"

class PEBBLELIBSHARED_EXPORT FFTfftw : public FFT
{
public:
    FFTfftw();
    ~FFTfftw();
	void fftParams(quint32 _fftSize, double _dBCompensation, double _sampleRate, int _samplesPerBuffer,
				   WindowFunction::WINDOWTYPE _windowType);
	void fftForward(CPX * in, CPX * out, int numSamples);
	void fftInverse(CPX * in, CPX * out, int numSamples);
	bool fftSpectrum(CPX *in, double *out, int numSamples);

private:
    fftw_plan plan_fwd;
    fftw_plan plan_rev;
    CPX *buf;
    int half_sz;
};


#endif // FFTW_H

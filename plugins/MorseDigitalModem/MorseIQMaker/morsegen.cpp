#include "morsegen.h"
#include "db.h"

//Todo: add variability to amplitude to simulate fading
//Todo: add variability to wpm to simulate real fists
//Todo: add noise option
//Todo: add timed tuning option before output
//Todo: add random non-morse option
MorseGen::MorseGen(double sampleRate, double frequency, double dbAmplitude, quint32 wpm)
{
	m_sampleRate = sampleRate;
	m_frequency = frequency;
	m_dbAmplitude = dbAmplitude;
	m_amplitude = DB::dBToAmplitude(dbAmplitude);
	m_wpm = wpm;
	m_msRiseFall = 10; //10ms attack/decay, 5ms ARRL recomendation, 20ms max

	//Calculate size of output elements
	quint32 msTcw = MorseCode::wpmToTcwMs(wpm);
	//How many samples do we need for each tcw
	quint32 samplesPerTcw = msTcw / (1000.0/sampleRate);

	//Samples for dot and dash have to include 50% of the rise and fall time
	//So are longer than just based on Tcw
	m_numSamplesRise = m_msRiseFall / (1000/m_sampleRate);
	m_numSamplesFall = m_msRiseFall / (1000/m_sampleRate);
	//Adjustment to full Tcw samples to account for 50% rise and fall time
	quint32 tcwRiseFall = (m_numSamplesRise + m_numSamplesFall) / 2;
	m_numSamplesDot = samplesPerTcw * MorseCode::c_tcwDot - tcwRiseFall;
	//Full buffer size, including rise and fall
	m_numSamplesDotBuf = m_numSamplesRise + m_numSamplesFall + m_numSamplesDot;
	m_dotSampleBuf = CPX::memalign(m_numSamplesDotBuf);

	m_numSamplesDash = samplesPerTcw * MorseCode::c_tcwDash - tcwRiseFall;
	m_numSamplesDashBuf =  m_numSamplesRise + m_numSamplesFall + m_numSamplesDash;
	m_dashSampleBuf = CPX::memalign(m_numSamplesDashBuf);

	m_numSamplesElementBuf = samplesPerTcw * MorseCode::c_tcwElementSpace;
	m_elementSampleBuf = CPX::memalign(m_numSamplesElementBuf);

	m_numSamplesCharBuf = samplesPerTcw * MorseCode::c_tcwCharSpace;
	m_charSampleBuf = CPX::memalign(m_numSamplesCharBuf);

	//A word space already has a character space (3 Tcws)
	//So adjust accordingly
	m_numSamplesWordBuf = samplesPerTcw * MorseCode::c_tcwWordSpace - 3;
	m_wordSampleBuf = CPX::memalign(m_numSamplesWordBuf);

	//Build IQ data for each element
	double phase = TWOPI * m_frequency / m_sampleRate;
	double acc = 0;

	//Dot samples
	//Ramp amplitude during rise
	double ampInc = m_amplitude / m_numSamplesRise;
	double amplitude = ampInc;
	quint32 outIndex = 0;
	for (quint32 i=0; i<m_numSamplesRise; i++, outIndex++) {
		m_dotSampleBuf[outIndex].re = cos(acc) * amplitude;
		m_dotSampleBuf[outIndex].im = sin(acc) * amplitude;
		amplitude += ampInc;
		acc += phase;
	}
	//Steady signal
	for (quint32 i=0; i< m_numSamplesDot; i++, outIndex++) {
		m_dotSampleBuf[outIndex].re = cos(acc) * m_amplitude;
		m_dotSampleBuf[outIndex].im = sin(acc) * m_amplitude;
		acc += phase;
	}
	//Decline amplitude during fall
	amplitude = m_amplitude - ampInc;
	for (quint32 i=0; i< m_numSamplesFall; i++, outIndex++) {
		m_dotSampleBuf[outIndex].re = cos(acc) * amplitude;
		m_dotSampleBuf[outIndex].im = sin(acc) * amplitude;
		amplitude -= ampInc;
		acc += phase;
	}

	acc = 0;
	ampInc = m_amplitude / m_numSamplesFall;
	amplitude = ampInc;
	outIndex = 0;
	for (quint32 i=0; i<m_numSamplesRise; i++, outIndex++) {
		m_dashSampleBuf[outIndex].re = cos(acc) * amplitude;
		m_dashSampleBuf[outIndex].im = sin(acc) * amplitude;
		amplitude += ampInc;
		acc += phase;
	}
	//Steady signal
	for (quint32 i=0; i< m_numSamplesDash; i++, outIndex++) {
		m_dashSampleBuf[outIndex].re = cos(acc) * m_amplitude;
		m_dashSampleBuf[outIndex].im = sin(acc) * m_amplitude;
		acc += phase;
	}
	//Decline amplitude during fall
	amplitude = m_amplitude - ampInc;
	for (quint32 i= 0; i< m_numSamplesFall; i++, outIndex++) {
		m_dashSampleBuf[outIndex].re = cos(acc) * amplitude;
		m_dashSampleBuf[outIndex].im = sin(acc) * amplitude;
		amplitude -= ampInc;
		acc += phase;
	}

	for (quint32 i=0; i<m_numSamplesElementBuf; i++) {
		m_elementSampleBuf[i].re = 0;
		m_elementSampleBuf[i].im = 0;
	}
	for (quint32 i=0; i<m_numSamplesCharBuf; i++) {
		m_charSampleBuf[i].re = 0;
		m_charSampleBuf[i].im = 0;
	}
	for (quint32 i=0; i<m_numSamplesWordBuf; i++) {
		m_wordSampleBuf[i].re = 0;
		m_wordSampleBuf[i].im = 0;
	}

	m_lastSymbol = MorseCode::WORD_SPACE;
}

quint32 MorseGen::genText(CPX *out, quint8 text)
{
	if (text == ' ') {
		return genWord(out);
	}
	//Look up character
	quint32 token = m_morseCode.txLookup(text);
	if (token > 0)
		return genToken(out, token);
	return 0;
}

quint32 MorseGen::genToken(CPX *out, quint8 token)
{
	quint32 len = 0;
	quint8 bit;
	bool hasHighBit = false;
	//Shift out each bit off token and ouput
	if (token > 0) {
		for (int i=0; i<8; i++) {
			bit = token & 0x80; //Check high order bit
			if (!hasHighBit && bit == 0) {
				//Looking for high order start bit
				token = token << 1;
				continue;
			} else if (!hasHighBit && bit > 0) {
				token = token << 1;
				hasHighBit = true;
				continue;
			}
			//Found high order start bit, process rest of bits
			if (bit == 0) {
				len += genDot(out+len); //Handles genElement()
			} else {
				len += genDash(out+len); //Handles genElement()
			}
			token = token << 1;
		}
		len += genChar(out+len);
		return len;
	}
	return 0;
}

quint32 MorseGen::genDot(CPX *out)
{
	quint32 len = 0;
	if (m_lastSymbol==MorseCode::DOT || m_lastSymbol == MorseCode::DASH) {
		//Output element space
		len = genElement(out);
	}
	for (quint32 i=0; i<m_numSamplesDotBuf; i++) {
		out[len+i].re = m_dotSampleBuf[i].re;
		out[len+i].im = m_dotSampleBuf[i].im;
	}
	m_lastSymbol = MorseCode::DOT;
	return len + m_numSamplesDotBuf;
}

quint32 MorseGen::genDash(CPX *out)
{
	quint32 len = 0;
	if (m_lastSymbol==MorseCode::DOT || m_lastSymbol == MorseCode::DASH) {
		//Output element space
		len = genElement(out);
	}
	for (quint32 i=0; i<m_numSamplesDashBuf; i++) {
		out[len+i].re = m_dashSampleBuf[i].re;
		out[len+i].im = m_dashSampleBuf[i].im;
	}
	m_lastSymbol = MorseCode::DASH;
	return len + m_numSamplesDashBuf;
}

quint32 MorseGen::genElement(CPX *out)
{
	for (quint32 i=0; i<m_numSamplesElementBuf; i++) {
		out[i].re = m_elementSampleBuf[i].re;
		out[i].im = m_elementSampleBuf[i].im;
	}
	m_lastSymbol = MorseCode::EL_SPACE;
	return m_numSamplesElementBuf;
}

quint32 MorseGen::genChar(CPX *out)
{
	for (quint32 i=0; i<m_numSamplesCharBuf; i++) {
		out[i].re = m_charSampleBuf[i].re;
		out[i].im = m_charSampleBuf[i].im;
	}
	m_lastSymbol = MorseCode::CH_SPACE;
	return m_numSamplesCharBuf;
}

quint32 MorseGen::genWord(CPX *out)
{
	for (quint32 i=0; i<m_numSamplesWordBuf; i++) {
		out[i].re = m_wordSampleBuf[i].re;
		out[i].im = m_wordSampleBuf[i].im;
	}
	m_lastSymbol = MorseCode::WORD_SPACE;
	return m_numSamplesWordBuf;

}

quint32 MorseGen::genTable(CPX *out, quint32 index)
{
	MorseSymbol *symbol;
	symbol = m_morseCode.tableLookup(index);
	if (symbol != NULL) {
		//qDebug()<<index<<" "<<symbol->chr<<" "<<symbol->display<<" "<<symbol->dotDash<<" "<<symbol->token;
		return genToken(out, symbol->token);
	}
	return 0;
}
#ifndef GHPSDR3DEVICE_H
#define GHPSDR3DEVICE_H

//GPL license and attributions are in gpl.h and terms are included in this file by reference
//QTRadio source fragments used in many places as reference for protocol

#include "gpl.h"
#include <QObject>
#include "deviceinterfacebase.h"
#include <QHostAddress>
#include <QTcpSocket>
//#include <QUdpSocket>
#include <QTimer>
#include "alawcompression.h"
#include "servers.h"

#if 0
On line servers can be found at http://qtradio.napan.ca/qtradio/qtradio.pl
74.85.89.174 is a Hermes transceiver that is good for testing
71.47.206.230 is SDR-IQ

WARNING: There are at least 2 different prototocols I found in various source code repositories

Codec2 from David Rowe at http://www.rowetel.com/blog/?page_id=452
https://svn.code.sf.net/p/freetel/code/codec2/
http://openhpsdr.org/wiki/index.php?title=Ghpsdr3_protocols

Documentation may not be up to date, look at client.c readcb() in ghpsdr3 source
	'!' Documented

	First byte == '*' signals hardware directed command
	First byte == 'q-' signals a query that returns data in ANSWER packet type

	"mic" Incoming microphone data
	! "getspectrum"
	! "setfrequency"
	"setpreamp"
	"setmode"
	"setfilter"
	"setagc"
	"setfixedagc"
	"enablenotchfilter"
	"setnotchfilter"
	"setagctlevel"
	"setrxgreqcmd"
	"setrx3bdgreq"
	"setrx10bdgreq"
	"settxgreqcmd"
	"settx3bdgreq"
	"settx10bdgreq"
	"setnr"
	"setnb"
	"setsdrom"
	"setanf"
	"setrxoutputgain"
	"setsubrxoutputgain"
	! "startaudiostream" <bufferSize> <sampleRate> <audioChannel> <micEncoding>
	! "startrtpstream" <rtpPort> <encoding> <rate> <channels>
	"stopaudiostream"
	"setencoding"
	"setsubrx"
	"setsubrxfrequency"
	"setpan"
	"setsubrxpan"
	"record"
	"setanfvals"
	"setnrvals"
	"setrxagcattack"
	"setrxagcdecay"
	"setrxagchang"
	"setrxagchanglevel"
	"setrxagchangthreshold"
	"setrxagcthresh"
	"setrxagcmaxgain"
	"setrxagcslope"
	"setnbvals"
	"setsdromvals"
	"setpwsmode"
	"setbin"
	"settxcompst"
	"settxcompvalue"
	"setoscphase"
	"setPanaMode"
	"setRxMeterMode"
	"setTxMeterMode"
	"setrxdcblockgain"
	"setrxdcblock"
	"settxdcblock"
	"mox"
	"off"
	"settxamcarrierlevel"
	"settxalcstate"
	"settxalcattack"
	"settxalcdecay"
	"settxalcbot"
	"settxalchang"
	"settxlevelerstate"
	"settxlevelerattack"
	"settxlevelerdecay"
	"settxlevelerhang"
	"settxlevelermaxgain"
	"settxagcff"
	"settxagcffcompression"
	"setcorrecttxiqmu"
	"setcorrecttxiqw"
	"setfadelevel"
	"setsquelchval"
	"setsubrxquelchval"
	"setsquelchstate"
	"setsubrxquelchstate"
	"setspectrumpolyphase"
	"setocoutputs"
	"setclient"
	"setiqenable"
	"testbutton"
	"testslider"
	"rxiqmuval"
	"txiqcorrectval"
	"txiqphasecorrectval"
	"txiqgaincorrectval"
	"rxiqphasecorrectval"
	"rxiqgaincorrectval"
	"rxiqcorrectwr"
	"rxiqcorrectwi"
	! "setfps" Spectrum fps
	"zoom"
	"setmaster"

  QTRadio Command Sequence
  Debug: Connection::sendCommand:  "SetNRVals 64 8 0.00064 1e-06"
  Debug: Connection::sendCommand:  "SetNRVals 64 8 0.00016 1e-06"
  Debug: Connection::sendCommand:  "SetANFVals 64 8 0.00064 1e-07"
  Debug: Connection::sendCommand:  "SetANFVals 64 8 0.00032 1e-07"
  Debug: Connection::sendCommand:  "SetIQEnable false"
  Debug: Connection::sendCommand:  "RxIQmuVal 0.25"
  Debug: Connection::sendCommand:  "SetIQEnable true"
  Debug: Connection::sendCommand:  "SetRXOutputGain 30"
  Debug: Connection::sendCommand:  "SetSubRXOutputGain 30"
  Debug: Connection::sendCommand:  "setrx10bdgreq 0 0 0 0 0 0 0 0 0 0 0"
  Debug: Connection::sendCommand:  "settx10bdgreq 0 0 0 0 0 0 0 0 0 0 0"
  Debug: Connection::sendCommand:  "setpwsmode 0"
  Debug: Connection::sendCommand:  "setFilter -3450 -150"
  Debug: Connection::sendCommand:  "setMode 0"
  Debug: Connection::sendCommand:  "setFrequency 1134900"

  Debug: Connection::connect: connectToHost:  "71.47.206.230" : 8000

  Debug: Connection::sendCommand:  "SetIQEnable false"
  Debug: Connection::sendCommand:  "RxIQmuVal 0.25"
  Debug: Connection::sendCommand:  "SetIQEnable true"
  Debug: Connection::Connected true
  Debug: Connection::sendCommand:  "setClient QtRadio"
  Debug: Connection::sendCommand:  "q-server"
  Debug: Connection::sendCommand:  "setFrequency 1134900"
  Debug: Connection::sendCommand:  "setMode 0"
  Debug: Connection::sendCommand:  "setFilter -3450 -150"
  Debug: Connection::sendCommand:  "setpwsmode 0"
  Debug: Connection::sendCommand:  "setEncoding 0"
  Debug: Connection::sendCommand:  "SetRXOutputGain 30"
  Debug: Connection::sendCommand:  "SetSubRXOutputGain 30"

  Debug: Connection::sendCommand:  "startAudioStream 800 8000 1 0"

  Debug: Connection::sendCommand:  "SetPan 0.5"
  Debug: Connection::sendCommand:  "SetAGC 2"
  Debug: Connection::sendCommand:  "setpwsmode 0"
  Debug: Connection::sendCommand:  "SetANFVals 64 8 0.00032 1e-07"
  Debug: Connection::sendCommand:  "SetNRVals 64 8 0.00016 1e-06"
  Debug: Connection::sendCommand:  "SetNBVals 3.3"
  Debug: Connection::sendCommand:  "SetSquelchVal -100"
  Debug: Connection::sendCommand:  "SetSquelchState off"
  Debug: Connection::sendCommand:  "SetANF false"
  Debug: Connection::sendCommand:  "SetNR false"
  Debug: Connection::sendCommand:  "SetNB false"

  Debug: Sending advanced setup commands.
  Debug: Connection::sendCommand:  "setrxdcblock 0"
  Debug: Connection::sendCommand:  "settxdcblock 0"
  Debug: Connection::sendCommand:  "setrxagcmaxgain 75"
  Debug: Connection::sendCommand:  "setrxagcattack 2"
  Debug: Connection::sendCommand:  "setrxagcdecay 250"
  Debug: Connection::sendCommand:  "setrxagchang 250"
  Debug: Connection::sendCommand:  "setfixedagc 22"
  Debug: Connection::sendCommand:  "setrxagchangthreshold 0"
  Debug: Connection::sendCommand:  "settxlevelerstate 0"
  Debug: Connection::sendCommand:  "settxlevelermaxgain 5"
  Debug: Connection::sendCommand:  "settxlevelerattack 2"
  Debug: Connection::sendCommand:  "settxlevelerdecay 500"
  Debug: Connection::sendCommand:  "settxlevelerhang 500"
  Debug: Connection::sendCommand:  "setalcstate 0"
  Debug: Connection::sendCommand:  "settxalcattack 2"
  Debug: Connection::sendCommand:  "settxalcdecay 10"
  Debug: Connection::sendCommand:  "settxalchang 500"
  Debug: Connection::sendCommand:  "*hardware?"
  Debug: Connection::sendCommand:  "enablenotchfilter 0 0 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 1 0 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 0 1 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 1 1 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 0 2 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 1 2 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 0 3 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 1 3 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 0 4 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 1 4 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 0 5 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 1 5 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 0 6 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 1 6 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 0 7 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 1 7 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 0 8 0"
  Debug: Connection::sendCommand:  "enablenotchfilter 1 8 0"
  Debug: Connection::sendCommand:  "q-version"
  Debug: Connection::sendCommand:  "q-loffset"
  Debug: Connection::sendCommand:  "q-protocol3"

  Debug: Connection::sendCommand:  "getSpectrum 964"
  Debug: Connection::sendCommand:  "getSpectrum 964"
  Debug: Connection::sendCommand:  "q-master"
  Debug: Connection::sendCommand:  "setFPS 964 15"
  Debug: Connection::sendCommand:  "q-cantx#None"
  Debug: Connection::sendCommand:  "q-master"
  Debug: Connection::sendCommand:  "q-info"
  Debug: Connection::sendCommand:  "setFrequency 18143000"
  Debug: Connection::sendCommand:  "setFilter -3450 -150"
  Debug: Connection::sendCommand:  "setMode 0"
  Debug: Connection::sendCommand:  "setFrequency 18143000"




#endif

class Ghpsdr3Device : public QObject, public DeviceInterfaceBase
{
	Q_OBJECT

	//Exports, FILE is optional
	//IID must be same that caller is looking for, defined in interfaces file
	Q_PLUGIN_METADATA(IID DeviceInterface_iid)
	//Let Qt meta-object know about our interface
	Q_INTERFACES(DeviceInterface)

public:
	friend class Servers;

	Ghpsdr3Device();
	~Ghpsdr3Device();

	//Required
	bool initialize(CB_ProcessIQData _callback,
					CB_ProcessBandscopeData _callbackBandscope,
					CB_ProcessAudioData _callbackAudio,
					quint16 _framesPerBuffer);
	QVariant get(StandardKeys _key, QVariant _option = 0);
	bool set(StandardKeys _key, QVariant _value, QVariant _option = 0);

signals:
	void CheckNewData();

private slots:
	void TCPSocketError(QAbstractSocket::SocketError socketError);
	void TCPSocketConnected();
	void TCPSocketDisconnected();
	void TCPSocketNewData();
	//void UDPSocketNewData();

	void cmdGetSpectrum();

private:
	bool connectDevice();
	bool disconnectDevice();
	void startDevice();
	void stopDevice();
	void readSettings();
	void writeSettings();
	//Display device option widget in settings dialog
	void setupOptionUi(QWidget *parent);

	enum AudioEncoding {
		ALAW = 0,
		PCM = 1,
		CODEC2 = 2
	}audioEncoding;

	enum PacketType {
		SpectrumData = 0,
		AudioData = 1,
		BandscopeData = 2,
		RTPReplyData = 3,
		AnswerData = 4
	};

	//3 byte header common to all data
	struct DspCommonHeader {
		union {
			quint8 bytes[3];
			//0 Packet type: 0=spectrum data, 1=audio data 1-31 Reserved
			struct {
				packStart
				quint8 packetType;
				//From source
				quint8 version;
				quint8 subVersion;
				packEnd
			}packStruct;
		};
	}dspCommonHeader;

	//48 byte header returned in every tcp spectrum response from dspserver
	struct DspServerHeader {
		packStart
		//0 Packet type: 0=spectrum data, 1=audio data 1-31 Reserved
		quint8 packetType;
		//From source
		quint8 version;
		quint8 subVersion;
		quint16 bufLen;
		quint8 reserved[27];
		//32-39 Sample rate in ASCII characters
		char asciiSampleRate[8];
		//40-47 Meter reading in ASCII characters
		unsigned char asciiMeter[8];
		//48-... spectrum or audio data
		packEnd
	}packStruct;

	//5 byte header returned with every tcp audio response from dspServer
	struct DspAudioHeader {
		packStart
		//3 byte commonHeader plus
		quint16 bufLen; //BigEndian, use qFromBigEndian() to platform quint16
		packEnd
	}packStruct dspAudioHeader;

	//15 byte header returned with every tcp spectrum response from dspServer
	struct DspSpectrumHeader {
		packStart
		//3 byte commonHeader plus
		//3-4
		quint16 bufLen; //BigEndian, use qFromBigEndian() to platform quint16
		//5-6
		qint16 meter;
		//7-8
		qint16 subRxMeter;
		//9-12 Sample rate in ASCII characters
		quint32 sampleRate;
		quint16 loOffset;
		packEnd
	}packStruct dspSpectrumHeader;

	enum gDemodMode {
		LSB=0, USB, DSB, CWL, CWH, FM, AM, DIGU, SPEC, DIGL, SAM, DRM
	};

	//State machine for tcp reads
	enum TcpReadState {
		READ_HEADER_TYPE,
		READ_AUDIO_HEADER,
		READ_AUDIO_BUFFER,
		READ_SPECTRUM_HEADER,
		READ_SPECTRUM_BUFFER,
		READ_RTP_REPLY,
		READ_ANSWER
	};
	TcpReadState tcpReadState;

	struct {
		bool isValid; //Set when q-info returns data
		QString serverName;
		QString rtpPort;
		bool isSlave;
		quint16 serverNum; //Not sure this is correct
		quint32 frequency;
		DeviceInterface::DemodMode demodMode; //Pebble demod mode, mapped to gDemodMode
		quint16 zoom;
		qint16 lowFilter;
		qint16 highFilter;
		double offset;
		bool protocol3;
	}serverInfo;

	void producerWorker(cbProducerConsumerEvents _event);
	void consumerWorker(cbProducerConsumerEvents _event);

	QTcpSocket *tcpSocket;
	QHostAddress deviceAddress;
	quint16 devicePort;

	static const quint16 SEND_BUFFER_SIZE = 64;
	static const quint16 AUDIO_OUTPUT_SIZE = 512; //#audio samples to sent to output at a time

	static const quint16 AUDIO_PACKET_SIZE = 2000; //# 8bit samples we get from server
	quint8 audioBuffer[AUDIO_PACKET_SIZE]; //Typically 2000
	quint16 audioBufferCount; //#bytes processed so far, reset every 2000

	static const quint16 SPECTRUM_PACKET_SIZE = 2048; //Check this
	quint8 spectrumBuffer[SPECTRUM_PACKET_SIZE];
	quint16 spectrumBufferIndex;

	static const quint16 ANSWER_PACKET_SIZE = 256; //Max length is 99
	quint8 answerBuf[ANSWER_PACKET_SIZE];
	quint16 answerLength;

	//QUdpSocket *udpSocket;
	QMutex mutex;

	CPX* producerBuf;
	quint16 producerBufIndex;
	ALawCompression alaw;

	Servers *servers;

	QTimer spectrumTimer;

	//DSPServer commands
	void cmdStartAudioStream(quint16 _bufferSize, quint16 _audioSampleRate, quint16 _audioChannels, quint16 _audioEncoding);
	bool cmdSetFrequency(double f);
	bool cmdSetMode(gDemodMode m);
	bool sendTcpCmd(QString buf);
	bool cmdSetRxOutputGain(quint8 gain);
	bool cmdSetFilter(qint16 low, qint16 high);
	void RequestInfo();

	};
#endif // GHPSDR3DEVICE_H

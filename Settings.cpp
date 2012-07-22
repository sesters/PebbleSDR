//GPL license and attributions are in gpl.h and terms are included in this file by reference
#include "gpl.h"
#include "QMessageBox"
#include "Settings.h"
#include "soundcard.h"
#include "demod.h" //For DEMODMODE

Settings::Settings(void)
{
	//Use ini files to avoid any registry problems or install/uninstall 
	//Scope::UserScope puts file C:\Users\...\AppData\Roaming\N1DDY
	//Scope::SystemScope puts file c:\ProgramData\n1ddy
	QString path = QCoreApplication::applicationDirPath();
	qSettings = new QSettings(path+"/pebble.ini",QSettings::IniFormat);
	//qSettings->beginGroup("IQ");

	//qSetting->endGroup("IQ");

    //Set font size, get from settings eventually, change for 1024x768 vs higher res like 1440x900 (mac)
    smFont.setFamily("Lucida Grande");
    smFont.setPointSize(8);
    medFont.setFamily("Lucida Grande");
    medFont.setPointSize(10);
    lgFont.setFamily("Lucida Grande");
    lgFont.setPointSize(12);

    selectedSDR = 0;

	settingsDialog = new QDialog();
	sd = new Ui::SettingsDialog();
	sd->setupUi(settingsDialog);

    settingsDialog->setWindowTitle("Pebble Settings");

    sd->saveButton->setFont(medFont);
    sd->cancelButton->setFont(medFont);
    sd->resetAllButton->setFont(medFont);
    sd->startupEdit1->setFont(medFont);

    SetupRecieverBox(sd->receiverBox1);
    SetupRecieverBox(sd->receiverBox2);
    SetupRecieverBox(sd->receiverBox3);
    SetupRecieverBox(sd->receiverBox4);

    sd->serialBox1->setFont(medFont);
    sd->serialBox1->addItem("Any",-1);
    sd->serialBox1->addItem("0",0);
    sd->serialBox1->addItem("1",1);
    sd->serialBox1->addItem("2",2);
    sd->serialBox1->addItem("3",3);
    sd->serialBox1->addItem("4",4);
    sd->serialBox1->addItem("5",5);
    sd->serialBox1->addItem("6",6);
    sd->serialBox1->addItem("7",7);
    sd->serialBox1->addItem("8",8);
    sd->serialBox1->addItem("9",9);

    sd->sampleRateBox1->addItem("48k",48000);
    sd->sampleRateBox1->addItem("96k",96000);
    sd->sampleRateBox1->addItem("192k",192000);
    sd->sampleRateBox1->addItem("384k",384000);
    sd->sampleRateBox1->setFont(medFont);

    sd->startupBox1->setFont(medFont);
    sd->startupBox1->addItem("Last Frequency",Settings::LASTFREQ);
    sd->startupBox1->addItem("Set Frequency", Settings::SETFREQ);
    sd->startupBox1->addItem("Device Default", Settings::DEFAULTFREQ);
    connect(sd->startupBox1,SIGNAL(currentIndexChanged(int)),this,SLOT(StartupChanged(int)));

    sd->IQSettings->setFont(medFont);
    sd->IQSettings->addItem("I/Q (normal)",IQ);
    sd->IQSettings->addItem("Q/I (swap)",QI);
    sd->IQSettings->addItem("I Only",IONLY);
    sd->IQSettings->addItem("Q Only",QONLY);
    connect(sd->IQSettings,SIGNAL(currentIndexChanged(int)),this,SLOT(IQOrderChanged(int)));

    sd->sourceBox1->setFont(medFont);
    sd->outputBox1->setFont(medFont);


    connect(sd->sdrEnabledButton1,SIGNAL(clicked()),this,SLOT(SelectedSDRChanged()));
    connect(sd->sdrEnabledButton2,SIGNAL(clicked()),this,SLOT(SelectedSDRChanged()));
    connect(sd->sdrEnabledButton3,SIGNAL(clicked()),this,SLOT(SelectedSDRChanged()));
    connect(sd->sdrEnabledButton4,SIGNAL(clicked()),this,SLOT(SelectedSDRChanged()));

    connect(sd->iqGain1,SIGNAL(valueChanged(double)),this,SLOT(IQGainChanged(double)));

    connect(sd->saveButton,SIGNAL(clicked(bool)),this,SLOT(SaveSettings(bool)));
    connect(sd->resetAllButton,SIGNAL(clicked(bool)),this,SLOT(ResetAllSettings(bool)));

	ReadSettings();
}

Settings::~Settings(void)
{
	if (settingsDialog->isVisible())
		settingsDialog->hide();
	delete qSettings;
	delete settingsDialog;
	delete sd;
}

void Settings::SetupRecieverBox(QComboBox *receiverBox)
{
    receiverBox->setFont(medFont);
    receiverBox->addItem("SR Ensemble",SDR::SR_ENSEMBLE);
    receiverBox->addItem("SR Ensemble 2M",SDR::SR_ENSEMBLE_2M);
    receiverBox->addItem("SR Ensemble 4M",SDR::SR_ENSEMBLE_4M);
    receiverBox->addItem("SR Ensemble 6M",SDR::SR_ENSEMBLE_6M);
    receiverBox->addItem("SR V9-ABPF",SDR::SR_V9);
    receiverBox->addItem("SR LITE II",SDR::SR_LITE);
    receiverBox->addItem("Elektor SDR",SDR::ELEKTOR);
    receiverBox->addItem("RFSpace SDR-IQ",SDR::SDR_IQ);
    receiverBox->addItem("HPSDR USB",SDR::HPSDR_USB);
    //receiverBox->addItem("HPSDR TCP",SDR::HPSDR_TCP);
    receiverBox->addItem("SDR Widget",SDR::SDRWIDGET);
    receiverBox->addItem("FUNcube Dongle",SDR::FUNCUBE);
    receiverBox->addItem("File",SDR::FILE);
    receiverBox->addItem("DVB-T",SDR::DVB_T);

    connect(receiverBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ReceiverChanged(int)));

}

void Settings::ShowSettings()
{
    int cur;
    cur = sd->receiverBox1->findData(ini_sdrDevice[0]);
    sd->receiverBox1->setCurrentIndex(cur);
    cur = sd->receiverBox2->findData(ini_sdrDevice[1]);
    sd->receiverBox2->setCurrentIndex(cur);
    cur = sd->receiverBox3->findData(ini_sdrDevice[2]);
    sd->receiverBox3->setCurrentIndex(cur);
    cur = sd->receiverBox4->findData(ini_sdrDevice[3]);
    sd->receiverBox4->setCurrentIndex(cur);

#if 0
	if (startup == SETFREQ)
		sd->startupEdit->setEnabled(true);
	else
		sd->startupEdit->setEnabled(false);
#endif

	//Audio devices may have been plugged or unplugged, refresh list on each show
    inputDevices = SoundCard::DeviceList(true);
    outputDevices = SoundCard::DeviceList(false);

    SetOptionsForSDR(selectedSDR); //Make sure fields are loaded
    SelectedSDRChanged(selectedSDR);

	//QSettings
	settingsDialog->show();
}

//Show realtime changes so we can see gain
void Settings::IQGainChanged(double i)
{
    iqGain = i;
}
//IQ order can be changed in real time, without saving
void Settings::IQOrderChanged(int i)
{
    iqOrder = (IQORDER)sd->IQSettings->itemData(i).toInt();
}

//If called from connect, s will = -1
//Otherwise s = button to select
void Settings::SelectedSDRChanged(int s)
{
    //Save previous settings
    SaveSettings(true);

    //If s == -1, then called from connect link and we need to figure out selection
    if (s == -1) {
        if (sd->sdrEnabledButton1->isChecked())
            s = 0;
        else if (sd->sdrEnabledButton2->isChecked())
            s = 1;
        else if (sd->sdrEnabledButton3->isChecked())
            s = 2;
        else if (sd->sdrEnabledButton4->isChecked())
            s = 3;
    }

    //s now has selection
    selectedSDR = s;
    //Load the options for selected SDR
    SetOptionsForSDR(selectedSDR);

    sd->receiverBox1->setEnabled(false);
    sd->receiverBox2->setEnabled(false);
    sd->receiverBox3->setEnabled(false);
    sd->receiverBox4->setEnabled(false);

    switch (selectedSDR)
    {
    case 0:
        sd->receiverBox1->setEnabled(true);
        sd->sdrEnabledButton1->setChecked(true);

        break;
    case 1:
        sd->receiverBox2->setEnabled(true);
        sd->sdrEnabledButton2->setChecked(true);

        break;
    case 2:
        sd->receiverBox3->setEnabled(true);
        sd->sdrEnabledButton3->setChecked(true);

        break;
    case 3:
        sd->receiverBox4->setEnabled(true);
        sd->sdrEnabledButton4->setChecked(true);

        break;
    }


}

void Settings::SetOptionsForSDR(int s)
{
    //Adding items triggers selection, block signals until list is complete
    sd->sourceBox1->blockSignals(true);
    sd->sourceBox1->clear();
    int id;
    //Input devices may be restricted form some SDRs
    for (int i=0; i<inputDevices.count(); i++)
    {
        id = inputDevices[i].left(2).toInt();
        sd->sourceBox1->addItem(inputDevices[i].mid(3),id);
        if (id == ini_inputDevice[s])
            sd->sourceBox1->setCurrentIndex(i);
    }
    sd->sourceBox1->blockSignals(false);

    sd->outputBox1->blockSignals(true);
    sd->outputBox1->clear();
    for (int i=0; i<outputDevices.count(); i++)
    {
        id = outputDevices[i].left(2).toInt();
        sd->outputBox1->addItem(outputDevices[i].mid(3),id);
        if (id == ini_outputDevice[s])
            sd->outputBox1->setCurrentIndex(i);
    }
    sd->outputBox1->blockSignals(false);

    sd->iqGain1->setValue(ini_iqGain[s]);
    sd->IQSettings->setCurrentIndex(ini_iqOrder[s]);

    int cur;
    cur = sd->sampleRateBox1->findData(ini_sampleRate[s]);
    sd->sampleRateBox1->setCurrentIndex(cur);

    cur = sd->startupBox1->findData(ini_startup[s]);
    sd->startupBox1->setCurrentIndex(cur);

    StartupChanged(cur);

    //Serial box only applies to SoftRocks for now
    ReceiverChanged(0); //Enable/disable
    sd->serialBox1->setCurrentIndex(ini_sdrNumber[s]+1);

}

void Settings::StartupChanged(int i)
{

    startup = (STARTUP)sd->startupBox1->itemData(i).toInt();
    sd->startupEdit1->setText(QString::number(this->ini_startupFreq[selectedSDR],'f',0));
    if (startup == SETFREQ) {
        sd->startupEdit1->setEnabled(true);
    }
    else {
        sd->startupEdit1->setEnabled(false);
    }

}
void Settings::ReceiverChanged(int i)
{
    int cur;
    SDR::SDRDEVICE dev;
    switch (selectedSDR)
    {
    case 0:
        cur = sd->receiverBox1->currentIndex();
        dev = (SDR::SDRDEVICE)sd->receiverBox1->itemData(cur).toInt();
        break;
    case 1:
        cur = sd->receiverBox2->currentIndex();
        dev = (SDR::SDRDEVICE)sd->receiverBox2->itemData(cur).toInt();
        break;
    case 2:
        cur = sd->receiverBox3->currentIndex();
        dev = (SDR::SDRDEVICE)sd->receiverBox3->itemData(cur).toInt();
        break;
    case 3:
        cur = sd->receiverBox4->currentIndex();
        dev = (SDR::SDRDEVICE)sd->receiverBox4->itemData(cur).toInt();
        break;
    }


	//Reset to default
    //!!sd->serialBox->setCurrentIndex(0);


	//Serial box only applies to SoftRocks for now
	if (dev == SDR::SR_ENSEMBLE||dev==SDR::SR_ENSEMBLE_2M||dev==SDR::SR_ENSEMBLE_4M||
        dev==SDR::SR_ENSEMBLE_6M || dev==SDR::SR_ENSEMBLE_LF || dev==SDR::SR_V9)
        sd->serialBox1->setEnabled(true);
	else
        sd->serialBox1->setEnabled(false);
}

void Settings::ReadSettings()
{
	//Read settings from ini file or set defaults
	//Todo: Make strings constants
	//If we don' specify a group, "General" is assumed

    selectedSDR = qSettings->value("selectedSDR",0).toInt();

    ini_startup[0] = (STARTUP)qSettings->value("Startup1", DEFAULTFREQ).toInt();
    ini_startup[1] = (STARTUP)qSettings->value("Startup2", DEFAULTFREQ).toInt();
    ini_startup[2] = (STARTUP)qSettings->value("Startup3", DEFAULTFREQ).toInt();
    ini_startup[3] = (STARTUP)qSettings->value("Startup4", DEFAULTFREQ).toInt();
    startup = ini_startup[selectedSDR];

    ini_startupFreq[0] = qSettings->value("StartupFreq1", 10000000).toDouble();
    ini_startupFreq[1] = qSettings->value("StartupFreq2", 10000000).toDouble();
    ini_startupFreq[2] = qSettings->value("StartupFreq3", 10000000).toDouble();
    ini_startupFreq[3] = qSettings->value("StartupFreq4", 10000000).toDouble();
    startupFreq = ini_startupFreq[selectedSDR];

	lastFreq = qSettings->value("LastFreq", 10000000).toDouble();

    ini_inputDevice[0] = qSettings->value("InputDevice1", SoundCard::DefaultInputDevice()).toInt();
    ini_inputDevice[1] = qSettings->value("InputDevice2", SoundCard::DefaultInputDevice()).toInt();
    ini_inputDevice[2] = qSettings->value("InputDevice3", SoundCard::DefaultInputDevice()).toInt();
    ini_inputDevice[3] = qSettings->value("InputDevice4", SoundCard::DefaultInputDevice()).toInt();
    inputDevice = ini_inputDevice[selectedSDR];

    ini_outputDevice[0] = qSettings->value("OutputDevice1", SoundCard::DefaultOutputDevice()).toInt();
    ini_outputDevice[1] = qSettings->value("OutputDevice2", SoundCard::DefaultOutputDevice()).toInt();
    ini_outputDevice[2] = qSettings->value("OutputDevice3", SoundCard::DefaultOutputDevice()).toInt();
    ini_outputDevice[3] = qSettings->value("OutputDevice4", SoundCard::DefaultOutputDevice()).toInt();
    outputDevice = ini_outputDevice[selectedSDR];

    ini_sampleRate[0] = qSettings->value("SampleRate1", 48000).toInt();
    ini_sampleRate[1] = qSettings->value("SampleRate2", 48000).toInt();
    ini_sampleRate[2] = qSettings->value("SampleRate3", 48000).toInt();
    ini_sampleRate[3] = qSettings->value("SampleRate4", 48000).toInt();
    sampleRate = ini_sampleRate[selectedSDR];

	decimateLimit = qSettings->value("DecimateLimit", 24000).toInt();
	postMixerDecimate = qSettings->value("PostMixerDecimate",true).toBool();
	//Be careful about changing this, has global impact 
	framesPerBuffer = qSettings->value("FramesPerBuffer",2048).toInt();

    ini_sdrDevice[0] = (SDR::SDRDEVICE)qSettings->value("sdrDevice1", SDR::SR_V9).toInt();
    ini_sdrDevice[1] = (SDR::SDRDEVICE)qSettings->value("sdrDevice2", SDR::SR_V9).toInt();
    ini_sdrDevice[2] = (SDR::SDRDEVICE)qSettings->value("sdrDevice3", SDR::SR_V9).toInt();
    ini_sdrDevice[3] = (SDR::SDRDEVICE)qSettings->value("sdrDevice4", SDR::SR_V9).toInt();
    sdrDevice = ini_sdrDevice[selectedSDR];

    ini_sdrNumber[0] = qSettings->value("sdrNumber1",-1).toInt();
    ini_sdrNumber[1] = qSettings->value("sdrNumber2",-1).toInt();
    ini_sdrNumber[2] = qSettings->value("sdrNumber3",-1).toInt();
    ini_sdrNumber[3] = qSettings->value("sdrNumber4",-1).toInt();
    sdrNumber = ini_sdrNumber[selectedSDR];

    ini_iqGain[0] = qSettings->value("iqGain1",1).toDouble();
    ini_iqGain[1] = qSettings->value("iqGain2",1).toDouble();
    ini_iqGain[2] = qSettings->value("iqGain3",1).toDouble();
    ini_iqGain[3] = qSettings->value("iqGain4",1).toDouble();
    iqGain = ini_iqGain[selectedSDR];

    ini_iqOrder[0] = (IQORDER)qSettings->value("IQOrder1", Settings::IQ).toInt();
    ini_iqOrder[1] = (IQORDER)qSettings->value("IQOrder2", Settings::IQ).toInt();
    ini_iqOrder[2] = (IQORDER)qSettings->value("IQOrder3", Settings::IQ).toInt();
    ini_iqOrder[3] = (IQORDER)qSettings->value("IQOrder4", Settings::IQ).toInt();
    iqOrder = ini_iqOrder[selectedSDR];

    dbOffset = qSettings->value("dbOffset",-70).toFloat();
	lastMode = qSettings->value("LastMode",0).toInt();
	lastDisplayMode = qSettings->value("LastDisplayMode",0).toInt();
	leftRightIncrement = qSettings->value("LeftRightIncrement",10).toInt();
	upDownIncrement = qSettings->value("UpDownIncrement",100).toInt();
}

//Slot for SaveSetting button or whenever we need to write ini file to disk
void Settings::SaveSettings(bool b)
{
    int cur;

    //SelectedSdr is already set, save current values

    cur = sd->sourceBox1->currentIndex();
    ini_inputDevice[selectedSDR] = sd->sourceBox1->itemData(cur).toInt();

    cur = sd->outputBox1->currentIndex();
    ini_outputDevice[selectedSDR] = sd->outputBox1->itemData(cur).toInt();

    cur = sd->receiverBox1->currentIndex();
    ini_sdrDevice[0] = (SDR::SDRDEVICE)sd->receiverBox1->itemData(cur).toInt();
    cur = sd->receiverBox2->currentIndex();
    ini_sdrDevice[1] = (SDR::SDRDEVICE)sd->receiverBox2->itemData(cur).toInt();
    cur = sd->receiverBox3->currentIndex();
    ini_sdrDevice[2] = (SDR::SDRDEVICE)sd->receiverBox3->itemData(cur).toInt();
    cur = sd->receiverBox4->currentIndex();
    ini_sdrDevice[3] = (SDR::SDRDEVICE)sd->receiverBox4->itemData(cur).toInt();

    cur = sd->serialBox1->currentIndex();
    ini_sdrNumber[selectedSDR] = sd->serialBox1->itemData(cur).toInt();

    cur = sd->sampleRateBox1->currentIndex();
    ini_sampleRate[selectedSDR] = sd->sampleRateBox1->itemData(cur).toInt();

    cur = sd->startupBox1->currentIndex();
    ini_startup[selectedSDR] = (STARTUP)sd->startupBox1->itemData(cur).toInt();
    ini_startupFreq[selectedSDR] = sd->startupEdit1->text().toDouble();

    ini_iqGain[selectedSDR] = sd->iqGain1->value();
    cur = sd->IQSettings->currentIndex();
    ini_iqOrder[selectedSDR] =  (IQORDER)sd->IQSettings->itemData(cur).toInt();


	WriteSettings();
    ReadSettings();
    //emit Restart();
}
//Save to disk
void Settings::WriteSettings()
{
	//Write back to ini file
    qSettings->setValue("Startup1",ini_startup[0]);
    qSettings->setValue("Startup2",ini_startup[1]);
    qSettings->setValue("Startup3",ini_startup[2]);
    qSettings->setValue("Startup4",ini_startup[3]);

    qSettings->setValue("StartupFreq1",ini_startupFreq[0]);
    qSettings->setValue("StartupFreq2",ini_startupFreq[1]);
    qSettings->setValue("StartupFreq3",ini_startupFreq[2]);
    qSettings->setValue("StartupFreq4",ini_startupFreq[3]);

	qSettings->setValue("LastFreq",lastFreq);
    qSettings->setValue("InputDevice1", ini_inputDevice[0]);
    qSettings->setValue("InputDevice2", ini_inputDevice[1]);
    qSettings->setValue("InputDevice3", ini_inputDevice[2]);
    qSettings->setValue("InputDevice4", ini_inputDevice[3]);

    qSettings->setValue("OutputDevice1", ini_outputDevice[0]);
    qSettings->setValue("OutputDevice2", ini_outputDevice[1]);
    qSettings->setValue("OutputDevice3", ini_outputDevice[2]);
    qSettings->setValue("OutputDevice4", ini_outputDevice[3]);

    qSettings->setValue("sdrDevice1",ini_sdrDevice[0]);
    qSettings->setValue("sdrDevice2",ini_sdrDevice[1]);
    qSettings->setValue("sdrDevice3",ini_sdrDevice[2]);
    qSettings->setValue("sdrDevice4",ini_sdrDevice[3]);

    qSettings->setValue("sdrNumber1",ini_sdrNumber[0]);
    qSettings->setValue("sdrNumber2",ini_sdrNumber[1]);
    qSettings->setValue("sdrNumber3",ini_sdrNumber[2]);
    qSettings->setValue("sdrNumber4",ini_sdrNumber[3]);

    qSettings->setValue("SampleRate1",ini_sampleRate[0]);
    qSettings->setValue("SampleRate2",ini_sampleRate[1]);
    qSettings->setValue("SampleRate3",ini_sampleRate[2]);
    qSettings->setValue("SampleRate4",ini_sampleRate[3]);

    qSettings->setValue("iqGain1",ini_iqGain[0]);
    qSettings->setValue("iqGain2",ini_iqGain[1]);
    qSettings->setValue("iqGain3",ini_iqGain[2]);
    qSettings->setValue("iqGain4",ini_iqGain[3]);

    qSettings->setValue("IQOrder1", ini_iqOrder[0]);
    qSettings->setValue("IQOrder2", ini_iqOrder[1]);
    qSettings->setValue("IQOrder3", ini_iqOrder[2]);
    qSettings->setValue("IQOrder4", ini_iqOrder[3]);

    //No UI Settings, only in file
	qSettings->setValue("dbOffset",dbOffset);
	qSettings->setValue("LastMode",lastMode);
	qSettings->setValue("LastDisplayMode",lastDisplayMode);
	qSettings->setValue("DecimateLimit",decimateLimit);
	qSettings->setValue("PostMixerDecimate",postMixerDecimate);
	qSettings->setValue("FramesPerBuffer",framesPerBuffer);
	qSettings->setValue("LeftRightIncrement",leftRightIncrement);
	qSettings->setValue("UpDownIncrement",upDownIncrement);

    qSettings->setValue("selectedSDR",selectedSDR);

	qSettings->sync();
}

void Settings::ResetAllSettings(bool b)
{
    //Confirm
    QMessageBox::StandardButton bt = QMessageBox::question(NULL,
            tr("Confirm Reset"),
            tr("Are you sure you want to reset all settings (ini files)")
            );
    if (bt != QMessageBox::Ok)
        return;

    //Delete all ini files and restart

    emit Restart();
}

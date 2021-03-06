#ifndef GLOBAL_H
#define GLOBAL_H
//GPL license and attributions are in gpl.h and terms are included in this file by reference
#include "gpl.h"

#include "QDebug"
#include "QFile"
#include <QSize>
#include <QMainWindow>
#include "perform.h"
#include <QSoundEffect>
#include <QScreen>

class Receiver;
class Settings;
class TestBench;
class DeviceInterface;

class Global
{
public:
    Global();
	~Global();

	QDebug *pLogfile;
    Receiver *receiver;
    DeviceInterface *sdr;
    char *revision;
    Settings *settings;
    Perform perform;
    TestBench* testBench;
    QSize defaultWindowSize;
    QMainWindow *mainWindow;
	QSoundEffect beep;
	QString appDirPath; //Location of executable, used to access pebbledata, plugins, etc
	QString pebbleDataPath; //Location of data files
	QScreen *primaryScreen;
private:
	QFile *file;
};

extern Global *global;
#define logfile (*global->pLogfile)
#endif // GLOBAL_H

#ifndef SPECTRUMWIDGET_H
#define SPECTRUMWIDGET_H
//GPL license and attributions are in gpl.h and terms are included in this file by reference
#include "gpl.h"
#include <QWidget>
#include <QThread>
#include <QImage>
#include <QHoverEvent>

#include "cpx.h"
#include "ui/ui_spectrumwidget.h"
#include "demod.h"
#include "signalspectrum.h"

class SpectrumThread;

class SpectrumWidget : public QWidget
{
	Q_OBJECT

public:
	SpectrumWidget(QWidget *parent = 0);
	~SpectrumWidget();

	void SetMixer(int m, double f);
	void SetFilter(int lo, int hi);
	void Run(bool r);
	void SetMode(DEMODMODE m);
	//Text is displayed when spectrum is 'off' for now
	void SetMessage(QStringList s);
	void SetSignalSpectrum(SignalSpectrum *s);

public slots:
		void updateSpectrum(); //For use by thread to trigger repaint
		void plotSelectionChanged(SignalSpectrum::DISPLAYMODE m);

signals:
		//User clicked in spectrum
		void mixerChanged(int m);
        void mixerChanged(int m, bool changeLO);

		private slots:
            void displayChanged(int item);
            void dbOffsetChanged(int i);
            void dbGainChanged(int t);
            void zoomChanged(int item);
private:
	SignalSpectrum *signalSpectrum; //Source of spectrum data
	float *averageSpectrum; 
	float *lastSpectrum;

	int upDownIncrement;
	int leftRightIncrement;

    double zoom; //Percentage of total spectrum to display

	SpectrumThread *st;
	Ui::SpectrumWidgetClass ui;
    void paintCursor(QColor color);

	//Event overrides
	void paintEvent(QPaintEvent *event);
	void mousePressEvent ( QMouseEvent * event );
	void keyPressEvent (QKeyEvent *event); //Change mixer with arrows
	void enterEvent ( QEvent *event );
	void leaveEvent (QEvent *event);
    void resizeEvent(QResizeEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent( QWheelEvent * event );
    void hoverEnter(QHoverEvent *event);
    void hoverLeave(QHoverEvent *event);

	//Show mixer freq
	int fMixer;
	double loFreq;
    double mouseFreq; //Freq under mouse pointer

	int loFilter; //Used to display bandpass
	int hiFilter;
    double spectrumOffset;
    double spectrumGain;
	SignalSpectrum::DISPLAYMODE spectrumMode;
	int sampleRate;
	DEMODMODE demodMode;

	bool isRunning;
	//QString *message;
	QStringList message;

    QPixmap *plotArea;

    double dbMax;
    double dbMin;
    int dbRange;

    QColor *spectrumColors;

    double GetMouseFreq();
    double GetXYFreq(int x, int y);


};

	class SpectrumThread:public QThread
	{
		Q_OBJECT
		public:
			SpectrumThread(SpectrumWidget *w) 
			{sw = w; 
			msSleep=100;
			doRun = true;}
			void stop() {doRun=false;}
			void SetRefresh(int ms) {
				msSleep = ms;
			} //Refresh rate in me
			void run() {
				doRun = true;
				while(doRun) {
				//We can't trigger a paint event cross thread, Qt design
				//But we can trigger a signal which main thread will get and that can trigger repaint
				//sw->repaint();
				emit repaint();
				//Sleep for resolution
				msleep(msSleep);
				}
			}
		signals:
				void repaint();
	private:
		bool doRun;
		SpectrumWidget * sw;
		int msSleep;
	};
#endif // SPECTRUMWIDGET_H

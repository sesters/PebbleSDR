#ifndef SPECTRUMWIDGET_H
#define SPECTRUMWIDGET_H
//GPL license and attributions are in gpl.h and terms are included in this file by reference
#include "gpl.h"
#include "global.h"

#include <QWidget>
#include <QImage>
#include <QHoverEvent>

#include "cpx.h"
#include "ui_spectrumwidget.h"
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
    void SetMode(DeviceInterface::DEMODMODE m, int _modeOffset);
	//Text is displayed when spectrum is 'off' for now
	void SetMessage(QStringList s);
	void SetSignalSpectrum(SignalSpectrum *s);

public slots:
		void plotSelectionChanged(SignalSpectrum::DISPLAYMODE _mode);
		void updatesPerSecChanged(int item);
		void splitterMoved(int x, int y);

signals:
		//User clicked in spectrum
		void mixerChanged(qint32 m);
		void mixerChanged(qint32 m, bool changeLO);
        void mixerLimitsChanged(int high, int low);

		private slots:
            void displayChanged(int item);
            void maxDbChanged(int t);
			void minDbChanged(int t);
            void zoomChanged(int item);
            void newFftData();

private:
	SignalSpectrum *signalSpectrum; //Source of spectrum data
    double *averageSpectrum;
    double *lastSpectrum;
    //Holds values mapped to screen using utility in fft
	qint32 *fftMap;
	qint32 *topPanelFftMap;

	int upDownIncrement;
	int leftRightIncrement;

    double zoom; //Percentage of total spectrum to display

	Ui::SpectrumWidgetClass ui;
    void DrawCursor(QPainter *painter, QRect plotFr, bool isZoomed, QColor color);
	void DrawOverlay();
	void drawSpectrum(QPixmap &_pixMap, QPixmap &_pixOverlayMap, qint32 *_fftMap);

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
	qint32 fMixer;
	double loFreq;
    double mouseFreq; //Freq under mouse pointer

	int loFilter; //Used to display bandpass
	int hiFilter;

    qint16 plotMaxDb; //Set by UI
    qint16 plotMinDb;
	const quint16 minMaxDbDelta = 30; //Delta between min and max can never be less than this

	SignalSpectrum::DISPLAYMODE spectrumMode;
	int sampleRate;
	DeviceInterface::DEMODMODE demodMode;

	bool isRunning;
	//QString *message;
	QStringList message;

    QPixmap plotArea;
    QPixmap plotOverlay;
    QPixmap plotLabel;
	QPixmap topPanelPlotArea;
	QPixmap topPanelPlotOverlay;
	QPixmap topPanelPlotLabel;

	QPoint *LineBuf;

    int dbRange;

    QColor *spectrumColors;

    double GetMouseFreq();
    int GetMouseDb();

    //Grid display, will change depending on plotArea size
    static const int maxHDivs = 50;
	int numXDivs; //Must be even number to split right around center
	int numYDivs;

    int modeOffset;

	void resizeFrames(bool scale = true);
	void DrawScale(QPainter *labelPainter, double centerFreq, bool drawZoomed);

	void updateTopPanel(SignalSpectrum::DISPLAYMODE _newMode, bool updateSlider);
	QRect mapFrameToWidget(QFrame *frame);

	bool twoPanel; //True if both panels are shown
	bool topPanelHighResolution; //True if top panel is in hi-res mode
	SignalSpectrum::DISPLAYMODE  topPanelDisplayMode; //Spectrum or Waterfall
	void paintSpectrum(bool paintTopPanel, QPainter *painter);
	void drawSpectrumOverlay(bool drawTopPanel);
	void drawWaterfallOverlay(bool drawTopPanel);
	void drawWaterfall(QPixmap &_pixMap, QPixmap &_pixOverlayMap, qint32 *_fftMap);
};

#endif // SPECTRUMWIDGET_H

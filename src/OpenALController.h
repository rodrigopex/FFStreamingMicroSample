/*
 * OpenALController.h
 *
 *  Created on: Jan 7, 2014
 *      Author: rodrigopex
 */

#ifndef OPENALCONTROLLER_H_
#define OPENALCONTROLLER_H_

#include <qobject.h>
#include <unistd.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bb/cascades/QmlDocument>
using namespace std;

//#define FREQ 44100
#define FREQ 8000
/**************************************************************
 * BUFFER_SIZE  = (productory of):
 * frequency
 * number of bytes for sample, AL_FORMAT_MONO16 == 2 butes)
 * number of tracks
 * duration in seconds
 **************************************************************/
#define BUFFER_SIZE FREQ * 2 * 1 * 6
//**************************************************************

class OpenALController: public QObject {
	Q_OBJECT
	Q_PROPERTY(bool recording READ recording NOTIFY recordingChanged)
	Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
public:
	OpenALController(QObject * parent = 0);
	~OpenALController();
	void recordToStreamBuffer(qint16 *samples, int frame_size,
			int nb_channels);
	Q_INVOKABLE void record();
	Q_INVOKABLE void play();
	Q_INVOKABLE void hello();
	static void registerQmlTypes() {
		qmlRegisterType<OpenALController>("tc.OpenAL", 1, 0, "OpenALController");
	}
	bool recording();
	bool playing();
	void setRecording(bool status);
	void setPlaying(bool status);
signals:
	void recordingChanged();
	void playingChanged();
private:
	void setupDevices();
	void checkError(QString where);

	bool m_recording;
	bool m_playing;
	ALCdevice * m_captureDev;
	ALubyte m_captureBuffer[BUFFER_SIZE];
	ALint m_samplesAvailable;
	ALint m_samplesCaptured;
};

#endif /* OPENALCONTROLLER_H_ */

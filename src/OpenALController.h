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
//#include <AL/efx.h>
#define FREQ 11025
#define BUFFER_SIZE FREQ * 2 * 1 * 6

class OpenALController: public QObject {
Q_OBJECT
public:
	OpenALController(QObject * parent = 0);
	~OpenALController();
	Q_INVOKABLE void record();
	Q_INVOKABLE void play();
	Q_INVOKABLE void hello();
private:
	void setupDevices();
	void checkError(QString where);
	ALCdevice * m_captureDev;
	ALubyte m_captureBuffer[BUFFER_SIZE];
	ALint m_samplesAvailable;
	ALint m_samplesCaptured;

	//ALuint m_buffer;
	//ALuint m_source;
};

#endif /* OPENALCONTROLLER_H_ */

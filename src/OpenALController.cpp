/*
 * OpenALController.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: rodrigopex
 */

#include "OpenALController.h"
#include <QDebug>

OpenALController::OpenALController(QObject * parent) :
		QObject(parent) {
	alutInit(NULL, NULL);
	this->setupDevices();
	m_playing = false;
	m_recording = false;
}

OpenALController::~OpenALController() {
	alcCaptureStop(m_captureDev);
	alcCaptureCloseDevice(m_captureDev);
	alutExit();
}

void OpenALController::setRecording(bool status) {
	if (status != m_recording) {
		m_recording = status;
		qDebug() << "OpenALController::setRecording:" << status;
		emit recordingChanged();
	}
}

bool OpenALController::recording() {
	return m_recording;
}

void OpenALController::setPlaying(bool status) {
	if (status != m_playing) {
		qDebug() << "OpenALController::setPlaying:" << status;
		m_playing = status;
		emit playingChanged();
	}
}

bool OpenALController::playing() {
	return m_playing;
}

void OpenALController::setupDevices() {
	const ALCchar * devices;
	const ALCchar * ptr;

	devices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
	ptr = devices;
	while (*ptr) {
		printf("   %s\n", ptr);
		ptr += strlen(ptr) + 1;
	}
	ptr = devices;
	while (*ptr) {
		qDebug() << "** Opening device [" << ptr << "]";
		m_captureDev = alcCaptureOpenDevice(ptr, FREQ, AL_FORMAT_MONO16,
				FREQ / 2);
		if (m_captureDev == NULL) {
			qDebug() << "** Unable to open capture device: [" << ptr << "]";
		} else {
			break;
		}
		ptr += strlen(ptr) + 1;
	}

	devices = alcGetString(m_captureDev, ALC_CAPTURE_DEVICE_SPECIFIER);
	qDebug() << "** Opened device [" << devices << "]";
}

void OpenALController::checkError(QString where) {
	if (alutGetError() != ALC_NO_ERROR)
		qDebug() << qPrintable(where) << alutGetErrorString(alutGetError());
}

void OpenALController::recordToStreamBuffer(qint16 *samples, int frame_size,
		int nb_channels) {
	//qDebug() << "Starting capture NOW!";

	Q_UNUSED(nb_channels);
	int samplesCaptured = 0, samplesAvailable = 0;
	qint16 * captureBufPtr;
	captureBufPtr = samples;

	//this->setRecording(true);
	alcCaptureStart(m_captureDev);
	alcGetIntegerv(m_captureDev, ALC_CAPTURE_SAMPLES,
			(ALCsizei) sizeof(qint16), &samplesAvailable);
	int samplesToCopy = frame_size - samplesCaptured;
	while (samplesCaptured < frame_size) {
		if (samplesAvailable > 0 && samplesToCopy > samplesAvailable) {
			samplesToCopy = samplesAvailable;
		}
		alcCaptureSamples(m_captureDev, captureBufPtr, samplesToCopy);
		checkError("alcCaptureSamples");
		samplesCaptured += samplesToCopy;
		///* Advance the buffer (two bytes per sample * number of samples) */
		captureBufPtr += samplesToCopy;// * 2;
		alcGetIntegerv(m_captureDev, ALC_CAPTURE_SAMPLES,
				(ALCsizei) sizeof(ALubyte), &samplesAvailable);
	}
	//qDebug() << "oal:" << frame_size;;
	//qDebug() << "\nDone capturing. samples captured:" << accum;
	/*alcCaptureStop(m_captureDev); */
	//this->setRecording(false);
}

void OpenALController::record() {
	ALubyte *captureBufPtr;
	qDebug() << "Starting capture NOW!";
	m_samplesCaptured = 0;
	captureBufPtr = m_captureBuffer;

	this->setRecording(true);
	alcCaptureStart(m_captureDev);
	alcGetIntegerv(m_captureDev, ALC_CAPTURE_SAMPLES,
			(ALCsizei) sizeof(ALubyte), &m_samplesAvailable);
	while (m_samplesCaptured < FREQ * 5) {
		if (0 < m_samplesAvailable && m_samplesAvailable <= FREQ) {
			alcCaptureSamples(m_captureDev, captureBufPtr, m_samplesAvailable);
			checkError("alcCaptureSamples");
			m_samplesCaptured += m_samplesAvailable;
			//qDebug()
			//		<<qPrintable(QString("Captured %1 samples (adding %2)\n").arg(m_samplesCaptured).arg(m_samplesAvailable));

			// Advance the buffer (two bytes per sample * number of samples)
			captureBufPtr += m_samplesAvailable * 2;
		} else {
			qDebug() << "No captured samples:" << m_samplesAvailable;
		}
		// Wait for a bit
		//alutSleep(0.02);
		printf(".");
		fflush(stdout);
		alcGetIntegerv(m_captureDev, ALC_CAPTURE_SAMPLES,
				(ALCsizei) sizeof(ALubyte), &m_samplesAvailable);
	}
	qDebug() << "\nDone capturing.\n";
	alcCaptureStop(m_captureDev);
	this->setRecording(false);
}

void OpenALController::play() {
	this->setPlaying(true);
	qDebug() << "Starting playback";

	// Generate an OpenAL source for the captured data
	ALuint source;
	alGenSources(1, &source);
	checkError("alGenSources");

	// Generate an OpenAL buffer for the captured data
	ALuint buffer;
	alGenBuffers(1, &buffer);
	checkError("alGenBuffers");

	/*ALuint effect;
	 alGenEffects(1, &effect);
	 checkError("alGenEffects");
	 alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
	 checkError("alEffecti");
	 alEffectf(effect, AL_REVERB_DECAY_TIME, 10.0f);
	 checkError("alEffectf");*/

	if (buffer) {
		alBufferData(buffer, AL_FORMAT_MONO16, m_captureBuffer,
				m_samplesCaptured * 2, FREQ);
		checkError("alBufferData");

		alSourcei(source, AL_BUFFER, buffer);
		checkError("alSourcei");
		float result;
		alGetSourcef(source, AL_SAMPLE_OFFSET, &result);
		checkError("alGetSourcef");
		qDebug() << "Recorded file duration (s): "
				<< m_samplesCaptured / (float) FREQ << "," << result;
		alSourcePlay(source);
		checkError("alSourcePlay");

		ALint playState = AL_PLAYING;
		while (playState == AL_PLAYING) {
			alGetSourcei(source, AL_SOURCE_STATE, &playState);
			checkError("alGetSourcei");
			//qDebug() << "Current byte play state:"
			//		<< (playState == AL_PLAYING ? "Playing" : "Stopped");
			printf(".");
			fflush(stdout);
			alutSleep(0.2);
		}
		alSourceStop(source);
		alDeleteBuffers(1, &buffer);
		alDeleteSources(1, &source);
		qDebug() << "\nDone with playback.\n";
	}
	this->setPlaying(false);
}

void OpenALController::hello() {
	ALuint helloBuffer, helloSource;
	//alutInit(NULL, NULL);
	helloBuffer = alutCreateBufferHelloWorld();
	alGenSources(1, &helloSource);
	alSourcei(helloSource, AL_BUFFER, helloBuffer);
	alSourcePlay(helloSource);
	alutSleep(1);
	alDeleteSources(1, &helloSource);
	alDeleteBuffers(1, &helloBuffer);
	//alutExit();
}


/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>

using namespace bb::cascades;
#define BUFFER_SIZE 11025 * 2 * 1 * 5
#define DELAY_500MS 500000

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
		QObject(app) {
	// prepare the localization
	m_pTranslator = new QTranslator(this);
	m_pLocaleHandler = new LocaleHandler(this);

	bool res = QObject::connect(m_pLocaleHandler,
			SIGNAL(systemLanguageChanged()), this,
			SLOT(onSystemLanguageChanged()));
	// This is only available in Debug builds
	Q_ASSERT(res);
	// Since the variable is not used in the app, this is added to avoid a
	// compiler warning
	Q_UNUSED(res);

	// initial load
	onSystemLanguageChanged();

	// Create scene document from main.qml asset, the parent is set
	// to ensure the document gets destroyed properly at shut down.
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
	qml->setContextProperty("app", this);
	// Create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();

	// Set created root object as the application scene
	app->setScene(root);
}

void ApplicationUI::onSystemLanguageChanged() {
	QCoreApplication::instance()->removeTranslator(m_pTranslator);
	// Initiate, load and install the application translation files.
	QString locale_string = QLocale().name();
	QString file_name = QString("FFStreamingMicroSample_%1").arg(locale_string);
	if (m_pTranslator->load(file_name, "app/native/qm")) {
		QCoreApplication::instance()->installTranslator(m_pTranslator);
	}
}

void checkError(QString where) {
	if(alutGetError() != ALC_NO_ERROR)
		qDebug() << qPrintable(where) << alutGetErrorString(alutGetError());
}

void ApplicationUI::playTest() {
	const ALCchar * devices;
	const ALCchar * ptr;
	ALCdevice * mainDev;
	ALCcontext * mainContext;
	ALCdevice * captureDev;
	ALubyte captureBuffer[BUFFER_SIZE];
	ALubyte *captureBufPtr;
	ALint samplesAvailable;
	ALint samplesCaptured;
	time_t currentTime;
	time_t lastTime;

	alutInit(NULL, NULL);
	int i;
	// Print the list of capture devices
/*	printf("Available playback devices:\n");

	devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	ptr = devices;
	while (*ptr) {
		printf("   %s\n", ptr);
		ptr += strlen(ptr) + 1;
	}

	// Open a playback device and create a context first
	printf("Opening playback device:\n");
	mainDev = alcOpenDevice(NULL);
	if (mainDev == NULL) {
		printf("Unable to open playback device!\n");
		exit(1);
	}

	devices = alcGetString(mainDev, ALC_DEVICE_SPECIFIER);
	printf("   opened device '%s'\n", devices);
	mainContext = alcCreateContext(mainDev, NULL);
	if (mainContext == NULL) {
		printf("Unable to create playback context!\n");
		exit(1);
	}
	qDebug() << "   created playback context\n";

	// Make the playback context current
	alcMakeContextCurrent(mainContext);
	alcProcessContext(mainContext);

	// Print the list of capture devices
	printf("Available capture devices:\n");
	*/
	devices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
	ptr = devices;
	while (*ptr) {
		printf("   %s\n", ptr);
		ptr += strlen(ptr) + 1;
	}
	ptr = devices;
	while (*ptr) {
		qDebug() << "** Opening device [" << ptr << "]";
		captureDev = alcCaptureOpenDevice(ptr, 11025, AL_FORMAT_MONO16,
				11025/2);
		if (captureDev == NULL) {
			qDebug() << "** Unable to open capture device: [" << ptr << "]";
		} else {
			break;
		}
		ptr += strlen(ptr) + 1;
	}

	devices = alcGetString(captureDev, ALC_CAPTURE_DEVICE_SPECIFIER);
	qDebug() << "** Opened device [" << devices << "]";

	// Wait for three seconds to prompt the user
	for (i = 3; i > 0; i--) {
		printf("Starting capture in %d...\r", i);
		fflush(stdout);
		lastTime = time(NULL);
		currentTime = lastTime;
		while (currentTime == lastTime) {
			currentTime = time(NULL);
			usleep(DELAY_500MS);
		}
	}
	qDebug() << "Starting capture NOW!\n";
	lastTime = currentTime;
	// Capture (roughly) five seconds of audio
	alcCaptureStart(captureDev);
	samplesCaptured = 0;
	captureBufPtr = captureBuffer;
	while (samplesCaptured < (11025 * 5)) {
		// Get the number of samples available
		alcGetIntegerv(captureDev, ALC_CAPTURE_SAMPLES, 1, &samplesAvailable);
		if ((samplesCaptured + samplesAvailable * 2) > BUFFER_SIZE) {
			break;
		}
		// Copy the samples to our capture buffer
		if (samplesAvailable > 0) {
			alcCaptureSamples(captureDev, captureBufPtr, samplesAvailable);
			samplesCaptured += samplesAvailable;
			qDebug()
					<<qPrintable(QString("Captured %1 samples (adding %2)\n").arg(samplesCaptured).arg(samplesAvailable));

			// Advance the buffer (two bytes per sample * number of samples)
			captureBufPtr += samplesAvailable * 2;
		} else {
			qDebug() << "No captured samples\n";
		}
		// Wait for a bit
		alutSleep(0.01);
		// Update the clock
		//currentTime = time(NULL);
	}
	qDebug() << "\nDone capturing.\n";
	alcCaptureStop(captureDev);

	// Play back the captured data
	qDebug() << "Starting playback...\n";

	ALuint source;
	alGenSources(1, &source);
	checkError("alGenSources");

	// Generate an OpenAL buffer for the captured data
	ALuint buffer;
	alGenBuffers(1, &buffer);
	checkError("alGenBuffers");

	if (buffer) {
		alBufferData(buffer, AL_FORMAT_MONO16, captureBuffer, samplesCaptured*2,
				11025);
		checkError("alBufferData");

		alSourcei(source, AL_BUFFER, buffer);
		checkError("alSourcei");
		float result;
		alGetSourcef(source, AL_SAMPLE_OFFSET, &result);
		checkError("alGetSourcef");
		qDebug() << "Recorded file duration (s): " << samplesCaptured / 11025.0
				<< "," << result;
		alSourcePlay(source);
		checkError("alSourcePlay");

//		ALint sampleOffset = 0;
		ALint playState = AL_PLAYING;
		while (playState == AL_PLAYING) { //sampleOffset < samplesCaptured*2) {
//			alGetSourcei(source, AL_SOURCE_STATE, &sampleOffset);
			alGetSourcei(source, AL_SOURCE_STATE, &playState);
			checkError("alGetSourcei");
			qDebug() << "Current byte play state:" << (playState == AL_PLAYING? "Playing": "Stopped");
			usleep(100000);
			currentTime = time(NULL);
		}
		qDebug() << "\nDone with playback.\n";

		alDeleteSources(1, &source);
		alDeleteBuffers(1, &buffer);
	}
	// Shut down OpenAL
	//alcMakeContextCurrent(NULL);
	//alcCloseDevice(mainDev);
	alutSleep(1);
	alcCaptureCloseDevice(captureDev);
	alutSleep(1);
	qDebug() << "\nend of OpenAL usage.\n";
	alutExit();
}

void ApplicationUI::hello() {
	ALuint helloBuffer, helloSource;
	alutInit(NULL, NULL);
	helloBuffer = alutCreateBufferHelloWorld();
	alGenSources(1, &helloSource);
	alSourcei(helloSource, AL_BUFFER, helloBuffer);
	alSourcePlay(helloSource);
	alutSleep(1);
	alutExit();
}

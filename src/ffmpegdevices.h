/*
 * ffmpegdevices.h
 *
 *  Created on: Jan 8, 2014
 *      Author: rodrigopex
 */

#ifndef FFMPEGDEVICES_H_
#define FFMPEGDEVICES_H_

#include <stdio.h>

extern "C" {

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>

}

#include <QDebug>

void listFFmpegDevices() {
	AVInputFormat *p = NULL;
	av_register_all();
	avdevice_register_all();
	qDebug() << "FFMPEG DEVICES:";
	while (p = av_iformat_next(p))
		qDebug()
				<< QString("%1: %2:\n").arg(p->name).arg(p->long_name).toLatin1();

	return;
}

#endif /* FFMPEGDEVICES_H_ */

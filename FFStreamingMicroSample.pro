APP_NAME = FFStreamingMicroSample

LIBS   += -lOpenAL -lalut -lasound -lEGL -lGLESv1_CM
Qt += network
CONFIG += qt warn_on cascades10

include(lib/x264/x264.pri)
include(lib/ffmpeg/ffmpeg.pri)
include(config.pri)

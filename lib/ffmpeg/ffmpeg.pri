#This file was auto-generated. Don't edit this.
BASEDIR =  $$quote($$_PRO_FILE_PWD_)

device {
    INCLUDEPATH += $$BASEDIR/lib/ffmpeg/include
    LIBS += -L$$BASEDIR/lib/ffmpeg/lib
        
    #LIBS += -lrtp 
    LIBS += -lavcodec 
    LIBS += -lavfilter 
    LIBS += -lpostproc 
    LIBS += -lswresample 
    LIBS += -lswscale 
    LIBS += -lavformat 
    LIBS += -lavdevice 
    LIBS += -lavutil
}

#This file was auto-generated. Don't edit this.
BASEDIR =  $$quote($$_PRO_FILE_PWD_)

#message($$BASEDIR/libs/x264/lib)

device {
    INCLUDEPATH += $$BASEDIR/lib/x264/include
    LIBS +=  -L$$BASEDIR/lib/x264/lib    
    LIBS += -lx264
}

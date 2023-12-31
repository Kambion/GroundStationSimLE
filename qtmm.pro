
QT += core gui widgets multimedia

TEMPLATE = app

macx {
    TARGET = "AFSK1200 Decoder"
} else {
    TARGET = afsk1200dec
}

# disable debug messages in release
CONFIG(debug, debug|release) {
    # Define version string (see below for releases)
    VER = $$system(git describe --abbrev=8)
} else {
    DEFINES += QT_NO_DEBUG
    DEFINES += QT_NO_DEBUG_OUTPUT
#    VER = 1.0
    VER = $$system(git describe --abbrev=8)
}

# Tip from: http://www.qtcentre.org/wiki/index.php?title=Version_numbering_using_QMake
VERSTR = '\\"$${VER}\\"'          # place quotes around the version string
DEFINES += VERSION=\"$${VERSTR}\" # create a VERSION macro containing the version string

SOURCES += \
    audiobuffer.cpp \
    librtlsdr.c \
    main.cpp\
    mainwindow.cpp \
    multimon/cafsk12.cpp \
    multimon/costabf.c \
    sdrworker.cpp \
    ssi.cpp \
    tuner_e4k.c \
    tuner_fc0012.c \
    tuner_fc0013.c \
    tuner_fc2580.c \
    tuner_r82xx.c

HEADERS += \
    audiobuffer.h \
    mainwindow.h \
    multimon/cafsk12.h \
    multimon/filter.h \
    multimon/filter-i386.h \
    reg_field.h \
    rtl-sdr.h \
    rtl-sdr_export.h \
    rtlsdr_i2c.h \
    sdrworker.h \
    ssi.h \
    tuner_e4k.h \
    tuner_fc0012.h \
    tuner_fc0013.h \
    tuner_fc2580.h \
    tuner_r82xx.h

FORMS += mainwindow.ui

win32 {
    # application icon on Windows
    RC_FILE = qtmm.rc
} else:macx {
    # app icon on OSX
    ICON = icons/qtmm.icns
}

OTHER_FILES += \
    README.txt

RESOURCES += \
    qtmm.qrc


unix|win32: LIBS += -L$$PWD/libusb-MinGW-x64/lib/ -llibusb-1.0.dll

INCLUDEPATH += $$PWD/libusb-MinGW-x64/include/libusb-1.0
DEPENDPATH += $$PWD/libusb-MinGW-x64/include/libusb-1.0

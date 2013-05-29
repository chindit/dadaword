#-------------------------------------------------
#
# Project created by QtCreator 2012-03-14T18:42:33
#
#-------------------------------------------------

QT       += core gui \
            xml widgets \
	    network printsupport

TARGET = dadaword
#TARGET.path = $$PREFIX/

TEMPLATE = app


SOURCES += main.cpp\
        dadaword.cpp \
    outils.cpp \
    opendocument.cpp \
    spellchecker.cpp \
    highlighterhtml.cpp \
    ddz.cpp \
    qzip.cpp \  
    style.cpp \
    settingsManager.cpp \
    errorManager.cpp \
    autoload.cpp \
    specialchars.cpp \
    replaceManager.cpp \
    orthManager.cpp \
    puces.cpp

HEADERS  += dadaword.h \
    constantes.h \
    outils.h \
    opendocument.h \
    spellchecker.h \
    highlighterhtml.h \
    ddz.h \
    qzipreader.h \
    qzipwriter.h \  
    style.h \
    settingsManager.h \
    errorManager.h \
    autoload.h \
    specialchars.h \
    replaceManager.h \
    orthManager.h \
    puces.h

RESOURCES += \
    images.qrc

#Hunspell
INCLUDEPATH += hunspell/src/hunspell
unix {
    LIBS += /usr/lib/libhunspell.so
    #Zlib
    LIBS += /usr/lib/libz.so
}
win32 {
    LIBS += C:\Users\Jean\Downloads\dadaword-master\libhunspell.dll
    LIBS += C:\Users\Jean\Downloads\dadaword-master\zlib1.dll
}

#Traductions
TRANSLATIONS = dadaword_es.ts
#CODECFORTR = UTF-8

INCLUDEPATH += /usr/include/qt5

FORMS += \
    orthManager.ui \
    puces.ui

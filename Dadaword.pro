#-------------------------------------------------
#
# Project created by QtCreator 2012-03-14T18:42:33
#
#-------------------------------------------------

QT       += core gui \
            xml \
	    network

TARGET = dadaword
TEMPLATE = app


SOURCES += main.cpp\
        dadaword.cpp \
    outils.cpp \
    opendocument.cpp \
    fen_puces.cpp \
    spellchecker.cpp \
    highlighterhtml.cpp \
    ddz.cpp \
    qzip.cpp \  
    style.cpp \
    settingsManager.cpp \
    errorManager.cpp \
    autoload.cpp \
    specialchars.cpp \
    replaceManager.cpp

HEADERS  += dadaword.h \
    constantes.h \
    outils.h \
    opendocument.h \
    fen_puces.h \
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
    replaceManager.h

RESOURCES += \
    images.qrc

#Hunspell
INCLUDEPATH += hunspell/src/hunspell
LIBS += /usr/local/lib/libhunspell-1.3.a

#Zlib
INCLUDEPATH += zlib

#Traductions
TRANSLATIONS = dadaword_es.ts
CODECFORTR = UTF-8


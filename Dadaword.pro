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
    erreur.cpp \
    opendocument.cpp \
    fen_puces.cpp \
    spellchecker.cpp \
    highlighterhtml.cpp

HEADERS  += dadaword.h \
    constantes.h \
    outils.h \
    erreur.h \
    opendocument.h \
    fen_puces.h \
    spellchecker.h \
    highlighterhtml.h

RESOURCES += \
    images.qrc

#QuaZip
INCLUDEPATH += quazip
LIBS += -lquazip

#Hunspell
INCLUDEPATH += hunspell/src/hunspell
LIBS += hunspell/src/hunspell/.libs/libhunspell-1.3.a

#Traductions
TRANSLATIONS = dadaword_es.ts



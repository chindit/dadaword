cmake_minimum_required(VERSION 2.8)

PROJECT(dadaword)
FIND_PACKAGE(Qt4 REQUIRED)

SET(dadaword_SOURCES main.cpp dadaword.cpp outils.cpp opendocument.cpp fen_puces.cpp spellchecker.cpp highlighterhtml.cpp ddz.cpp qzip.cpp style.cpp settingsManager.cpp errorManager.cpp autoload.cpp specialchars.cpp replaceManager.cpp)
SET(dadaword_HEADERS dadaword.h constantes.h outils.h opendocument.h fen_puces.h spellchecker.h highlighterhtml.h ddz.h qzipreader.h qzipwriter.h style.h settingsManager.h errorManager.h autoload.h specialchars.h replaceManager.h)
SET(dadaword_RESOURCES images.qrc)

QT4_ADD_RESOURCES(dadaword_RESOURCES_RCC ${dadaword_RESOURCES})

link_directories(/usr/lib/)
include(hunspell/src/hunspell)
#include(/usr/share/qt4/mkspecs/win32-g++/)

add_definitions ( -Wall )

IF (QT_LIBRARY_DIR AND NOT QT_MKSPECS_DIR  OR  QT_QMAKE_CHANGED)
    EXEC_PROGRAM( ${QT_QMAKE_EXECUTABLE}
      ARGS "-query QMAKE_MKSPECS"
      OUTPUT_VARIABLE qt_mkspecs_dirs )
    # do not replace : on windows as it might be a drive letter
    # and windows should already use ; as a separator
    IF(UNIX)
      STRING(REPLACE ":" ";" qt_mkspecs_dirs "${qt_mkspecs_dirs}")
    ENDIF(UNIX)
    SET(QT_MKSPECS_DIR NOTFOUND)
    FIND_PATH(QT_MKSPECS_DIR qconfig.pri PATHS ${qt_mkspecs_dirs}
      DOC "The location of the Qt mkspecs containing qconfig.pri"
      NO_DEFAULT_PATH )
  ENDIF (QT_LIBRARY_DIR AND NOT QT_MKSPECS_DIR  OR  QT_QMAKE_CHANGED)

#INCLUDE(${QT_QT_INCLUDE_DIR} ${QT_MKSPECS_DIR}/default)

if ( HUNSPELL_INCLUDE_DIR AND HUNSPELL_LIBRARIES )
# in cache already
SET(Hunspell_FIND_QUIETLY TRUE)
endif ( HUNSPELL_INCLUDE_DIR AND HUNSPELL_LIBRARIES )

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if( NOT WIN32 )
find_package(PkgConfig)

pkg_check_modules(HUNSPELL_PKG QUIET hunspell)
endif( NOT WIN32 )

SET(QT_USE_NETWORK TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTXMLPATTERNS TRUE)

QT4_WRAP_CPP(dadaword_HEADERS_MOC ${dadaword_HEADERS})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_EXECUTABLE(dadaword ${dadaword_SOURCES} 
    ${dadaword_HEADERS_MOC})
TARGET_LINK_LIBRARIES(dadaword ${QT_LIBRARIES})

#-----------------------------
# ZLIB
#-----------------------------
find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( dadaword ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

#-----------------------------
# HUNSPELL
#-----------------------------
INCLUDE_DIRECTORIES (./hunspell hunspell/src/hunspell hunspell/src/parsers hunspell/src/tools hunspell/src/win_api)
# LIBS
SET(SRCS
    hunspell/src/hunspell/affentry.cxx
    hunspell/src/hunspell/affixmgr.cxx
    hunspell/src/hunspell/csutil.cxx
    hunspell/src/hunspell/dictmgr.cxx
    hunspell/src/hunspell/hashmgr.cxx
    hunspell/src/hunspell/suggestmgr.cxx
    hunspell/src/hunspell/license.myspell
    hunspell/src/hunspell/license.hunspell
    hunspell/src/hunspell/phonet.cxx
    hunspell/src/hunspell/filemgr.cxx
    hunspell/src/hunspell/hunzip.cxx
    hunspell/src/hunspell/hunspell.cxx
    hunspell/src/hunspell/replist.cxx)

SET(HEADERS
    hunspell/src/hunspell/affentry.hxx
    hunspell/src/hunspell/htypes.hxx
    hunspell/src/hunspell/affixmgr.hxx
    hunspell/src/hunspell/csutil.hxx
    hunspell/src/hunspell/hunspell.hxx
    hunspell/src/hunspell/atypes.hxx
    hunspell/src/hunspell/dictmgr.hxx
    hunspell/src/hunspell/hunspell.h
    hunspell/src/hunspell/suggestmgr.hxx
    hunspell/src/hunspell/baseaffix.hxx
    hunspell/src/hunspell/hashmgr.hxx
    hunspell/src/hunspell/langnum.hxx
    hunspell/src/hunspell/phonet.hxx
    hunspell/src/hunspell/filemgr.hxx
    hunspell/src/hunspell/hunzip.hxx
    hunspell/src/hunspell/w_char.hxx
    hunspell/src/hunspell/replist.hxx
    hunspell/src/hunspell/hunvisapi.h)

ADD_LIBRARY( hunspell-static STATIC ${HEADERS} ${SRCS})
TARGET_LINK_LIBRARIES( dadaword hunspell-static )



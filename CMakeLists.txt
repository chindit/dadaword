cmake_minimum_required(VERSION 3.13)
set(TARGET_NAME dadaword)

project(${TARGET_NAME})

# Check the build type and ask the user to set concrete one
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
    message(WARNING "CMAKE_BUILD_TYPE is not set, forcing to RelWithDebInfo")
endif()

# Set compiler flags
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g3")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")
endif()

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Xml REQUIRED)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Source files
set(SOURCES
        main.cpp
        dadaword.cpp
        outils.cpp
        opendocument.cpp
        puces.cpp
        storageManager.cpp
        spellchecker.cpp
        highlighter.cpp
        ddz.cpp
        qzip.cpp
        style.cpp
        settingsManager.cpp
        errorManager.cpp
        autoload.cpp
        specialchars.cpp
        replaceManager.cpp
        orthManager.cpp
        )

# User interface files
set(FORMS
        orthManager.ui
        puces.ui
        )

# Resource files
set(RESOURCES
        images.qrc
        )

# Shared libraries
set(LIBRARIES
        Qt5::Widgets
        Qt5::Network
        Qt5::PrintSupport
        Qt5::Xml
        )

# Generate additional sources with MOC and UIC
qt5_wrap_ui(UIC_SOURCES ${FORMS})
qt5_add_resources(RCC_SOURCES ${RESOURCES})

# Set target
add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS} ${UIC_SOURCES} ${RCC_SOURCES})

set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)

# Link with libraries
target_link_libraries(${TARGET_NAME} ${LIBRARIES})

#-----------------------------
# ZLIB
#-----------------------------
find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries(${TARGET_NAME} ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

#-----------------------------
# HUNSPELL
#-----------------------------
include_directories( hunspell )
link_directories(hunspell)

target_link_libraries(${TARGET_NAME} /usr/lib/x86_64-linux-gnu/libhunspell-1.7.so.0)

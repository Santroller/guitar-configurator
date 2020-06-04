cmake_minimum_required( VERSION 3.1 )
project("guitar-configurator")
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )
set( THREADS_PREFER_PTHREAD_FLAG ON )
add_definitions ( -Wall )
find_package ( Qt5Widgets REQUIRED )
find_package ( Qt5Core REQUIRED )
find_package ( Qt5Gui REQUIRED )
find_package ( Qt5SerialPort REQUIRED )
find_package ( Qt5Network REQUIRED )
find_package ( Qt5QuickControls2 REQUIRED )
find_package ( Qt5Svg REQUIRED )
find_package ( Threads REQUIRED )

execute_process(
             COMMAND
				 git describe --always --tags --abbrev=0 
		     WORKING_DIRECTORY 
				 "${PROJECT_SOURCE_DIR}"
             RESULT_VARIABLE
                 SHORT_HASH_RESULT
             OUTPUT_VARIABLE
                 VERSION_NUMBER)
add_definitions( -DVERSION_NUMBER=${VERSION_NUMBER} )
set ( guitar-configurator_HDRS
	status.h
	submodules/Ardwiino/src/shared/config/config.h
	submodules/Ardwiino/src/shared/config/defaults.h
	submodules/Ardwiino/src/shared/config/defines.h
	submodules/Ardwiino/src/shared/controller/controller.h
	winserialhotplug.h
	)

set ( guitar-configurator_SRCS
	arduino_defines.cpp
	ardwiinolookup.cpp
	ledhandler.cpp
	main.cpp
	port.cpp
	portscanner.cpp
	programmer.cpp
	unixserialhotplug.cpp
	updatehandler.cpp
	winserialhotplug.cpp
	)

set ( guitar-configurator_RSCS
	resources.qrc
	)
QT5_ADD_RESOURCES(RSCS ${guitar-configurator_RSCS})

set ( guitar-configurator_MOCS
	ardwiino_defines.h
	ardwiinolookup.h
	ledhandler.h
	port.h
	portscanner.h
	programmer.h
	unixserialhotplug.h
	updatehandler.h
	status.h
	)
QT5_WRAP_CPP(MOCS ${guitar-configurator_MOCS})
math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")
file(COPY firmware DESTINATION .)
file(COPY memory-locations.json DESTINATION .)
if (APPLE)
    file(GLOB BINARIES "binaries/mac/*")
    file(COPY binaries/mac DESTINATION .)
    file(DOWNLOAD https://dlpublic.b-cdn.net/mac-index.json ch-index.json)
    file(DOWNLOAD https://dltest.b-cdn.net/mac-index.json ch-index-test.json)
elseif (UNIX) 
    file(COPY guitar-configurator.desktop DESTINATION usr/share/applications/guitar-configurator.desktop )
    file(COPY icon.png DESTINATION usr/share/icons/hicolor/256x256/apps )
    file(GLOB BINARIES "binaries/linux-${BITS}/*")
    file(DOWNLOAD https://dlpublic.b-cdn.net/linux-index.json ch-index.json)
    file(DOWNLOAD https://dltest.b-cdn.net/linux-index.json ch-index-test.json)
elseif (WIN32)
    file(GLOB BINARIES "binaries/win/*")
    file(DOWNLOAD https://dlpublic.b-cdn.net/win${BITS}-index.json ch-index.json)
    file(DOWNLOAD https://dltest.b-cdn.net/win${BITS}-index.json ch-index-test.json)
endif()
file(COPY ${BINARIES} DESTINATION binaries)
add_executable ( guitar-configurator ${guitar-configurator_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( guitar-configurator  Qt5::Core Qt5::Gui Qt5::Widgets Qt5::SerialPort Qt5::Network Qt5::QuickControls2 Qt5::Svg Threads::Threads )

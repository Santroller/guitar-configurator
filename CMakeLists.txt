cmake_minimum_required( VERSION 3.2 )
project("guitar-configurator")
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )
set( THREADS_PREFER_PTHREAD_FLAG ON )
add_definitions ( -Wall )
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/submodules/QtUsb")
find_package ( Qt5Core REQUIRED )
find_package ( Qt5SerialPort REQUIRED )
find_package ( Qt5Network REQUIRED )
find_package ( Qt5QuickControls2 REQUIRED )
find_package ( Qt5Svg REQUIRED )
find_package ( Threads REQUIRED )
find_package ( Qt5Usb REQUIRED )

execute_process(
             COMMAND
				 git describe --always --tags --abbrev=0 
		     WORKING_DIRECTORY 
				 "${PROJECT_SOURCE_DIR}"
             RESULT_VARIABLE
                 SHORT_HASH_RESULT
             OUTPUT_VARIABLE
				 VERSION_NUMBER
			 OUTPUT_STRIP_TRAILING_WHITESPACE)

add_definitions( -DVERSION_NUMBER="${VERSION_NUMBER}" )
set ( guitar-configurator_HDRS
		status.h
		submodules/Ardwiino/src/shared/config/config.h
		submodules/Ardwiino/src/shared/config/defaults.h
		submodules/Ardwiino/src/shared/config/defines.h
		submodules/Ardwiino/src/shared/controller/controller.h
        serial/winserialhotplug.h
		devices/dfu_arduino.h
		devices/ardwiino.h
	)

set ( guitar-configurator_SRCS
		arduino_defines.cpp
		ardwiinolookup.cpp
		ledhandler.cpp
		main.cpp
		port.cpp
		portscanner.cpp
		programmer.cpp
		updatehandler.cpp
		devices/outdated_ardwiino.cpp
		serial/winserialhotplug.cpp
		serial/unixserialhotplug.cpp
		devices/device.cpp
		devices/serialdevice.cpp
		devices/dfu_arduino.cpp
		devices/null_device.cpp
		devices/ardwiino.cpp
		devices/arduino.cpp
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
		updatehandler.h
		status.h
		devices/outdated_ardwiino.h
		serial/unixserialhotplug.h
		devices/device.h
        devices/serialdevice.h
		devices/null_device.h
		devices/arduino.h
	)
QT5_WRAP_CPP(MOCS ${guitar-configurator_MOCS})
math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")
file(COPY firmware DESTINATION .)
file(COPY resources/memory-locations.json DESTINATION .)
set(myApp_ICON ${CMAKE_CURRENT_SOURCE_DIR}/images/icons/icon.icns)
if (APPLE)
    file(GLOB BINARIES "binaries/mac/*")
    file(COPY binaries/mac DESTINATION .)
    file(DOWNLOAD https://dlpublic.b-cdn.net/mac-index.json ${CMAKE_BINARY_DIR}/ch-index.json)
    file(DOWNLOAD https://dltest.b-cdn.net/mac-index.json ${CMAKE_BINARY_DIR}/ch-index-test.json)
    set_source_files_properties(${myApp_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(MACOSX_BUNDLE_ICON_FILE resources/icon.icns)
	set(MACOSX_BUNDLE_INFO_PLIST resources/MacOSXDebugBundleInfo.plist.in)
	set(HIDAPI hidapi)
elseif (UNIX) 
    file(COPY resources/guitar-configurator.desktop DESTINATION appdir/usr/share/applications )
    file(COPY images/icons/icon.png DESTINATION appdir/usr/share/icons/hicolor/256x256/apps )
    file(GLOB BINARIES "binaries/linux-${BITS}/*")
    file(DOWNLOAD https://dlpublic.b-cdn.net/linux-index.json ${CMAKE_BINARY_DIR}/ch-index.json)
	file(DOWNLOAD https://dltest.b-cdn.net/linux-index.json ${CMAKE_BINARY_DIR}/ch-index-test.json)
	set(HIDAPI hidapi-hidraw)
elseif (WIN32)
    file(GLOB BINARIES "binaries/win/*")
    file(DOWNLOAD https://dlpublic.b-cdn.net/win${BITS}-index.json ${CMAKE_BINARY_DIR}/ch-index.json)
    file(DOWNLOAD https://dltest.b-cdn.net/win${BITS}-index.json ${CMAKE_BINARY_DIR}/ch-index-test.json)
	file(COPY images/icons/icon.ico DESTINATION .)
	set(HIDAPI hidapi)
	target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/submodules/QtUsb/hidapi")
endif()
file(COPY ${BINARIES} DESTINATION binaries)
add_executable ( ${PROJECT_NAME} MACOSX_BUNDLE WIN32 ${myApp_ICON} ${guitar-configurator_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} resources/project.rc)
target_link_libraries ( ${PROJECT_NAME}  Qt5::Core Qt5::SerialPort Qt5::Network Qt5::QuickControls2 Qt5::Svg Threads::Threads Qt5::Usb ${HIDAPI} usb-1.0  )

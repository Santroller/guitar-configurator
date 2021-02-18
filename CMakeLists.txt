cmake_minimum_required(VERSION 3.2)
project("guitar-configurator")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
# add_definitions ( -Wall )
find_package(Qt5Core REQUIRED)
find_package(Qt5SerialPort REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Threads REQUIRED)
math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")
if(UNIX)
  find_package(PkgConfig)
  pkg_search_module(LIBUSB1 REQUIRED libusb-1.0)
elseif(WIN32)
  set(WIN_LINK_LIBRARIES -lpsapi winusb.lib legacy_stdio_definitions.lib)
  set(LIBUSB1_STATIC_LDFLAGS
      ${CMAKE_SOURCE_DIR}/libraries/libusb-win/MS${BITS}/static/libusb-1.0.lib)
  set(LIBUSB1_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libraries/libusb-win/include)
endif()
execute_process(
  COMMAND git describe --always --tags --abbrev=0
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  RESULT_VARIABLE SHORT_HASH_RESULT
  OUTPUT_VARIABLE VERSION_NUMBER
  OUTPUT_STRIP_TRAILING_WHITESPACE)

add_definitions(-DVERSION_NUMBER="${VERSION_NUMBER}")
set(guitar-configurator_HDRS
    status.h
    submodules/Ardwiino/src/shared/config/config.h
    submodules/Ardwiino/src/shared/config/defaults.h
    submodules/Ardwiino/src/shared/config/defines.h
    submodules/Ardwiino/src/shared/controller/controller.h
    devices/dfu_arduino.h
    devices/ardwiino.h
    hotplug/winhotplug.h
    board.h)

set(guitar-configurator_SRCS
    usb/unixusb.cpp
    usb/winusb.cpp
    arduino_defines.cpp
    ardwiinolookup.cpp
    ledhandler.cpp
    main.cpp
    portscanner.cpp
    programmer.cpp
    updatehandler.cpp
    devices/outdated_ardwiino.cpp
    hotplug/winhotplug.cpp
    hotplug/unixhotplug.cpp
    devices/device.cpp
    devices/serialdevice.cpp
    devices/dfu_arduino.cpp
    devices/null_device.cpp
    devices/ardwiino.cpp
    devices/arduino.cpp
    deviceconfiguration.cpp)

set(guitar-configurator_RSCS resources.qrc)
qt5_add_resources(RSCS ${guitar-configurator_RSCS})

set(guitar-configurator_MOCS
    ardwiino_defines.h
    ardwiinolookup.h
    ledhandler.h
    portscanner.h
    programmer.h
    updatehandler.h
    status.h
    devices/outdated_ardwiino.h
    devices/device.h
    usb/usbdevice.h
    devices/serialdevice.h
    devices/null_device.h
    devices/arduino.h
    deviceconfiguration.h
    hotplug/unixhotplug.h)
qt5_wrap_cpp(MOCS ${guitar-configurator_MOCS})
file(COPY firmware DESTINATION .)
file(COPY resources/memory-locations.json DESTINATION .)
set(myApp_ICON ${CMAKE_CURRENT_SOURCE_DIR}/images/icons/icon.icns)
if(APPLE)
  file(GLOB BINARIES "binaries/mac/*")
  file(COPY binaries/mac DESTINATION .)
  file(DOWNLOAD https://dlpublic.b-cdn.net/mac-index.json
       ${CMAKE_BINARY_DIR}/ch-index.json)
  file(DOWNLOAD https://dltest.b-cdn.net/mac-index.json
       ${CMAKE_BINARY_DIR}/ch-index-test.json)
  set_source_files_properties(${myApp_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                       Resources)
  set(MACOSX_BUNDLE_ICON_FILE images/icons/icon.icns)
  set(MACOSX_BUNDLE_INFO_PLIST resources/MacOSXDebugBundleInfo.plist.in)
  set(RESOURCE_FILES ${myApp_ICON})
  set(OS_BUNDLE MACOSX_BUNDLE)
elseif(UNIX)
  file(COPY resources/guitar-configurator.desktop
       DESTINATION appdir/usr/share/applications)
  file(COPY images/icons/icon.png
       DESTINATION appdir/usr/share/icons/hicolor/256x256/apps)
  file(GLOB BINARIES "binaries/linux-${BITS}/*")
  file(DOWNLOAD https://dlpublic.b-cdn.net/linux-index.json
       ${CMAKE_BINARY_DIR}/ch-index.json)
  file(DOWNLOAD https://dltest.b-cdn.net/linux-index.json
       ${CMAKE_BINARY_DIR}/ch-index-test.json)
elseif(WIN32)
  file(GLOB BINARIES "binaries/win/*")
  file(DOWNLOAD https://dlpublic.b-cdn.net/win${BITS}-index.json
       ${CMAKE_BINARY_DIR}/ch-index.json)
  file(DOWNLOAD https://dltest.b-cdn.net/win${BITS}-index.json
       ${CMAKE_BINARY_DIR}/ch-index-test.json)
  file(COPY images/icons/icon.ico DESTINATION .)
  file(COPY ${HIDAPI_LINK_DLL} DESTINATION .)
  set(RESOURCE_FILES resources/project.rc)
  set(OS_BUNDLE WIN32)
endif()
add_subdirectory(libraries/picotool/picoboot_connection)
add_subdirectory(libraries/pico-sdk/src/common/pico_binary_info
                 pico_binary_info)
add_subdirectory(libraries/pico-sdk/src/common/boot_uf2 boot_uf2_headers)
add_subdirectory(libraries/pico-sdk/src/common/boot_picoboot
                 boot_picoboot_headers)
add_subdirectory(libraries/pico-sdk/src/host/pico_platform pico_platform)
file(COPY ${BINARIES} DESTINATION binaries)
add_executable(
  ${PROJECT_NAME}
  ${OS_BUNDLE}
  ${guitar-configurator_SRCS}
  ${UIS}
  ${RSCS}
  ${TRS}
  ${MOCS}
  ${RESOURCE_FILES})
target_link_libraries(
  ${PROJECT_NAME}
  Qt5::Core
  Qt5::SerialPort
  Qt5::Network
  Qt5::QuickControls2
  Qt5::Svg
  Threads::Threads
  pico_binary_info
  boot_uf2_headers
  boot_picoboot_headers
  pico_platform_headers
  picoboot_connection_cxx
  dfu_programmer
  ${LIBUSB1_STATIC_LDFLAGS}
  ${WIN_LINK_LIBRARIES})
include_directories(${LIBUSB1_INCLUDE_DIRS})
add_library(dfu_programmer INTERFACE)
set(SRC ${CMAKE_CURRENT_LIST_DIR}/libraries/dfu-programmer/src)
target_sources(
  dfu_programmer INTERFACE ${SRC}/atmel.c ${SRC}/commands.c
                           ${SRC}/dfu.c ${SRC}/intel_hex.c ${SRC}/stm32.c)
target_link_libraries(
  dfu_programmer
  INTERFACE
  ${LIBUSB1_STATIC_LDFLAGS}
  ${WIN_LINK_LIBRARIES})
target_compile_definitions(dfu_programmer INTERFACE HAVE_CONFIG_H)
target_include_directories(dfu_programmer INTERFACE ${SRC})

QT += quickcontrols2 serialport
CONFIG += c++11 qtquickcompiler

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ardwiinolookup.cpp \
        main.cpp \
        port.cpp \
        portscanner.cpp \
        programmer.cpp \
        unixserialhotplug.cpp \
        winserialhotplug.cpp

RESOURCES += \
    resources.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ardwiinolookup.h \
    controllers.h \
    input_types.h \
    joy_types.h \
    mpu_orientations.h \
    port.h \
    portscanner.h \
    programmer.h \
    status.h \
    submodules/Ardwiino/src/shared/config/config.h \
    submodules/Ardwiino/src/shared/config/defaults.h \
    submodules/Ardwiino/src/shared/config/defines.h \
    submodules/Ardwiino/src/shared/controller/controller.h \
    tilt_types.h \
    unixserialhotplug.h \
    wii_extensions.h \
    winserialhotplug.h

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    target.path = $$PREFIX/bin

    desktop.path = $$PREFIX/share/applications/
    desktop.files += guitar-configurator.desktop
    icon256.path = $$PREFIX/share/icons/hicolor/256x256/apps
    icon256.files += icon.png
    binaries.path = $$PREFIX/bin/binaries
    binaries.files += binaries/linux-64/*
    firmware.path = $$PREFIX/bin/firmware
    firmware.files += firmware/*
    INSTALLS += icon256
    INSTALLS += desktop
    INSTALLS += binaries
    INSTALLS += firmware

    copybinaries.commands = $(MKDIR) $$OUT_PWD/binaries && $(COPY_DIR) $$PWD/binaries/linux-64/* $$OUT_PWD/binaries
    copyfirmware.commands = $(MKDIR) $$OUT_PWD/firmware && $(COPY_DIR) $$PWD/firmware/* $$OUT_PWD/firmware
    first.depends = $(first) copyfirmware copybinaries
    QMAKE_EXTRA_TARGETS += first copyfirmware copybinaries
}

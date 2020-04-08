VERSION = $$str_member($$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags --abbrev=0), 1, -1)
QT += quickcontrols2 serialport
CONFIG += c++11 qtquickcompiler


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += "VERSION_NUMBER=\\\"$${VERSION}\\\""

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        arduino_defines.cpp \
        ardwiinolookup.cpp \
        ledhandler.cpp \
        main.cpp \
        port.cpp \
        portscanner.cpp \
        programmer.cpp \
        unixserialhotplug.cpp \
        updatehandler.cpp \
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
    ardwiino_defines.h \
    ardwiinolookup.h \
    ledhandler.h \
    port.h \
    portscanner.h \
    programmer.h \
    status.h \
    submodules/Ardwiino/src/shared/config/config.h \
    submodules/Ardwiino/src/shared/config/defaults.h \
    submodules/Ardwiino/src/shared/config/defines.h \
    submodules/Ardwiino/src/shared/controller/controller.h \
    submodules/Ardwiino/src/shared/output/usb/API.h \
    unixserialhotplug.h \
    updatehandler.h \
    winserialhotplug.h
BDIR = $$OUT_PWD
win32 {
    BDIR_STRIPPED = $$replace(BDIR,Release,)
    equals (BDIR,$$BDIR_STRIPPED): BDIR=$$BDIR/debug/
    message($$BDIR)
    contains(QMAKE_TARGET.arch, x86_64) {
        system("powershell -Command \"Invoke-WebRequest -Uri https://dlpublic.b-cdn.net/win64-index.json -OutFile $$BDIR/ch-index.json\"")
        system("powershell -Command \"Invoke-WebRequest -Uri https://dltest.b-cdn.net/win64-index.json -OutFile $$BDIR/ch-index-test.json\"")
    } else {
        system("powershell -Command \"Invoke-WebRequest -Uri https://dlpublic.b-cdn.net/win32-index.json -OutFile $$BDIR/ch-index.json\"")
        system("powershell -Command \"Invoke-WebRequest -Uri https://dltest.b-cdn.net/win32-index.json -OutFile $$BDIR/ch-index-test.json\"")
    }
} else {
    macx {
        system("curl https://dlpublic.b-cdn.net/mac-index.json -o $$OUT_PWD/guitar-configurator.app/Contents/MacOS/ch-index.json")
        system("curl https://dltest.b-cdn.net/mac-index.json -o $$OUT_PWD/guitar-configurator.app/Contents/MacOS/ch-index-test.json")
    } else {
        system("curl https://dlpublic.b-cdn.net/linux-index.json -o $$OUT_PWD/ch-index.json")
        system("curl https://dltest.b-cdn.net/linux-index.json -o $$OUT_PWD/ch-index-test.json")
    }
}
macx {
    QMAKE_POST_LINK += codesign --entitlements $$PWD/entitlements.xml -s "guitar-configurator" "$$OUT_PWD/guitar-configurator.app" --deep
    QMAKE_INFO_PLIST = MyInfo.plist
}
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
    copylocations.commands = $(COPY) $$PWD/memory-locations.json $$OUT_PWD
    first.depends = $(first) copyfirmware copybinaries copylocations
    QMAKE_EXTRA_TARGETS += first copyfirmware copybinaries copylocations
}
DISTFILES += \
    MyInfo.plist \
    entitlements.xml \
    firmware/ardwiino-uno-usb-at90usb82-16000000.hex \
    firmware/version \
    memory-locations.json

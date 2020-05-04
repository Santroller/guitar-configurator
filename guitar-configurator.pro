VERSION = $$str_member($$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags --abbrev=0), 1, -1)
QT += quickcontrols2 serialport
CONFIG += c++11 qtquickcompiler svg


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
    RC_ICONS = icon.ico
} else {
    macx {
        system("curl https://dlpublic.b-cdn.net/mac-index.json -o $$OUT_PWD/guitar-configurator.app/Contents/MacOS/ch-index.json")
        system("curl https://dltest.b-cdn.net/mac-index.json -o $$OUT_PWD/guitar-configurator.app/Contents/MacOS/ch-index-test.json")
        ICON = icon.icns
    } else {
        system("curl https://dlpublic.b-cdn.net/linux-index.json -o $$OUT_PWD/ch-index.json")
        system("curl https://dltest.b-cdn.net/linux-index.json -o $$OUT_PWD/ch-index-test.json")
    }
}
macx {
#   QMAKE_POST_LINK += codesign --entitlements $$PWD/entitlements.xml -s "guitar-configurator" "$$OUT_PWD/guitar-configurator.app" --deep
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
    images/360Accessories/ArcadePad/Base.svg \
    images/360Accessories/ArcadeStick/Base.svg \
    images/360Accessories/ArcadeStick/components/gLB.svg \
    images/360Accessories/ArcadeStick/components/gRB.svg \
    images/360Accessories/ArcadeStick/components/ga.svg \
    images/360Accessories/ArcadeStick/components/gb.svg \
    images/360Accessories/ArcadeStick/components/gdpad.svg \
    images/360Accessories/ArcadeStick/components/ghome.svg \
    images/360Accessories/ArcadeStick/components/glt.svg \
    images/360Accessories/ArcadeStick/components/grt.svg \
    images/360Accessories/ArcadeStick/components/gx.svg \
    images/360Accessories/ArcadeStick/components/gy.svg \
    images/360Accessories/DancePad/Base.svg \
    images/360Accessories/DancePad/components/ga.svg \
    images/360Accessories/DancePad/components/gb.svg \
    images/360Accessories/DancePad/components/gback.svg \
    images/360Accessories/DancePad/components/gdown.svg \
    images/360Accessories/DancePad/components/gleft.svg \
    images/360Accessories/DancePad/components/gright.svg \
    images/360Accessories/DancePad/components/gstart.svg \
    images/360Accessories/DancePad/components/gup.svg \
    images/360Accessories/DancePad/components/gx.svg \
    images/360Accessories/DancePad/components/gy.svg \
    images/360Accessories/FlightStick/Base.svg \
    images/360Accessories/Wheel/Base.svg \
    images/360Accessories/Wheel/components/ga.svg \
    images/360Accessories/Wheel/components/gb.svg \
    images/360Accessories/Wheel/components/gback.svg \
    images/360Accessories/Wheel/components/gdown.svg \
    images/360Accessories/Wheel/components/ghome.svg \
    images/360Accessories/Wheel/components/gl_x.svg \
    images/360Accessories/Wheel/components/gleft.svg \
    images/360Accessories/Wheel/components/glt.svg \
    images/360Accessories/Wheel/components/gright.svg \
    images/360Accessories/Wheel/components/grt.svg \
    images/360Accessories/Wheel/components/gstart.svg \
    images/360Accessories/Wheel/components/gup.svg \
    images/360Accessories/Wheel/components/gx.svg \
    images/360Accessories/Wheel/components/gy.svg \
    images/360Ctrl/Base.svg \
    images/360Ctrl/components/gLB.svg \
    images/360Ctrl/components/gRB.svg \
    images/360Ctrl/components/ga.svg \
    images/360Ctrl/components/gb.svg \
    images/360Ctrl/components/gback.svg \
    images/360Ctrl/components/gdown.svg \
    images/360Ctrl/components/ghome.svg \
    images/360Ctrl/components/gleft.svg \
    images/360Ctrl/components/gljoy.svg \
    images/360Ctrl/components/glt.svg \
    images/360Ctrl/components/gright.svg \
    images/360Ctrl/components/grjoy.svg \
    images/360Ctrl/components/grt.svg \
    images/360Ctrl/components/gstart.svg \
    images/360Ctrl/components/gup.svg \
    images/360Ctrl/components/gx.svg \
    images/360Ctrl/components/gy.svg \
    images/ArduinoLeonardo.svg \
    images/ArduinoProMicro.svg \
    images/ArduinoUno.svg \
    images/ArduinoUnoDFU.svg \
    images/GuitarHero/360/Drum/Base.svg \
    images/GuitarHero/360/Drum/components/gLB.svg \
    images/GuitarHero/360/Drum/components/gRB.svg \
    images/GuitarHero/360/Drum/components/ga.svg \
    images/GuitarHero/360/Drum/components/ga2.svg \
    images/GuitarHero/360/Drum/components/gb.svg \
    images/GuitarHero/360/Drum/components/gback.svg \
    images/GuitarHero/360/Drum/components/gdown.svg \
    images/GuitarHero/360/Drum/components/ghome.svg \
    images/GuitarHero/360/Drum/components/gleft.svg \
    images/GuitarHero/360/Drum/components/gljoy.svg \
    images/GuitarHero/360/Drum/components/gright.svg \
    images/GuitarHero/360/Drum/components/gstart.svg \
    images/GuitarHero/360/Drum/components/gup.svg \
    images/GuitarHero/360/Drum/components/gx.svg \
    images/GuitarHero/360/Drum/components/gy.svg \
    images/GuitarHero/360/LesPaul/Base.svg \
    images/GuitarHero/360/LesPaul/components/gLB.svg \
    images/GuitarHero/360/LesPaul/components/ga.svg \
    images/GuitarHero/360/LesPaul/components/gb.svg \
    images/GuitarHero/360/LesPaul/components/gback.svg \
    images/GuitarHero/360/LesPaul/components/gdown.svg \
    images/GuitarHero/360/LesPaul/components/ghome.svg \
    images/GuitarHero/360/LesPaul/components/gleft.svg \
    images/GuitarHero/360/LesPaul/components/gljoy.svg \
    images/GuitarHero/360/LesPaul/components/gr_x.svg \
    images/GuitarHero/360/LesPaul/components/gright.svg \
    images/GuitarHero/360/LesPaul/components/gstart.svg \
    images/GuitarHero/360/LesPaul/components/gstrum.svg \
    images/GuitarHero/360/LesPaul/components/gup.svg \
    images/GuitarHero/360/LesPaul/components/gx.svg \
    images/GuitarHero/360/LesPaul/components/gy.svg \
    images/GuitarHero/360/WorldTour/Base.svg \
    images/GuitarHero/360/WorldTour/components/g949.svg \
    images/GuitarHero/360/WorldTour/components/gLB.svg \
    images/GuitarHero/360/WorldTour/components/ga.svg \
    images/GuitarHero/360/WorldTour/components/gb.svg \
    images/GuitarHero/360/WorldTour/components/gback.svg \
    images/GuitarHero/360/WorldTour/components/gdown.svg \
    images/GuitarHero/360/WorldTour/components/ghome.svg \
    images/GuitarHero/360/WorldTour/components/gleft.svg \
    images/GuitarHero/360/WorldTour/components/gljoy.svg \
    images/GuitarHero/360/WorldTour/components/gr_x.svg \
    images/GuitarHero/360/WorldTour/components/gright.svg \
    images/GuitarHero/360/WorldTour/components/gstrum.svg \
    images/GuitarHero/360/WorldTour/components/gup.svg \
    images/GuitarHero/360/WorldTour/components/gx.svg \
    images/GuitarHero/360/WorldTour/components/gy.svg \
    images/GuitarHero/Live/Base.svg \
    images/GuitarHero/Live/components/gLB.svg \
    images/GuitarHero/Live/components/gRB.svg \
    images/GuitarHero/Live/components/ga.svg \
    images/GuitarHero/Live/components/gb.svg \
    images/GuitarHero/Live/components/gcapture.svg \
    images/GuitarHero/Live/components/gdown.svg \
    images/GuitarHero/Live/components/ghome.svg \
    images/GuitarHero/Live/components/gleft.svg \
    images/GuitarHero/Live/components/gljoy.svg \
    images/GuitarHero/Live/components/gr_x.svg \
    images/GuitarHero/Live/components/gright.svg \
    images/GuitarHero/Live/components/gselect.svg \
    images/GuitarHero/Live/components/gstart.svg \
    images/GuitarHero/Live/components/gstrum.svg \
    images/GuitarHero/Live/components/gup.svg \
    images/GuitarHero/Live/components/gx.svg \
    images/GuitarHero/Live/components/gy.svg \
    images/GuitarHero/PS3/Drum/Base.svg \
    images/GuitarHero/PS3/Drum/components/gLB.svg \
    images/GuitarHero/PS3/Drum/components/gRB.svg \
    images/GuitarHero/PS3/Drum/components/ga.svg \
    images/GuitarHero/PS3/Drum/components/gb.svg \
    images/GuitarHero/PS3/Drum/components/gback.svg \
    images/GuitarHero/PS3/Drum/components/gdown.svg \
    images/GuitarHero/PS3/Drum/components/ghome.svg \
    images/GuitarHero/PS3/Drum/components/gleft.svg \
    images/GuitarHero/PS3/Drum/components/gljoy.svg \
    images/GuitarHero/PS3/Drum/components/gright.svg \
    images/GuitarHero/PS3/Drum/components/gstart.svg \
    images/GuitarHero/PS3/Drum/components/gup.svg \
    images/GuitarHero/PS3/Drum/components/gx.svg \
    images/GuitarHero/PS3/Drum/components/gy.svg \
    images/GuitarHero/PS3/LesPaul/Base.svg \
    images/GuitarHero/PS3/LesPaul/components/gLB.svg \
    images/GuitarHero/PS3/LesPaul/components/ga.svg \
    images/GuitarHero/PS3/LesPaul/components/gb.svg \
    images/GuitarHero/PS3/LesPaul/components/gback.svg \
    images/GuitarHero/PS3/LesPaul/components/gdown.svg \
    images/GuitarHero/PS3/LesPaul/components/ghome.svg \
    images/GuitarHero/PS3/LesPaul/components/gleft.svg \
    images/GuitarHero/PS3/LesPaul/components/gljoy.svg \
    images/GuitarHero/PS3/LesPaul/components/gr_x.svg \
    images/GuitarHero/PS3/LesPaul/components/gright.svg \
    images/GuitarHero/PS3/LesPaul/components/gstart.svg \
    images/GuitarHero/PS3/LesPaul/components/gstrum.svg \
    images/GuitarHero/PS3/LesPaul/components/gup.svg \
    images/GuitarHero/PS3/LesPaul/components/gx.svg \
    images/GuitarHero/PS3/LesPaul/components/gy.svg \
    images/GuitarHero/PS3/WorldTour/Base.svg \
    images/GuitarHero/PS3/WorldTour/components/g949.svg \
    images/GuitarHero/PS3/WorldTour/components/gLB.svg \
    images/GuitarHero/PS3/WorldTour/components/ga.svg \
    images/GuitarHero/PS3/WorldTour/components/gb.svg \
    images/GuitarHero/PS3/WorldTour/components/gback.svg \
    images/GuitarHero/PS3/WorldTour/components/gdown.svg \
    images/GuitarHero/PS3/WorldTour/components/ghome.svg \
    images/GuitarHero/PS3/WorldTour/components/gleft.svg \
    images/GuitarHero/PS3/WorldTour/components/gljoy.svg \
    images/GuitarHero/PS3/WorldTour/components/gr_x.svg \
    images/GuitarHero/PS3/WorldTour/components/gright.svg \
    images/GuitarHero/PS3/WorldTour/components/gstrum.svg \
    images/GuitarHero/PS3/WorldTour/components/gup.svg \
    images/GuitarHero/PS3/WorldTour/components/gx.svg \
    images/GuitarHero/PS3/WorldTour/components/gy.svg \
    images/Keyboard/Base.svg \
    images/NoController.svg \
    images/PS3Ctrl/Base.svg \
    images/PS3Ctrl/components/gLB.svg \
    images/PS3Ctrl/components/gRB.svg \
    images/PS3Ctrl/components/ga.svg \
    images/PS3Ctrl/components/gb.svg \
    images/PS3Ctrl/components/gback.svg \
    images/PS3Ctrl/components/gdown.svg \
    images/PS3Ctrl/components/ghome.svg \
    images/PS3Ctrl/components/gleft.svg \
    images/PS3Ctrl/components/gljoy.svg \
    images/PS3Ctrl/components/glt.svg \
    images/PS3Ctrl/components/gright.svg \
    images/PS3Ctrl/components/grjoy.svg \
    images/PS3Ctrl/components/grt.svg \
    images/PS3Ctrl/components/gstart.svg \
    images/PS3Ctrl/components/gup.svg \
    images/PS3Ctrl/components/gx.svg \
    images/PS3Ctrl/components/gy.svg \
    images/RockBand/360/Drum/Base.svg \
    images/RockBand/360/Drum/components/gRB.svg \
    images/RockBand/360/Drum/components/ga.svg \
    images/RockBand/360/Drum/components/gb.svg \
    images/RockBand/360/Drum/components/gback.svg \
    images/RockBand/360/Drum/components/gbc.svg \
    images/RockBand/360/Drum/components/gdown.svg \
    images/RockBand/360/Drum/components/ggc.svg \
    images/RockBand/360/Drum/components/ghome.svg \
    images/RockBand/360/Drum/components/gleft.svg \
    images/RockBand/360/Drum/components/gljoy.svg \
    images/RockBand/360/Drum/components/gright.svg \
    images/RockBand/360/Drum/components/gstart.svg \
    images/RockBand/360/Drum/components/gup.svg \
    images/RockBand/360/Drum/components/gx.svg \
    images/RockBand/360/Drum/components/gy.svg \
    images/RockBand/360/Drum/components/gyc.svg \
    images/RockBand/360/Guitar/Base.svg \
    images/RockBand/360/Guitar/components/gLB.svg \
    images/RockBand/360/Guitar/components/ga.svg \
    images/RockBand/360/Guitar/components/gb.svg \
    images/RockBand/360/Guitar/components/gback.svg \
    images/RockBand/360/Guitar/components/gdown.svg \
    images/RockBand/360/Guitar/components/ghome.svg \
    images/RockBand/360/Guitar/components/gleft.svg \
    images/RockBand/360/Guitar/components/gljoy.svg \
    images/RockBand/360/Guitar/components/glt.svg \
    images/RockBand/360/Guitar/components/gr_x.svg \
    images/RockBand/360/Guitar/components/gright.svg \
    images/RockBand/360/Guitar/components/gstart.svg \
    images/RockBand/360/Guitar/components/gstrum.svg \
    images/RockBand/360/Guitar/components/gup.svg \
    images/RockBand/360/Guitar/components/gx.svg \
    images/RockBand/360/Guitar/components/gy.svg \
    images/RockBand/Logos/Drum.svg \
    images/RockBand/Logos/Guitar.svg \
    images/RockBand/PS3/Drum/Base.svg \
    images/RockBand/PS3/Drum/components/gRB.svg \
    images/RockBand/PS3/Drum/components/ga.svg \
    images/RockBand/PS3/Drum/components/gb.svg \
    images/RockBand/PS3/Drum/components/gback.svg \
    images/RockBand/PS3/Drum/components/gbc.svg \
    images/RockBand/PS3/Drum/components/gdown.svg \
    images/RockBand/PS3/Drum/components/ggc.svg \
    images/RockBand/PS3/Drum/components/ghome.svg \
    images/RockBand/PS3/Drum/components/gleft.svg \
    images/RockBand/PS3/Drum/components/gljoy.svg \
    images/RockBand/PS3/Drum/components/gright.svg \
    images/RockBand/PS3/Drum/components/gstart.svg \
    images/RockBand/PS3/Drum/components/gup.svg \
    images/RockBand/PS3/Drum/components/gx.svg \
    images/RockBand/PS3/Drum/components/gy.svg \
    images/RockBand/PS3/Drum/components/gyc.svg \
    images/RockBand/PS3/Guitar/Base.svg \
    images/RockBand/PS3/Guitar/components/gLB.svg \
    images/RockBand/PS3/Guitar/components/ga.svg \
    images/RockBand/PS3/Guitar/components/gb.svg \
    images/RockBand/PS3/Guitar/components/gback.svg \
    images/RockBand/PS3/Guitar/components/gdown.svg \
    images/RockBand/PS3/Guitar/components/ghome.svg \
    images/RockBand/PS3/Guitar/components/gleft.svg \
    images/RockBand/PS3/Guitar/components/gljoy.svg \
    images/RockBand/PS3/Guitar/components/glt.svg \
    images/RockBand/PS3/Guitar/components/gr_x.svg \
    images/RockBand/PS3/Guitar/components/gright.svg \
    images/RockBand/PS3/Guitar/components/gstart.svg \
    images/RockBand/PS3/Guitar/components/gstrum.svg \
    images/RockBand/PS3/Guitar/components/gup.svg \
    images/RockBand/PS3/Guitar/components/gx.svg \
    images/RockBand/PS3/Guitar/components/gy.svg \
    images/RockBand/Wii/Drum/Base.svg \
    images/RockBand/Wii/Drum/components/gRB.svg \
    images/RockBand/Wii/Drum/components/ga.svg \
    images/RockBand/Wii/Drum/components/gb.svg \
    images/RockBand/Wii/Drum/components/gback.svg \
    images/RockBand/Wii/Drum/components/gbc.svg \
    images/RockBand/Wii/Drum/components/gdown.svg \
    images/RockBand/Wii/Drum/components/ggc.svg \
    images/RockBand/Wii/Drum/components/ghome.svg \
    images/RockBand/Wii/Drum/components/gleft.svg \
    images/RockBand/Wii/Drum/components/gljoy.svg \
    images/RockBand/Wii/Drum/components/gright.svg \
    images/RockBand/Wii/Drum/components/gstart.svg \
    images/RockBand/Wii/Drum/components/gup.svg \
    images/RockBand/Wii/Drum/components/gx.svg \
    images/RockBand/Wii/Drum/components/gy.svg \
    images/RockBand/Wii/Drum/components/gyc.svg \
    images/RockBand/Wii/Guitar/Base.svg \
    images/RockBand/Wii/Guitar/components/gLB.svg \
    images/RockBand/Wii/Guitar/components/ga.svg \
    images/RockBand/Wii/Guitar/components/gb.svg \
    images/RockBand/Wii/Guitar/components/gback.svg \
    images/RockBand/Wii/Guitar/components/gdown.svg \
    images/RockBand/Wii/Guitar/components/ghome.svg \
    images/RockBand/Wii/Guitar/components/gleft.svg \
    images/RockBand/Wii/Guitar/components/gljoy.svg \
    images/RockBand/Wii/Guitar/components/glt.svg \
    images/RockBand/Wii/Guitar/components/gr_x.svg \
    images/RockBand/Wii/Guitar/components/gright.svg \
    images/RockBand/Wii/Guitar/components/gstart.svg \
    images/RockBand/Wii/Guitar/components/gstrum.svg \
    images/RockBand/Wii/Guitar/components/gup.svg \
    images/RockBand/Wii/Guitar/components/gx.svg \
    images/RockBand/Wii/Guitar/components/gy.svg \
    images/SwitchCtrl/Base.svg \
    images/SwitchCtrl/components/gLB.svg \
    images/SwitchCtrl/components/gRB.svg \
    images/SwitchCtrl/components/ga.svg \
    images/SwitchCtrl/components/gb.svg \
    images/SwitchCtrl/components/gback.svg \
    images/SwitchCtrl/components/gcapture.svg \
    images/SwitchCtrl/components/gdown.svg \
    images/SwitchCtrl/components/ghome.svg \
    images/SwitchCtrl/components/gleft.svg \
    images/SwitchCtrl/components/gljoy.svg \
    images/SwitchCtrl/components/glt.svg \
    images/SwitchCtrl/components/gright.svg \
    images/SwitchCtrl/components/grjoy.svg \
    images/SwitchCtrl/components/grt.svg \
    images/SwitchCtrl/components/gstart.svg \
    images/SwitchCtrl/components/gup.svg \
    images/SwitchCtrl/components/gx.svg \
    images/SwitchCtrl/components/gy.svg \
    images/Upload.svg \
    memory-locations.json

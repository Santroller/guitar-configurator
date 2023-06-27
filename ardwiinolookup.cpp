#include "ardwiinolookup.h"

#include <QCoreApplication>
#include <QDir>
#include <QRegularExpression>

#include "QDebug"
QVersionNumber ArdwiinoLookup::currentVersion = QVersionNumber(-1);
const static auto versionRegex = QRegularExpression("^version-([\\d.]+)$");
ArdwiinoLookup::ArdwiinoLookup(QObject *parent) : QObject(parent) {
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cd("firmware");
    QFile f(dir.filePath("version"));
    f.open(QFile::ReadOnly | QFile::Text);
    auto match2 = versionRegex.match(f.readAll());
    currentVersion = QVersionNumber::fromString(match2.captured(1));
}
auto ArdwiinoLookup::isOutdatedArdwiino(const unsigned short releaseID) -> bool {
    uint8_t major = (releaseID >> 8) & 0xff;
    uint8_t minor = (releaseID >> 4) & 0xf;
    uint8_t revision = (releaseID)&0xf;
    return QVersionNumber(major, minor, revision) < currentVersion;
}
auto ArdwiinoLookup::isOldAPIArdwiino(const QSerialPortInfo &serialPortInfo) -> bool {
    return isArdwiino(serialPortInfo) && serialPortInfo.serialNumber() == "1.2";
}
// Ardwino PS3 Controllers use sony vids. No other sony controller should expose a serial port however, so we should be fine doing this.
auto ArdwiinoLookup::isArdwiino(const QSerialPortInfo &serialPortInfo) -> bool {
    return serialPortInfo.vendorIdentifier() == HARMONIX_VID || serialPortInfo.vendorIdentifier() == SONY_VID || serialPortInfo.vendorIdentifier() == SWITCH_VID || (serialPortInfo.vendorIdentifier() == ARDWIINO_VID && serialPortInfo.productIdentifier() == ARDWIINO_PID);
}
auto ArdwiinoLookup::isArdwiino(const UsbDevice_t &usbDeviceId) -> bool {
    return usbDeviceId.vid == HARMONIX_VID || usbDeviceId.vid == SONY_VID || usbDeviceId.vid == SWITCH_VID || (usbDeviceId.vid == ARDWIINO_VID && usbDeviceId.pid == ARDWIINO_PID);
}
ArdwiinoLookup *ArdwiinoLookup::instance = nullptr;
const board_t ArdwiinoLookup::empty = {"", "", "", 0, {}, "", "", 0, "", false, false};
const board_t ArdwiinoLookup::pico = {"pico", "adafruit_feather_rp2040", "Raspberry PI Pico", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true};
const board_t ArdwiinoLookup::boards[21] = {
    {"uno-atmega16u2", "uno-usb", "Arduino Uno", 57600, {0x2FEF}, "dfu", "atmega16u2", 16000000, "/images/ArduinoUno.svg", true, false},
    {"uno-at90usb82", "uno-usb", "Arduino Uno", 57600, {0x2FF7}, "dfu", "at90usb82", 16000000, "/images/ArduinoUno.svg", true, false},
    {"uno", "uno-main", "Arduino Uno", 115200, {0x0043, 0x7523, 0x0001, 0xea60, 0x0243}, "arduino", "atmega328p", 16000000, "/images/ArduinoUno.svg", true, false},
    {"mini", "mini-main", "Arduino Pro Mini", 57600, {}, "arduino", "atmega328p", 16000000, "/images/ArduinoUno.svg", false, true},
    {"a-micro", "a-micro", "Arduino Micro in Bootloader Mode", 57600, {0x0037, 0x0237}, "avr109", "atmega32u4", 16000000, "/images/ArduinoMicro.svg", false, true},
    {"a-micro", "a-micro", "Arduino Micro", 57600, {0x8037, 0x8237}, "avr109", "atmega32u4", 16000000, "/images/ArduinoMicro.svg", false, false},
    {"micro", "micro", "Arduino Pro Micro", 57600, {0x9204}, "avr109", "atmega32u4", 8000000, "/images/ArduinoProMicro.svg", false, false},
    {"micro", "micro", "Arduino Pro Micro", 57600, {0x9206}, "avr109", "atmega32u4", 16000000, "/images/ArduinoProMicro.svg", false, false},
    {"leonardo", "leonardo", "Arduino Leonardo", 57600, {0x8036, 0x800c}, "avr109", "atmega32u4", 16000000, "/images/ArduinoLeonardo.svg", false, false},
    {"leonardo", "leonardo", "Arduino Micro / Pro Micro / Leonardo in Bootloader Mode", 57600, {0x0036}, "avr109", "atmega32u4", 16000000, "/images/ArduinoLeonardo.svg", false, true},
    {"micro", "micro", "Arduino Pro Micro in Bootloader Mode", 57600, {0x9203, 0x9207}, "avr109", "atmega32u4", 8000000, "/images/ArduinoLeonardo.svg", false, true},
    {"micro", "micro", "Arduino Pro Micro in Bootloader Mode", 57600, {0x9205}, "avr109", "atmega32u4", 16000000, "/images/ArduinoLeonardo.svg", false, true},
    {"mega2560-atmega16u2", "mega2560-usb", "Arduino Mega 2560", 57600, {0x2FEF}, "dfu", "atmega16u2", 16000000, "/images/ArduinoMega.svg", true, false},
    {"mega2560-at90usb82", "mega2560-usb", "Arduino Mega 2560", 57600, {0x2FF7}, "dfu", "at90usb82", 16000000, "/images/ArduinoMega.svg", true, false},
    {"mega2560", "mega2560-main", "Arduino Mega 2560", 115200, {0x0010, 0x0042}, "wiring", "atmega2560", 16000000, "/images/ArduinoMega.svg", true, false},
    {"megaadk-atmega16u2", "megaadk-usb", "Arduino Mega ADK", 57600, {0x2FEF}, "dfu", "atmega16u2", 16000000, "/images/ArduinoMegaADK.svg", true, false},
    {"megaadk-at90usb82", "megaadk-usb", "Arduino Mega ADK", 57600, {0x2FF7}, "dfu", "at90usb82", 16000000, "/images/ArduinoMegaADK.svg", true, false},
    {"megaadk", "megaadk-main", "Arduino Mega ADK", 115200, {0x003f, 0x0044}, "wiring", "atmega2560", 16000000, "/images/ArduinoMegaADK.svg", true, false},
    {"pico", "adafruit_feather_rp2040", "Raspberry PI Pico", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, false},
    {"pico", "adafruit_feather_rp2040", "Raspberry PI Pico", 0, {}, "pico", "rp2040", 0, "/images/Pico.svg", false, true},
    {"generic", "generic", "Generic Serial Device", 0, {}, "arduino", "", 0, "/images/ArduinoUno.svg", false, false},
};
const QString pico_boards[] = {
    "pico",
    "picow",
    "0xcb_helios",
    "adafruit_feather_rp2040",
    "adafruit_itsybitsy_rp2040",
    "adafruit_qtpy_rp2040",
    "adafruit_trinkey_qt2040",
    "adafruit_feather",
    "adafruit_feather_scorpio",
    "adafruit_feather_dvi",
    "arduino_nano_rp2040_connect",
    "adafruit_itsybitsy",
    "adafruit_qtpy",
    "adafruit_stemmafriend",
    "adafruit_trinkeyrp2040qt",
    "adafruit_macropad2040",
    "adafruit_kb2040",
    "arduino_nano_connect",
    "bridgetek_idm2040-7a",
    "cytron_maker_nano_rp2040",
    "cytron_maker_pi_rp2040",
    "datanoisetv_picoadk",
    "flyboard2040_core",
    "dfrobot_beetle_rp2040",
    "electroniccats_huntercat_nfc",
    "extelec_rc2040",
    "challenger_2040_lte",
    "challenger_2040_lora",
    "challenger_2040_subghz",
    "challenger_2040_wifi",
    "challenger_2040_wifi_ble",
    "challenger_nb_2040_wifi",
    "challenger_2040_sdrtc",
    "challenger_2040_nfc",
    "ilabs_rpico32",
    "melopero_cookie_rp2040",
    "melopero_shake_rp2040",
    "nullbits_bit_c_pro",
    "pimoroni_pga2040",
    "pimoroni_interstate75_rp2040",
    "pimoroni_keybow2040",
    "pimoroni_picolipo_4mb",
    "pimoroni_picolipo_16mb",
    "pimoroni_picosystem_rp2040",
    "pimoroni_plasma2040",
    "pimoroni_tiny2040",
    "pybstick26_rp2040",
    "solderparty_rp2040_stamp",
    "sparkfun_promicrorp2040",
    "sparkfun_micromod_rp2040",
    "vgaboard_rp2040",
    "sparkfun_thingplusrp2040",
    "upesy_rp2040_devkit",
    "seeed_xiao_rp2040",
    "vccgnd_yd_rp2040",
    "viyalab_mizu",
    "waveshare_rp2040_zero",
    "waveshare_rp2040_one",
    "waveshare_rp2040_plus_4mb",
    "waveshare_rp2040_plus_16mb",
    "waveshare_rp2040_lcd_0_96",
    "waveshare_rp2040_lcd_0.96",
    "waveshare_rp2040_lcd_1_28",
    "wiznet_5100s_evb_pico",
    "wiznet_wizfi360_evb_pico",
    "wiznet_5500_evb_pico"};
auto ArdwiinoLookup::findByBoard(const QString &board_name, bool inBootloader) -> const board_t {
    // the mini and is always in bootloader mode
    QString temp_name = board_name;
    if (board_name == "mini") inBootloader = true;
    for (const auto &board : pico_boards) {
        if (board_name == board) {
            temp_name = "pico";
            break;
        }
    }
    for (const auto &board : boards) {
        if (board.shortName == temp_name && board.inBootloader == inBootloader) {
            return board;
        }
    }
    return empty;
}

auto ArdwiinoLookup::detectBoard(const QSerialPortInfo &serialPortInfo) -> const board_t {
    for (const auto &board : boards) {
        for (auto &pid : board.productIDs) {
            if (pid && pid == serialPortInfo.productIdentifier()) {
                return board;
            }
        }
    }
    return boards[(sizeof(boards) / sizeof(board_t)) - 1];
}
auto ArdwiinoLookup::getInstance() -> ArdwiinoLookup * {
    if (!ArdwiinoLookup::instance) {
        ArdwiinoLookup::instance = new ArdwiinoLookup();
    }
    return ArdwiinoLookup::instance;
}

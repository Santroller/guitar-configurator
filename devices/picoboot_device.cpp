#include "picoboot_device.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QRegularExpression>
#include <QStorageInfo>
#include <QStringList>
#include <QTemporaryFile>
#include <QtEndian>
#include <algorithm>
#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <sstream>
#include <vector>

#include "ardwiinolookup.h"
#include "uf2.h"
#define MAIN_RAM_START 0x20000000u
#define MAIN_RAM_END 0x20042000u
#define FLASH_START 0x10000000u
#define FLASH_END 0x15000000u
#define XIP_SRAM_START 0x15000000u
#define XIP_SRAM_END 0x15004000u
// we require 256 (as this is the page size supported by the device)
#define LOG2_PAGE_SIZE 8u
#define PAGE_SIZE (1u << LOG2_PAGE_SIZE)
#define CONFIG_SECTION FLASH_START + (256 * 1024)
#define RF_SECTION FLASH_START + (512 * 1024)
PicobootDevice::PicobootDevice(UsbDevice_t devt, QObject *parent) : Device(devt, parent) {
    m_board = ArdwiinoLookup::empty;
    setBoardType("pico");
    auto dir = QDir(QCoreApplication::applicationDirPath());
    dir.cd("firmware");
    QFile file(dir.filePath("ardwiino-pico-rp2040.uf2"));
    auto f = [=](long a, long b, int step, int stepCount) {
        // return this->setPercentage(a, b, step, stepCount);
    };
    program(&file, NULL, f);
}
QString PicobootDevice::getDescription() {
    return "Raspberry PI Pico ("+m_deviceID.drivePath+")";
}
bool PicobootDevice::isReady() {
    return true;
}
void PicobootDevice::close() {
}
bool PicobootDevice::open() {
    return true;
}
void PicobootDevice::bootloader() {
}

void PicobootDevice::program(QFile *file, Ardwiino *parent, std::function<void(long, long, int, int)> progress) {
    QTemporaryFile tmp(QDir::temp().filePath("XXXXXX.uf2"));
    tmp.open();
    file->open(QIODevice::ReadOnly);
    uf2_block block;
    uint pos = 0;
    uint res;
    do {
        res = file->read((char *)&block, sizeof(uf2_block));
        if (res == 0) {
            break;
        }
        if (res < 0) {
            qDebug() << "There was a problem reading the uf2 file";
            break;
        }
        if (block.magic_start0 == UF2_MAGIC_START0 && block.magic_start1 == UF2_MAGIC_START1 &&
            block.magic_end == UF2_MAGIC_END) {
            if (block.flags & UF2_FLAG_FAMILY_ID_PRESENT && block.file_size == RP2040_FAMILY_ID &&
                !(block.flags & UF2_FLAG_NOT_MAIN_FLASH) && block.payload_size == PAGE_SIZE) {
                if (block.target_addr == CONFIG_SECTION) {
                    // We don't even need to write the config section, it will configure itself based on the defaults.
                    pos += sizeof(uf2_block);
                    continue;
                } else if (block.target_addr == RF_SECTION) {
                    // Copy the rf ids into the flash
                    uint32_t tx = parent->getRFID();
                    uint32_t rx = parent->generateClientRFID();
                    memcpy(block.data, &tx, sizeof(tx));
                    memcpy(block.data + sizeof(tx), &rx, sizeof(rx));
                }
            }
        }
        tmp.write((char *)&block);
        pos += sizeof(uf2_block);
    } while (true);
    QDir drive(m_deviceID.drivePath);
    tmp.copy(drive.absoluteFilePath("image.uf2"));
}
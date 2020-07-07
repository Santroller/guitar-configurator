#pragma once
typedef struct {
    QString shortName;
    QString hexFile;
    QString name;
    uint baudRate;
    int productIDs[9];
    QString protocol;
    QString processor;
    uint cpuFrequency;
    QString image;
    bool hasDFU;
} board_t;

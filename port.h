#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "ardwiinolookup.h"
#include "submodules/Ardwiino/src/shared/config/config.h"
#include "submodules/Ardwiino/src/shared/controller/controller.h"

class Port : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(bool isArdwiino READ isArdwiino)
    Q_PROPERTY(bool hasDFU MEMBER m_hasDFU NOTIFY dfuFound)
public:
    explicit Port(const QSerialPortInfo &serialPortInfo, QObject *parent = nullptr);
    explicit Port(QObject *parent = nullptr);
    board_t getBoard() const {
        return m_board;
    }
    void prepareUpload();
    void findNew();
    void open(const QSerialPortInfo &serialPortInfo);
    void close();
signals:
    void descriptionChanged(QString newValue);
    void dfuFound(bool found);

public slots:
    QString description() const {
        return m_description;
    }
    board_t board() const {
        return m_board;
    }
    QString boardName() const {
        return m_board.hexFile;
    }
    void setBoardFreq(uint freq) {
        m_board.cpuFrequency = freq;
    }
    bool isArdwiino() const {
        return m_isArdwiino;
    }
    QString getPort() const {
        return m_port;
    }
    void handleError(QSerialPort::SerialPortError serialPortError);
    bool findNewAsync();
    void update();
private:
    void rescan(const QSerialPortInfo &serialPortInfo);
    QString m_description;
    QString m_port;
    QSerialPort* m_serialPort;
    QByteArray readData;
    board_t m_board;
    QList<QSerialPortInfo> m_port_list;
    bool m_isArdwiino;
    bool m_hasDFU;
    bool readDFU;
};

#endif // PORT_H

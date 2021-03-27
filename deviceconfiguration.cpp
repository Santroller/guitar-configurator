#include "deviceconfiguration.h"
DeviceConfiguration::DeviceConfiguration(Configuration_t config, QObject* parent):QObject(parent),m_config(config) {
};
const QStringList DeviceConfiguration::pins = {"Up","Down","Left","Right","Start","Back","LeftStick","RightStick","LB","RB","Home","Capture","A","B","X","Y","Lt","Rt","LX","LY","RX","RY","SP"};
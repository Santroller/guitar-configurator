#include "status.h"
const QString Status::values[8] = {
    "Waiting", "Programming Main Controller",
    "Waiting for DFU Mode", "Programming USB Controller",
    "Programming USB Controller", "Programming USB Controller",
    "Waiting for Reconnection", "Finished Programming"
};
Status::Status()
{

}

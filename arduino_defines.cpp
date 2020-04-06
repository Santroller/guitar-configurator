#include "ardwiino_defines.h"
ArdwiinoDefines* ArdwiinoDefines::instance = nullptr;
auto ArdwiinoDefines::getInstance() -> ArdwiinoDefines* {
    if (!ArdwiinoDefines::instance) {
        ArdwiinoDefines::instance = new ArdwiinoDefines();
    }
    return ArdwiinoDefines::instance;
}

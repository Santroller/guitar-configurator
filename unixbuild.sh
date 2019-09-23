#!/bin/bash

UTARGET=$(uname)
BINSUFFIX=""

if [ "$UTARGET" = "Darwin" ]; then
  export UPLATFORM="mac"
elif [ "$UTARGET" = "Linux" ]; then
  export UPLATFORM="linux_$(uname -m)"
elif [ "${UTARGET/MINGW32/}" != "$UTARGET" ]; then
  export UPLATFORM="win32"
  export BINSUFFIX=".exe"
else
  # Fallback to something...
  export UPLATFORM="$UTARGET"
fi

if [ "$UPLATFORM" = "mac" ]; then
  if [ ! -d /opt/qt56sm ]; then
    curl -L -o /tmp/qt-5.6.3-static-mac.zip https://github.com/distdb/qtbuilds/blob/master/qt-5.6.3-static-mac.zip?raw=true || exit 1
    qtsum=$(shasum -a 256 /tmp/qt-5.6.3-static-mac.zip | cut -f1 -d' ')
    qtexpsum="214d22d8572ea6162753c8dd251d79275f3b22d49204718c637d722409e0cfcb"
    if [ "$qtsum" != "$qtexpsum" ]; then
      echo "Qt hash $qtsum does not match $qtexpsum"
      exit 1
    fi
    sudo mkdir -p /opt || exit 1
    cd /opt || exit 1
    sudo unzip -q /tmp/qt-5.6.3-static-mac.zip || exit 1
    cd - || exit 1
  fi

  export PATH="/opt/qt56sm/bin:$PATH"
elif [ "$UPLATFORM" = "win32" ]; then
  # Install missing dependencies
  pacman -S --noconfirm --needed zip unzip curl perl mingw-w64-i686-toolchain mingw-w64-i686-cmake || exit 1

  # Fix PATH to support running shasum.
  export PATH="/usr/bin/core_perl:$PATH"

  if [ ! -d "/c/Qt/5.6/mingw49_32_release_static/" ]; then
    curl -L -o /tmp/qt-5.6.3-static-win32.zip https://github.com/distdb/qtbuilds/blob/master/qt-5.6.3-static-win32.zip?raw=true || exit 1
    qtsum=$(shasum -a 256 /tmp/qt-5.6.3-static-win32.zip | cut -f1 -d' ')
    qtexpsum="bcd85145d6fed00da37498c08c49d763c6fa883337f754880b5c786899e6bb1d"
    if [ "$qtsum" != "$qtexpsum" ]; then
      echo "Qt hash $qtsum does not match $qtexpsum"
      exit 1
    fi
    mkdir -p /c/Qt/5.6 || exit 1
    cd /c/Qt/5.6 || exit 1
    unzip -q /tmp/qt-5.6.3-static-win32.zip || exit 1
    cd - || exit 1
  fi

  export PATH="/c/Qt/5.6/mingw49_32_release_static/bin:$PATH"
fi

echo "Attempting to build guitar-configurator for ${UPLATFORM}..."

rm -rf dist
mkdir -p dist || exit 1

# Build
# -flto is flawed on CI atm
if [ "$UPLATFORM" = "mac" ]; then
    qmake QMAKE_CXXFLAGS+=-flto QMAKE_LFLAGS+=-flto CONFIG+=optimize_size || exit 1
elif [ "$UPLATFORM" = "win32" ]; then
    qmake QMAKE_CXXFLAGS="-static -flto -Os" QMAKE_LFLAGS="-static -flto -Os" CONFIG+=optimize_size CONFIG+=staticlib CONFIG+=static || exit 1
else
    qmake CONFIG+=optimize_size || exit 1
fi

make || exit 1

# Move the binary out of the dir
if [ "$UPLATFORM" = "win32" ]; then
    mv "release/${1}${BINSUFFIX}" "${1}${BINSUFFIX}" || exit 1
fi

if [ "$UPLATFORM" = "mac" ]; then
    strip -x UEFITool.app/Contents/MacOS/UEFITool || exit 1
    zip -qry dist/"${1}_${2}_${UPLATFORM}.zip" UEFITool.app ${4} || exit 1
else
    strip -x "${1}${BINSUFFIX}" || exit 1
    zip -qry dist/"${1}_${2}_${UPLATFORM}.zip" "${1}${BINSUFFIX}" ${4} || exit 1
fi

exit 0
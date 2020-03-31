#!/bin/bash
cp org.kde.kf5auth.example.service /usr/share/dbus-1/system-services
cp org.kde.kf5auth.example.conf /usr/share/dbus-1/system.d
/lib/kauth/kauth-policy-gen org.kde.kf5auth.example.actions /usr/share/polkit-1/actions/org.kde.kf5auth.example.policy 
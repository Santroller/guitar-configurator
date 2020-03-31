#!/bin/sh
sudo /lib/kauth/kauth-policy-gen org.kde.kf5auth.example.policy /usr/share/polkit-1/actions/org.kde.kf5auth.example.policy
sudo cp org.kde.kf5auth.example.conf /usr/share/dbus-1/system.d
sudo cp org.kde.kf5auth.example.service /usr/share/dbus-1/service
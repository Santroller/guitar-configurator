/*
 *   Copyright (C) 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation; either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .
 */

#undef QT_NO_CAST_FROM_ASCII
#include <main.h>
ActionReply MyHelper::read(const QVariantMap& args)
{
    ActionReply reply;
    QString filename = args["filename"].toString();
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
       reply = ActionReply::HelperErrorReply();
       reply.setErrorDescription(file.errorString());
       return reply;
    }
    QTextStream stream(&file);
    QString contents;
    stream >> contents;
    reply.addData("contents", contents);
    return reply;
}
ActionReply MyHelper::write(const QVariantMap &args)
{
    Q_UNUSED(args)
    return ActionReply::SuccessReply();
}

ActionReply MyHelper::longaction(const QVariantMap&)
{
    for (int i = 1; i <= 100; i++) {
       if (HelperSupport::isStopped())
          break;
       HelperSupport::progressStep(i);
       QThread::usleep(250000);
    }
    return ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("org.kde.kf5auth.example", MyHelper)


// Mule Microcontroller Simulator - Arduino-like microcontroller simulator for debugging and testing purposes
// Copyright (C) 2018 Tim K <timprogrammer@rambler.ru>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "microcontrollerwindow.h"
#include <QApplication>
#include <QStringList>
#include <QProcess>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList argsList = a.arguments();
    if (argsList.size() > 1 && QFile::exists(argsList.at(1))) {
	QString cmd = "\"" + argsList.at(1) + "\"";
	if (QFile::exists("./" + argsList.at(1)))
		cmd = "\"./" + argsList.at(1) + "\"";
#ifdef Q_OS_WIN
	cmd.replace('/', '\\');
#endif
	QProcess::startDetached(cmd);

    }
    MicrocontrollerWindow w;
    w.show();

    return a.exec();
}

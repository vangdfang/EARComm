/*
 * Project:     EARComm - Emergency Alert Radio Tool
 * Name:        main.cpp
 * Description: Main function
 * Copyright:   (c) 2010 Doug Kelly
 *
 *  EARComm is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  EARComm is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with EARComm.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>
#include "earcomm.h"
#include "data.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EARComm w;

    w.show();
    return a.exec();
}

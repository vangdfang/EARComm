/*
 * Project:     EARComm - Emergency Alert Radio Tool
 * Name:        earcomm.h
 * Description: Main application class
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

#ifndef EARCOMM_H
#define EARCOMM_H

#include <QMainWindow>
#include "data.h"

class QComboBox;
class QTimer;

namespace Ui
{
    class EARComm;
}

class EARComm : public QMainWindow
{
    Q_OBJECT

public:
    EARComm(QWidget *parent = 0);
    ~EARComm();

private:
    Ui::EARComm *ui;
    QTimer *timer;
    Data d;
    int EARtype;
    void updateFrequencyList();
    void populateRegionList(QComboBox &regionBox);
    void populateStateList(std::string state, std::string code);
    void updateCountyList(QComboBox &countyBox, const QString &state);
    void updateRegionList(QComboBox &regionBox, const QString &state);
    std::string getSynthSetup(int channel);

private slots:
    void on_FIPS1St_currentIndexChanged(const QString &text);
    void on_FIPS2St_currentIndexChanged(const QString &text);
    void on_FIPS3St_currentIndexChanged(const QString &text);
    void on_FIPS4St_currentIndexChanged(const QString &text);
    void on_readButton_clicked();
    void on_programButton_clicked();
    void on_testButton_clicked();
    void updateEvents();
    void parseSAME(const std::string &data);
};

#endif // EARCOMM_H

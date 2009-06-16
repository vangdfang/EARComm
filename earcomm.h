#ifndef EARCOMM_H
#define EARCOMM_H

#include <QtGui/QMainWindow>
#include <QtGui/QComboBox>
#include "data.h"

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
    Data d;
    int EARtype;
    void updateFrequencyList();
    void updateCountyList(QComboBox &countyBox, const QString &state);

private slots:
    void on_FIPS1St_currentIndexChanged(const QString &text);
    void on_FIPS2St_currentIndexChanged(const QString &text);
    void on_FIPS3St_currentIndexChanged(const QString &text);
    void on_FIPS4St_currentIndexChanged(const QString &text);
    void on_readButton_clicked();
    void on_programButton_clicked();
    void on_testButton_clicked();
};

#endif // EARCOMM_H

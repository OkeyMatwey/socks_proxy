#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <fwpmu.h>
#include <fwpmtypes.h>
#include <fwptypes.h>
#include <iketypes.h>
#include <ipsectypes.h>
#include <netiodef.h>
#include <stdio.h>
#include <Winerror.h>

#pragma comment(lib, "fwpuclnt.lib")

#define cout qDebug()

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    GUID GUID_SUBLAYER;
    GUID GUID_CALLOUT;
    GUID GUID_FILTER;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void Install();
};

#endif // MAINWINDOW_H

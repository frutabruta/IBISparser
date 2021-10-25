#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "soubor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString saveToFile();

    QString loadFromFile();
public slots:
    void pridejChybuDoOkna(QString vstup);
private slots:
    void on_pushButton_process_clicked();

    void on_pushButtonInputFile_clicked();

    void on_pushButton_outputFile_clicked();

private:
    Ui::MainWindow *ui;
    Soubor soubor;

    QByteArray obsahSouboru;
};
#endif // MAINWINDOW_H

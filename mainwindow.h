#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QIntValidator>
#include <QMessageBox>
#include <qfile.h>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>
#include <QFutureWatcher>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_ChooseFile_clicked();
    void on_run_clicked();
    void init();
    bool isDataValid();
    void getDataFromLines();

private:
    Ui::MainWindow *ui;
    QString fileName;
    qint64 x;
    qint64 y;
    qint64 xb;
    qint64 xs;
    qint64 yb;
    qint64 ys;

};
#endif // MAINWINDOW_H

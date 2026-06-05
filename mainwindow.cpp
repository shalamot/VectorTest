#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    validator();
    ui->lineX->setValidator(validator);
    ui->lineY->setValidator(validator);
    ui->lineXs->setValidator(validator);
    ui->lineXb->setValidator(validator);
    ui->lineYs->setValidator(validator);
    ui->lineYb->setValidator(validator);
    ui->results->setReadOnly(true);
}

void MainWindow::on_ChooseFile_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt)"));
    ui->ChoosenFile->setText(fileName);
}

void MainWindow::on_run_clicked()
{
    ui->run->setEnabled(false);
    getDataFromLines();
    if (!isDataValid())
    {
        QMessageBox::warning(this, "Ошибка валидации данных", "Проверьте, что:\nx > 40, y > 40\nxb > xs, yb > ys\nx > xb, y > yb\nxb, xs, yb, ys нечетные");
        ui->run->setEnabled(true);
        return;
    }

    QVector<float> matrix;
    QString errorStr;

    if (!processor.loadMatrix(fileName, x, y, matrix, errorStr)) {
        QMessageBox::critical(this, "Ошибка валидации файла", errorStr);
        ui->run->setEnabled(true);
        return;
    }

    QFuture<QVector<CfarPeak>> future = QtConcurrent::run([=, matrix = std::move(matrix)]() {
        QVector<double> matrixI = processor.buildIntegralMatrix(matrix, x, y);
        return processor.runCfar(matrix, matrixI, x, y, xb, yb, xs, ys);
    });

    QFutureWatcher<QVector<CfarPeak>> *watcher = new QFutureWatcher<QVector<CfarPeak>>(this);

    connect(watcher, &QFutureWatcher<QVector<CfarPeak>>::finished, this, [=]() {
        QVector<CfarPeak> results = watcher->result();

        ui->results->clear();
        ui->results->append(QString("Обнаружено превышений порога: %1").arg(results.size()));

        for (CfarPeak &peak : results) {
            ui->results->append(QString("Строка: %1, Столбец: %2 -> Значение: %3")
                                        .arg(peak.y + 1).arg(peak.x + 1).arg(peak.value));
        }

        ui->run->setEnabled(true);
        watcher->deleteLater();
    });

    watcher->setFuture(future);
}

void MainWindow::getDataFromLines()
{
    x = ui->lineX->text().toInt();
    y = ui->lineY->text().toInt();
    xb = ui->lineXb->text().toInt();
    xs = ui->lineXs->text().toInt();
    yb = ui->lineYb->text().toInt();
    ys = ui->lineYs->text().toInt();
}

bool MainWindow::isDataValid()
{
    return x > 40 && y > 40 && xb > xs && yb > ys && x > xb && y > yb &&
           xb % 2 == 1 && xs % 2 == 1 && yb % 2 == 1 && ys % 2 == 1;
}

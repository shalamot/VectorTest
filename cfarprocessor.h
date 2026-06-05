#ifndef CFARPROCESSOR_H
#define CFARPROCESSOR_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QFile>
#include <QFileInfo>
#include <cmath>
#include <QRegularExpression>

struct CfarPeak {
    qint64 x;
    qint64 y;
    float value;
};

class CfarProcessor
{
public:
    explicit CfarProcessor();

    bool loadMatrix(const QString &filePath, qint64 X, qint64 Y, QVector<float> &outMatrix, QString &errorStr);

    QVector<double> buildIntegralMatrix(const QVector<float> &matrixA, qint64 X, qint64 Y);

    QVector<CfarPeak> runCfar(const QVector<float> &matrixA,
                              const QVector<double> &matrixI,
                              qint64 X, qint64 Y,
                              qint64 Xb, qint64 Yb,
                              qint64 Xs, qint64 Ys);
};

#endif // CFARPROCESSOR_H

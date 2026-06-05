#include "cfarprocessor.h"

CfarProcessor::CfarProcessor(){}

bool CfarProcessor::loadMatrix(const QString &filePath, qint64 X, qint64 Y, QVector<float> &outMatrix, QString &errorStr) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorStr = "Не удалось открыть файл для чтения.";
        return false;
    }

    QTextStream in(&file);
    outMatrix.clear();
    outMatrix.reserve(X * Y);
    qint64 rowCount = 0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }

        QStringList tokens = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        rowCount++;

        if (rowCount > Y) {
            errorStr = QString("Ошибка: Файл содержит больше строк, чем задано параметром Y (%1).").arg(Y);
            return false;
        }
        if (tokens.size() != X) {
            errorStr = QString("Ошибка в строке %1: обнаружено %2 элементов, а ожидалось X = %3.")
                           .arg(rowCount).arg(tokens.size()).arg(X);
            return false;
        }

        for (QString &token : tokens) {
            bool isFloat;
            float value = token.toFloat(&isFloat);
            if (!isFloat) {
                errorStr = QString("Ошибка в строке %1: элемент '%2' не является числом типа float.")
                               .arg(rowCount).arg(token);
                return false;
            }
            outMatrix.append(value);
        }
    }
    file.close();

    if (rowCount != Y) {
        errorStr = QString("Ошибка: Файл содержит меньше строк (%1), чем задано параметром Y (%2).")
                       .arg(rowCount).arg(Y);
        return false;
    }
    return true;
}

QVector<double> CfarProcessor::buildIntegralMatrix(const QVector<float> &matrixA, qint64 X, qint64 Y) {
    qint64 intX = X + 1;
    qint64 intY = Y + 1;
    QVector<double> matrixI(intX * intY, 0.0);

    for (qint64 y = 1; y <= Y; ++y) {
        qint64 curr_row = y * intX;
        qint64 prev_row = (y - 1) * intX;
        qint64 src_row = (y - 1) * X;

        for (qint64 x = 1; x <= X; ++x) {
            float valA = matrixA[src_row + (x - 1)];
            double top = matrixI[prev_row + x];
            double left = matrixI[curr_row + (x - 1)];
            double top_left = matrixI[prev_row + (x - 1)];

            matrixI[curr_row + x] = valA + top + left - top_left;
        }
    }
    return matrixI;
}

QVector<CfarPeak> CfarProcessor::runCfar(const QVector<float> &matrixA,
                                         const QVector<double> &matrixI,
                                         qint64 X, qint64 Y,
                                         qint64 Xb, qint64 Yb,
                                         qint64 Xs, qint64 Ys) {
    QVector<CfarPeak> peaks;

    double err = 0.01;
    double sq = (Xb * Yb) - (Xs * Ys);
    double Th = std::pow(1.0 / err, 1.0 / sq) - 1.0;

    qint64 hXb = Xb / 2;
    qint64 hYb = Yb / 2;
    qint64 hXs = Xs / 2;
    qint64 hYs = Ys / 2;

    qint64 intX = X + 1;

    for (qint64 y = hYb; y < Y - hYb; ++y) {
            for (qint64 x = hXb; x < X - hXb; ++x) {
            int xb1 = x - hXb, yb1 = y - hYb, xb2 = x + hXb, yb2 = y + hYb;
            int xs1 = x - hXs, ys1 = y - hYs, xs2 = x + hXs, ys2 = y + hYs;

            double Sb = matrixI[(yb2 + 1) * intX + (xb2 + 1)]
                        - matrixI[yb1 * intX + (xb2 + 1)]
                        - matrixI[(yb2 + 1) * intX + xb1]
                        + matrixI[yb1 * intX + xb1];

            double Ss = matrixI[(ys2 + 1) * intX + (xs2 + 1)]
                        - matrixI[ys1 * intX + (xs2 + 1)]
                        - matrixI[(ys2 + 1) * intX + xs1]
                        + matrixI[ys1 * intX + xs1];

            float V = matrixA[y * X + x];

            if ((Sb - Ss) * Th < V) {
                    peaks.append({x, y, V});
            }
        }
    }
    return peaks;
}
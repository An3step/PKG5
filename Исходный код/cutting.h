#ifndef CUTTING_H
#define CUTTING_H

#include <QtWidgets>

QT_BEGIN_NAMESPACE
namespace Ui { class Cutting; }
QT_END_NAMESPACE

class Cutting : public QMainWindow
{
    Q_OBJECT

public:
    Cutting(QWidget *parent = nullptr);

    ~Cutting();

    void SystemCoord();

    void SetStartLines();

    void resizeEvent(QResizeEvent*);

    void MiddlePoint(QLineF&);

    void setRect();

    void SetPolugon();

    double scalar(const QLineF&,const QLineF&) const;

    void SetNewL();

    void SetNewL(QLineF&, QColor c = Qt::red);

    void SetDashL(QLineF&, QColor c = Qt::black);

    void CutLines();

    double length(QLineF&) const;

    bool UBorder(QLineF&) const;

    bool UNBorder(QLineF&) const;

    QString to_number(int) const;

    bool isConvex() const;

    void CutPolygon();

    void InfoWindow(bool error = false);

private slots:
    void on_DirectoryC_clicked();

    void on_RateChanging_valueChanged(int value);

private:
    Ui::Cutting *ui;
    QPixmap* pixmap;
    QRectF rect;
    QVector<QLineF> lines;
    QVector<QLineF> dash;
    int xborder = 100;
    int yborder = 100;
    const double e = 10e-7;
    int z = 0;
    int rate = 25;
    bool q = false;
    bool isSq = true;
    QVector<QLineF> NLines;
    QVector<QLineF> polygon;
    QPointF center;
};
#endif // CUTTING_H

#include "cutting.h"
#include "ui_cutting.h"
#include <fstream>
#include <string>
double min(double q1, double q2)
{
    return (q1 < q2) ? q1 : q2;
}
double max(double q1, double q2)
{
    return (q2 >= q1) ? q2 : q1;
}
Cutting::Cutting(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Cutting)
{
    ui->setupUi(this);
    this->resize(600, 600);
    ui->label->resize(600, 600);
    pixmap = new QPixmap(ui->label->size());
    pixmap->fill(Qt::white);
    ui->label->setMinimumSize(40, 40);
    ui->RateChanging->setRange(1, 40);
    ui->RateChanging->setValue(25);
    SystemCoord();
    InfoWindow();
}

Cutting::~Cutting()
{
    delete ui;
}

void Cutting::SystemCoord()
{
    *pixmap = pixmap->scaled(ui->label->size());
    pixmap->fill(Qt::white);
    QPainter painter(pixmap);
    QPen pen;
    pen.setWidth(5);
    painter.setPen(pen);
    painter.drawLine(0, pixmap->height()/2, pixmap->width(), pixmap->height()/2);
    painter.drawLine(pixmap->width()/2, 0, pixmap->width()/2, pixmap->height());
    pen.setWidth(1);
    painter.setPen(pen);
    int w = pixmap->width() / rate;
    int h = pixmap->height() / rate;
    int hw = pixmap->width() / 2;
    int hh = pixmap->height()/2;
    for(int i = 1; i < w/2; i++)
    {
        painter.drawText(QPointF(hw + rate * i - 7, hh + 10), QString::number(i));
        painter.drawText(QPointF(hw - rate * i - 7, hh + 10), QString::number(-i));
        painter.drawLine(hw - rate * i, 0,hw -  rate * i, pixmap->height());
        painter.drawLine(hw + rate * i, 0,hw +  rate * i, pixmap->height());
    }
    for(int i = 1; i < h/2; i++)
    {
        painter.drawText(QPointF(hw - 14, hh + rate * i + 10), QString::number(-i));
        painter.drawText(QPointF(hw - 14, hh -  rate * i + 10), QString::number(i));
        painter.drawLine(0, hh - i * rate, pixmap->width(), hh - i * rate);
        painter.drawLine(0, hh + i * rate, pixmap->width(), hh + i * rate);
    }
    painter.drawText(QPointF(hw - 7, hh + 10), "0");
    painter.end();
    center = QPoint(ui->label->width() / 2, ui->label->height() / 2);
    SetStartLines();
    if(q)
    {
        if(isSq)
        {
            setRect();
            CutLines();
        }
        else
        {
            CutPolygon();
            SetPolugon();
        }
    }
    ui->label->setPixmap(*pixmap);
}

void Cutting::SetStartLines()
{
    for(int i = 0; i < dash.size(); i++)
    {
        SetDashL(dash[i]);
    }
    ui->label->setPixmap(*pixmap);
}

void Cutting::resizeEvent(QResizeEvent * e)
{
    center = QPoint(pixmap->width() / 2, pixmap->height() / 2);
    SystemCoord();
    QMainWindow::resizeEvent(e);
}

void Cutting::MiddlePoint(QLineF& l)
{
    if(false)
    {
        return;
    }
    if(UBorder(l))
    {
        SetNewL(l);
        return;
    }
    if(UNBorder(l))
    {
        return;
    }
    QLineF nl = l;
    nl.setP1(QPointF(float(l.x2() + l.x1())/2, float(l.y2() + l.y1()) / 2));
    MiddlePoint(nl);
    nl.setP1(l.p1());
    nl.setP2(QPointF((l.x2() + l.x1())/2, (l.y2() + l.y1()) / 2));
    MiddlePoint(nl);
}

void Cutting::setRect()
{
    QPainter painter(pixmap);
    QPen pen;
    pen.setWidth(5);
    pen.setColor(Qt::blue);
    painter.setPen(pen);
    QRectF DRRect(QPointF(rect.left() * rate + center.x(), center.y() - rect.top() * rate),
                 QPointF(rect.right()* rate + center.x(), center.y() - rect.bottom()*rate));
    painter.drawRect(DRRect);
    painter.end();
    ui->label->setPixmap(*pixmap);
}

void Cutting::SetPolugon()
{
    for(int i = 0; i < polygon.size(); i++)
    {
        SetNewL(polygon[i], Qt::blue);
    }
}

double Cutting::scalar(const QLineF & line1,const QLineF & line2) const
{
    return (line1.x2() - line1.x1()) * (line2.y2() - line2.y1()) - (line1.y2() - line1.y1()) * (line2.x2() - line2.x1());
}

void Cutting::SetNewL()
{
    QPainter painter(pixmap);
    QPen pen;
    pen.setWidth(4);
    pen.setColor(Qt::magenta);
    painter.setPen(pen);
    for(QLineF& l: NLines)
    {
        painter.drawLine(l);
    }
    ui->label->setPixmap(*pixmap);
}

void Cutting::SetNewL(QLineF & l, QColor c)
{
    QLineF nl = l;
    nl.setP1(center + QPointF(rate*l.p1().x(), -rate*l.p1().y()));
    nl.setP2(center + QPointF(rate*l.p2().x(), -rate*l.p2().y()));
    QPainter painter(pixmap);
    QPen pen;
    pen.setWidth(4);
    pen.setColor(c);
    painter.setPen(pen);
    painter.drawLine(nl);
    ui->label->setPixmap(*pixmap);
}

void Cutting::SetDashL(QLineF & l, QColor c)
{
    QLineF nl = l;
    nl.setP1(center + QPointF(rate*l.p1().x(), -rate*l.p1().y()));
    nl.setP2(center + QPointF(rate*l.p2().x(), -rate*l.p2().y()));
    QPainter painter(pixmap);
    QPen pen;
    pen.setWidth(4);
    pen.setColor(c);
    pen.setStyle(Qt::DashLine);
    pen.setDashOffset(4);
    painter.setPen(pen);
    painter.drawLine(nl);
    ui->label->setPixmap(*pixmap);
}

void Cutting::CutLines()
{
    for(int i = 0; i < lines.size(); i++)
    {
        z = 0;
        MiddlePoint(lines[i]);
    }
    SetNewL();
}

bool Cutting::UBorder(QLineF& line) const
{
    if(line.x1() >= rect.left() && line.x1() <= rect.right())
    {
        if(line.x2() >= rect.left() && line.x2() <= rect.right())
        {
            if(line.y1() >= rect.bottom() && line.y1() <= rect.top())
            {
                if(line.y2() >= rect.bottom() && line.y2() <= rect.top())
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Cutting::UNBorder(QLineF & line) const
{
    if(line.x1() <= rect.left() && line.x2() <= rect.left())
    {
        return true;
    }
    if(line.x1() >= rect.right() && line.x2() >= rect.right())
    {
        return true;
    }
    if(line.y1() >= rect.top() && line.y2() >= rect.top())
    {
        return true;
    }
    if(line.y2() <= rect.bottom() && line.y1() <= rect.bottom())
    {
        return true;
    }
    return false;
}

QString Cutting::to_number(int number) const
{
    return QString::number(number);
}

bool Cutting::isConvex() const
{
    if(polygon.size() < 3)
    {
        return false;
    }
    if(scalar(polygon[0], polygon[1]) >= 0)
    {
        for(int i = 1; i < polygon.size() - 1; i++)
        {
            if(scalar(polygon[i], polygon[i + 1]) < 0)
            {
                return false;
            }
        }
        if(scalar(polygon[polygon.size() - 1], polygon[0]) < 0)
        {
            return false;
        }
    }
    else
    {
        for(int i = 1; i < polygon.size() - 1; i++)
        {
            if(scalar(polygon[i], polygon[i + 1]) > 0)
            {
                return false;
            }
        }
        if(scalar(polygon[polygon.size() - 1], polygon[0]) > 0)
        {
            return false;
        }
    }
    return true;
}

void Cutting::CutPolygon()
{
    for(QLineF& line: lines)
    {
        double p, q;
        double tmin = 0;
        double tmax = 1;
        for(int i = 0; i < polygon.size() - 1; i++)
        {
            QLineF l(line.p1(), polygon[i].p1());
            p = scalar(line, polygon[i]);
            q = scalar(l, polygon[i]);
            if(p == 0)
            {
                if(q < 0)
                {
                    tmin = 0;
                    tmax = -1;
                    break;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                if(p > 0)
                {
                    tmax = min(double(q/p), tmax);
                }
                else
                {
                    tmin = max(double(q / p), tmin);
                }
            }
            if(tmin > tmax)
            {
                tmin = 0;
                tmax = -1;
                break;
            }
        }
        if(tmin < tmax)
        {
            QPointF p1(line.x1() + tmin * (line.x2() - line.x1()), line.y1() + tmin * (line.y2() - line.y1()));
            QPointF p2(line.x1() + tmax * (line.x2() - line.x1()), line.y1() + tmax * (line.y2() - line.y1()));
            line = QLineF(p1, p2);
            SetNewL(line);
        }
    }
}

void Cutting::InfoWindow(bool error)
{
    QString info = "";
    if(error)
    {
        info += "НЕКОРРЕКТНЫЕ ВХОДНЫЕ ДАННЫЕ\n";
    }
    info += "Формат входного файла\n";
    info += "isSq - вариант(Square или Polygon)\n";
    info += "n  - число отрезков\n";
    info += "X1_1 Y1_1 X2_1 Y2_1\n";
    info += "X1_2 Y1_2 X2_2 Y2_2\n";
    info += "...\n";
    info += "X1_N Y1_N X2_N Y2_N\n";
    info += "Если прямоугольник:\n";
    info += "X_MIN Y_MIN X_MAX Y_MAX - координаты отсекающего прямоугольника\n";
    info += "Если полигон:\n";
    info += "q - число точек в полигоне(>= 3)\n";
    info += "Xq_1 Yq_1\n";
    info += "Xq_2 Yq_2\n";
    info += "...\n";
    info += "Xq_Q Yq_Q\n";
    info += "Примечания:\n";
    info += "X_MIN < X_MAX, Y_MIN < Y_MAX\n";
    info += "Точки должны задавать Выпуклый многоулольник\n";
    QDialog qd(this);
    qd.resize(500, 500);
    QVBoxLayout* qv = new QVBoxLayout(&qd);
    qv->addWidget(new QLabel(info, &qd));
    qd.exec();
}

double Cutting::length(QLineF & line) const
{
    float x = line.x2() - line.x1();
    float y = line.y2() - line.y1();
    return sqrt(x * x + y * y);
}


void Cutting::on_DirectoryC_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Выбор текстового файла"), "", tr("Text Files(*.txt)"));
    q = true;

    std::ifstream file(path.toStdString());
    int size;
    polygon.clear();
    lines.clear();
    q = false;
    SystemCoord();
    std::string mode;
    file >> mode;

    if (mode != "Square" && mode != "Polygon")
    {
        InfoWindow(true);
        return;
    }

    file >> size;
    if (size <= 0)
    {
        InfoWindow(true);
        return;
    }

    lines.clear();
    lines.resize(size);
    int x, y, xe, ye;

    try
    {
        for (int i = 0; i < size; i++)
        {
            if (!(file >> x >> y >> xe >> ye))
            {
                throw std::fstream::failure("Invalid line format");
            }
            lines[i] = QLineF(x, y, xe, ye);
        }
    }
    catch (std::fstream::failure&)
    {
        InfoWindow(true);
        return;
    }

    dash = lines;
    SetStartLines();

    if (mode == "Square") {
        if (!(file >> x >> y >> xe >> ye)) {
            InfoWindow(true);
            return;
        }
        rect = QRectF(QPointF(x, y), QPointF(xe, ye));
        file.close();
        CutLines();
        setRect();
    } else {
        polygon.clear();
        int spol;
        if (!(file >> spol) || spol < 3) {
            InfoWindow();
            return;
        }
        polygon = QVector<QLineF>(spol);
        if (!(file >> x >> y)) {
            InfoWindow();
            return;
        }
        for (int i = 0; i < spol - 1; i++) {
            if (!(file >> xe >> ye)) {
                InfoWindow(true);
                return;
            }
            polygon[i] = QLineF(x, y, xe, ye);
            x = xe;
            y = ye;
        }
        polygon[spol - 1] = QLineF(x, y, polygon.first().x1(), polygon.first().y1());
        file.close();
        if(!isConvex())
        {
            InfoWindow(true);
            return;
        }
        CutPolygon();
        SetPolugon();
    }

    q = true;
}

void Cutting::on_RateChanging_valueChanged(int value)
{
    rate = value;
    SystemCoord();
}

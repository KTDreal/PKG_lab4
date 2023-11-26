
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <iostream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1200, 800);
    ui->radius->setDisabled(true);
    ui->radius->setValue(7);
    pix = new QPixmap(700, 700);
    delta = 0.9 * pix->width() / (2 * range);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    ui->time_label->setText(QString::number(duration));
    switch(index)
    {
    case STEP_BY_STEP:
        ui->groupBox->setTitle("Point 1");
        ui->radius->setDisabled(true);
        ui->x2->setDisabled(false);
        ui->y2->setDisabled(false);
        break;
    case DDA:
        ui->groupBox->setTitle("Point 1");
        ui->radius->setDisabled(true);
        ui->x2->setDisabled(false);
        ui->y2->setDisabled(false);
        break;
    case BRESENHAM:
        ui->groupBox->setTitle("Point 1");
        ui->radius->setDisabled(true);
        ui->x2->setDisabled(false);
        ui->y2->setDisabled(false);
        break;
    case BRESENHAM_CIRCLE:
        ui->groupBox->setTitle("Central point");
        ui->radius->setDisabled(false);
        ui->x2->setDisabled(true);
        ui->y2->setDisabled(true);
        break;
    default:
        break;
    }
}

void MainWindow::paintEvent(QPaintEvent *)
{
    pix->fill(Qt::white);
    QPainter painter(pix);
    painter.setBrush(Qt::black);
    int textStep = range / 5;
    for(int i = 0; i <= 2 * range;i++)
    {
        painter.setPen(QPen(Qt::gray,1));
        painter.drawLine(pix->width()*(0.05+step*i),pix->height()*0.05,pix->width()*(0.05+step*i),pix->height()*0.95);
        painter.drawLine(pix->width()*0.05,pix->height()*(0.05+step*i),pix->width()*0.95,pix->height()*(0.05+step*i));
        painter.setPen(QPen(Qt::black,1));
        if(i % textStep == 0)
            painter.drawText(pix->width()*(0.04+step*i),pix->height()/1.90, QString::number((i-range)));
        if(i!=range && i % textStep == 0)
            painter.drawText(pix->width()/2.10,pix->height()*(0.955-step*i), QString::number((i-range)));
    }
    painter.setPen(QPen(Qt::black,3));
    painter.drawLine(pix->width()*0.05,pix->height()/2,pix->width()*0.95,pix->height()/2);
    painter.drawLine(pix->width()/2,pix->height()*0.95,pix->width()/2,pix->height()*0.05);

    painter.setPen(QPen(Qt::blue,3));
    QVector<QPoint *> points;
    auto start = std::chrono::steady_clock::now();
    switch(ui->comboBox->currentIndex())
    {
    case STEP_BY_STEP:
        points = drawStepByStep(ui->x1->value(), ui->y1->value(), ui->x2->value(), ui->y2->value());
        break;
    case DDA:
        points = drawDDA(ui->x1->value(), ui->y1->value(), ui->x2->value(), ui->y2->value());
        break;
    case BRESENHAM:
        points = drawBresenham(ui->x1->value(), ui->y1->value(), ui->x2->value(), ui->y2->value());
        break;
    case BRESENHAM_CIRCLE:
        points = drawBresenhamCircle(ui->x1->value(), ui->y1->value(), ui->radius->value());
        break;
    default:
        break;
    }
    auto end = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    for(int i = 1; i < points.size(); i++)
    {
        int x1 = getXY(points[i - 1]->x(),points[i - 1]->y()).first, y1 = getXY(points[i - 1]->x(),points[i - 1]->y()).second
            , x2 = getXY(points[i]->x(),points[i]->y()).first, y2 = getXY(points[i]->x(),points[i]->y()).second;
        painter.setPen(QPen(Qt::blue,3));
        painter.drawLine(x1, y1, x2, y2);
        painter.setPen(QPen(Qt::red,3));
        painter.drawPoint(x1, y1);
        painter.drawPoint(x2, y2);
    }

    ui->label->setPixmap(*pix);
}

std::pair<int, int> MainWindow::getXY(int x, int y)
{
    std::pair<int, int> res;
    int centerX = pix->width()/2;
    int centerY = pix->height()/2;
    res.first = centerX + delta * x;
    res.second = centerY - delta * y;
    return res;
}

QVector<QPoint *> MainWindow::drawStepByStep(int x1, int y1, int x2, int y2)
{
    QVector<QPoint *> result;
    result.push_back(new QPoint(x1, y1));
    double k;
    if(x2 != x1)
        k = (double) (y2 - y1) / (double)(x2 - x1);
    else
        k = INFINITY;
    double b = y1 - k * x1;
    if (k >= -1 && k <= 1 && x2 > x1)
    {
        for (int x = x1 + 1; x < x2; x++)  //only for I, VIII octant
        {
            result.push_back(new QPoint(x, round(k*x + b)));
        }
    }
    if((k > 1 && x2 > x1) || (k < -1 && x2 < x1 && y2 > y1))
    {
        for (int y = y1 + 1; y < y2; y++)  //only for II, III octant
        {
            result.push_back(new QPoint((round((y - b) / k)), y));
        }
    }
    if((k < -1 && x2 > x1) || (k > 1 && x2 < x1 && y2 < y1))
    {
        for (int y = y1 - 1; y > y2; y--)  //only for VI, VII octant
        {
            result.push_back(new QPoint((round((y - b) / k)), y));
        }
    }
    if (k >= -1 && k <= 1 && x2 < x1)
    {
        for (int x = x1 - 1; x > x2; x--)  //only for IV, V octant
        {
            result.push_back(new QPoint(x, round(k*x + b)));
        }
    }
    result.push_back(new QPoint(x2, y2));
    return result;
}

QVector<QPoint *> MainWindow::drawDDA(int x1, int y1, int x2, int y2)
{
    QVector<QPoint *> result;
    result.push_back(new QPoint(x1, y1));
    double dx = x2 - x1;
    double dy = y2 - y1;
    double x = x1, y = y1;
    if (x2 > x1 && x2 - x1 >= abs(y2 - y1))
    {
        while (x < x2)  //only for I, VIII octant
        {
            x += 1.0;
            y += dy/dx;
            result.push_back(new QPoint(x, round(y)));
        }
    }
    else if((x2 > x1 && x2 - x1 < y2 - y1) || (x2 < x1 && abs(x2 - x1) < abs(y2 - y1) && y2 > y1))
    {
        while (y < y2)  //only for II, III octant
        {
            y += 1.0;
            x += dx/dy;
            result.push_back(new QPoint(round(x), y));
        }
    }
    else if (x2 < x1 && abs(x2 - x1) >= abs(y2 - y1))
    {
        while (x > x2)  //only for IV, V octant
        {
            x -= 1.0;
            y -= dy/dx;
            result.push_back(new QPoint(x, round(y)));
        }
    }
    else if((x2 > x1 && x2 - x1 < abs(y2 - y1) && y2 < y1) || (x2 < x1 && abs(x2 - x1) < abs(y2 - y1) && y2 < y1))
    {
        while (y > y2)  //only for VI, VII octant
        {
            y -= 1.0;
            x -= dx/dy;
            result.push_back(new QPoint(round(x), y));
        }
    }
    result.push_back(new QPoint(x2, y2));
    return result;
}

QVector<QPoint *> MainWindow::drawBresenham(int x1, int y1, int x2, int y2)
{
    QVector<QPoint *> result;
    result.push_back(new QPoint(x1, y1));
    double dx = x2 - x1;
    double dy = y2 - y1;
    double x = x1, y = y1;
    double e = abs(dy/dx) - 1.0/2.0;
    double e2 = abs(dx/dy) - 1.0/2.0;
    if (x2 > x1 && x2 - x1 >= abs(y2 - y1))
    {
        while (x < x2)  //only for I, VIII octant
        {
            if(e >= 0)
            {
                x++;
                if(y2 > y1)
                    y++;
                else
                    y--;
                e += abs(dy / dx) - 1;
            }
            else
            {
                x++;
                e += abs(dy / dx);
            }
            result.push_back(new QPoint(x, y));
        }
    }
    else if((x2 > x1 && x2 - x1 < y2 - y1) || (x2 < x1 && abs(x2 - x1) < abs(y2 - y1) && y2 > y1))
    {
        while (y < y2)  //only for II, III octant
        {
            if(e2 >= 0)
            {
                y++;
                if(x2 > x1)
                    x++;
                else
                    x--;
                e2 += abs(dx / dy) - 1;
            }
            else
            {
                y++;
                e2 += abs(dx / dy);
            }
            result.push_back(new QPoint(x, y));
        }
    }
    else if (x2 < x1 && abs(x2 - x1) >= abs(y2 - y1))
    {
        while (x > x2)  //only for IV, V octant
        {
            if(e >= 0)
            {
                x--;
                if(y2 > y1)
                    y++;
                else
                    y--;
                e += abs(dy / dx) - 1;
            }
            else
            {
                x--;
                e += abs(dy / dx);
            }
            result.push_back(new QPoint(x, y));
        }
    }
    else if((x2 > x1 && x2 - x1 < abs(y2 - y1) && y2 < y1) || (x2 < x1 && abs(x2 - x1) < abs(y2 - y1) && y2 < y1))
    {
        while (y > y2)  //only for VI, VII octant
        {
            if(e2 >= 0)
            {
                y--;
                if(x2 > x1)
                    x++;
                else
                    x--;
                e2 += abs(dx / dy) - 1;
            }
            else
            {
                y--;
                e2 += abs(dx / dy);
            }
            result.push_back(new QPoint(x, y));
        }
    }
    result.push_back(new QPoint(x2, y2));
    return result;
}

QVector<QPoint *> MainWindow::drawBresenhamCircle(int x0, int y0, int r)
{
    QVector<QPoint *> result;
    int x = 0, y = r, e = 3 - 2*r;
    result.push_back(new QPoint(x + x0, y + y0));
    while(x < y)
    {
        if(e >= 0)
        {
            e = e + 4*(x - y) + 10;
            x++;
            y--;
        }
        else
        {
            e = e + 4*x + 6;
            x++;
        }
        result.push_back(new QPoint(x + x0, y + y0));    // II octant
    }
    QVector<QPoint *> temp = result;
    for(int i = temp.size() - 1; i >= 0; i--)
    {
        result.push_back(new QPoint(temp[i]->y() - y0 + x0, temp[i]->x() - x0 + y0));  // I octant
    }
    for(int i = 0; i < temp.size(); i++)
    {
        result.push_back(new QPoint(temp[i]->y()- y0 + x0, -temp[i]->x() + x0 + y0));  // VIII octant
    }
    for(int i = temp.size() - 1; i >= 0; i--)
    {
        result.push_back(new QPoint(temp[i]->x(), -temp[i]->y() + y0 + y0));  // VII octant
    }
    for(int i = 0; i < temp.size(); i++)
    {
        result.push_back(new QPoint(-temp[i]->x() + x0 + x0, -temp[i]->y() + y0 + y0));  // VI octant
    }
    for(int i = temp.size() - 1; i >= 0; i--)
    {
        result.push_back(new QPoint(-temp[i]->y() + y0 + x0, -temp[i]->x() + x0 + y0));  // V octant
    }
    for(int i = 0; i < temp.size(); i++)
    {
        result.push_back(new QPoint(-temp[i]->y() + y0 + x0, temp[i]->x() - x0 + y0));  // IV octant
    }
    for(int i = temp.size() - 1; i >= 0; i--)
    {
        result.push_back(new QPoint(-temp[i]->x() + x0 + x0, temp[i]->y()));  // III octant
    }
    return result;
}

void repaintOnChangedVal()
{

}
void MainWindow::on_x1_valueChanged(int arg1)
{
    ui->time_label->setText(QString::number(duration));

}

void MainWindow::on_y1_valueChanged(int arg1)
{
    ui->time_label->setText(QString::number(duration));
}


void MainWindow::on_x2_valueChanged(int arg1)
{
    ui->time_label->setText(QString::number(duration));
}


void MainWindow::on_y2_valueChanged(int arg1)
{
    ui->time_label->setText(QString::number(duration));

}

void MainWindow::on_zoomIn_clicked()
{
    if(range > 5)
    {
        range -= 5;
        step = 0.9/(2 * range);
        delta = 0.9 * pix->width() / (2 * range);
        update();
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if(range < 100)
    {
        range += 5;
        step = 0.9/(2 * range);
        delta = 0.9 * pix->width() / (2 * range);
        update();
    }
}


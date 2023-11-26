
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <chrono>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_x1_valueChanged(int arg1);

    void on_y1_valueChanged(int arg1);

    void on_x2_valueChanged(int arg1);

    void on_y2_valueChanged(int arg1);

    void on_zoomIn_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QPixmap *pix;
    void paintEvent(QPaintEvent*);
    int range = 15;
    double step = 0.9/(2 * range);
    double delta;
    double duration = 0;
    enum type {STEP_BY_STEP = 0, DDA, BRESENHAM, BRESENHAM_CIRCLE};
    std::pair<int, int> getXY(int x, int y);
    void repaintOnChangedVal();
    QVector<QPoint *> drawStepByStep(int x1, int y1, int x2, int y2);
    QVector<QPoint *> drawDDA(int x1, int y1, int x2, int y2);
    QVector<QPoint *> drawBresenham(int x1, int y1, int x2, int y2);
    QVector<QPoint *> drawBresenhamCircle(int x0, int y0, int r);
};

#endif // MAINWINDOW_H

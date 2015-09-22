#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GraphSolver.h"
#include <opencv2/opencv.hpp>

namespace Ui {
class MainWindow;
}

class GraphSolver;

enum ImageType {
    IT_PREVIEW = 0,
    IT_THRESHOLDED,
    IT_ANALYZED
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_spinBox_valueChanged(int arg1);

    void on_actionOpen_triggered();

    void on_actionSwitch_displayed_image_triggered();

    void on_prelineBetweenTwoPoints(int x1, int y1, int x2, int y2);
    void on_lineBetweenTwoPoints(int x1, int y1, int x2, int y2);

private:
    Ui::MainWindow *ui;
    double threshold;
    int polynomDegree;
    GraphSolver solver;
    cv::Mat sourceImage;
    cv::Mat maskPreview;
    ImageType displayedImage;

    void recalculate();
    void displayImage();
};

#endif // MAINWINDOW_H

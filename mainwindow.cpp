#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfiledialog.h>
#include "GraphSolver.h"
#include <string>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    polynomDegree(3),
    threshold(0.3),
    displayedImage(IT_PREVIEW)
{
    ui->setupUi(this);
    connect(ui->labelForImage, SIGNAL(lineBetweenTwoPoints(int,int,int,int)), this, SLOT(on_lineBetweenTwoPoints(int,int,int,int)));
    connect(ui->labelForImage, SIGNAL(prelineBetweenTwoPoints(int,int,int,int)), this, SLOT(on_prelineBetweenTwoPoints(int,int,int,int)));
    connect(ui->labelForImage, SIGNAL(clicked(int,int)), this, SLOT(on_imageClicked(int,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    const double maxThreshold = 255;
    threshold = (double)value / maxThreshold;
    recalculate();
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    polynomDegree = arg1;
    recalculate();
}

void MainWindow::on_lineBetweenTwoPoints(int x1, int y1, int x2, int y2)
{
    if (!maskPreview.empty())
    {
        cv::line(solver.mask, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0,0,0), 10);
        maskPreview.release();
        recalculate();
    }
}

void MainWindow::on_prelineBetweenTwoPoints(int x1, int y1, int x2, int y2)
{
    if (!solver.mask.empty())
    {
        solver.mask.copyTo(maskPreview);
        cv::line(maskPreview, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0,0,0), 10);
        displayImage();
    }
}

void MainWindow::displayImage()
{
    cv::Mat dst;
    QImage image;
    switch (displayedImage) {
    case IT_PREVIEW:
        sourceImage.copyTo(dst);
        solver.drawDebugDataOnImage(dst);
        image = QImage(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);
        break;
    case IT_THRESHOLDED:
        solver.thresholdedImage.copyTo(dst);
        solver.drawGraphicOnImage(dst, cv::Scalar(0));
        image = QImage(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_Indexed8);
        break;
    case IT_ANALYZED:
        if (!maskPreview.empty())
        {
            cv::bitwise_and(maskPreview, solver.masked, dst);
        }
        else if (!solver.masked.empty())
        {
            solver.masked.copyTo(dst);
        }
        solver.drawGraphicOnImage(dst, cv::Scalar(255));
        image = QImage(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_Indexed8);
        break;
    }
    ui->labelForImage->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::recalculate()
{
    if (!sourceImage.empty())
    {
        result = solver.calculatePolynomOnImage(sourceImage, polynomDegree + 1, threshold);

        displayImage();

        ui->listWidget->clear();
        for (int i = 0; i < result.size(); i++)
        {
            ui->listWidget->addItem(("a[" + std::to_string(i) + "] = " + std::to_string(result[i])).c_str());
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    result.clear();
    QString fileName = QFileDialog::getOpenFileName(this, "Open image for processing", "", "Images (*.png *.xpm *.jpg)");
    sourceImage = cv::imread(fileName.toStdString());
    recalculate();
}

void MainWindow::on_actionSwitch_displayed_image_triggered()
{
    displayedImage = (ImageType)((displayedImage + 1) % 3);
    switch (displayedImage) {
    case IT_PREVIEW:
        ui->groupBoxImage->setTitle("Preview");
        break;
    case IT_ANALYZED:
        ui->groupBoxImage->setTitle("Analyzed");
        break;
    case IT_THRESHOLDED:
        ui->groupBoxImage->setTitle("Thresholded");
        break;
    default:
        break;
    }
    displayImage();
}

void MainWindow::on_imageClicked(int x, int y)
{
    solver.setIdentity(cv::Point(x, y), 1, 10);
}

void MainWindow::on_actionExport_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Export results", "results.csv", "CSV files (.csv);;Zip files (.zip, *.7z)", 0, 0);
    QFile data(filename);

    if(data.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream output(&data);

        for (auto param : result)
        {
            output << param << ',';
        }
    }

    data.close();
}

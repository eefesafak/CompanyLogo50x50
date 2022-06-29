// headers
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

//namespace
using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->imageLabel->setScaledContents(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::process()
{
    foreach(const QString& path, m_fileList)
    {
        QImage   image(path);
        QString  logoPath = "/home/efe/Downloads/logo.png";
        QImage   logo(logoPath);
        QImage   logo2 = logo.scaled(50, 50, Qt::KeepAspectRatio);
        QPainter painter(&image); painter.setOpacity(0.5);

        painter.drawImage(contourDetection(path), logo2);

        image.save(path, "PNG");
    }
}

QPoint MainWindow::contourDetection(const QString path)
{
    Mat image = imread(path.toStdString());

    Mat image_gray;
    cvtColor(image, image_gray, COLOR_BGR2GRAY);

    Mat thresh;
    Canny(image_gray, thresh, 50, 150);

    int width = thresh.size().width;
    int height = thresh.size().height;

    QPoint point;
    Mat square50x50;
    int min = 10000;
    for(int i=0; i<width; i+=50)
    {
        for(int j=0; j<height; j+=50)
        {
            square50x50 = thresh(Range(i, i+50), Range(j, j+50));
            if(contoursArea(square50x50) < min)
            {
                min = contoursArea(square50x50);
                point = QPoint(j, i);
            }
        }
    }
    return point;
}

double MainWindow::contoursArea(cv::Mat corner)
{
    Mat cornerCopy;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(corner, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    cornerCopy = corner.clone();
    drawContours(cornerCopy, contours, -1, Scalar(255, 0, 0), 1);

    double area = 0;
    for (unsigned int i = 0;  i < contours.size();  i++)
    {
         area += contourArea(contours[i]);
    }
    return area;
}

void MainWindow::on_process_clicked()
{
    QtConcurrent::run(this, &MainWindow::process);
}

void MainWindow::on_browse_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
                                                      this,
                                                      "Select one or more files to open",
                                                      "/home/efe/Desktop/resim",
                                                      "Images (*.png *.xpm *.jpg *.jpeg)");
    if(files.isEmpty())
        return;

    ui->label->setText("");
    ui->label->setText(ui->imageLabel->text() + "Added successfully");
    m_fileList = files;
}

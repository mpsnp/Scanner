#include "imagelabel.h"
#include <QMouseEvent>

ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent), dragging(false)
{
    setMouseTracking(true);
}

void ImageLabel::mousePressEvent(QMouseEvent *ev)
{
    startPoint = ev->pos();
    endPoint = ev->pos();
    dragging = true;
    emit prelineBetweenTwoPoints(startPoint.x(), startPoint.y(), endPoint.x(), endPoint.y());
}

void ImageLabel::mouseMoveEvent(QMouseEvent *ev)
{
    if (dragging)
    {
        endPoint = ev->pos();
        emit prelineBetweenTwoPoints(startPoint.x(), startPoint.y(), endPoint.x(), endPoint.y());
    }
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    dragging = false;
    endPoint = ev->pos();
    emit lineBetweenTwoPoints(startPoint.x(), startPoint.y(), endPoint.x(), endPoint.y());
}

ImageLabel::~ImageLabel()
{

}

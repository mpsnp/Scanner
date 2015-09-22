#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>
#include <QEvent>

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabel(QWidget *parent = 0);
    virtual ~ImageLabel();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

private:
    QPoint startPoint;
    QPoint endPoint;
    bool dragging;

signals:
   void clicked(int x, int y);
   void lineBetweenTwoPoints(int x1, int y1, int x2, int y2);
   void prelineBetweenTwoPoints(int x1, int y1, int x2, int y2);

public slots:

};

#endif // IMAGELABEL_H

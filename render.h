#ifndef RENDER_H
#define RENDER_H

#include <QObject>
#include <QThread>
#include <QImage>
#include <QMap>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGraphicsScene>


class Render : public QObject
{
    Q_OBJECT
public:
    bool isFull;
    size_t mainWidth,mainHeight;
    explicit Render(QObject *parent = nullptr);
    void grow(QImage &image1);
    QColor findMaxValue(QMap<QColor,int> &map);
    void add_grains(QImage &image, int numOfGrains);
    int random(int min, int max);
public slots:
    void render();
signals:
    void resultReady(const QImage &result);


private:

    QImage image1;
};

#endif // RENDER_H

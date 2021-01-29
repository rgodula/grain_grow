#ifndef RENDER_H
#define RENDER_H

#include <QDebug>
#include <QObject>
#include <QThread>
#include <QImage>
#include <QMap>
#include <QList>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QRandomGenerator>

#define DEFAULT_WIDTH 900
#define DEFAULT_HEIGHT 900
#define DEFAULT_GRAINS 1000
#define DEFAULT_INC_NUM 20
#define DEFAULT_INC_SIZE 50
#define DEFAULT_INC_SHAPE "Square"

class Render : public QObject
{
    Q_OBJECT
public:
    QImage tempImage;
    QImage referenceImage;
    bool isPeriodic;
    int probability, grainsFactor,grainAmount,grainXposition,grainYposition,thickness;
    QMap<int,QString> *intToColorR;
    QMap<QString,int> *colorToIntR;
    QList<QColor> colorlist,choosenGrains;
    void generateColorMaps();
    bool isFull;
    size_t width, height, grains,numberOfInclusions, sizeOfInclusions;
    QString inclusionType,algorithm;
    explicit Render(QObject *parent = nullptr);
    QColor findMaxValue(QMap<QColor,int> &map);
    void add_grains(QImage &image, int numOfGrains);
    void addGrainsSubstructure(QImage &image, int numOfGrains, int factor);
    void addGrainsDualphase(QImage &image, int numOfGrains);
    void add_inclusions_before(QImage &image,int numberOfInclusions, int sizeOfInclusions, QString inclusionType);
    void add_inclusions_after(QImage &image,int numberOfInclusions, int sizeOfInclusions, QString inclusionType);
    int random(int min, int max);
    void setParam(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT, int grains = DEFAULT_GRAINS );
    void setInclusionsParam(int numberOfInclusions, int sizeOfInclusions, QString inclusionType);
    void mooreGrow(QImage &image);
    void periodicMooreGrow(QImage &image);
    void boundaryShapeControl(QImage &image);
    void periodicBoundaryShapeControl(QImage &image);
    void mooreGrowSubstructure(QImage &image);
    void periodicMooreGrowSubstructure(QImage &image);
    void mooreGrowDualphase(QImage &image);
    void periodicMooreGrowDualphase(QImage &image);
    void boundaryShapeControlSubstructure(QImage &image);
    void boundaryShapeControlDualphse(QImage &image);
    void periodicBoundaryShapeControlSubstructure(QImage &image);
    void periodicBoundaryShapeControlDualphse(QImage &image);
    void chooseGrains(QImage &image);

    QColor checkRule2(QImage image);
    QColor checkRule22(QImage image);
    QColor checkRule3(QImage image);
    QColor checkRule4(QImage image);
    QList<QPoint> findBoundaries(QImage image);
    bool checkBackgroundBefore(QImage image,int x,int y,int size);
    bool checkBackgroundAfter(QImage image,int x,int y,int size);



public slots:
    void render();
    void createSubstructure(QImage image);
    void createDualphase(QImage image);
    void inclusions(QImage image, bool imageFlag);
    void renderBoundaries(QImage image);
signals:
    void resultReady(const QImage &result);
    void imageIsDone(QImage image);
    void inclusionsAreDone(QImage image);
    void inclusionWasAdded(QImage image);



private:

    QImage image_1;
};

#endif // RENDER_H

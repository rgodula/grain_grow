#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QMap>
#include <QVector>
#include <QString>
#include <QMenuBar>
#include <QMenu>
#include <QSlider>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QFileDialog>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QImage>
#include <QRadioButton>
#include <QPixmap>
#include <QMenu>
#include <QSize>
#include <QComboBox>
#include <QVector>
#include "render.h"
#include <QtXml>
#include <QMouseEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread renderThread;
public:
    MainWindow(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event);
    ~MainWindow();
public slots:
    void setInclusionFlag(QImage image);
    void saveBmp();
    void saveTxt();
    void loadBmp();
    void loadTxt();
    void generate();
    void generateSubstructure();
    void generateDualphase();
    void chooseGrains();
    void generateBoundaries();
    void inclusions();
    void turnOnButton(QImage image);
    void clearSpace();
    void reset();

signals:
    void grow();
    void substructure(QImage image);
    void dualphase(QImage image);
    void insertInclusions(QImage image, bool imageFlag);
    void boundaries(QImage image);


private:
    bool inclusionFlag, imageFlag, chooseGrainsFlag;
    size_t mainWidth,mainHeight;
    QImage image1,finalImage,inclusionImage,scaledImage;
    QColor _color;
    void setRenderObject();
    void setLayout();
    void setView();
    void setMenu();
    void setButtons();
    int random(int min, int max);
    void add_grains(QImage &image, int numOfGrains);
    void showImage(QImage image);
    void createColorToIntXml();
    void createIntToColorXml();
    int randomColor(int min, int max);
    void imageToTxt(QImage image, QFile &file);
    QColor findMaxValue(QMap<QColor,int> &map);
    void generateColorMaps();

    QMap<int,QString> *intToColor;
    QMap<QString,int> *colorToInt;
    Render *render;
    QSize *buttonSize;
    QAction *quit,*savetxt,*savebmp,*loadtxt,*loadbmp;
    QPushButton *generateButton, *inclusionsButton, *boundariesButton, *substructureButton, *dualPhaseButton, *resetButton, *chooseGrainsButton, *clearSpaceButton;
    QWidget *centralwidget;
    QLineEdit *grainY,*grainX,*grains,*probability,*inclusionNum,*inclusionSize, *grainsFactor,*grainAmount;
    QComboBox *inclusionsShape,*algorithm, *algorithmSub, *algorithmDual, *boundaryThickness;
    QMenu *fileMenu;
    QHBoxLayout * hlayout;
    QGraphicsPixmapItem *m_item;
    QGraphicsScene *scene;
    QGraphicsView *view;
    QVBoxLayout *mainLayout;
    QGroupBox *grainBox,*inclusionBox, *boundariesBox, *substructureBox, *dualPhaseBox;
    QRadioButton *radioButton, *radioButtonSub, *radioButtonDual;


};
#endif // MAINWINDOW_H

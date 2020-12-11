#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
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
#include <QPixmap>
#include <QMenu>
#include <QSize>
#include <QComboBox>
#include <QVector>
#include <QtConcurrent/QtConcurrent>
#include "render.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread renderThread;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void saveBmp();
    void saveTxt();
    void loadBmp();
    void loadTxt();
    void generate();

signals:
    void operate();

private:

    size_t mainWidth,mainHeight;
    QImage image1,image2;
    QColor _color;
    void startWorkInAThread();
    void setLayout();
    void setView();
    void setMenu();
    void setButtons();
    int random(int min, int max);
    void add_grains(QImage &image, int numOfGrains);
    void grow(QImage &image1,QImage &image2);
    void showS(QImage image);
    QColor findMaxValue(QMap<QColor,int> &map);
    QSize *buttonSize;
    QAction *quit,*savetxt,*savebmp,*loadtxt,*loadbmp;
    QPushButton *generateButton, *addInclusionsButton;
    QWidget *centralwidget;
    QLineEdit *grainY,*grainX,*grains,*inclusionNum,*inclusionSize;
    QComboBox *inclusionsShape;
    QMenu *fileMenu;
    QHBoxLayout * hlayout;
    QGraphicsPixmapItem *m_item;
    QGraphicsScene *scene;
    QGraphicsView *view;
    QVBoxLayout *mainLayout;
    QGroupBox *grainBox,*inclusionBox;


};
#endif // MAINWINDOW_H

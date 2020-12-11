#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mainWidth(1400)
    , mainHeight(920)
{

    Render *render = new Render;
    render->moveToThread(&renderThread);
    connect(&renderThread, &QThread::finished, render, &QObject::deleteLater);
    connect(this, &MainWindow::operate, render, &Render::render);
    connect(render, &Render::resultReady, this, &MainWindow::showS);
    renderThread.start();


    setFixedSize(QSize(mainWidth,mainHeight));
    centralwidget = new QWidget(this);
    centralwidget->setGeometry(menuBar()->width(),0,mainWidth - menuBar()->width(),mainHeight);
    this->setCentralWidget(centralwidget);
    setLayout();
    setMenu();
    setView();
    setButtons();
}

MainWindow::~MainWindow()
{
    renderThread.quit();
    renderThread.wait();
}

void MainWindow::setLayout()
{
    mainLayout = new QVBoxLayout(this);
    fileMenu = new QMenu();
    grainBox = new QGroupBox(this);
    inclusionBox = new QGroupBox(this);

    mainLayout->addWidget(fileMenu);
    mainLayout->addWidget(grainBox);
    mainLayout->addWidget(inclusionBox);

}

void MainWindow::setMenu(){
    fileMenu = menuBar()->addMenu(tr("&File"));
    auto save = fileMenu->addMenu("Save");
    auto load = fileMenu->addMenu("Load");
    savebmp = save->addAction("bmp");
    savetxt = save->addAction("txt");
    loadbmp = load->addAction("bmp");
    loadtxt = load->addAction("txt");
    quit = menuBar()->addAction("&Quit");
    connect(savebmp, SIGNAL(triggered()), this, SLOT(saveBmp()));
    connect(savetxt, SIGNAL(triggered()), this, SLOT(saveTxt()));
    connect(loadbmp, SIGNAL(triggered()), this, SLOT(loadBmp()));
    connect(loadtxt, SIGNAL(triggered()), this, SLOT(loadTxt()));
    connect(quit, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::setButtons()
{

    generateButton = new QPushButton(this);
    generateButton->setText(tr("Generate"));


    grainX = new QLineEdit();
    grainY = new QLineEdit();
    grains = new QLineEdit();

    grainBox->setTitle(tr("Grains"));
    grainBox->setGeometry(25,35,450,150);
    QFormLayout *layoutGr = new QFormLayout;
    layoutGr->addRow(new QLabel(tr("Width :")), grainX);
    layoutGr->addRow(new QLabel(tr("Height:")), grainY);
    layoutGr->addRow(new QLabel(tr("Grains:")), grains);
    layoutGr->addRow(generateButton);
    grainBox->setLayout(layoutGr);

    addInclusionsButton = new QPushButton(this);
    addInclusionsButton->setText(tr("Add inclusions"));

    inclusionNum = new QLineEdit();
    inclusionSize = new QLineEdit();
    inclusionsShape = new QComboBox();
    inclusionsShape->addItem("");
    inclusionsShape->addItem("Circle");
    inclusionsShape->addItem("Square");

    inclusionBox->setTitle(tr("Inclusions"));
    inclusionBox->setGeometry(25,200,450,150);
    QFormLayout *layoutIn = new QFormLayout;
    layoutIn->addRow(new QLabel(tr("Amount :")), inclusionNum);
    layoutIn->addRow(new QLabel(tr("Size :  ")), inclusionSize);
    layoutIn->addRow(new QLabel(tr("Type :  ")), inclusionsShape);
    layoutIn->addRow(addInclusionsButton);
    inclusionBox->setLayout(layoutIn);

    connect(generateButton, SIGNAL(clicked()) , SLOT(generate()));


}

void MainWindow::saveBmp()
{

}

void MainWindow::saveTxt()
{

}

void MainWindow::loadBmp()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open bmp File"), "",
            tr("Address Book (*.bmp);;All Files (*)"));
}
void MainWindow::loadTxt()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open txt File"), "",
            tr("Address Book (*.txt);;All Files (*)"));

}

void MainWindow::generate()
{
    Render render;
    emit operate();
}

void MainWindow::showS(QImage image)
{
   m_item->setPixmap(QPixmap::fromImage(image));
   view->setScene(new QGraphicsScene(this));
   view->scene()->addItem(m_item);
}

void MainWindow::setView()
{
        QImage image(900,900, QImage::Format_ARGB32);
        image.fill(Qt::white);
        image2 = image;
        view = new QGraphicsView(this);
        view->setGeometry(500,20,mainWidth-500,mainHeight);
        m_item = new QGraphicsPixmapItem();
        m_item->setPixmap(QPixmap::fromImage(image));
        view->setScene(new QGraphicsScene(this));
        view->scene()->addItem(m_item);
}


int MainWindow::random(int min, int max) //range : [min, max]
{
   static bool first = true;
   if (first)
   {
      srand( time(NULL) ); //seeding for the first time only!
      first = false;
   }
   return min + rand() % (( max + 1 ) - min);
}

void MainWindow::add_grains(QImage &image, int numOfGrains)
{
    for(int i=0; i<numOfGrains;i++)
    {
        QColor color(random(0,255), random(0,255), random(0,255), 255);
        while(color == Qt::white) color = QColor(random(0,255), random(0,255), random(0,255), 255);
        image.setPixel(random(0,image.width()-1), random(0,image.height()-1), color.rgb()); // TO DO avoid replace grains
    }

}

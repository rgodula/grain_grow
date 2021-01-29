#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mainWidth(1400)
    , mainHeight(920)
{

    setWindowTitle("Grain Growth");
    generateColorMaps();
    setRenderObject();
    finalImage = QImage(500,500, QImage::Format_ARGB32);
    finalImage.fill(Qt::white);
    inclusionFlag = false;
    imageFlag = false;
    chooseGrainsFlag = false;
    setFixedSize(QSize(mainWidth,mainHeight));
    centralwidget = new QWidget(this);
    centralwidget->setGeometry(menuBar()->width(),0,mainWidth - menuBar()->width(),mainHeight);
    this->setCentralWidget(centralwidget);
    setLayout();
    setMenu();
    setView();
    setButtons();
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    if(chooseGrainsFlag)
    {
        int posx = event->pos().x();
        int posy = event->pos().y();
        render->grainXposition = event->pos().x();
        render->grainYposition = event->pos().y();
        QColor color = scaledImage.pixelColor(posx-500,posy-20);
        if (!(render->choosenGrains.contains(color))) render->choosenGrains.append(color);
        //if ( posx > 500 && posy > 20 ) {qDebug() << event->pos() << posx << posy << color.name() ;}

        for(int i =0; i<finalImage.width(); i++) {
          for(int j =0; j<finalImage.height(); j++) {
              if (finalImage.pixelColor(i,j) == color) finalImage.setPixelColor(i,j,Qt::red);
          }
        }
        showImage(finalImage);
    }
}

MainWindow::~MainWindow()
{
    renderThread.quit();
    renderThread.wait();
    delete render;
}

void MainWindow::generateColorMaps()
{
    intToColor = new QMap<int,QString>();
    colorToInt = new QMap<QString,int>();
    QColor color;
    QString colorHex;
    for (int i = 0; i<10000 ;) {
        color.setRgb(random(1,255), random(1,255), random(1,255), 255);
        colorHex = color.name(QColor::HexRgb);
        if (!colorToInt->contains(colorHex))
        {
            intToColor->insert( i,colorHex);
            colorToInt->insert(colorHex,i);
            i++;
        }
    }
}



void MainWindow::setRenderObject()
{
    render = new Render;
    render->intToColorR = intToColor;
    render->colorToIntR = colorToInt;
    render->moveToThread(&renderThread);
    connect(&renderThread, &QThread::finished, render, &QObject::deleteLater);
    connect(this, &MainWindow::grow, render, &Render::render);
    connect(this, &MainWindow::substructure, render, &Render::createSubstructure);
    connect(this, &MainWindow::dualphase, render, &Render::createDualphase);
    connect(this, &MainWindow::insertInclusions, render, &Render::inclusions);
    connect(this, &MainWindow::boundaries, render, &Render::renderBoundaries);
    connect(render, &Render::inclusionWasAdded, this, &MainWindow::setInclusionFlag );
    connect(render, &Render::resultReady, this, &MainWindow::showImage);
    connect(render, &Render::imageIsDone, this, &MainWindow::turnOnButton);
    renderThread.start();
}

void MainWindow::setInclusionFlag(QImage image)
{
    inclusionImage = image;
    inclusionFlag = true;
}

void MainWindow::setLayout()
{
    mainLayout = new QVBoxLayout(this);
    fileMenu = new QMenu();
    grainBox = new QGroupBox(this);
    inclusionBox = new QGroupBox(this);
    boundariesBox = new QGroupBox(this);
    substructureBox = new QGroupBox(this);
    dualPhaseBox = new QGroupBox(this);

    mainLayout->addWidget(fileMenu);
    mainLayout->addWidget(grainBox);
    mainLayout->addWidget(inclusionBox);
    mainLayout->addWidget(boundariesBox);
    mainLayout->addWidget(substructureBox);
    mainLayout->addWidget(dualPhaseBox);
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
    //grain box
    generateButton = new QPushButton(this);
    generateButton->setText(tr("Generate"));

    radioButton = new QRadioButton("Periodic", this);

    grainX = new QLineEdit("300");
    grainY = new QLineEdit("300");
    grains = new QLineEdit("100");
    probability = new QLineEdit("100");

    grainX->setValidator( new QIntValidator(0, 1000, this) );
    grainY->setValidator( new QIntValidator(0, 1000, this) );
    grains->setValidator( new QIntValidator(0, 1000, this) );
    probability->setValidator( new QIntValidator(0, 1000, this) );

    algorithm = new QComboBox();
    algorithm->addItem("Moore");
    algorithm->addItem("Grain boundary shape control");



    grainBox->setTitle(tr("Growth"));
    grainBox->setGeometry(25,35,450,200);
    QFormLayout *layoutGr = new QFormLayout;
    layoutGr->addRow(new QLabel(tr("Width :")), grainX);
    layoutGr->addRow(new QLabel(tr("Height:")), grainY);
    layoutGr->addRow(new QLabel(tr("Grains:")), grains);
    layoutGr->addRow(new QLabel(tr("Probability:")), probability);
    layoutGr->addRow(new QLabel(tr("Algorithm :  ")), algorithm);
    layoutGr->addRow(radioButton, generateButton);
    grainBox->setLayout(layoutGr);

    // inclusion box

    inclusionsButton = new QPushButton(this);
    inclusionsButton->setText(tr("Add inclusions"));

    inclusionNum = new QLineEdit("50");
    inclusionSize = new QLineEdit("5");
    inclusionsShape = new QComboBox();
    inclusionsShape->addItem("Square");
    inclusionsShape->addItem("Circle");


    inclusionBox->setTitle(tr("Inclusions"));
    inclusionBox->setGeometry(25,250,450,150);
    QFormLayout *layoutIn = new QFormLayout;
    layoutIn->addRow(new QLabel(tr("Amount :")), inclusionNum);
    layoutIn->addRow(new QLabel(tr("Size :  ")), inclusionSize);
    layoutIn->addRow(new QLabel(tr("Type :  ")), inclusionsShape);
    layoutIn->addRow(inclusionsButton);
    inclusionBox->setLayout(layoutIn);

    // boundaries box

    boundariesButton = new QPushButton(this);
    boundariesButton->setText(tr("Generate boundaries"));

    boundaryThickness = new QComboBox();
    boundaryThickness->addItem("1");
    boundaryThickness->addItem("2");
    boundaryThickness->addItem("3");
    boundaryThickness->addItem("4");
    boundaryThickness->addItem("5");
    boundaryThickness->addItem("6");

    boundariesBox->setTitle(tr("Boundaries"));
    boundariesBox->setGeometry(25,420,450,100);
    QFormLayout *layoutBn = new QFormLayout;
    layoutBn->addRow(new QLabel(tr("Thickness")), boundaryThickness);
    layoutBn->addRow(boundariesButton);
    boundariesBox->setLayout(layoutBn);

    //substructure box
    substructureButton = new QPushButton(this);
    substructureButton->setText(tr("Generate substructure"));

    grainsFactor = new QLineEdit("4");

    substructureBox->setTitle(tr("Substructure"));
    substructureBox->setGeometry(25,530,450,110);
    QFormLayout *layoutSub = new QFormLayout;

    //layoutSub->addRow(new QLabel(tr("Algorithm :  ")), algorithmSub);
    layoutSub->addRow(new QLabel(tr("Grains factor")), grainsFactor);
    layoutSub->addRow( substructureButton);
    substructureBox->setLayout(layoutSub);




    //dual phase box

    dualPhaseButton = new QPushButton(this);
    dualPhaseButton->setText(tr("Generate dual phase"));

    chooseGrainsButton = new QPushButton(this);
    chooseGrainsButton->setText(tr("Choose grains"));

    clearSpaceButton = new QPushButton(this);
    clearSpaceButton->setText(tr("Clear space"));



    dualPhaseBox->setTitle(tr("Dual phase"));
    dualPhaseBox->setGeometry(25,660,450,140);
    QFormLayout *layoutDual = new QFormLayout;
    grainAmount = new QLineEdit("4");

    layoutDual->addRow(chooseGrainsButton);
    layoutDual->addRow(clearSpaceButton);
    layoutDual->addRow( dualPhaseButton);
    dualPhaseBox->setLayout(layoutDual);

    // reset button
    resetButton = new QPushButton(this);
    resetButton->setText(tr("Reset"));
    resetButton->setGeometry(25,860,450,25);


    //connect buttons with slots

    connect(generateButton, SIGNAL(clicked()) , SLOT(generate()));
    connect(inclusionsButton, SIGNAL(clicked()) , SLOT(inclusions()));
    connect(resetButton, SIGNAL(clicked()) , SLOT(reset()));
    connect(substructureButton, SIGNAL(clicked()) , SLOT(generateSubstructure()));
    connect(dualPhaseButton, SIGNAL(clicked()) , SLOT(generateDualphase()));
    connect(chooseGrainsButton, SIGNAL(clicked()) , SLOT(chooseGrains()));
    connect(boundariesButton, SIGNAL(clicked()) , SLOT(generateBoundaries()));
    connect(clearSpaceButton, SIGNAL(clicked()) , SLOT(clearSpace()));


}
void MainWindow::clearSpace()
{
    for(int i =0; i<finalImage.width(); i++) {
      for(int j =0; j<finalImage.height(); j++) {
        if (finalImage.pixelColor(i,j) != Qt::red) finalImage.setPixelColor(i,j,Qt::white);
      }
    }
    showImage(finalImage);
}
void MainWindow::reset()
{
    finalImage = QImage(grainX->text().toInt(),grainY->text().toInt(),QImage::Format_ARGB32);
    finalImage.fill(Qt::white);
    inclusionImage.fill(Qt::white);
    inclusionFlag = false;
    imageFlag = false;
    showImage(finalImage);
}

void MainWindow::saveBmp()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save image to bmp File"), "",
            tr("Windows Bitmap (*.bmp);;All Files (*)"));
    finalImage.save(fileName);
}

void MainWindow::saveTxt()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save image to txt File"), "",
            tr("Text file (*.txt);;All Files (*)"));
    QFile file(fileName);
    imageToTxt(finalImage, file);


}

void MainWindow::loadBmp()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open bmp File"), "",
            tr("Address Book (*.bmp);;All Files (*)"));
    QImage img = QImage(fileName);

    showImage(img);
}

void MainWindow::loadTxt()
{
    QString txtFile = QFileDialog::getOpenFileName(this,
        tr("Open txt File"), "",
        tr("Address Book (*.txt);;All Files (*)"));

    if (txtFile.isEmpty())
        return;
    else {
        QFile file(txtFile);

        if (!file.open(QIODevice::ReadOnly)) {
            //QMessageBox::information(this, tr("Unable to open file"),
            file.errorString();
            return;
        }
        QTextStream in(&file);
        QString line = in.readLine();
        QStringList list = line.split(" ");
        int width = list[0].toInt();
        int height = list[1].toInt();
        QImage image(width,height, QImage::Format_ARGB32);

        while (!file.atEnd()) {
            line = in.readLine();
            list = line.split(" ");

            image.setPixelColor(list[0].toInt(),list[1].toInt(),QColor(intToColor->find(list[3].toInt()).value()));
        }
        showImage(image);
        file.close();
    }
}

void MainWindow::generate()
{
    if(imageFlag) reset();
    render->isPeriodic = radioButton->isChecked();
    render->algorithm  = algorithm->currentText() ;
    if (algorithm->currentText() == "Grain boundary shape control" )
    {
        render->probability = probability->text().toInt();
        qDebug() << probability->text().toInt();
    }

    if ((grainX->text() == nullptr) || (grainY->text() == nullptr) || (grains->text() == nullptr)) render->setParam();
    else
    {
        render->setParam(grainX->text().toInt(),grainY->text().toInt(),grains->text().toInt());
        finalImage = QImage(grainX->text().toInt(),grainY->text().toInt(), QImage::Format_ARGB32);
        finalImage.fill(Qt::white);
    }

    if(inclusionFlag && !imageFlag) render->tempImage=inclusionImage;
    else render->tempImage=finalImage;
    generateButton->setDisabled(true);
    imageFlag = true;
    emit grow();
}
void MainWindow::generateSubstructure()
{
    qDebug() << "here";
    render->isPeriodic = radioButton->isChecked();
    render->algorithm  = algorithm->currentText() ;
    render->probability   = probability->text().toInt();
    render->grainsFactor  = grainsFactor->text().toInt() ;
    emit substructure(finalImage);
}
void MainWindow::generateDualphase()
{
    qDebug() << "here2";
    chooseGrainsFlag = false;
    render->isPeriodic  = radioButton->isChecked();
    render->algorithm   = algorithm->currentText();
    render->probability = probability->text().toInt();
    render->grainAmount = grains->text().toInt();
    emit dualphase(finalImage);
}

void MainWindow::chooseGrains()
{
    render->choosenGrains.clear();
    chooseGrainsFlag = true;
}

void MainWindow::inclusions()
{
    int number = inclusionNum->text().toInt();
    int size   = inclusionSize->text().toInt();
    QString shape = inclusionsShape->currentText();

    if ((grainX->text() == nullptr) || (grainY->text() == nullptr) || (grains->text() == nullptr)) render->setParam();
    else render->setParam(grainX->text().toInt(),grainY->text().toInt(),grains->text().toInt());

    finalImage = finalImage.scaled(grainX->text().toInt(), grainY->text().toInt(), Qt::KeepAspectRatio);
    render->setInclusionsParam(number,size,shape);
    if(inclusionFlag) reset();
    emit insertInclusions(finalImage,imageFlag);

}

void MainWindow::generateBoundaries()
{
    render->thickness = boundaryThickness->currentText().toInt();
    emit boundaries(finalImage);
}

void MainWindow::showImage(QImage image)
{
   scaledImage = image;
   finalImage = scaledImage;
   scaledImage = scaledImage.scaled(900, 900, Qt::KeepAspectRatio);
   m_item->setPixmap(QPixmap::fromImage(scaledImage));
   view->setScene(new QGraphicsScene(this));
   view->scene()->addItem(m_item);
}

void MainWindow::turnOnButton(QImage image )
{
    finalImage = image;
    generateButton->setEnabled(true);
}

void MainWindow::setView()
{
        QImage image(500,500, QImage::Format_ARGB32);
        image.fill(Qt::white);
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

int MainWindow::randomColor(int min, int max) //range : [min, max]
{
   static bool first = true;
   if (first)
   {
      srand( time(NULL) ); //seeding for the first time only!
      first = false;
   }
   return min + rand() % (( max + 1 ) - min);
}

void MainWindow::imageToTxt(QImage image, QFile &file)
{
    QTextStream stream(&file);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            stream << image.width() << " " << image.height() << '\n';

            for (int i=0; i<image.width() ; i++ ) {
                for (int j=0; j<image.height() ; j++ ) {
                    stream << i << " " << j << " " << 1 << " " << colorToInt->find((image.pixelColor(i,j)).name(QColor::HexRgb)).value() << '\n';
                    }
                }
            file.close();
            qDebug() << "Writing finished";
        }
    else
        {
            qDebug() << "Could not open file";
        }
}





#include "render.h"

bool operator<(const QColor & a, const QColor & b) {
   return a.redF() < b.redF()
       || a.greenF() < b.greenF()
       || a.blueF() < b.blueF()
       || a.alphaF() < b.alphaF();
}

Render::Render(QObject *parent) : QObject(parent)
  , isFull(false)
{

}


void Render::setParam(int width, int height, int grains)
{
    this->width  = width;
    this->height = height;
    this->grains = grains;
}

void Render::setInclusionsParam(int numberOfInclusions, int sizeOfInclusions, QString inclusionType)
{
    this->numberOfInclusions  = numberOfInclusions;
    this->sizeOfInclusions = sizeOfInclusions;
    this->inclusionType = inclusionType;
}

void Render::render()
{
    add_grains(tempImage,grains);

    if (this->algorithm == "Moore")
    {
        isFull = false;
        if (isPeriodic) while(!isFull) periodicMooreGrow(tempImage);
        else while(!isFull) mooreGrow(tempImage);
        emit imageIsDone(tempImage);
    }
    else
    {
        isFull = false;
        if (isPeriodic) while(!isFull) periodicBoundaryShapeControl(tempImage);
        else while(!isFull) boundaryShapeControl(tempImage);
        emit imageIsDone(tempImage);
    }

}

void Render::createSubstructure(QImage image)
{
    colorlist.clear();
    referenceImage = image;
    addGrainsSubstructure(image,grains,grainsFactor);
    if (this->algorithm == "Moore")
    {
        isFull = false;
        if (isPeriodic) while(!isFull) periodicMooreGrowSubstructure(image);
        else while(!isFull) mooreGrowSubstructure(image);
    }
    else
    {
        isFull = false;
        if (isPeriodic) while(!isFull) periodicBoundaryShapeControlSubstructure(image);
        else while(!isFull) boundaryShapeControlSubstructure(image);

    }
}

void Render::createDualphase(QImage image)
{
    chooseGrains(image);
    addGrainsDualphase(image,grainAmount);
    if (this->algorithm == "Moore")
    {
        isFull = false;
        if (isPeriodic) while(!isFull) periodicMooreGrowDualphase(image);
        else while(!isFull) mooreGrowDualphase(image);
    }
    else
    {
        isFull = false;
        if (isPeriodic) while(!isFull) periodicBoundaryShapeControlDualphse(image);
        else while(!isFull) boundaryShapeControlDualphse(image);
    }
}

void Render::inclusions(QImage image, bool imageflag)
{
    if(imageflag) add_inclusions_after(image,numberOfInclusions,sizeOfInclusions,inclusionType);
    else {
        add_inclusions_before(image,numberOfInclusions,sizeOfInclusions,inclusionType);
    }
    emit resultReady(image);

}

void Render::mooreGrow(QImage &image)
{
    QImage image2(image);
    int width = image.width();
    int height = image.height();
    isFull=true;
    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
        if (image.pixelColor(i,j) != Qt::white || image.pixelColor(i,j) == Qt::black)
        {
            image2.setPixelColor(i,j,image.pixelColor(i,j));
        }
        else{
          isFull=false;
          QMap<QColor,int> map1;
          for(int c = -1; c < 2; c++)
          {
            for(int d = -1; d < 2; d++)
            {
              if (d==0 && c==0) continue;
              int l=i+c;
              int k=j+d;
              if ((l < 0 || l > (width-1)) || (k < 0 || k > (height-1)))  continue; // check if we are in image
              QColor key = image.pixelColor(l,k);
              if (key == Qt::white) continue;
              if (key == Qt::black) continue;

              if(map1.contains(key)) map1[key]++;
              else  map1.insert(key, 1);
            }
          }
          image2.setPixelColor(i,j,findMaxValue(map1));
        }
      }
    }
    image=image2;
    emit resultReady(image);
}

void Render::periodicMooreGrow(QImage &image)
{
    QImage image2(image);
    int width = image.width();
    int height = image.height();
    isFull=true;
    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
        if (image.pixelColor(i,j) != Qt::white || image.pixelColor(i,j) == Qt::black)
        {
            image2.setPixelColor(i,j,image.pixelColor(i,j));
        }
        else{
          isFull=false;
          QMap<QColor,int> map1;
          for(int c = -1; c < 2; c++)
          {
            for(int d = -1; d < 2; d++)
            {
              if (d==0 && c==0) continue;
              int l=i+c;
              int k=j+d;

              if (l < 0) {
                  l = width - 1;
              } else if (l > width - 1) l = 0;
              if (k < 0) {
                  k = height - 1;
              } else if (k > height - 1) k = 0;
              QColor key = image.pixelColor(l,k);
              if (key == Qt::white) continue;
              if (key == Qt::black) continue;

              if(map1.contains(key)) map1[key]++;
              else  map1.insert(key, 1);
            }
          }
          image2.setPixelColor(i,j,findMaxValue(map1));
        }
      }
    }
    image=image2;
    emit resultReady(image);
}

void Render::boundaryShapeControl(QImage &image)
{
    QImage image2(image);
    QImage subImage(3,3, QImage::Format_ARGB32);
    int width = image.width();
    int height = image.height();
    bool whiteSquare = true;
    isFull=true;

    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
        if (image.pixelColor(i,j) != Qt::white)
          {
            image2.setPixelColor(i,j,image.pixelColor(i,j));
          }
        else{
          isFull=false;
          QMap<QColor,int> map1;
          subImage.fill(Qt::white);

          // fill subimage
          for(int c = -1; c < 2; c++)
            {
              for(int d = -1; d < 2; d++)
              {
                if (d==0 && c==0) continue;
                int l=i+c;
                int k=j+d;
                if ((l < 0 || l > (width-1)) || (k < 0 || k > (height-1)))  continue; // check if we are in image
                else if(image.pixelColor(l,k) != Qt::white) //set pixel in subimage
                {
                        subImage.setPixelColor(c+1,d+1,image.pixelColor(l,k));
                        whiteSquare = false; // set flag if least one neighbour is different than white
                }

              }
            }
          if (!whiteSquare) // skip if all neighbours are white
          {
            // find most common color
            for(int m = 0; m < 3; m++)
              {
                for(int n = 0; n < 3; n++)
                {
                    QColor key = subImage.pixelColor(m,n);
                    if ((key == Qt::white) || (key == Qt::black)) continue;

                    if(map1.contains(key)) map1[key]++;
                    else  map1.insert(key, 1);
                }
              }

            QColor resultColor = findMaxValue(map1);
            int result = map1[resultColor];

            //rule 1
            if(result >= 5)
            {
                image2.setPixelColor(i,j,resultColor);
                continue;
            }
            //rule 2
            if(checkRule2(subImage) != Qt::white)
            {
                image2.setPixelColor(i,j,checkRule2(subImage));
                continue;
            }
            //rule 3
            if(checkRule3(subImage) != Qt::white)
            {
                image2.setPixelColor(i,j,checkRule3(subImage));
                continue;
            }
            //rule 3
            else
            {
                int rand = QRandomGenerator::global()->bounded(0, 100);
                if(rand <= probability)
                {
                    auto it = map1.begin();
                    it = it + QRandomGenerator::global()->bounded(0, map1.size()); //take random key from map
                    image2.setPixelColor(i,j,it.key());
                }
            }
          }
          whiteSquare = false;
        }



      } //second for
    } //first for

    image=image2;
    emit resultReady(image);

}

void Render::periodicBoundaryShapeControl(QImage &image)
{
    QImage image2(image);
    QImage subImage(3,3, QImage::Format_ARGB32);
    int width = image.width();
    int height = image.height();
    bool whiteSquare = true;
    isFull=true;

    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
        if (image.pixelColor(i,j) != Qt::white)
          {
            image2.setPixelColor(i,j,image.pixelColor(i,j));
          }
        else{
          isFull=false;
          QMap<QColor,int> map1;
          subImage.fill(Qt::white);

          // fill subimage
          for(int c = -1; c < 2; c++)
            {
              for(int d = -1; d < 2; d++)
              {
                if (d==0 && c==0) continue;
                int l=i+c;
                int k=j+d;
                if (l < 0) {
                    l = width - 1;
                } else if (l > width - 1) l = 0;
                if (k < 0) {
                    k = height - 1;
                } else if (k > height - 1) k = 0;
                if(image.pixelColor(l,k) != Qt::white) //set pixel in subimage
                {
                        subImage.setPixelColor(c+1,d+1,image.pixelColor(l,k));
                        whiteSquare = false; // set flag if least one neighbour is different than white
                }

              }
            }
          if (!whiteSquare) // skip if all neighbours are white
          {
            // find most common color
            for(int m = 0; m < 3; m++)
              {
                for(int n = 0; n < 3; n++)
                {
                    QColor key = subImage.pixelColor(m,n);
                    if ((key == Qt::white) || (key == Qt::black)) continue;

                    if(map1.contains(key)) map1[key]++;
                    else  map1.insert(key, 1);
                }
              }

            QColor resultColor = findMaxValue(map1);
            int result = map1[resultColor];

            //rule 1
            if(result >= 5)
            {
                image2.setPixelColor(i,j,resultColor);
                continue;
            }
            //rule 2
            if(checkRule2(subImage) != Qt::white)
            {
                image2.setPixelColor(i,j,checkRule2(subImage));
                continue;
            }
            //rule 3
            if(checkRule3(subImage) != Qt::white)
            {
                image2.setPixelColor(i,j,checkRule3(subImage));
                continue;
            }
            //rule 3
            else
            {
                int rand = QRandomGenerator::global()->bounded(0, 100);
                if(rand <= probability)
                {
                    auto it = map1.begin();
                    it = it + QRandomGenerator::global()->bounded(0, map1.size()); //take random key from map
                    image2.setPixelColor(i,j,it.key());
                }
            }
          }
          whiteSquare = false;
        }



      } //second for
    } //first for

    image=image2;
    emit resultReady(image);

}

void Render::mooreGrowSubstructure(QImage &image)
{
    QImage image2(image);
    int width = image.width();
    int height = image.height();
    isFull=true;
    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
        if (!(colorlist.contains(image.pixelColor(i,j))) || image.pixelColor(i,j) == Qt::black)
        {
            image2.setPixelColor(i,j,image.pixelColor(i,j));
        }
        else{
          isFull=false;
          QMap<QColor,int> map1;
          for(int c = -1; c < 2; c++)
          {
            for(int d = -1; d < 2; d++)
            {
              if (d==0 && c==0) continue;
              int l=i+c;
              int k=j+d;
              if ((l < 0 || l > (width-1)) || (k < 0 || k > (height-1)))  continue; // check if we are in image
              if(referenceImage.pixelColor(i,j) != referenceImage.pixelColor(l,k)) continue;
              QColor key = image.pixelColor(l,k);

              if (colorlist.contains(key)) continue;
              //if ()
              if (key == Qt::black) continue;

              if(map1.contains(key)) map1[key]++;
              else  map1.insert(key, 1);
            }
          }
          if(map1.empty() ) image2.setPixelColor(i,j,image.pixelColor(i,j));
          else image2.setPixelColor(i,j,findMaxValue(map1));
        }
      }
    }
    image=image2;
    emit resultReady(image);
}

void Render::periodicMooreGrowSubstructure(QImage &image)
{
    QImage image2(image);
    int width = image.width();
    int height = image.height();
    isFull=true;
    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
        if (!(colorlist.contains(image.pixelColor(i,j))) || image.pixelColor(i,j) == Qt::black)
        {
            image2.setPixelColor(i,j,image.pixelColor(i,j));
        }
        else{
          isFull=false;
          QMap<QColor,int> map1;

          for(int c = -1; c < 2; c++)
          {
            for(int d = -1; d < 2; d++)
            {
              if (d==0 && c==0) continue;
              int l=i+c;
              int k=j+d;

              if (l < 0)
              {
                  l = width - 1;
              }
              else if (l > width - 1) l = 0;
              if (k < 0)
              {
                  k = height - 1;
              }
              else if (k > height - 1) k = 0;

              if(referenceImage.pixelColor(i,j) != referenceImage.pixelColor(l,k)) continue;
              QColor key = image.pixelColor(l,k);
              //QColor key = subImage.pixelColor(c+1,d+1);

              if (colorlist.contains(key)) continue;
              if (key == Qt::black || key == Qt::white) continue;

              if(map1.contains(key)) map1[key]++;
              else  map1.insert(key, 1);
            }
          }
          if(map1.empty() ) image2.setPixelColor(i,j,image.pixelColor(i,j));
          else image2.setPixelColor(i,j,findMaxValue(map1));
        }
      }
    }
    image=image2;
    emit resultReady(image);
}

void Render::mooreGrowDualphase(QImage &image)
{
    QImage image2(image);
    int width = image.width();
    int height = image.height();
    isFull=true;
    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
        if (image.pixelColor(i,j) != Qt::white || image.pixelColor(i,j) == Qt::black || image.pixelColor(i,j) == Qt::red)
        {
            image2.setPixelColor(i,j,image.pixelColor(i,j));
        }
        else{
          isFull=false;
          QMap<QColor,int> map1;
          for(int c = -1; c < 2; c++)
          {
            for(int d = -1; d < 2; d++)
            {
              if (d==0 && c==0) continue;
              int l=i+c;
              int k=j+d;
              if ((l < 0 || l > (width-1)) || (k < 0 || k > (height-1)))  continue; // check if we are in image
              QColor key = image.pixelColor(l,k);
              if (key == Qt::white) continue;
              if (key == Qt::red) continue;
              if (key == Qt::black) continue;

              if(map1.contains(key)) map1[key]++;
              else  map1.insert(key, 1);
            }
          }
          image2.setPixelColor(i,j,findMaxValue(map1));
        }
      }
    }
    image=image2;
    emit resultReady(image);
}

void Render::periodicMooreGrowDualphase(QImage &image)
{
    QImage image2(image);
    int width = image.width();
    int height = image.height();
    isFull=true;
    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
        if (image.pixelColor(i,j) != Qt::white || image.pixelColor(i,j) == Qt::black || image.pixelColor(i,j) == Qt::red)
        {
            image2.setPixelColor(i,j,image.pixelColor(i,j));
        }
        else{
          isFull=false;
          QMap<QColor,int> map1;
          for(int c = -1; c < 2; c++)
          {
            for(int d = -1; d < 2; d++)
            {
              if (d==0 && c==0) continue;
              int l=i+c;
              int k=j+d;
              if (l < 0)
              {
                  l = width - 1;
              }
              else if (l > width - 1) l = 0;
              if (k < 0)
              {
                  k = height - 1;
              }
              else if (k > height - 1) k = 0;
              QColor key = image.pixelColor(l,k);
              if (key == Qt::white) continue;
              if (key == Qt::red) continue;
              if (key == Qt::black) continue;

              if(map1.contains(key)) map1[key]++;
              else  map1.insert(key, 1);
            }
          }
          image2.setPixelColor(i,j,findMaxValue(map1));
        }
      }
    }
    image=image2;
    emit resultReady(image);
}

void Render::boundaryShapeControlSubstructure(QImage &image)
{
    QImage image2(image);

    int width = image.width();
    int height = image.height();
    isFull=true;
    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
        if (!(colorlist.contains(image.pixelColor(i,j))) || image.pixelColor(i,j) == Qt::black)
        {
            image2.setPixelColor(i,j,image.pixelColor(i,j));
        }
        else{
          isFull=false;
          QMap<QColor,int> map1;
          QImage subImage(3,3, QImage::Format_ARGB32);
          subImage.fill(Qt::white);
          for(int c = 0; c < 3; c++)
          {
            for(int d = 0; d < 3; d++)
            {
                int l=i+c-1;
                int k=j+d-1;
                if ((l < 0 || l > (width-1)) || (k < 0 || k > (height-1)))  continue;
                else if (!(colorlist.contains(image.pixelColor(l,k)))) subImage.setPixelColor(c,d,image.pixelColor(l,k));
            }
          }
          for(int c = -1; c < 2; c++)
          {
            for(int d = -1; d < 2; d++)
            {
              if (d==0 && c==0) continue;
              int l=i+c;
              int k=j+d;
              if ((l < 0 || l > (width-1)) || (k < 0 || k > (height-1)))  continue; // check if we are in image
              if(referenceImage.pixelColor(i,j) != referenceImage.pixelColor(l,k)) continue;
              QColor key = subImage.pixelColor(c+1,d+1);

              if (colorlist.contains(key)) continue;
              //if ()
              if (key == Qt::black || key == Qt::white) continue;

              if(map1.contains(key)) map1[key]++;
              else  map1.insert(key, 1);
            }
          }
          if(map1.empty() ) image2.setPixelColor(i,j,image.pixelColor(i,j));
          else
          {
              QMap<QColor,int>::Iterator it = map1.find(Qt::white);
              if (it != map1.end())
              {
                  map1.erase(it);
                  qDebug() << "usuwam bialy";
              }
              QColor resultColor = findMaxValue(map1);
              int result = map1[resultColor];

              //rule 1
              if(result >= 5)
              {
                  image2.setPixelColor(i,j,resultColor);
                  continue;
              }


              ////rule 2
              if(checkRule2(subImage) != Qt::white)
              {
                  image2.setPixelColor(i,j,checkRule2(subImage));
                  continue;
              }
              ////rule 3
              if(checkRule3(subImage) != Qt::white)
              {
                  image2.setPixelColor(i,j,checkRule3(subImage));
                  continue;
              }
              ////rule 3
              else
              {
                  int rand = QRandomGenerator::global()->bounded(0, 100);
                  if(rand <= probability)
                  {
                      auto it = map1.begin();
                      it = it + QRandomGenerator::global()->bounded(0, map1.size()); //take random key from map
                      image2.setPixelColor(i,j,it.key());
                  }
              }
          }
        }
      }
    }
    image=image2;
    emit resultReady(image);
}

void Render::periodicBoundaryShapeControlSubstructure(QImage &image)
{
    QImage image2(image);
    int width = image.width();
    int height = image.height();
    isFull=true;
    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
        if (!(colorlist.contains(image.pixelColor(i,j))) || image.pixelColor(i,j) == Qt::black)
        {
            image2.setPixelColor(i,j,image.pixelColor(i,j));
        }
        else{
          isFull=false;
          QMap<QColor,int> map1;
          QImage subImage(3,3, QImage::Format_ARGB32);
          subImage.fill(Qt::white);
          for(int c = 0; c < 3; c++)
          {
            for(int d = 0; d < 3; d++)
            {
                int l=i+c-1;
                int k=j+d-1;
                if (l < 0) {
                    l = width - 1;
                } else if (l > width - 1) l = 0;
                if (k < 0) {
                    k = height - 1;
                } else if (k > height - 1) k = 0;
                if (!(colorlist.contains(image.pixelColor(l,k)))) subImage.setPixelColor(c,d,image.pixelColor(l,k));
            }
          }
          for(int c = -1; c < 2; c++)
          {
            for(int d = -1; d < 2; d++)
            {
              if (d==0 && c==0) continue;
              int l=i+c;
              int k=j+d;
              if (l < 0) {
                  l = width - 1;
              } else if (l > width - 1) l = 0;
              if (k < 0) {
                  k = height - 1;
              } else if (k > height - 1) k = 0;
              if(referenceImage.pixelColor(i,j) != referenceImage.pixelColor(l,k)) continue;
              QColor key = subImage.pixelColor(c+1,d+1);

              if (colorlist.contains(key)) continue;
              //if ()
              if (key == Qt::black || key == Qt::white) continue;

              if(map1.contains(key)) map1[key]++;
              else  map1.insert(key, 1);
            }
          }
          if(map1.empty() )
              image2.setPixelColor(i,j,image.pixelColor(i,j));
          else
          {
              QMap<QColor,int>::Iterator it = map1.find(Qt::white);
              if (it != map1.end())
              {
                  map1.erase(it);
              }
              QColor resultColor = findMaxValue(map1);
              int result = map1[resultColor];

              //rule 1
              if(result >= 5)
              {
                  image2.setPixelColor(i,j,resultColor);
                  continue;
              }


              ////rule 2
              if(checkRule2(subImage) != Qt::white)
              {
                  image2.setPixelColor(i,j,checkRule2(subImage));
                  continue;
              }
              ////rule 3
              if(checkRule3(subImage) != Qt::white)
              {
                  image2.setPixelColor(i,j,checkRule3(subImage));
                  continue;
              }
              ////rule 3
              else
              {
                  int rand = QRandomGenerator::global()->bounded(0, 100);
                  if(rand <= probability)
                  {
                      auto it = map1.begin();
                      it = it + QRandomGenerator::global()->bounded(0, map1.size()); //take random key from map
                      image2.setPixelColor(i,j,it.key());
                  }
              }
          }
        }
      }
    }
    image=image2;
    emit resultReady(image);

}

void Render::boundaryShapeControlDualphse(QImage &image)
{
    QImage image2(image);
    QImage subImage(3,3, QImage::Format_ARGB32);
    int width = image.width();
    int height = image.height();
    bool whiteSquare = true;
    isFull=true;

    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
        if (image.pixelColor(i,j) != Qt::white || image.pixelColor(i,j) == Qt::red)
          {
            image2.setPixelColor(i,j,image.pixelColor(i,j));
          }
        else{
          isFull=false;
          QMap<QColor,int> map1;
          subImage.fill(Qt::white);

          // fill subimage
          for(int c = -1; c < 2; c++)
            {
              for(int d = -1; d < 2; d++)
              {
                if (d==0 && c==0) continue;
                int l=i+c;
                int k=j+d;
                if ((l < 0 || l > (width-1)) || (k < 0 || k > (height-1)))  continue; // check if we are in image
                else if(image.pixelColor(l,k) != Qt::white) //set pixel in subimage
                {
                        subImage.setPixelColor(c+1,d+1,image.pixelColor(l,k));
                        whiteSquare = false; // set flag if least one neighbour is different than white
                }

              }
            }
          if (!whiteSquare) // skip if all neighbours are white
          {
            // find most common color
            for(int m = 0; m < 3; m++)
              {
                for(int n = 0; n < 3; n++)
                {
                    QColor key = subImage.pixelColor(m,n);
                    if ((key == Qt::white) || (key == Qt::black) || (key == Qt::red)) continue;

                    if(map1.contains(key)) map1[key]++;
                    else  map1.insert(key, 1);
                }
              }

            QColor resultColor = findMaxValue(map1);
            int result = map1[resultColor];

            //rule 1
            if(result >= 5)
            {
                image2.setPixelColor(i,j,resultColor);
                continue;
            }
            //rule 2
            if(checkRule2(subImage) != Qt::white)
            {
                image2.setPixelColor(i,j,checkRule2(subImage));
                continue;
            }
            //rule 3
            if(checkRule3(subImage) != Qt::white)
            {
                image2.setPixelColor(i,j,checkRule3(subImage));
                continue;
            }
            //rule 3
            else
            {
                int rand = QRandomGenerator::global()->bounded(0, 100);
                if(rand <= probability)
                {
                    auto it = map1.begin();
                    it = it + QRandomGenerator::global()->bounded(0, map1.size()); //take random key from map
                    image2.setPixelColor(i,j,it.key());
                }
            }
          }
          whiteSquare = false;
        }



      } //second for
    } //first for

    image=image2;
    emit resultReady(image);

}

void Render::periodicBoundaryShapeControlDualphse(QImage &image)
{
    QImage image2(image);
    QImage subImage(3,3, QImage::Format_ARGB32);
    int width = image.width();
    int height = image.height();
    bool whiteSquare = true;
    isFull=true;

    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
        if (image.pixelColor(i,j) != Qt::white || image.pixelColor(i,j) == Qt::red)
          {
            image2.setPixelColor(i,j,image.pixelColor(i,j));
          }
        else{
          isFull=false;
          QMap<QColor,int> map1;
          subImage.fill(Qt::white);

          // fill subimage
          for(int c = -1; c < 2; c++)
            {
              for(int d = -1; d < 2; d++)
              {
                if (d==0 && c==0) continue;
                int l=i+c;
                int k=j+d;
                if (l < 0) {
                    l = width - 1;
                } else if (l > width - 1) l = 0;
                if (k < 0) {
                    k = height - 1;
                } else if (k > height - 1) k = 0;
                if(image.pixelColor(l,k) != Qt::white) //set pixel in subimage
                {
                        subImage.setPixelColor(c+1,d+1,image.pixelColor(l,k));
                        whiteSquare = false; // set flag if least one neighbour is different than white
                }

              }
            }
          if (!whiteSquare) // skip if all neighbours are white
          {
            // find most common color
            for(int m = 0; m < 3; m++)
              {
                for(int n = 0; n < 3; n++)
                {
                    QColor key = subImage.pixelColor(m,n);
                    if ((key == Qt::white) || (key == Qt::black) || (key == Qt::red)) continue;

                    if(map1.contains(key)) map1[key]++;
                    else  map1.insert(key, 1);
                }
              }

            QColor resultColor = findMaxValue(map1);
            int result = map1[resultColor];

            //rule 1
            if(result >= 5)
            {
                image2.setPixelColor(i,j,resultColor);
                continue;
            }
            //rule 2
            if(checkRule2(subImage) != Qt::white)
            {
                image2.setPixelColor(i,j,checkRule2(subImage));
                continue;
            }
            //rule 3
            if(checkRule3(subImage) != Qt::white)
            {
                image2.setPixelColor(i,j,checkRule3(subImage));
                continue;
            }
            //rule 3
            else
            {
                int num = 50;
                int rand = QRandomGenerator::global()->bounded(0, 100);
                if(rand <= num)
                {
                    auto it = map1.begin();
                    it = it + QRandomGenerator::global()->bounded(0, map1.size()); //take random key from map
                    image2.setPixelColor(i,j,it.key());
                }
            }
          }
          whiteSquare = false;
        }



      } //second for
    } //first for

    image=image2;
    emit resultReady(image);
}

void Render::chooseGrains(QImage &image)
{

    int width = image.width();
    int height = image.height();

    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {
          if (image.pixelColor(i,j) == Qt::red) continue;
          else image.setPixelColor(i,j,Qt::white);
      }
    }
}

QColor Render::checkRule22(QImage image)
{

    QMap<QColor,int> map;
    //first quarter
    QColor color1 = image.pixelColor(0,1);
    if(color1 != Qt::white){
        if(map.contains(color1)) map[color1]++;
        else  map.insert(color1, 1);
    }
        //second quarter
    QColor color2 = image.pixelColor(1,0);
    if(color2 != Qt::white){
        if(map.contains(color2)) map[color2]++;
        else  map.insert(color2, 1);
        }
    //third quarter
    QColor color3 = image.pixelColor(2,1);
    if(color3 != Qt::white){
        if(map.contains(color3)) map[color3]++;
        else  map.insert(color3, 1);
        //fourth quarter
    }
    QColor color4 = image.pixelColor(1,2);
    if(color4 != Qt::white){

        if(map.contains(color4)) map[color4]++;
        else  map.insert(color4, 1);
    }
    QColor finalColor = findMaxValue(map);
    int result = map[finalColor];

    if (result >= 3) { return finalColor;}
    else return Qt::white;
}

QColor Render::checkRule2(QImage image)
{

    QMap<QColor,int> map;
    //first quarter
    QColor color1 = image.pixelColor(0,1);
    if(map.contains(color1)) map[color1]++;
    else  map.insert(color1, 1);
    //second quarter
    QColor color2 = image.pixelColor(1,0);
    if(map.contains(color2)) map[color2]++;
    else  map.insert(color2, 1);
    //third quarter
    QColor color3 = image.pixelColor(2,1);
    if(map.contains(color3)) map[color3]++;
    else  map.insert(color3, 1);
    //fourth quarter
    QColor color4 = image.pixelColor(1,2);
    if(map.contains(color4)) map[color4]++;
    else  map.insert(color4, 1);

    QColor finalColor = findMaxValue(map);
    int result = map[finalColor];

    if (result >= 3) { return finalColor;}
    else return Qt::white;
}

QColor Render::checkRule3(QImage image)
{
    QMap<QColor,int> map;
    //first quarter
    QColor color1 = image.pixelColor(0,0);
    if(map.contains(color1)) map[color1]++;
    else  map.insert(color1, 1);
    //second quarter
    QColor color2 = image.pixelColor(0,2);
    if(map.contains(color2)) map[color2]++;
    else  map.insert(color2, 1);
    //third quarter
    QColor color3 = image.pixelColor(2,0);
    if(map.contains(color3)) map[color3]++;
    else  map.insert(color3, 1);
    //fourth quarter
    QColor color4 = image.pixelColor(2,2);
    if(map.contains(color4)) map[color4]++;
    else  map.insert(color4, 1);

    QColor finalColor = findMaxValue(map);
    int result = map[finalColor];

    if (result >= 3) return finalColor;
    else return Qt::white;
}

QColor Render::findMaxValue(QMap<QColor,int> &map)
{
    int currentMax = 0;
    QColor arg_max = Qt::white;
    for(auto it = map.cbegin(); it != map.cend(); ++it )
     {
        if (it.value() > currentMax) {
                arg_max = it.key();
                currentMax = it.value();
            }
     }
    return arg_max;
}

void Render::add_grains(QImage &image, int numOfGrains)
{

    QColor color;
    for(int i=0; i<numOfGrains;i++)
    {
        int x=random(0,image.width()-1);
        int y=random(0,image.height()-1);
        color = intToColorR->find(i).value();

        while (image.pixelColor(x,y) == Qt::black)
        {
            x=random(0,image.width()-1);
            y=random(0,image.height()-1);
        }

        image.setPixel(x, y, color.rgb()); // TO DO avoid replace grains
    }

}

void Render::addGrainsSubstructure(QImage &image, int numOfGrains, int factor)
{
    colorlist.clear();
    QList<QColor> backgroundColors;
    int width = image.width();
    int height = image.height();
    int colorIndex = numOfGrains;
    for(int i =0; i<width; i++) {
      for(int j =0; j<height; j++) {

        QColor backColor = image.pixelColor(i,j);

        if(backgroundColors.contains(backColor) || backColor == Qt::black) continue;
        else backgroundColors.append(backColor);
      }
    }

    colorlist = backgroundColors;
    QMap<QColor,QList<QPoint>> grainsMap;
    QMap<QColor,QList<QPoint>>::Iterator it;
    //put grains in random places

    QList<QPoint> pointsList;
    for (auto it : backgroundColors)
    {
        if (pointsList.size() != 0) pointsList.clear();

        for(int l =0; l<width; l++) {
              for(int j =0; j<height; j++) {
                QColor grainColor = image.pixelColor(l,j);
                if (it == grainColor) pointsList.append(QPoint(l,j));
              }
        }
        grainsMap.insert(it,pointsList);
    }

    QColor color;

    for (auto iter : grainsMap)
    {
        for( int i=0; i<factor; i++)
        {
            if(iter.size()>0)
            {
                int rand = QRandomGenerator::global()->bounded(0,iter.size());
                QPoint point = iter.value(rand);
                color = intToColorR->find(colorIndex).value();
                colorIndex++;
                image.setPixelColor(point,color);
                iter.removeOne(point);
            }
        }
    }
}

void Render::addGrainsDualphase(QImage &image, int numOfGrains)
{

    QColor color;
    for(int i=0; i<numOfGrains;i++)
    {
        int x=random(0,image.width()-1);
        int y=random(0,image.height()-1);
        color = intToColorR->find(i).value();

        while (image.pixelColor(x,y) == Qt::black || image.pixelColor(x,y) == Qt::red)
        {
            x=random(0,image.width()-1);
            y=random(0,image.height()-1);
        }

        image.setPixel(x, y, color.rgb());
    }

}

void Render::add_inclusions_before(QImage &image,int numberOfInclusions, int sizeOfInclusions, QString inclusionType)
{
    QPainter p(&image);
    p.setBrush( Qt::black );
    int x, y;
    bool colisonFlag=true;

    for (int i=0; i<numberOfInclusions; i++)
    {
        while(colisonFlag){
            x=random(0,image.width());
            y=random(0,image.height());
            colisonFlag = checkBackgroundBefore(image,x,y,sizeOfInclusions );
        }
        colisonFlag = true;

       if (inclusionType == "Square") p.drawRect(x,y,sizeOfInclusions,sizeOfInclusions);
       else p.drawEllipse(x,y,sizeOfInclusions,sizeOfInclusions);
    }
    p.end();
    emit inclusionWasAdded(image);


}

void Render::add_inclusions_after(QImage &image,int numberOfInclusions, int sizeOfInclusions, QString inclusionType)
{
    QList<QPoint> list = findBoundaries(image);
    QPainter p(&image);
    QPoint point;
    int x,y;
    p.setBrush( Qt::black );
    bool colisonFlag=true;
    for (int i=0; i<numberOfInclusions; i++)
    {
        while(colisonFlag){
            point = list[QRandomGenerator::global()->bounded(0, list.size())];
            x = point.x() - sizeOfInclusions/2;
            y = point.y() - sizeOfInclusions/2;
           colisonFlag = checkBackgroundAfter(image,x,y,sizeOfInclusions );
        }

       colisonFlag = true;
       if (inclusionType == "Square") p.drawRect(x,y,sizeOfInclusions,sizeOfInclusions);
       else p.drawEllipse(x,y,sizeOfInclusions,sizeOfInclusions);
    }

    p.end();
    emit inclusionWasAdded(image);


}

bool Render::checkBackgroundAfter(QImage image,int x,int y,int size)
{
    for(int i =x-2; i<x+size+2; i++) {
      for(int j =y-2; j<y+size+2; j++) {
          if (i < 0 || j < 0 || i > (image.width()-1) || j > (image.height())-1) return true;
          if (image.pixelColor(i,j) == Qt::black) return true;
      }
    }
    return false;
}

bool Render::checkBackgroundBefore(QImage image,int x,int y,int size)
{
    for(int i =x-2; i<x+size+2; i++) {
      for(int j =y-2; j<y+size+2; j++) {
          if (i < 0 || j < 0 || i > (image.width()-1) || j > (image.height())-1) return true;
          if (image.pixelColor(i,j) == Qt::black) return true;
      }
    }
    return false;
}

int Render::random(int min, int max) //range : [min, max]
{
   return QRandomGenerator::global()->bounded(min, max);
}
QList<QPoint> Render::findBoundaries(QImage image)
{
     QList<QPoint> list;
     QColor refColor;
     int width = image.width();
     int height = image.height();
     bool skipFlag = false;
     for(int i =0; i<width; i++) {
        for(int j =0; j<height; j++) {
            refColor = image.pixelColor(i,j);
                for(int c = -1; c < 2; c++){
                    for(int d = -1; d < 2; d++){
                            if (d==0 && c==0) continue;
                            int l=i+c;
                            int k=j+d;

                            if (l >= 0 && l < width && k >= 0 && k < height) {
                                if(image.pixelColor(l,k) != refColor ){
                                    list.append(QPoint(l,k));
                                    skipFlag = true;
                                    continue;
                                    }
                                }
                        if(skipFlag){
                            skipFlag =false;
                            continue;
                            }
                        }
                    }
            }
        }
     return list;
}

void Render::renderBoundaries(QImage image)
{
    QList<QPoint> list = findBoundaries(image);
    image.fill(Qt::white);
    QPainter p(&image);
    p.setBrush( Qt::black );

    foreach(QPoint item, list)
    {
        //image.setPixelColor(item,Qt::black);
        p.drawRect(item.x(),item.y(), thickness,thickness);
    }
    emit resultReady(image);
}


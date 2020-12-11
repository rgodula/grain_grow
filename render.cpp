#include "render.h"

bool operator<(const QColor & a, const QColor & b) {
   return a.redF() < b.redF()
       || a.greenF() < b.greenF()
       || a.blueF() < b.blueF()
       || a.alphaF() < b.alphaF();
}

Render::Render(QObject *parent) : QObject(parent)
  , isFull(false)
  , mainWidth(1400)
  , mainHeight(920)


{
    QImage image(900,900, QImage::Format_ARGB32);
    image.fill(Qt::white);
    image1 = image;
    add_grains(image1,1000);
}

void Render::render()
{
    while(!isFull) grow(image1);
}

void Render::grow(QImage &image)
{
    QImage image2(image);
    int width = image.width();
    int height = image.height();
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
          for(int c = -1; c < 2; c++)
          {
            for(int d = -1; d < 2; d++)
            {
              if (d==0 && c==0) continue;
              int l=i+c;
              int k=j+d;
              if ((l < 0 || l > (width-1)) || (k < 0 || k > (height-1)))  continue; // check if we are in matrix
              QColor key = image.pixelColor(l,k);
              if (key == Qt::white) continue;

              if(map1.contains(key)) map1[key]++;
              else  map1.insert(key, 1);
            }
          }
          //QMap<QColor,int>::iterator best = std::max_element(map1.begin(),map1.end(),[] (const QPair<QColor,int>& a, const QPair<QColor,int>& b)->bool{ return a.second < b.second; } );
          image2.setPixelColor(i,j,findMaxValue(map1));
        }
      }
    }
    image=image2;
    emit resultReady(image);
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
    for(int i=0; i<numOfGrains;i++)
    {
        QColor color(random(0,255), random(0,255), random(0,255), 255);
        while(color == Qt::white) color = QColor(random(0,255), random(0,255), random(0,255), 255);
        image.setPixel(random(0,image.width()-1), random(0,image.height()-1), color.rgb()); // TO DO avoid replace grains
    }

}

int Render::random(int min, int max) //range : [min, max]
{
   static bool first = true;
   if (first)
   {
      srand( time(NULL) ); //seeding for the first time only!
      first = false;
   }
   return min + rand() % (( max + 1 ) - min);
}

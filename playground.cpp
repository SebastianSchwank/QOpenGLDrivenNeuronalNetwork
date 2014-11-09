#include "playground.h"

Playground::Playground(int width, int height) :
    QImage(width,height,QImage::Format_ARGB32)
{

    for(int x = 0; x < this->width(); x++){
        for(int y = 0; y < this->height(); y++){
            //QColor *color = new QColor(qrand()%255,qrand()%255,qrand()%255,qrand()%255);
            //QRgb col = qRgba(((float)x/this->width())*255,((float)y/this->height())*255,0,255);

            unsigned int weight = ((qrand()+qrand()*RAND_MAX)%4294967296);
            this->setPixel(x,y,weight);
        }
        this->setPixel(x,0,0);
        this->setPixel(0,x,0);
    }
}

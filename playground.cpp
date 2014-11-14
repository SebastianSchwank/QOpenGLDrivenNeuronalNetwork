#include "playground.h"

Playground::Playground(unsigned int neurons)
    : QImage(neurons*2,neurons,QImage::Format_ARGB32)
{
    this->fill(0);
    for(int x = 0; x < this->width()/2; x++){
        for(int y = 0; y < this->height(); y++){
            //QColor *color = new QColor(qrand()%255,qrand()%255,qrand()%255,qrand()%255);
            //QRgb col = qRgba(((float)x/this->width())*255,((float)y/this->height())*255,0,255);

            unsigned int weight = ((qrand()+qrand()*RAND_MAX)%4294967296);
            this->setPixel(x,y,weight);
        }
    }
}

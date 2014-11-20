#include "playground.h"

Playground::Playground(unsigned int neurons)
    : QImage(neurons,neurons,QImage::Format_ARGB32)
{
    for(int x = 0; x < this->width(); x++){
        for(int y = 0; y < this->height(); y++){
            //QColor *color = new QColor(qrand()%255,qrand()%255,qrand()%255,qrand()%255);
            //QRgb col = qRgba(((float)x/this->width())*255,((float)y/this->height())*255,0,255);

            FloatBits weight;
            weight.f_ = 1.0;//(2.0*qrand()/RAND_MAX-1.0)*1.0;//[-1-1]
            this->setPixel(x,y,weight.i_);
        }
    }
}

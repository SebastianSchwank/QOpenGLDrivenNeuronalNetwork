#include "playground.h"

Playground::Playground(int width, int height) :
    QImage(width,height,QImage::Format_ARGB32)
{

    for(int x = 0; x < this->width(); x++){
        for(int y = 0; y < this->height(); y++){
            QColor *color = new QColor(qrand()%255,qrand()%255,qrand()%255,qrand()%255);
            this->setPixel(x,y,color->rgba());
        }
    }
}

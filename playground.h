#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <QImage>
#include <QColor>
#include <QVector>

#include <qmath.h>

class Playground : public QImage
{
public:
    Playground(unsigned int neurons);

    typedef union
    {
        float f_;
        unsigned int i_;
    } FloatBits;

};

#endif // PLAYGROUND_H

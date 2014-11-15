#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <QImage>
#include <QColor>
#include <QVector>

#include <qmath.h>

class Playground : public QVector< QVector<float> >
{
public:
    Playground(unsigned int neurons);

    unsigned int convertToPixels(float activationFl);
    float convertFromPixels(unsigned int activationUI);
};

#endif // PLAYGROUND_H

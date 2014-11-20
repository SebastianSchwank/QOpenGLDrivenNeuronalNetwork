#ifndef GLANN_H
#define GLANN_H

#include <QGLShaderProgram>
#include <QGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QGLPixelBuffer>

#include <QBasicTimer>
#include <QTime>
#include <QImage>
#include <QVector>

#include "playground.h"


enum MODE{start,fwPropagation,fwFinished,backPropagation,backFinished};

class GLANN : public QGLWidget,protected QGLFunctions
{
    Q_OBJECT

public:
    GLANN(unsigned int neuronsCount, QImage *weightmap = 0);

    bool setInput(QVector<unsigned int> input);
    bool setError(QVector<unsigned int> error);
    QVector<unsigned int> getOutput();
    MODE getMode();

    unsigned int getNeuronsCount();

protected:

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void timerEvent(QTimerEvent *);

private:
    bool propagateInput();
    bool propagateBckWrds();
    void justDrawMaps();

    void getFeedbackTexture(unsigned int propCycle);
    void getCorrectedWeights();
    void normalizeDeltaWeights();

    void initShader();
    void initTextures();

    QBasicTimer timer;

    QImage *mWeightmap;
    QImage *mPropagation;

    QImage *sumDeltas;

    QGLShaderProgram program;

    GLuint pixelsActivation;
    GLuint pixelsWeightmap;
    unsigned char *renderedPropagation;
    unsigned char *renderedWeights;

    unsigned int mNeurons;
    unsigned int propCycle;

    MODE mode = start;

    //Input Variables
    Playground::FloatBits x;
    Playground::FloatBits y;
};

#endif // GLANN_H

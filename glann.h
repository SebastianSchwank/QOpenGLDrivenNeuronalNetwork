#ifndef GLANN_H
#define GLANN_H

#include <QGLShaderProgram>
#include <QGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QGLPixelBuffer>

#include <QBasicTimer>

#include <QImage>
#include <QVector>


enum MODE{finished,fwPropagation,backPropagation};

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
    void justDrawMaps();

    void initShader();
    void initTextures();

    unsigned int convertPixels(unsigned int RGBA);

    QBasicTimer timer;

    QImage *mWeightmap;
    QImage *mPropagation;

    QGLShaderProgram program;

    GLuint pixelsActivation;
    GLuint pixelsWeightmap;
    unsigned char *renderedPropagation;

    unsigned int mNeurons;
    unsigned int propCycle;

    MODE mode = finished;
};

#endif // GLANN_H

#ifndef GLANN_H
#define GLANN_H

#include <QGLShaderProgram>
#include <QGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QGLPixelBuffer>

#include <QImage>
#include <QVector>

class GLANN : public QGLWidget,protected QGLFunctions
{
    Q_OBJECT

public:
    GLANN(unsigned int neuronsCount, QImage *weightmap = 0);

    bool setInput(QVector<unsigned int> input);
    bool propagateInput();
    QVector<unsigned int> getOutput();

    unsigned int getNeuronsCount();

protected:

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    void initShader();
    void initTextures();

    QImage *mWeightmap;
    QImage *mPropagation;

    QGLShaderProgram program;

    GLuint pixelsActivation;
    GLuint pixelsWeightmap;
    GLuint *renderedPropagation;

    unsigned int mNeurons;
    unsigned int propCycle;
};

#endif // GLANN_H

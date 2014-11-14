#include "glann.h"

GLANN::GLANN(unsigned int neuronsCount, QImage *weightmap)
{
    setFixedSize(neuronsCount*2,neuronsCount);

    mNeurons = 0;
    if(weightmap == 0){
        mWeightmap = new QImage(neuronsCount,neuronsCount,QImage::Format_ARGB32);
        mNeurons = neuronsCount;
        mWeightmap->fill(qRgba(0,0,0,0));
    }else{
        mWeightmap = weightmap;
        if(mWeightmap->width()/2 != mWeightmap->height()){
            qDebug("Weightmap's height doesn't equal it's height ! Abort here.");
        }
        if(mWeightmap->width()/2 != neuronsCount){
            qDebug("Weightmap's size doesn't eqal the neurons Count ! Abort here.");
        }
        mNeurons = neuronsCount;
    }

    mPropagation = new QImage(neuronsCount*2+1,neuronsCount,QImage::Format_ARGB32);
    mPropagation->fill(qRgba(0,0,0,0));
}


void GLANN::initializeGL(){
    initializeGLFunctions();
    initShader();
    initTextures();
    // Use QBasicTimer because its faster than QTimer
    timer.start(0, this);
}

void GLANN::resizeGL(int w, int h){
    glViewport(0,0,w,h);
}

void GLANN::paintGL(){

    if(mode == fwPropagation) if(propagateInput()) mode=finished;
    if(mode == finished) justDrawMaps();
}

void GLANN::timerEvent(QTimerEvent *)
{
    // Update scene
    update();
}

void GLANN::initTextures(){
    // Load cube.png image
    glEnable(GL_TEXTURE_2D);

    //FEEDBACK Texture Propagtion + Corrected Weights
    renderedPropagation = new unsigned char [mNeurons*mNeurons*4];

    //Bind WeightmapTexture
    pixelsWeightmap = QGLWidget::bindTexture(*mWeightmap);

    // Set nearest filtering mode for texture minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set bilinear filtering mode for texture magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void GLANN::initShader(){

    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();

    // Use texture unit 0
    program.setUniformValue("texture", 0);

    // Use texture unit 1
    program.setUniformValue("IO",1);

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("imageSize", mNeurons);
}



bool GLANN::setInput(QVector<unsigned int> input){
    if(input.size() != mNeurons){
        qDebug("Input Vector doesn't match neuron-vector-size ! Abort here.");
        return false;
    }
    for(int i = 0; i < mNeurons; i++){
        mPropagation->setPixel(0,i,input[i]);
    }

    propCycle = 0;
    mode = fwPropagation;
    return true;
}

void GLANN::justDrawMaps(){
    // SetMode
    program.setUniformValue("mode", finished);

    //Bind ActivationMatrix
    pixelsActivation = bindTexture(*mPropagation);

    //Load Identity
    glLoadIdentity();

    //Move to rendering point
    glTranslatef( -1.0, -1.0, 0.0f );

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, pixelsWeightmap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pixelsActivation);

    // Draw geometry
    //Render textured quad
    glBegin( GL_QUADS );
        glTexCoord2f( 0.f, 0.f ); glVertex2f( 0, 0);
        glTexCoord2f( 1.f, 0.f ); glVertex2f( 2.0, 0);
        glTexCoord2f( 1.f, 1.f ); glVertex2f( 2.0, 2.0);
        glTexCoord2f( 0.f, 1.f ); glVertex2f( 0, 2.0);
     glEnd();
}

bool GLANN::propagateInput(){

    // SetMode
    program.setUniformValue("mode", fwPropagation);

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("propCycle", propCycle);

    //Bind WeightmapTexture
    pixelsActivation = bindTexture(*mPropagation);

    //Load Identity
    glLoadIdentity();

    //Move to rendering point
    glTranslatef( -1.0, -1.0, 0.0f );

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pixelsWeightmap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pixelsActivation);

    // Draw geometry
    //Render textured quad
    glBegin( GL_QUADS );
        glTexCoord2f( 0.f, 0.f ); glVertex2f( 0, 0);
        glTexCoord2f( 1.f, 0.f ); glVertex2f( 2.0, 0);
        glTexCoord2f( 1.f, 1.f ); glVertex2f( 2.0, 2.0);
        glTexCoord2f( 0.f, 1.f ); glVertex2f( 0, 2.0);
     glEnd();

    //geometries.drawCubeGeometry(&program);

    propCycle++;

    //Playground TexImage(thisSize.width(), thisSize.height());
    glReadPixels(propCycle+mNeurons,0,1,mNeurons,GL_RGBA,GL_UNSIGNED_BYTE,renderedPropagation);

    //qDebug("%i , %i" ,TexImage->size().width(),TexImage->size().height());

    for(int i = 0; i < mNeurons; i++){
        mPropagation->setPixel(propCycle,i,
                               qRgba(renderedPropagation[i*4],
                                     renderedPropagation[i*4+1],
                                     renderedPropagation[i*4+2],
                                     renderedPropagation[i*4+3]));
    }

    if(propCycle % mNeurons == 0){propCycle = 0; return true;}
    else return false;
}

unsigned int GLANN::convertPixels(unsigned int RGBA){
    unsigned int Alpha = (RGBA & 15) << 24;
    unsigned int ARGB = (RGBA >> 8) | Alpha;
    return ARGB;
}

QVector<unsigned int> GLANN::getOutput(){
    QVector<unsigned int> output;
    for(int i = 0; i < mNeurons; i++){
        unsigned int activation = mPropagation->pixel(mNeurons,i);
        output.append(activation);
    }
    return output;
}

MODE GLANN::getMode(){
    return mode;
}

unsigned int GLANN::getNeuronsCount(){
    return mNeurons;
}

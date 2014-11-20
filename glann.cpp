#include "glann.h"

GLANN::GLANN(unsigned int neuronsCount, QImage *weightmap)
{
    setFixedSize(neuronsCount*2+5,neuronsCount);

    qsrand((uint)QTime::currentTime().msec());

    mNeurons = 0;
    if(weightmap == 0){
        mWeightmap = new QImage(neuronsCount,neuronsCount,QImage::Format_ARGB32);
        mNeurons = neuronsCount;
        mWeightmap->fill(qRgba(0,0,0,0));
    }else{
        mWeightmap = weightmap;
        mNeurons = neuronsCount;
    }

    mPropagation = new QImage(neuronsCount,neuronsCount,QImage::Format_ARGB32);
    mPropagation->fill(qRgba(0,0,0,0));

    sumDeltas = new QImage(neuronsCount,neuronsCount,QImage::Format_ARGB32);
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
    if(mode == start){
        QVector<unsigned int> inputV;
        for(unsigned int i = 0; i < mNeurons; i++){
            Playground::FloatBits act;
            act.f_ = 0.0;
            inputV.append(act.i_); //4294967295 MAX_ACTIVATION
        }
        x.f_ = ((1.0*(qrand()%RAND_MAX))/RAND_MAX); //SCALE OUTPUT VALUE !
        //y.f_ = ((1.0*(qrand()%RAND_MAX))/RAND_MAX)/10.0;

        inputV[0] = x.i_;
        //inputV[1] = y.i_;

        this->setInput(inputV);
        propCycle = 1;
        mode = fwPropagation;
    }

    if(mode == fwPropagation) if(propagateInput()) mode=fwFinished;
    if(mode == fwFinished){
        QVector<unsigned int> out = getOutput();
        Playground::FloatBits result;
        //Playground::FloatBits error1;
        Playground::FloatBits error2;
        //result.i_ = out[mNeurons-1];
        //qDebug() << "x: " << y.f_ << "=? " << result.f_ << "Error: " << y.f_-result.f_;
        //error1.f_ = (y.f_-result.f_) * result.f_ * (1.0-result.f_);

        result.i_ = out[1];
        qDebug() << "y: " << x.f_ << "=? " << result.f_ << "Error: " << (x.f_-result.f_);
        error2.f_ = (x.f_-result.f_) * result.f_ * (1.0-result.f_);;

        qDebug() << "--------------------------------------------";

        QVector<unsigned int> Error;
        for(unsigned int i = 0; i < mNeurons; i++){
            Error.append(0); //4294967295 MAX_ACTIVATION
        }
        Error[1] = error2.i_ ;
        //Error[mNeurons-2] = error2.i_;
        setError(Error);
        mode = backPropagation;
        propCycle = mNeurons-2;
        sumDeltas->fill(qRgba(0,0,0,0));
    }
    if(mode == backPropagation) if(propagateBckWrds()) mode = backFinished;
    if(mode == backFinished){
        normalizeDeltaWeights();
        mode = start;
    }
}

void GLANN::timerEvent(QTimerEvent *)
{
    // Update scene
    update();
}

void GLANN::initTextures(){
    // Load cube.png image
    glEnable(GL_TEXTURE_2D);

    //FEEDBACK Texture Propagtion
    renderedPropagation = new unsigned char [mNeurons*4];

    //FEEDBACK Texture corrected weights
    renderedWeights = new unsigned char [mNeurons*mNeurons*4];

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
    program.setUniformValue("weights", 0);

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
    return true;
}

void GLANN::justDrawMaps(){
    // SetMode
    program.setUniformValue("mode", fwFinished);

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

    getFeedbackTexture(propCycle);

    propCycle++;

    if(propCycle % mNeurons == 0)return true;
    else return false;
}

bool GLANN::propagateBckWrds(){


    // SetMode
    program.setUniformValue("mode", backPropagation);

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("propCycle", propCycle);

    //Bind Propagation Map
    pixelsActivation = bindTexture(*mPropagation);

    //Bind Weightmap
    pixelsWeightmap = bindTexture(*mWeightmap);

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

    getFeedbackTexture(propCycle);
    getCorrectedWeights();

    propCycle--;

    //qDebug() << "BCKWRDS PROP" << propCycle;

    if(propCycle == 1){return true;}
    else return false;
}

void GLANN::getCorrectedWeights(){

    //Playground TexImage(thisSize.width(), thisSize.height());
    glReadPixels(0,0,mNeurons,mNeurons,GL_RGBA,GL_UNSIGNED_BYTE,renderedWeights);

    //qDebug("%i , %i" ,TexImage->size().width(),TexImage->size().height());

    for(int i = 0; i < mNeurons*mNeurons; i++){
        Playground::FloatBits currDelta;
        currDelta.i_ = qRgba(renderedWeights[i*4],renderedWeights[i*4+1],renderedWeights[i*4+2],renderedWeights[i*4+3]);
        Playground::FloatBits accDelta;
        accDelta.i_ = sumDeltas->pixel(i%mNeurons,(mNeurons-1)-(i/mNeurons));

        accDelta.f_ += currDelta.f_;

        sumDeltas->setPixel(i%mNeurons,(mNeurons-1)-(i/mNeurons),accDelta.i_);
    }

}

void GLANN::normalizeDeltaWeights(){
    for(int i = 0; i < mNeurons*mNeurons; i++){
        Playground::FloatBits accDelta;
        accDelta.i_ = sumDeltas->pixel(i%mNeurons,(mNeurons-1)-(i/mNeurons));

        accDelta.f_ = accDelta.f_/mNeurons;

        Playground::FloatBits weights;
        weights.i_ = mWeightmap->pixel(i%mNeurons,(mNeurons-1)-(i/mNeurons));

        weights.f_ += accDelta.f_;
        mWeightmap->setPixel(i%mNeurons,(mNeurons-1)-(i/mNeurons),weights.i_);
    }
}

void GLANN::getFeedbackTexture(unsigned int propCycle){

    //Playground TexImage(thisSize.width(), thisSize.height());
    glReadPixels(propCycle+mNeurons,0,1,mNeurons,GL_RGBA,GL_UNSIGNED_BYTE,renderedPropagation);

    //qDebug("%i , %i" ,TexImage->size().width(),TexImage->size().height());

    for(int i = 0; i < mNeurons; i++){
        mPropagation->setPixel(propCycle,(mNeurons-1)-i,
                               qRgba(renderedPropagation[i*4],
                                     renderedPropagation[i*4+1],
                                     renderedPropagation[i*4+2],
                                     renderedPropagation[i*4+3]));
    }

}

QVector<unsigned int> GLANN::getOutput(){
    QVector<unsigned int> output;
    for(int i = 0; i <= mNeurons-1; i++){
        unsigned int activation = mPropagation->pixel(mNeurons-1,i);
        output.append(activation);
    }
    return output;
}

bool GLANN::setError(QVector<unsigned int> error){
    for(int i = 0; i < mNeurons;i++) mPropagation->setPixel(mNeurons-1,i,error[i]);
    return true;
}

MODE GLANN::getMode(){
    return mode;
}

unsigned int GLANN::getNeuronsCount(){
    return mNeurons;
}

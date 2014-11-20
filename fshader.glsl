#version 440

//ANN Shader

uniform sampler2D IO;

uniform sampler2D weights;
uniform int imageSize;

uniform int propCycle;
uniform int mode; //0 is just drawing| 1 is fwd 2| is backward


float b2f( vec4 _packed)
{
    uint valueUINT = packUnorm4x8(_packed);
    return uintBitsToFloat(valueUINT);
}

vec4 f2b(float f)
{
    uint valueUINT = floatBitsToUint(f);
    return unpackUnorm4x8(valueUINT);
}

void main()
{
    ivec2 Coord = ivec2(gl_FragCoord);
    vec2 TexCoord = Coord;
    vec4 currTexel =  f2b(b2f(texelFetch(weights,ivec2(Coord.x,Coord.y),0)));

    float learningRate = 0.25;
    float bias = 0.4;

    if(mode == 1){

        if(Coord.x >= imageSize){
        currTexel =  texelFetch(IO,ivec2(Coord.x-imageSize,Coord.y),0);

            if(Coord.x == propCycle+imageSize){
                highp float sum = 0.0;

                    for(int i = 0; i < imageSize; i++){
                        float inputActivation = b2f(texelFetch(IO,ivec2(propCycle-1,i),0));
                        float weight = b2f(texelFetch(weights,ivec2(Coord.y,i),0));
                        sum = sum + inputActivation * weight; //* inputActivation;
                    }
                float sigmond = (1.0/(1.0 + exp(sum + bias)));// ACTIVATION FUNCTION
                currTexel =  f2b(sigmond);
            }
        }
    }

    if(mode == 3){

        if(Coord.x >= imageSize){

            if(Coord.x == propCycle+imageSize){
                highp float errorSumWeighted = 0.0;

                for(int i = 0; i < imageSize; i++){
                    float outputError = b2f(texelFetch(IO,ivec2(propCycle+1,i),0));
                    //Weight from N(i) to N(Coord.y)
                    float weight = b2f(texelFetch(weights,ivec2(i,Coord.y),0));
                    errorSumWeighted = errorSumWeighted + outputError * weight;
                }

                float mActivation = b2f(texelFetch(IO,ivec2(propCycle,Coord.y),0));
                currTexel = f2b(errorSumWeighted * mActivation * (1.0-mActivation));
            }else{
                currTexel = texelFetch(IO,ivec2(Coord.x-imageSize,Coord.y),0);
            }

        }else{
            //Weight from N(Coord.x) to N(Coord.y)
            float currWeight = b2f(texelFetch(weights,ivec2(Coord.x,Coord.y),0));
            float mActivation = b2f(texelFetch(IO,ivec2(propCycle,Coord.x),0));
            float Error = b2f(texelFetch(IO,ivec2(propCycle+1,Coord.y),0));

            currTexel = f2b(mActivation * Error * learningRate);
        }

    }

    if((mode == 0) || (mode == 2) || (mode == 4)){
        if(Coord.x >= imageSize){
            currTexel = texelFetch(IO,ivec2(Coord.x-imageSize,Coord.y),0);
        }
    }

    gl_FragColor = currTexel;
}


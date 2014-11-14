#version 330 core

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

//ANN Shader
#pragma STDGL invariant(all)

uniform sampler2D IO;

uniform sampler2D texture;
uniform int imageSize;

uniform int propCycle;
uniform int mode; //0 is just drawing| 1 is fwd 2| is backward


float b2f(vec4 fbytes) {
    ivec4 bytes = ivec4(fbytes * 255.0 + 0.5);
    float f = float(bytes[0] + bytes[1] * 256) +
              float(bytes[2] - bytes[2] / 128 * 128 + 128) * 256.0 * 256.0;
    int exp = 150 - (bytes[3] - bytes[3] / 128 * 128) * 2 - (bytes[2] / 128);
    f /= exp2(float(exp));
    if (bytes[3] / 128 == 1)
        return -f;
    else
        return f;
}

vec4 f2b(float f) {
    bool pos = f > 0.0;
    if (!pos)
        f = -f;
    int expo = int(floor(log2(f)));
    f *= pow(2.0, 23.0 - float(expo));
    f -= 8388608.0;
    int b = int(floor(f + 0.5));
    expo += 127;
    int posbit = pos ? 0 : 128;
    return vec4(b - b / 256 * 256, b / 256 - b / 256 / 256 * 256,
                b / 256 / 256 + (expo - expo / 2 * 2) * 128,
                expo / 2 + posbit) / 255.0;
}
void main()
{
    ivec2 Coord = ivec2(gl_FragCoord);
    vec2 TexCoord = Coord;
    vec4 currTexel = f2b(b2f(texture(texture,gl_TexCoord[0].st)));

    if(mode == 1){
        if(Coord.x == propCycle+1+imageSize){
            float sum = 0.0;

            float inputActivation = b2f(texelFetch(IO,ivec2(propCycle,Coord.y),0));
                for(int x = 0; x < imageSize; x++){
                    float weight = b2f(texelFetch(texture,ivec2(x,Coord.y),0));
                    sum = sum + inputActivation * weight; //* inputActivation;
                }
            currTexel =  f2b((1.0/(1.0 + exp(-2.0 * sum)))); // ACTIVATION FUNCTION
        }
    }

    if(mode == 3){

    }

    if((mode == 0) || (mode == 2) || (mode == 4)){
        if(Coord.x >= imageSize){
            currTexel = texelFetch(IO,ivec2(Coord.x-imageSize,Coord.y),0);
        }
    }

    gl_FragColor = currTexel;
}


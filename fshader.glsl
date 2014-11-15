#version 400

#ifdef GL_ES
// Set default precision to high
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

//unpack a 32bit float from 4 8bit, [0;1] clamped floats
float b2f( vec4 _packed)
{
    vec4 rgba = 255.0 * _packed;
    float sign =  step(-128.0, -rgba[1]) * 2.0 - 1.0;
    float exponent = rgba[0] - 127.0;
    if (abs(exponent + 127.0) < 0.001)
        return 0.0;
    float mantissa =  mod(rgba[1], 128.0) * 65536.0 + rgba[2] * 256.0 + rgba[3] + (0x800000);
    return sign *  exp2(exponent-23.0) * mantissa ;
}

//pack a 32bit float into 4 8bit, [0;1] clamped floats
vec4 f2b(float f)
{
    float F = abs(f);
    if(F == 0.0)
    {
        return  vec4(0,0,0,0);
    }
    float Sign =  step(0.0, -f);
    float Exponent = floor( log2(F));

    float Mantissa = F/ exp2(Exponent);
    //std::cout << "  sign: " << Sign << ", exponent: " << Exponent << ", mantissa: " << Mantissa << std::endl;
    //denormalized values if all exponent bits are zero
    if(Mantissa < 1.0)
        Exponent -= 1;

    Exponent +=  127;

    vec4 rgba;
    rgba[0] = Exponent;
    rgba[1] = 128.0 * Sign +  mod(floor(Mantissa * float(128.0)),128.0);
    rgba[2] = floor( mod(floor(Mantissa* exp2(float(23.0 - 8.0))), exp2(8.0)));
    rgba[3] = floor( exp2(23.0)* mod(Mantissa, exp2(-15.0)));
    return (1 / 255.0) * rgba;
}

void main()
{
    ivec2 Coord = ivec2(gl_FragCoord);
    vec2 TexCoord = Coord;
    vec4 currTexel = f2b(b2f(texture(texture,gl_TexCoord[0].st)));//-texture(texture,gl_TexCoord[0].st);

    if(mode == 1){
        if(Coord.x == propCycle+1+imageSize){
            highp float sum = 0.0;

            float inputActivation = b2f(texelFetch(IO,ivec2(propCycle,Coord.y),0));
                for(int x = 0; x < imageSize; x++){
                    float weight = b2f(texelFetch(texture,ivec2(x,Coord.y),0));
                    sum = sum + inputActivation * weight; //* inputActivation;
                }
            float sigmond = (1.0/(1.0 + exp(-4.0 * sum)));// ACTIVATION FUNCTION
            currTexel =  f2b((float(Coord.x)/float(imageSize)));
        }
    }

    if(mode == 3){
        if(Coord.x <= imageSize){

        }

    }

    if((mode == 0) || (mode == 2) || (mode == 4)){
        if(Coord.x >= imageSize){
            currTexel = texelFetch(IO,ivec2(Coord.x-imageSize,Coord.y),0);
        }
    }

    gl_FragColor = currTexel;
}


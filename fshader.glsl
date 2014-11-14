#version 330 core

#ifdef GL_ES
// Set default precision to medium
precision highp int;
precision highp float;
#endif

//ANN Shader
#pragma STDGL invariant(all)

uniform sampler2D IO;

uniform sampler2D texture;
uniform int imageSize;

uniform int propCycle;
uniform int mode; //0 is just drawing| 1 is fwd 2| is backward


vec4 pack_depth(const in float depth)
{
    const vec4 bit_shift = vec4(256.0*256.0*256.0, 256.0*256.0, 256.0, 1.0);
    const vec4 bit_mask  = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);
    vec4 res = fract(depth * bit_shift);
    res -= res.xxyz * bit_mask;
    return res;
}

float unpack_depth(const in vec4 rgba_depth)
{
    const vec4 bit_shift = vec4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1.0);
    float depth = dot(rgba_depth, bit_shift);
    return depth;
}


void main()
{
    ivec2 Coord = ivec2(gl_FragCoord);
    vec2 TexCoord = Coord;
    vec4 currTexel = pack_depth(unpack_depth(texture(texture,gl_TexCoord[0].st)));

    if(mode == 1){
        if(Coord.x == propCycle+1+imageSize){
            highp vec4 sum = vec4(0.0,0.0,0.0,0.0);

                for(int x = 0; x < imageSize; x++){
                    vec4 inputActivation = texelFetch(IO,ivec2(propCycle,Coord.y),0);
                    vec4 weight = texelFetch(texture,ivec2(x,Coord.y),0);
                    sum = sum + weight * inputActivation; //* inputActivation;
                }
                currTexel =  (1.0/(1.0 + exp(-2.0 * sum))); // ACTIVATION FUNCTION

        }
    }



    if(mode == 0){

        if(Coord.x >= imageSize){
            currTexel = texelFetch(IO,ivec2(Coord.x-imageSize,Coord.y),0);
        }

    }

    gl_FragColor = currTexel;
}


#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

//CopyShader

uniform sampler2D IO;

uniform sampler2D texture;
uniform int imageSize;

void main()
{
    vec4 currTexel = texture2D(texture,gl_TexCoord[0].st);

    if(gl_TexCoord[0].st.x > 0.5){
        vec4 sum = vec4(0.0f,0.0f,0.0f,0.0f);

        for(int x = 0; x < imageSize; x++){
            currTexel = texture2D(IO,vec2(gl_TexCoord[0].st.x-0.5,0));
            //vec4 inputActivation = texture2D(IO,vec2(gl_TexCoord[0].st.x-0.5,0));
            //sum = sum + weight * inputActivation;
        }

        //vec4 sigmoid = (1.0/(1.0 + exp(-2.0 * sum))); // ACTIVATION FUNCTION
        //weight = sigmoid;
    }

    gl_FragColor = currTexel;
}

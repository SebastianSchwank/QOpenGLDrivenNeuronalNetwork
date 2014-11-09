#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;
uniform int imageSize;

void main()
{
    vec4 color = texture2D(texture,gl_TexCoord[0].st);
    gl_FragColor = color;
}

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

void main() {

    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_Position = ftransform();
}

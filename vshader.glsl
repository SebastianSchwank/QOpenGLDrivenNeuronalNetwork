#version 400

#ifdef GL_ES
// Set default precision to high
precision mediump int;
precision mediump float;
#endif

void main() {

    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_Position = ftransform();
}

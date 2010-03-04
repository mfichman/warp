varying vec3 normal;
varying vec3 view;

void main() {
    // Transform to homogeneous coordinates
    gl_Position = ftransform();
    gl_TexCoord[0] = gl_MultiTexCoord0;
    normal = gl_NormalMatrix * gl_Normal;
    view = vec3(gl_ModelViewMatrix * gl_Vertex);
}
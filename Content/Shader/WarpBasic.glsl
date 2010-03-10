varying vec3 normal;
varying vec3 view;
varying float fog;

void main() {
    // Transform to homogeneous coordinates
    gl_Position = ftransform();
    gl_TexCoord[0] = gl_MultiTexCoord0;
    normal = gl_NormalMatrix * gl_Normal;
    view = vec3(gl_ModelViewMatrix * gl_Vertex);
    
    gl_FogFragCoord = length(view);
    fog = (gl_Fog.end - gl_FogFragCoord)*gl_Fog.scale;
    fog = clamp(fog, 0.0, 1.0);
}
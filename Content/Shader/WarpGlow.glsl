extern void phong(out vec4, out vec4, out vec4);

uniform sampler2D glow_sampler; // Glow texture
uniform sampler2D tex_sampler;
uniform float time;

void main() {
    vec4 diffuse;
    vec4 specular;
    vec4 ambient;
    phong(diffuse, specular, ambient);
    
    vec2 texcoord = gl_TexCoord[0].st;
    texcoord[0] *= 5.0;
    
    vec4 tex = texture2D(tex_sampler, texcoord);
    vec4 glow = texture2D(glow_sampler, texcoord);
    vec4 color = (ambient + diffuse*tex + specular);
    glow.a *= (sin(time) + 1.0)/2.0;

    // This is the final color of the pixel
    gl_FragColor = glow.a*glow + (1.0-glow.a)*color;
}
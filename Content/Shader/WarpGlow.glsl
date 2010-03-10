void phong(out vec4, out vec4, out vec4);

uniform sampler2D glow_sampler; // Glow texture
//uniform sampler2D tex_sampler;
uniform float time;
uniform vec2 texscale;
varying float fog;

void main() {
    vec4 diffuse = vec4(0);
    vec4 specular = vec4(0);
    vec4 ambient = vec4(0);
    phong(diffuse, specular, ambient);
    
    vec2 texcoord = gl_TexCoord[0].st * texscale;
    texcoord.t += time / 2.0;
    
    //vec4 tex = texture2D(tex_sampler, texcoord);
    vec4 glow = texture2D(glow_sampler, texcoord);
    vec4 color = (ambient + diffuse * gl_FrontMaterial.diffuse/**tex*/ + specular);
    glow.a *= (cos(time) + 1.0)/2.0;

    // This is the final color of the pixel
    gl_FragColor = vec4(glow.a*glow.xyz + (1.0-glow.a)*color.xyz, 1.0);
    gl_FragColor = mix(gl_Fog.color, gl_FragColor, fog);
    //gl_FragColor = vec4(glow.xyz * color.xyz, 1.0);
}

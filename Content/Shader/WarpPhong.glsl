uniform vec3 ambient_light;
varying vec3 normal;
varying vec3 view;
uniform int light_count;

void phong(out vec4 diffuse, out vec4 specular, out vec4 ambient) {
    
    // Normalize vectors
    vec3 n = normalize(normal);
    vec3 v = normalize(view);
    
    // Initialize colors
    vec3 r = reflect(v, n);
    
    // Two-sided Phong lighting shader.    
    for (int i = 0; i < 1; i++) {
        // Directional light
        //vec3 light = vec3(gl_LightSource[i].position) - view;
        vec3 light = gl_LightSource[i].position.xyz;
        vec3 l = normalize(light);
        
        // Calculate attenuation
        float d = length(light);
        float c0 = gl_LightSource[i].constantAttenuation;
        float c1 = gl_LightSource[i].linearAttenuation;
        float c2 = gl_LightSource[i].quadraticAttenuation;
        float a = 1.0 / (c0 + c1*d + c2*d*d);

        // Calculate diffuse and specular coefficients
        float s = a * max(0.0, dot(l, n));
        //float t = a * max(pow(dot(l, r), gl_FrontMaterial.shininess), 0.0);
        
        diffuse += s * gl_LightSource[i].diffuse;
        //if (s > 0.0) {
        //    specular += t * gl_LightSource[i].specular;
        //}
        ambient += a * gl_FrontMaterial.ambient * gl_LightSource[i].ambient;
    }
    ambient += gl_FrontMaterial.ambient * vec4(ambient_light, 1.0);
    
    clamp(diffuse, 0.0, 1.0);
    clamp(specular, 0.0, 1.0);
    clamp(ambient, 0.0, 1.0);
}
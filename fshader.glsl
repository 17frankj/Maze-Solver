#version 120

// Varying variables from the vertex shader
varying vec4 N;       // Normal vector (in Eye Coordinates)
varying vec4 L_eye;   // Vector to light source (in Eye Coordinates)
varying vec4 V_eye;   // View vector (in Eye Coordinates)
varying float dist;   // Distance to light source
varying vec2 texCoord;

// Uniforms for texture and lighting parameters
uniform sampler2D simpler2D_texture; // Corrected sampler name to be standard and clear
uniform float shininess;
uniform vec4 uLightColor;  // Light color/intensity

// --- NEW UNIFORMS FOR LIGHTING CONTROL ---
uniform vec4 uAmbientIntensity;  // Controls ambient light color/strength
uniform vec4 uDiffuseIntensity;  // Controls diffuse light color/strength
uniform vec4 uSpecularColor;     // Controls specular highlight color (usually white)

void main()
{
    // 1. Get the texture color, which will serve as the base diffuse color
    // The original fragment shader uses 'texture' as the uniform name, 
    // but the type is 'simpler2D'. Using 'sampler2D' is the standard type.
    vec4 tex_color = texture2D(simpler2D_texture, texCoord);
    
    // 2. Normalize all vectors
    vec3 NN = normalize(N.xyz);
    vec3 LL = normalize(L_eye.xyz);
    vec3 VV = normalize(V_eye.xyz);
    
    // 3. Calculate Halfway Vector (H)
    vec3 H = normalize(LL + VV);

    // 4. Ambient Component (Using a simplified ambient factor)
    // We use a constant ambient factor (e.g., 0.2) multiplied by the texture color
    vec4 ambient = tex_color * uAmbientIntensity * uLightColor;
    
    // 5. Diffuse Component
    // Diffuse is dot(L, N) * Material_Color * Light_Intensity
    float diffuse_factor = max(dot(LL, NN), 0.0);
    // Use the texture color (tex_color) as the material's diffuse color
    vec4 diffuse = diffuse_factor * uDiffuseIntensity * uLightColor * tex_color; 
    
    // 6. Specular Component
    // Specular is pow(dot(N, H), shininess) * Specular_Color * Light_Intensity
    float specular_factor = pow(max(dot(NN, H), 0.0), shininess);
    // Use white for specular highlight, as is common
    vec4 specular = specular_factor * uSpecularColor * uLightColor;

    // 8. Final Color
    // Final Color = Ambient + Attenuation * (Diffuse + Specular)
    vec4 final_color = ambient + diffuse + specular;
    
    gl_FragColor = final_color;
}



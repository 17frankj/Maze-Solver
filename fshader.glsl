#version 120

// Varying variables from the vertex shader
varying vec4 N;       // Normal vector (in Eye Coordinates)
varying vec4 L_eye;   // Vector to light source (in Eye Coordinates)
varying vec4 V_eye;   // View vector (in Eye Coordinates)
varying float dist;   // Distance to light source
varying vec2 texCoord;
varying vec3 spot_dir_eye; // Spotlight direction in Eye Coordinates

// Uniforms for texture and lighting parameters
uniform sampler2D simpler2D_texture; // Corrected sampler name to be standard and clear
uniform float shininess;
uniform vec4 uLightColor;  // Light color/intensity

// --- NEW UNIFORMS FOR LIGHTING CONTROL ---
uniform vec4 uAmbientIntensity;  // Controls ambient light color/strength
uniform vec4 uDiffuseIntensity;  // Controls diffuse light color/strength
uniform vec4 uSpecularColor;     // Controls specular highlight color (usually white)
uniform int sun_mode_toggle;     // Controls which lighting mode is active
// --- NEW UNIFORMS FOR FLASHLIGHT CONTROL ---
uniform float spot_cutoff;      // Cosine of the cutoff angle
uniform float spot_exponent;    // Exponent for falloff

void main()
{
    // Get the texture color, which will serve as the base diffuse color
    vec4 tex_color = texture2D(simpler2D_texture, texCoord);

    // Normalize all vectors
    // These need to be normalized because interpolation (varying) can distort their length.
    vec3 NN = normalize(N.xyz);    // Normalized Normal Vector
    vec3 LL = normalize(L_eye.xyz); // Normalized Light Vector
    vec3 VV = normalize(V_eye.xyz); // Normalized View Vector

    // Calculate Halfway Vector (H)
    // H = normalize(L + V)
    vec3 H = normalize(LL + VV);

    vec4 final_color;

    // Calculate lighting components using the uniform variables

    // Ambient: TexColor * Material_Ambient_Coefficient * Light_Color
    vec4 ambient = tex_color * uAmbientIntensity * uLightColor; 

    // Diffuse: dot(L, N) * TexColor * Material_Diffuse_Coefficient * Light_Color
    float diffuse_factor = max(dot(LL, NN), 0.0);
    vec4 diffuse = diffuse_factor * tex_color * uDiffuseIntensity * uLightColor; 
 
    // Specular: pow(dot(N, H), shininess) * Material_Specular_Color * Light_Color
    float specular_factor = pow(max(dot(NN, H), 0.0), shininess);
    // Note: Specular highlights usually don't use the texture color, only the light color and material specular color.
    vec4 specular = specular_factor * uSpecularColor * uLightColor; 
 
    // Check the toggle uniform to determine the final color calculation
    if (sun_mode_toggle == 0) {
    // Mode 0: LIGHTING OFF (Render raw texture color, ignoring all light calculations)
        final_color = tex_color; 
    }
    else if (sun_mode_toggle == 1) {
    // Mode 1: FULL PHONG LIGHTING (Ambient + Diffuse + Specular)
        final_color = ambient + diffuse + specular;
    }
    else if (sun_mode_toggle == 2) {
    // Mode 2: AMBIENT ONLY
        final_color = ambient;
    }
    else if (sun_mode_toggle == 3) {
    // Mode 3: DIFFUSE ONLY (Plus a small ambient term to see texture in shadow)
        final_color = (ambient * 0.1) + diffuse; 
    }
    else if (sun_mode_toggle == 4) {
    // Mode 4: SPECULAR ONLY (Plus a medium ambient term for surface visibility)
        final_color = (ambient * 0.5) + specular; 
    }
    
    // --- SPOTLIGHT CALCULATION ---
    else if (sun_mode_toggle == 5) {
        // The flashlight direction (spot_dir_eye) should be normalized.
        vec3 SD = normalize(spot_dir_eye); 

        // Calculate the cosine of the angle between the light vector (-LL) and 
        // the spotlight direction (SD). Note: LL points *from* the surface *to* the light, 
        // so we use -LL to point *from* the light *to* the surface.
        float angle_cos = dot(-LL, SD);

        // Check if the fragment is within the spotlight cone
        if (angle_cos >= spot_cutoff) {
            // Calculate the spotlight intensity factor.
            // The closer angle_cos is to 1.0 (center of the beam), the higher the power.
            float spot_factor = pow(angle_cos, spot_exponent);

            // Mode 5: FLASHLIGHT MODE (Ambient + Diffuse, multiplied by spot_factor)
            // Only ambient and diffuse are requested, and both are attenuated.
            final_color = (ambient + diffuse) * spot_factor;

        } 
        else {
        // Outside the spotlight cone, only the base ambient light remains.
        // This ensures surfaces are still minimally visible.
        final_color = ambient * 0.1; // Reduced ambient for darkness
        }
    }
    else {
    // Fallback to Full Lighting
        final_color = ambient + diffuse + specular;
    }

    gl_FragColor = final_color;
}



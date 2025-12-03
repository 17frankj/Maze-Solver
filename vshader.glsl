#version 120

attribute vec4 vPosition;
attribute vec4 vNormal; // Need normal for lighting
attribute vec2 vTexCoord;

varying vec4 N;       // Transformed Normal (in Eye Coordinates)
varying vec4 L_eye;   // Light vector (in Eye Coordinates)
varying vec4 V_eye;   // View vector (in Eye Coordinates)
varying float dist;   // Distance to the light source
varying vec2 texCoord;
varying vec3 spot_dir_eye; // Spotlight direction in Eye Coordinates
uniform vec4 flashlight_direction_uniform;

uniform int sun_mode_toggle;
uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;
uniform mat4 ctm;
uniform vec4 light_position;    // Light position (in World Coordinates)

void main()
{
	//texCoord = vTexCoord;
	//gl_Position = projection_matrix * model_view_matrix * ctm * vPosition;

	// Pass texture coordinates
	texCoord = vTexCoord;

	// Transform vertex position to Eye Coordinates
	vec4 p_eye = model_view_matrix * ctm * vPosition;
	
	// Transform normal to Eye Coordinates (only rotation matters)
	N = normalize(model_view_matrix * ctm * vNormal);
	
	// Calculate Light Vector (L_eye) and Distance (dist)
	// light_position is typically in World or Eye coordinates.
	// Assuming light_position is in World coordinates like in the original shader, 
	// we transform it to Eye coordinates first.
	vec4 light_pos_eye = model_view_matrix * light_position; // old, pre flashlight

	// --- FLASHLIGHT LOGIC (sun_mode_toggle == 5) ---
	if (sun_mode_toggle == 5) {
		// Flashlight is at the viewer's position (origin in Eye Coordinates)
		light_pos_eye = vec4(0.0, 0.0, 0.0, 1.0);
		// Flashlight shines down the negative Z-axis (camera's forward) in Eye Coordinates
		//spot_dir_eye = vec3(0.0, -0.3, -1.0);
		spot_dir_eye = flashlight_direction_uniform.xyz;
	} 

	// Eye Light (Point light at eye, shines in all directions)
	else if (sun_mode_toggle == 7) { 
		light_pos_eye = vec4(0.0, 0.0, 0.0, 1.0); // Light source is at the viewer
		spot_dir_eye = vec3(0.0); // Dummy value, not a spotlight
	}

	else {
		// Standard light source (transform world position to eye coordinates)
        light_pos_eye = model_view_matrix * light_position;
        // Use a dummy value for non-flashlight modes
        spot_dir_eye = vec3(0.0); 
    }
	// Vector from vertex (p_eye) to light (light_pos_eye)
	vec4 L_temp = light_pos_eye - p_eye;
	
	L_eye = L_temp; // Pass the vector (will be normalized in fragment shader)
	dist = length(L_temp);

	// Calculate View Vector (V_eye)
	// In eye coordinates, the viewer is at the origin (0, 0, 0, 1)
	vec4 eye_position = vec4(0.0, 0.0, 0.0, 1.0);
	V_eye = eye_position - p_eye; // Vector from vertex (p_eye) to eye (origin)
	
	// Final vertex position
	gl_Position = projection_matrix * p_eye;
	
}

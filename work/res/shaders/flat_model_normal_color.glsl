#version 330 core

uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;

#ifdef _VERTEX_

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aMultiTexCoord0;

// Model data (out to the fragment shader)
out VertexData {
	vec3 model_normal;
} v_out;

void main() {
	v_out.model_normal = aNormal;
	gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1);
}

#endif



#ifdef _FRAGMENT_

// Viewspace data (in from the vertex shader)
in VertexData {
	vec3 model_normal;
} f_in;

out vec3 fb_color;

void main() {
	fb_color = abs(f_in.model_normal);
}

#endif
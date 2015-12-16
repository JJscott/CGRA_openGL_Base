

// Constant across both shaders
uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;

#ifdef _VERTEX_

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aMultiTexCoord0;

// Model data (to pass to fragment shader)
out VertexData {
	vec3 position;
	vec3 normal;
	vec2 textureCoord0;
} v_out;

void main() {
	v_out.position = (uModelViewMatrix * vec4(aPosition, 1)).xyz;
	v_out.normal = normalize((uModelViewMatrix * vec4(aNormal, 0)).xyz);
	v_out.textureCoord0 = aMultiTexCoord0;
	gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1);
}

#endif



#ifdef _FRAGMENT_

// Viewspace data (passed in from the vertex shader)
in VertexData {
	vec3 position;
	vec3 normal;
	vec2 textureCoord0;
} f_in;

out vec3 color;

void main() {
	vec3 eye = normalize(-f_in.position);
	color = abs(dot(normalize(f_in.normal), eye)) * vec3(0.7) + vec3(0.2);
}

#endif
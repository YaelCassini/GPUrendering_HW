uniform vec4 lightPos;
varying vec3 normal;

void main(){
	
	vec4 vert = gl_ModelViewMatrix * gl_Vertex;
	normal = gl_NormalMatrix * gl_Normal;

	vec4 offset = vec4(normal.xy * 0.1,0.0,0.0);
	offset = normalize(offset);
	gl_Position = (gl_ModelViewProjectionMatrix * gl_Vertex + offset*0.2);
}
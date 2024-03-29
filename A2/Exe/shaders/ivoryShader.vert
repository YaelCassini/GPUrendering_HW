uniform vec4 lightPos;
varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;

void main(){
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	vec4 vert = gl_ModelViewMatrix * gl_Vertex;
	normal = gl_NormalMatrix * gl_Normal;
	lightVec = vec3(lightPos - vert);
	viewVec = -vec3(vert);
}
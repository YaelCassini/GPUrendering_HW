uniform vec4 lightPos;
varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;

void main(){
	vec4 vert = gl_ModelViewMatrix * gl_Vertex;
	normal = gl_NormalMatrix * gl_Normal;
	lightVec = vec3(lightPos - vert);
	viewVec = - vec3(vert);
	/*vec3 norm = normalize(normal);
	vec3 L = normalize (lightVec);
	vec3 V = normalize (viewVec);
	float NdotV = 1-abs(dot(norm,V));*/
	vec4 offset = vec4(normal.xy*0.1,0.0,0.0);
	offset = normalize(offset);

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
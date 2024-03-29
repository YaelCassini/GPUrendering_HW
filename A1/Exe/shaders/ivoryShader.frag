varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;
void main(){
vec3 norm = normalize(normal);
vec3 L = normalize(lightVec);
vec3 V = normalize(viewVec);
vec3 halfAngle = normalize(L + V);
float NdotL = dot(L, norm);
float NdotH = clamp(dot(halfAngle, norm), 0.0, 1.0);
// "Half-Lambert" technique for more pleasing diffuse term
float diffuse = 0.5 * NdotL + 0.5;
float specular = pow(NdotH, 64.0);
float result = diffuse + specular;
gl_FragColor = vec4(result);
}
uniform vec3 ambient;
varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;
void main(){
const float b = 0.55;
const float y = 0.3;
const float Ka = 1.0;
const float Kd = 0.8;
const float Ks = 0.9;
vec3 specularcolor = vec3(1.0, 1.0, 1.0);
vec3 norm = normalize(normal);
vec3 L = normalize (lightVec);
vec3 V = normalize (viewVec);
vec3 halfAngle = normalize (L + V);
vec3 orange = vec3(.88,.81,.49);
vec3 purple = vec3(.58,.10,.76);
vec3 kCool = purple;
vec3 kWarm = orange;
float NdotL = dot(L, norm);
float NdotH = clamp(dot(halfAngle, norm), 0.0, 1.0);
float specular = pow(NdotH, 64.0);
float blendval = 0.5 * NdotL + 0.5;
vec3 Cgooch = mix(kWarm, kCool, blendval);
vec3 result = Ka * ambient + Kd * Cgooch + specularcolor * Ks * specular;
gl_FragColor = vec4(result, 1.0);
}
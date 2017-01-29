struct Material {
	lowp vec3 ka;
	lowp vec3 kd;
	lowp vec3 ks;
	lowp float shininess;
};

struct Light {
	lowp vec4 position;
	lowp vec3 intensity;
};

uniform Material mat;
uniform Light light;
varying mediump vec3 N;
varying highp vec3 v;

void main(void) {
	mediump vec3 n = normalize(N);
	highp vec3 L = normalize(light.position.xyz - v);
	highp vec3 E = normalize(-v);
	mediump vec3 R = normalize(reflect(-L, n));

	lowp float LdotN = dot(L, n);
	lowp float diffuse = max(LdotN, 0.0);
	lowp vec3 spec = vec3(0, 0, 0);

	if (LdotN > 0.0) {
		float RdotE = max(dot(R, E), 0.0);
		spec = light.intesity*pow(RdotE, mat.shininess);
	}
	vec3 color = light.intesity * (mat.ka + mat.kd*diffuse + mat.ks*spec);

	gl_FragColor = vec4(Color, 1.0);
}
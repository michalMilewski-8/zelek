#version 400 core
out vec4 FragColor;
in vec4 f_color;
in vec3 worldPos;
in vec3 tangent;
in vec3 binormal;
in vec3 normal;
in vec3 view;

const vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
const float kd = 0.5, ks = 0.2f, m = 100.0f;
const vec4 lightPos = vec4(1.0f, 2.0f, 0.0f, 1.0f);

void main()
{
	vec3 V = normalize(view);
	vec3 N = normalize(normal);

	vec3 col = f_color.xyz;
	
	vec3 L = normalize(lightPos.xyz - worldPos);
	vec3 H = normalize(V + L);
	col += lightColor.xyz * kd * clamp(dot(N, L), 0.0f, 1.0f);
	float nh = dot(N, H);
	nh = clamp(nh, 0.0f, 1.0f);
	nh = ks * pow(nh, m);
	col += lightColor.xyz * nh;

	FragColor = vec4(col,0.5f);

//	vec4 color = f_color;
//	color.w = 0.5f;
//    FragColor = color;
}
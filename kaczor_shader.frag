#version 400 core
out vec4 FragColor;
in vec3 worldPos;
in vec3 normal;
in vec3 view;
in vec2 tex;
in vec3 acolor;

const vec3 lightColor = vec3(1, 1, 1);
const float kd = 0.5f, ks = 0.2f, m = 1.0f;
const vec4 lightPos = vec4(0.0f, 0.0f, 10.0f, 1.0f);

uniform sampler2D wood_texture;

void main()
{
//	vec3 V = normalize(view);
//	vec3 N = normalize(normal);
//
//	vec4 colorr = texture(wood_texture, tex);
//	vec3 col = colorr.xyz;
//	
//	vec3 L = normalize(lightPos.xyz - worldPos);
//	vec3 H = normalize(V + L);
//	col += lightColor.xyz * kd * clamp(dot(N, L), 0.0f, 1.0f);
//	float nh = dot(N, H);
//	nh = clamp(nh, 0.0f, 1.0f);
//	nh = ks * pow(nh, m);
//	col += lightColor.xyz * nh;
//
//	FragColor = vec4(col,colorr.w);

    vec3 objectColor = texture(wood_texture, tex).xyz;
	  // ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    float diffStrength = 0.3;
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos.xyz - worldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffStrength * diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(view);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 12);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
//    FragColor = vec4(acolor, 1.0);

}
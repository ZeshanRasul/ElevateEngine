#version 460 core

in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirLight dirLight;

vec3 CalcDirLight(DirLight light, vec3 normal);

uniform vec3 objectColor;

uniform sampler2D tex;

void main() {
  vec3 norm = normalize(vNormal);

	vec3 result = CalcDirLight(dirLight, norm);

	FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal)
{
	vec3 lightDir = normalize(-light.direction);

	float diff = max(dot(normal, lightDir), 0.0);

 vec3 texColor = texture(tex, vTexCoord).rgb;
	vec3 ambient = light.ambient * texColor;
	vec3 diffuse = light.diffuse * diff * texColor;

	return (ambient + diffuse);
}

precision mediump float;

struct Material
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
};

struct Light
{
	vec3 Position;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 vFragPos;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec3 viewPosition;

uniform Material material;
uniform Light light;

void main()
{
	vec3 ambient = light.Ambient * material.Ambient;

	vec4 textureColor = mix(texture2D(texture0, vTexCoord), texture2D(texture1, vTexCoord), 0.2);

	vec3 normal = normalize(vNormal);
	vec3 lightDir = normalize(light.Position - vFragPos);

	vec3 viewDir = normalize(viewPosition - vFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
	vec3 specular = material.Specular * spec * light.Specular;

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = material.Diffuse * diff * light.Diffuse;

	vec3 result = ambient + diffuse + specular;

	gl_FragColor = vec4(result, 1.0);
}

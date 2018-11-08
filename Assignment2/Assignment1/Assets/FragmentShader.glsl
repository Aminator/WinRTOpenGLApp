precision mediump float;

struct Material
{
	sampler2D Diffuse;
	sampler2D Specular;
	float Shininess;
};

struct DirectionalLight
{
	vec3 Direction;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

struct PointLight
{
	vec3 Position;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;

	float Constant;
	float Linear;
	float Quadratic;
};

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 vFragPos;

uniform vec3 viewPosition;

uniform Material material;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[4];

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);

void main()
{
	vec3 normal = normalize(vNormal);
	vec3 viewDirection = normalize(viewPosition - vFragPos);

	vec3 outputColor = CalculateDirectionalLight(directionalLight, normal, viewDirection);

	for (int i = 0; i < 4; i++)
	{
		outputColor += CalculatePointLight(pointLights[i], normal, vFragPos, viewDirection);
	}

	gl_FragColor = vec4(outputColor, 1.0);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
	vec3 diffuseColor = vec3(texture2D(material.Diffuse, vTexCoord));
	vec3 specularColor = vec3(texture2D(material.Specular, vTexCoord));

	vec3 lightDirection = normalize(-light.Direction);
	vec3 reflectDirection = reflect(-lightDirection, normal);

	vec3 ambient = diffuseColor * light.Ambient;

	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = diffuseColor * diff * light.Diffuse;

	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.Shininess);
	vec3 specular = specularColor * spec * light.Specular;

	return ambient + diffuse + specular;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection)
{
	vec3 diffuseColor = vec3(texture2D(material.Diffuse, vTexCoord));
	vec3 specularColor = vec3(texture2D(material.Specular, vTexCoord));

	vec3 lightDirection = normalize(light.Position - vFragPos);
	vec3 reflectDirection = reflect(-lightDirection, normal);

	vec3 ambient = diffuseColor * light.Ambient;

	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = diffuseColor * diff * light.Diffuse;

	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.Shininess);
	vec3 specular = specularColor * spec * light.Specular;

	float distance = length(light.Position - fragmentPosition);
	float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * distance * distance);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}

precision mediump float;

struct Material
{
	sampler2D Diffuse;
	sampler2D Specular;
	float Shininess;
};

struct Spotlight
{
	vec3 Position;
	vec3 Direction;
	float Cutoff;

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

varying vec4 lightClipSpacePosition;

uniform vec3 viewPosition;
uniform Material material;
uniform Spotlight spotlight;

uniform sampler2D shadowMap;

float IsInShadow(vec3 lightCoords);
vec3 CalculateSpotlight(Spotlight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection, float shadow);

void main()
{
	vec3 lightCoords = lightClipSpacePosition.xyz / lightClipSpacePosition.w;
	lightCoords = lightCoords * 0.5 + 0.5;

	vec3 normal = normalize(vNormal);
	vec3 viewDirection = normalize(viewPosition - vFragPos);

	float shadow = IsInShadow(lightCoords);

	vec3 outputColor = CalculateSpotlight(spotlight, normal, vFragPos, viewDirection, shadow);

	gl_FragColor = vec4(outputColor, 1.0);
}

float IsInShadow(vec3 lightCoords)
{
	if (lightCoords.x > 0.0 && lightCoords.x < 1.0 && lightCoords.y > 0.0 && lightCoords.y < 1.0)
	{
		if (texture2D(shadowMap, lightCoords.xy).r + 0.001 < lightCoords.z)
		{
			return 1.0;
		}
	}
	else
	{
		return 1.0;
	}

	return 0.0;
}

vec3 CalculateSpotlight(Spotlight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection, float shadow)
{
	vec3 diffuseColor = vec3(texture2D(material.Diffuse, vTexCoord));
	vec3 specularColor = vec3(texture2D(material.Specular, vTexCoord));

	vec3 lightDirection = normalize(light.Position - vFragPos);

	float theta = dot(lightDirection, normalize(-light.Direction));

	vec3 ambient = diffuseColor * light.Ambient;

	vec3 reflectDirection = reflect(-lightDirection, normal);

	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = diffuseColor * diff * light.Diffuse;

	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.Shininess);
	vec3 specular = specularColor * spec * light.Specular;

	float distance = length(light.Position - fragmentPosition);
	float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * distance * distance);

	if (theta > light.Cutoff)
	{
		//ambient *= attenuation;
		//diffuse *= attenuation;
		//specular *= attenuation;

		return ambient + (1.0 - shadow) * (diffuse + specular);
	}
	else
	{
		return ambient;
	}
}

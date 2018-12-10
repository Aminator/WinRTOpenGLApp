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

uniform float constantBias;
uniform float slopeScale;

uniform sampler2D shadowMap;
uniform sampler2D lightTexture;

uniform float filterRadius;
uniform int samples;

float IsInShadow(vec3 lightCoords, vec3 normal, Spotlight light);
vec3 CalculateSpotlight(Spotlight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection, float shadow);

float Rand(in vec2 co)
{
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec3 lightCoords = lightClipSpacePosition.xyz / lightClipSpacePosition.w;
	lightCoords = lightCoords * 0.5 + 0.5;

	Spotlight light = spotlight;
	light.Diffuse = texture2D(lightTexture, lightCoords.xy).rgb;

	vec3 normal = normalize(vNormal);
	vec3 viewDirection = normalize(viewPosition - vFragPos);

	float shadow = IsInShadow(lightCoords, normal, light);

	vec3 outputColor = CalculateSpotlight(light, normal, vFragPos, viewDirection, shadow);

	gl_FragColor = vec4(outputColor, 1.0);
}

float IsInShadow(vec3 lightCoords, vec3 normal, Spotlight light)
{
	vec2 poissonDisk[64];

	poissonDisk[0] = vec2(-0.613392, 0.617481);
	poissonDisk[1] = vec2(0.170019, -0.040254);
	poissonDisk[2] = vec2(-0.299417, 0.791925);
	poissonDisk[3] = vec2(0.645680, 0.493210);
	poissonDisk[4] = vec2(-0.651784, 0.717887);
	poissonDisk[5] = vec2(0.421003, 0.027070);
	poissonDisk[6] = vec2(-0.817194, -0.271096);
	poissonDisk[7] = vec2(-0.705374, -0.668203);
	poissonDisk[8] = vec2(0.977050, -0.108615);
	poissonDisk[9] = vec2(0.063326, 0.142369);
	poissonDisk[10] = vec2(0.203528, 0.214331);
	poissonDisk[11] = vec2(-0.667531, 0.326090);
	poissonDisk[12] = vec2(-0.098422, -0.295755);
	poissonDisk[13] = vec2(-0.885922, 0.215369);
	poissonDisk[14] = vec2(0.566637, 0.605213);
	poissonDisk[15] = vec2(0.039766, -0.396100);
	poissonDisk[16] = vec2(0.751946, 0.453352);
	poissonDisk[17] = vec2(0.078707, -0.715323);
	poissonDisk[18] = vec2(-0.075838, -0.529344);
	poissonDisk[19] = vec2(0.724479, -0.580798);
	poissonDisk[20] = vec2(0.222999, -0.215125);
	poissonDisk[21] = vec2(-0.467574, -0.405438);
	poissonDisk[22] = vec2(-0.248268, -0.814753);
	poissonDisk[23] = vec2(0.354411, -0.887570);
	poissonDisk[24] = vec2(0.175817, 0.382366);
	poissonDisk[25] = vec2(0.487472, -0.063082);
	poissonDisk[26] = vec2(-0.084078, 0.898312);
	poissonDisk[27] = vec2(0.488876, -0.783441);
	poissonDisk[28] = vec2(0.470016, 0.217933);
	poissonDisk[29] = vec2(-0.696890, -0.549791);
	poissonDisk[30] = vec2(-0.149693, 0.605762);
	poissonDisk[31] = vec2(0.034211, 0.979980);
	poissonDisk[32] = vec2(0.503098, -0.308878);
	poissonDisk[33] = vec2(-0.016205, -0.872921);
	poissonDisk[34] = vec2(0.385784, -0.393902);
	poissonDisk[35] = vec2(-0.146886, -0.859249);
	poissonDisk[36] = vec2(0.643361, 0.164098);
	poissonDisk[37] = vec2(0.634388, -0.049471);
	poissonDisk[38] = vec2(-0.688894, 0.007843);
	poissonDisk[39] = vec2(0.464034, -0.188818);
	poissonDisk[40] = vec2(-0.440840, 0.137486);
	poissonDisk[41] = vec2(0.364483, 0.511704);
	poissonDisk[42] = vec2(0.034028, 0.325968);
	poissonDisk[43] = vec2(0.099094, -0.308023);
	poissonDisk[44] = vec2(0.693960, -0.366253);
	poissonDisk[45] = vec2(0.678884, -0.204688);
	poissonDisk[46] = vec2(0.001801, 0.780328);
	poissonDisk[47] = vec2(0.145177, -0.898984);
	poissonDisk[48] = vec2(0.062655, -0.611866);
	poissonDisk[49] = vec2(0.315226, -0.604297);
	poissonDisk[50] = vec2(-0.780145, 0.486251);
	poissonDisk[51] = vec2(-0.371868, 0.882138);
	poissonDisk[52] = vec2(0.200476, 0.494430);
	poissonDisk[53] = vec2(-0.494552, -0.711051);
	poissonDisk[54] = vec2(0.612476, 0.705252);
	poissonDisk[55] = vec2(-0.578845, -0.768792);
	poissonDisk[56] = vec2(-0.772454, -0.090976);
	poissonDisk[57] = vec2(0.504440, 0.372295);
	poissonDisk[58] = vec2(0.155736, 0.065157);
	poissonDisk[59] = vec2(0.391522, 0.849605);
	poissonDisk[60] = vec2(-0.620106, -0.328104);
	poissonDisk[61] = vec2(0.789239, -0.419965);
	poissonDisk[62] = vec2(-0.545396, 0.538133);
	poissonDisk[63] = vec2(-0.178564, -0.596057);

	float shadow = 0.0;

	vec3 lightDirection = normalize(light.Position - vFragPos);

	if (lightCoords.x > 0.0 && lightCoords.x < 1.0 && lightCoords.y > 0.0 && lightCoords.y < 1.0)
	{
		float maxBias = 0.01;
		float slopeScaleBias = slopeScale * max(tan(acos(dot(normal, lightDirection))), maxBias);

		float randomAngle = Rand(gl_FragCoord.xy);

		int width = int(sqrt(float(samples)));

		for (int y = -width / 2; y <= width / 2; y++)
		{
			for (int x = -width / 2; x <= width / 2; x++)
			{
				vec2 poissonValue = poissonDisk[(y + width / 2) * 8 + (x + width / 2)];

				vec2 rotatedValue = vec2(0.0);
				rotatedValue.x = poissonValue.x * cos(randomAngle) - poissonValue.y * sin(randomAngle);
				rotatedValue.y = poissonValue.x * sin(randomAngle) + poissonValue.y * cos(randomAngle);

				rotatedValue *= filterRadius;

				float texelSize = 1.0 / 512.0;
				float pcfDepth = texture2D(shadowMap, lightCoords.xy + (vec2(x, y) + rotatedValue) * texelSize).r;
				shadow += lightCoords.z - (slopeScaleBias + constantBias) > pcfDepth ? 1.0 : 0.0;
			}
		}

		shadow /= float(samples);
	}
	else
	{
		shadow = 1.0;
	}

	return shadow;
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

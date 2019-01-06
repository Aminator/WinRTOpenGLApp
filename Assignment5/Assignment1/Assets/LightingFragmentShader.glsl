precision mediump float;

varying vec2 uv;

uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light
{
	vec3 Position;
	vec3 Color;
};

const int NumLights = 4;
uniform Light lights[NumLights];
uniform vec3 viewPosition;

uniform	mat4 viewMatrixInv;
uniform	mat4 projectionMatrixInv;

vec3 WorldPositionFromDepth(float depth)
{
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = projectionMatrixInv * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = viewMatrixInv * viewSpacePosition;

    return worldSpacePosition.xyz;
}

void main()
{
	float depth = texture2D(gDepth, uv).r;
	vec3 fragPos = WorldPositionFromDepth(depth);

	vec3 normal = texture2D(gNormal, uv).rgb;
	vec3 albedo = texture2D(gAlbedoSpec, uv).rgb;
	float specular = texture2D(gAlbedoSpec, uv).a;

	vec3 lighting = albedo * 0.1;
	vec3 viewDir = normalize(viewPosition - fragPos);

	for (int i = 0; i < NumLights; i++)
	{
		vec3 lightDirection = normalize(lights[i].Position - fragPos);
		vec3 diffuse = max(dot(normal, lightDirection), 0.0) * albedo * lights[i].Color;
		lighting += diffuse;
	}

	gl_FragColor = vec4(lighting, 1.0);
}

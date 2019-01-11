precision mediump float;

uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

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

uniform PointLight light;
uniform vec3 viewPosition;

uniform	mat4 viewMatrixInv;
uniform	mat4 projectionMatrixInv;

uniform vec2 viewPortSize;

vec3 WorldPositionFromDepth(float depth, vec2 uv)
{
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = projectionMatrixInv * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = viewMatrixInv * viewSpacePosition;

    return worldSpacePosition.xyz;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection, vec3 albedo, vec3 specularColor)
{
    vec3 diffuseColor = albedo;

    vec3 lightDirection = normalize(light.Position - fragmentPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    vec3 ambient = diffuseColor * light.Ambient;

    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diffuseColor * diff * light.Diffuse;

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    vec3 specular = specularColor * spec * light.Specular;

    float distance = length(light.Position - fragmentPosition);
    float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / viewPortSize;

    float depth = texture2D(gDepth, uv).r;
    vec3 fragPos = WorldPositionFromDepth(depth, uv);

    vec3 normal = texture2D(gNormal, uv).rgb;
    vec3 albedo = texture2D(gAlbedoSpec, uv).rgb;
    float specular = texture2D(gAlbedoSpec, uv).a;

    vec3 viewDirection = normalize(viewPosition - fragPos);

    vec3 lighting = CalculatePointLight(light, normal, fragPos, viewDirection, albedo, vec3(specular));

    gl_FragColor = vec4(lighting, 1.0);
}

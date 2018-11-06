precision mediump float;

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 vFragPos;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;

void main()
{
	float specularStrength = 0.5;
	vec3 ambient = vec3(1.0, 1.0, 1.0);
	vec4 textureColor = mix(texture2D(texture0, vTexCoord), texture2D(texture1, vTexCoord), 0.2);
	textureColor = vec4(0.0, 0.5, 0.5, 1.0);

	vec3 normal = normalize(vNormal);
	vec3 lightDir = normalize(lightPos - vFragPos);

	vec3 viewDir = normalize(viewPosition - vFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256.0);
	vec3 specular = specularStrength * spec * lightColor;

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec4 result = vec4((ambient + diffuse + specular), 1.0) * textureColor;

	gl_FragColor = result;
}

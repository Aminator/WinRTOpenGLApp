precision mediump float;

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 vLightPos;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec3 lightPos;

void main()
{
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 ambient = vec3(1.0, 1.0, 1.0);
	vec4 textureColor = mix(texture2D(texture0, vTexCoord), texture2D(texture1, vTexCoord), 0.2);

	vec3 normal = normalize(vNormal);
	vec3 lightDir = normalize(lightPos - vLightPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	vec4 result = vec4((ambient + diffuse), 1.0) * textureColor;

	gl_FragColor = result;
}

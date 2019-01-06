precision mediump float;

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 vFragPos;

struct Material
{
	sampler2D Diffuse;
	sampler2D Specular;
	float Shininess;
};

uniform int bufferType;

uniform Material material;

void main()
{
	if (bufferType == 1)
	{
		gl_FragColor = vec4(normalize(vNormal), 0);
	}
	else if (bufferType == 2)
	{
		vec4 gAlbedoSpec;
		
		gAlbedoSpec.rgb = texture2D(material.Diffuse, vTexCoord).rgb;
		gAlbedoSpec.a = texture2D(material.Specular, vTexCoord).r;

		gl_FragColor = gAlbedoSpec;
	}
}

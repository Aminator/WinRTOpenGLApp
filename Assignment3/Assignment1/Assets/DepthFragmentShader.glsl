precision mediump float;

uniform sampler2D depthTexture;
uniform float zNear;
uniform float zFar;

varying vec2 uv;

void main()
{
	float z = texture2D( depthTexture, uv ).r;
	float linearZ = (2.0 * zNear) / (zFar + zNear - z * (zFar - zNear));

	if (true)
	{
		gl_FragColor = vec4(vec3(linearZ), 1.0);
	}
	else
	{
		gl_FragColor = vec4(vec3( z ), 1.0);
	}
}

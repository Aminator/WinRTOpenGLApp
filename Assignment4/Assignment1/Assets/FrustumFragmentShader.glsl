precision mediump float;

uniform vec3 frustumColor;

void main()
{
	gl_FragColor = vec4(frustumColor, 1.0);
}

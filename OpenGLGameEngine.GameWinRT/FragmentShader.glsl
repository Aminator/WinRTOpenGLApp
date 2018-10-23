precision mediump float;

varying vec2 vTexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
	gl_FragColor = mix(texture2D(texture0, vTexCoord), texture2D(texture1, vTexCoord), 0.2);
}

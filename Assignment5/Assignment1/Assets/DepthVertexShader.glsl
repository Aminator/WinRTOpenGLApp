attribute vec2 vertexPos;
attribute vec2 texCoord;

varying vec2 uv;

void main()
{
	uv = texCoord;
    gl_Position = vec4(vertexPos, 0.0, 1.0);
}

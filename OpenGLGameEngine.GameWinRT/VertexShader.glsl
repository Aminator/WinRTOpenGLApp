attribute vec3 vertexPos;
attribute vec2 texCoord;

varying vec2 vTexCoord;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(vertexPos, 1.0);
	vTexCoord = texCoord;
}

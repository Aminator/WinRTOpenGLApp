attribute vec3 vertexPos;
attribute vec2 texCoord;
attribute vec3 normal;

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 vFragPos;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(vertexPos, 1.0);
	vTexCoord = texCoord;
	vNormal = normal;
	vFragPos = vec3(worldMatrix * vec4(vertexPos, 1.0));
}

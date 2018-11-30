attribute vec3 vertexPos;
attribute vec2 texCoord;
attribute vec3 normal;

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 vFragPos;

varying vec4 lightClipSpacePosition;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat4 lightViewMatrix;
uniform mat4 lightProjectionMatrix;

void main()
{
    lightClipSpacePosition = lightProjectionMatrix * lightViewMatrix * worldMatrix * vec4(vertexPos, 1.0);
    gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(vertexPos, 1.0);
	vTexCoord = texCoord;
	vNormal = normal;
	vFragPos = vec3(worldMatrix * vec4(vertexPos, 1.0));
}

attribute vec3 vertexPos;

uniform mat4 lightND2worldTf;
uniform mat4 world2cameraClipTf;

void main()
{
	vec4 worldPosition = lightND2worldTf * vec4(vertexPos, 1.0);
	vec3 vertexPosition = worldPosition.xyz / worldPosition.w;

    gl_Position = world2cameraClipTf * vec4(vertexPosition, 1.0);
}

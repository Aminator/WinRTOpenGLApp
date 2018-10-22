attribute vec3 vertexPos;
attribute vec4 vertexColor;

uniform float time;

varying vec4 vColor;

void main()
{
    gl_Position = vec4(vertexPos, 1.0);
    vColor = vec4(sin(time), vertexColor.y, vertexColor.z, 1.0);
    vColor = vertexColor;
}

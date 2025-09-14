#version 460

layout (location = 0) in vec3 apos;

void main()
{
    gl_Position = vec4(apos.x, apos.y, apos.z, 1.0f);
}
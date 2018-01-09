R"(#version 330

uniform sampler2D Diffuse;
in vec2 fUV;
out vec4 outDiffuse;
void main()
{
    outDiffuse = vec4(texture2D(Diffuse, fUV).xyz, 1.0);
}

)"
R"(#version 330

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

in vec3 Position;
in vec2 UV;
out vec2 fUV;
void main()
{
    fUV = UV;
    gl_Position = projection * view * model * vec4(Position, 1.0);
}

)"
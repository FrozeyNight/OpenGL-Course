#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord; // this sends data to the fragment shader

uniform mat4 u_MVP; // MVP - model view projection matrix (not exactly, will learn in later videos)

void main()
{
   gl_Position = u_MVP * position;
   v_TexCoord = texCoord;
};

#shader fragment
#version 330 core

 layout(location = 0) out vec4 color;

 in vec2 v_TexCoord; // this receives the data from the vertex shader

 uniform vec4 u_Color; // u_ means that it's a uniform (comes from the CPU)
 uniform sampler2D u_Texture;

void main()
{
    //color = vec4(0.2, 0.3, 0.8, 1.0);
    vec4 texColor = texture(u_Texture, v_TexCoord); // the first param means the slot, 
    // the second is the texture coordinate of the pixel we want to take the color from, that we want to draw on our object

    //color = u_Color;
    color = texColor;
};
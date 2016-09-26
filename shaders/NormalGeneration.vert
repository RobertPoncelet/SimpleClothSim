#version 330 core
/// @file NormalGeneration.vert
/// @brief Calculates a normal for rendering to a framebuffer.
/// @author Robert Poncelet
/// @version 1.0
/// @date 23/03/15

/// @brief[in] The vertex position passed in.
layout (location = 0) in vec3 inVert;
/// @brief[in,out] The texture co-ordinate passed in.
layout (location = 1) in vec2 inUV;
/// @brief[in] The texture containing vertex positions.
uniform samplerBuffer vertPositions;

/// @brief[out] The same texture co-ordinate.
out vec2 UV;

void main()
{
    gl_Position = vec4(inVert,1.0);
    UV = inUV;
}

#version 330 core
/// @file NormalGeneration.frag
/// @brief Calculates a normal for rendering to a framebuffer.
/// @author Robert Poncelet
/// @version 1.0
/// @date 23/03/15

/// @brief[in] The texture co-ordinate.
in vec2 UV;
/// @brief[in] The texture containing vertex positions.
uniform samplerBuffer vertPositions;
/// @brief[in] The width of the texture.
uniform int widthNum;
/// @brief[in] The height of the texture.
uniform int heightNum;

/// @brief[out] The final fragment colour.
layout (location = 0) out vec4 outColour;

/// @brief Converts a pair of integer co-ordinates to a single int index and returns the position at that index in the texture.
vec3 positionAt(int x, int y)
{
    x = clamp(x, 0, widthNum-1);
    y = clamp(y, 0, heightNum-1);
    int index = x + (y * widthNum);
    return vec3(texelFetch(vertPositions, index));
}

void main(void)
{
    int x = int(UV.x * float(widthNum));
    int y = int(UV.y * float(heightNum));

    vec3 thisPos        = positionAt(x  ,y  );

    vec3 upVector       = positionAt(x  ,y-1) - thisPos;
    vec3 downVector     = positionAt(x  ,y+1) - thisPos;
    vec3 leftVector     = positionAt(x-1,y  ) - thisPos;
    vec3 rightVector    = positionAt(x+1,y  ) - thisPos;

    vec3 normal_NE      = cross(upVector, rightVector);
    vec3 normal_NW      = cross(leftVector, upVector);
    vec3 normal_SW      = cross(downVector, leftVector);
    vec3 normal_SE      = cross(rightVector, downVector);

    vec3 thisNormal = normalize(normal_NE + normal_NW + normal_SE + normal_SW);
    //no need to normalize since this is done on the frag shader anyway
    //normalize(thisNormal);

    outColour = vec4(thisNormal,1.f);
}

#version 330 core
/// @file TextureVert.glsl
/// @brief A modified phong shader used for rendering a textured sheet of cloth with holes.
/// @author Jon Macey (modified by Robert Poncelet)
/// @version 1.0
/// @date 23/03/15

/// @brief MVP passed from app
uniform mat4 MVP;
// first attribute the vertex values from our VAO
layout (location=0) in vec3 inVert;
//layout (location=0) in int widthNum;
// second attribute the UV values from our VAO
layout (location=1)in float inIndex;
//layout (location=1)in int heightNum;
// normals for lighting
//layout (location = 2) in vec3 inNormal;
// we use this to pass the UV values to the frag shader
out vec2 vertUV;
out vec3 vertPos;

/// @brief flag to indicate if model has unit normals if not normalize
uniform bool Normalize;
// the eye position of the camera
uniform vec3 viewerPos;
/// @brief the current fragment normal for the vert being processed
out vec3 fragmentNormal;

/// @brief[in] texture containing vertex positions
uniform samplerBuffer vertPositions;
/// @brief[in] texture containing vertex normals
uniform sampler2D vertNormals;

struct Lights
{
  vec4 position;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float constantAttenuation;
  float spotCosCutoff;
  float quadraticAttenuation;
  float linearAttenuation;
};

// array of lights
uniform Lights light;
// direction of the lights used for shading
out vec3 lightDir;
// out the blinn half vector
out vec3 halfVector;
out vec3 eyeDirection;
out vec3 vPosition;

uniform mat4 MV;
//uniform mat4 MVP;
uniform mat3 normalMatrix;
uniform mat4 M;

uniform int widthNum;
uniform int heightNum;

vec3 positionAt(int x, int y)
{
    x = clamp(x, 0, widthNum-1);
    y = clamp(y, 0, heightNum-1);
    int index = x + (y * widthNum);
    return vec3(texelFetch(vertPositions, index));
}

void main()
{
    int index = int(inIndex);

    vertPos = texelFetch(vertPositions, index).rgb;

    // calculate the vertex position
    gl_Position = MVP*vec4(vertPos,1.0);

    vec4 worldPosition = M * vec4(vertPos, 1.0);
    eyeDirection = normalize(viewerPos - worldPosition.xyz);
    // Get vertex position in eye coordinates
    // Transform the vertex to eye co-ordinates for frag shader
    /// @brief the vertex in eye co-ordinates  homogeneous
    vec4 eyeCord=MV*vec4(vertPos,1);

    vPosition = eyeCord.xyz / eyeCord.w;;

    float dist;

    lightDir=vec3(light.position.xyz-eyeCord.xyz);
    dist = length(lightDir);
    lightDir/= dist;
    halfVector = normalize(eyeDirection + lightDir);

    //calculate normals here for now because OpenGL is refusing to render to framebuffers
        int x = index % widthNum;
        int y = index / widthNum;

        vec3 thisPos        = positionAt(x  ,y  );

        vec3 upVector       = positionAt(x  ,y-1) - thisPos;
        vec3 downVector     = positionAt(x  ,y+1) - thisPos;
        vec3 leftVector     = positionAt(x-1,y  ) - thisPos;
        vec3 rightVector    = positionAt(x+1,y  ) - thisPos;

        vec3 normal_NE      = cross(upVector, rightVector);
        vec3 normal_NW      = cross(leftVector, upVector);
        vec3 normal_SW      = cross(downVector, leftVector);
        vec3 normal_SE      = cross(rightVector, downVector);

        fragmentNormal = normalMatrix * (normal_NE + normal_NW + normal_SE + normal_SW);
        //normalization is done on the frag shader anyway, no need to do it here
        //normalize(thisNormal);

    // pass the UV values to the frag shader
    vertUV=vec2(float(x)/float(widthNum-1), float(y)/float(heightNum-1));
}

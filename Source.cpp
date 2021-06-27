#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnOpengl/camera.h> // Camera class


using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Final Project"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint tableVAO;         // Handle for the vertex array object
        GLuint planeVAO;         
        GLuint carpetVAO;
        GLuint windowVAO;
        GLuint teacupVAO;
        GLuint saucerVAO;
        GLuint tableVBO;         // Handle for the vertex buffer object
        GLuint planeVBO;         
        GLuint carpetVBO;
        GLuint windowVBO;
        GLuint teacupVBO;
        GLuint saucerVBO;
        GLuint tableVertices;    // Number of indices of the mesh
        GLuint planeVertices;   
        GLuint carpetVertices;
        GLuint windowVertices;
        GLuint teacupVertices;
        GLuint saucerVertices;
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMesh;
    // Texture
    GLuint gTableTextureId;
    GLuint gCarpetTextureId;
    GLuint gCeramicTextureId;
    GLuint gPlaneTextureId;
    glm::vec2 gUVScale(1.0f, 1.0f);
    GLint gTexWrapMode = GL_REPEAT;

    // Shader programs
    GLuint gTableProgramId;
    GLuint gCarpetProgramId;
    GLuint gLampProgramId;
    GLuint gCeramicProgramId;
    GLuint gPlaneProgramId;

    // Camera
    Camera gCamera(glm::vec3(0.0f, 1.0f, 5.0f));
    float  gLastX = WINDOW_WIDTH / 2.0f;
    float  gLastY = WINDOW_HEIGHT / 2.0f;
    bool   gFirstMouse = true;
    bool   ortho = false;

    // Timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    // Table position and scale
    glm::vec3 gTablePosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gTableScale(2.0f);
    // Carpet position and scale
    glm::vec3 gCarpetPosition(0.0f, 0.1f, 0.0f);
    glm::vec3 gCarpetScale(2.0f);
    // Teacup position and scale
    glm::vec3 gTeacupPosition(0.0f, -0.50f, 0.0f);
    glm::vec3 gTeacupScale(0.5f);
    // Saucer position and scale
    glm::vec3 gSaucerPosition(0.0f, -0.50f, 0.0f);
    glm::vec3 gSaucerScale(0.5f);
    // Object and light color
    glm::vec3 gObjectColor(0.5f, 0.5f, 0.5f);
    glm::vec3 gWindowLightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 gLampLightColor(0.8f, 0.2f, 0.2f);
    // Light position and scale
    glm::vec3 gWindowLightPosition(0.0f, 0.5f, 0.0f);
    glm::vec3 gLampLightPosition(0.0f, 0.5f, 20.0f);
    glm::vec3 gLightScale(1.0f);
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
int  UCreateTexturePrograms();
void USetShaderProgram(GLuint programId, glm::vec3 gPos, glm::vec3 gScale);

// Carpet
//-----------------------------------
/* Carpet Vertex Shader Source Code*/
const GLchar* carpetVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);

/* Carpet Fragment Shader Source Code*/
const GLchar* carpetFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 fillLightColor;
uniform vec3 fillLightPos;
uniform vec3 keyLightColor;
uniform vec3 keyLightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/
    //Calculate Key Lamp Ambient lighting*/
    float ambientStrength = 1.0f; // Set ambient or global lighting strength
    vec3 keyAmbient = ambientStrength * keyLightColor; // Generate ambient light color
    //Calculate Key Lamp Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(keyLightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.1);// Calculate diffuse impact by generating dot product of normal and light
    vec3 keyDiffuse = impact * keyLightColor; // Generate diffuse light color
    // Calculate Key Lamp Specular lighting
    float specularIntensity = 5.0f; // Set specular light strength
    float highlightSize = 16.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm); // Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 keySpecular = specularIntensity * specularComponent * keyLightColor;
    //Calculate Fill Lamp Ambient lighting*/
    ambientStrength = 1.0f; // Set ambient or global lighting strength
    vec3 fillAmbient = ambientStrength * fillLightColor; // Generate ambient light color
    //Calculate Fill Lamp Diffuse lighting*/
    norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    lightDirection = normalize(fillLightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    impact = max(dot(norm, lightDirection), 0.1);// Calculate diffuse impact by generating dot product of normal and light
    vec3 fillDiffuse = impact * fillLightColor; // Generate diffuse light color
    // Calculate Fill Lamp Specular lighting
    specularIntensity = 5.0f; // Set specular light strength
    highlightSize = 16.0f; // Set specular highlight size
    viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    reflectDir = reflect(-lightDirection, norm); // Calculate reflection vector
    //Calculate specular component
    specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 fillSpecular = specularIntensity * specularComponent * fillLightColor;

    // Texture holds the color to be used for all three components
    vec3 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale).xyz;

    // Calculate phong result
    vec3 fillResult = (fillAmbient + fillDiffuse + fillSpecular);
    vec3 keyResult = (keyAmbient + keyDiffuse + keySpecular);
    vec3 lightingResult = fillResult + keyResult;
    vec3 phong = lightingResult * textureColor;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

// Plane
//-----------------------------------
/* Floor Vertex Shader Source Code*/
const GLchar* planeVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);

/* Floor Fragment Shader Source Code*/
const GLchar* planeFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 fillLightColor;
uniform vec3 fillLightPos;
uniform vec3 keyLightColor;
uniform vec3 keyLightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Key Lamp Ambient lighting*/
    float ambientStrength = 1.0f; // Set ambient or global lighting strength
    vec3 keyAmbient = ambientStrength * keyLightColor; // Generate ambient light color

    //Calculate Key Lamp Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(keyLightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.1);// Calculate diffuse impact by generating dot product of normal and light
    vec3 keyDiffuse = impact * keyLightColor; // Generate diffuse light color

    // Calculate Key Lamp Specular lighting
    float specularIntensity = 5.0f; // Set specular light strength
    float highlightSize = 16.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm); // Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 keySpecular = specularIntensity * specularComponent * keyLightColor;



    //Calculate Fill Lamp Ambient lighting*/
    ambientStrength = 1.0f; // Set ambient or global lighting strength
    vec3 fillAmbient = ambientStrength * fillLightColor; // Generate ambient light color

    //Calculate Fill Lamp Diffuse lighting*/
    norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    lightDirection = normalize(fillLightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    impact = max(dot(norm, lightDirection), 0.1);// Calculate diffuse impact by generating dot product of normal and light
    vec3 fillDiffuse = impact * fillLightColor; // Generate diffuse light color

    // Calculate Fill Lamp Specular lighting
    specularIntensity = 5.0f; // Set specular light strength
    highlightSize = 16.0f; // Set specular highlight size
    viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    reflectDir = reflect(-lightDirection, norm); // Calculate reflection vector
    //Calculate specular component
    specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 fillSpecular = specularIntensity * specularComponent * fillLightColor;

    // Texture holds the color to be used for all three components
    vec3 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale).xyz;

    // Calculate phong result
    vec3 fillResult = (fillAmbient + fillDiffuse + fillSpecular);
    vec3 keyResult = (keyAmbient + keyDiffuse + keySpecular);
    vec3 lightingResult = fillResult + keyResult;
    vec3 phong = lightingResult * textureColor;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

/* Pyramid Vertex Shader Source Code*/
const GLchar* tableVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Pyramid Fragment Shader Source Code*/
const GLchar* tableFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 fillLightColor;
uniform vec3 fillLightPos;
uniform vec3 keyLightColor;
uniform vec3 keyLightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Key Lamp Ambient lighting*/
    float ambientStrength = 1.0f; // Set ambient or global lighting strength
    vec3 keyAmbient = ambientStrength * keyLightColor; // Generate ambient light color

    //Calculate Key Lamp Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(keyLightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.1);// Calculate diffuse impact by generating dot product of normal and light
    vec3 keyDiffuse = impact * keyLightColor; // Generate diffuse light color

    // Calculate Key Lamp Specular lighting
    float specularIntensity = 5.0f; // Set specular light strength
    float highlightSize = 16.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm); // Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 keySpecular = specularIntensity * specularComponent * keyLightColor;



    //Calculate Fill Lamp Ambient lighting*/
    ambientStrength = 1.0f; // Set ambient or global lighting strength
    vec3 fillAmbient = ambientStrength * fillLightColor; // Generate ambient light color

    //Calculate Fill Lamp Diffuse lighting*/
    norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    lightDirection = normalize(fillLightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    impact = max(dot(norm, lightDirection), 0.1);// Calculate diffuse impact by generating dot product of normal and light
    vec3 fillDiffuse = impact * fillLightColor; // Generate diffuse light color

    // Calculate Fill Lamp Specular lighting
    specularIntensity = 5.0f; // Set specular light strength
    highlightSize = 16.0f; // Set specular highlight size
    viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    reflectDir = reflect(-lightDirection, norm); // Calculate reflection vector
    //Calculate specular component
    specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 fillSpecular = specularIntensity * specularComponent * fillLightColor;

    // Texture holds the color to be used for all three components
    vec3 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale).xyz;

    // Calculate phong result
    vec3 fillResult = (fillAmbient + fillDiffuse + fillSpecular);
    vec3 keyResult = (keyAmbient + keyDiffuse + keySpecular);
    vec3 lightingResult = fillResult + keyResult;
    vec3 phong = lightingResult * textureColor;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);


/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

        //Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);


/* Lamp Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource = GLSL(440,

out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

uniform vec4 lampcolor;

void main()
{
    fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);

/* Ceramic Vertex Shader Source Code*/
const GLchar* ceramicVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Teaset Fragment Shader Source Code*/
const GLchar* ceramicFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 fillLightColor;
uniform vec3 fillLightPos;
uniform vec3 keyLightColor;
uniform vec3 keyLightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Key Lamp Ambient lighting*/
    float ambientStrength = 0.5f; // Set ambient or global lighting strength
    vec3 keyAmbient = ambientStrength * keyLightColor; // Generate ambient light color

    //Calculate Key Lamp Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(keyLightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.1);// Calculate diffuse impact by generating dot product of normal and light
    vec3 keyDiffuse = impact * keyLightColor; // Generate diffuse light color

    // Calculate Key Lamp Specular lighting
    float specularIntensity = 1.0f; // Set specular light strength
    float highlightSize = 16.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm); // Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 keySpecular = specularIntensity * specularComponent * keyLightColor;



    //Calculate Fill Lamp Ambient lighting*/
    ambientStrength = 1.0f; // Set ambient or global lighting strength
    vec3 fillAmbient = ambientStrength * fillLightColor; // Generate ambient light color

    //Calculate Fill Lamp Diffuse lighting*/
    norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    lightDirection = normalize(fillLightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    impact = max(dot(norm, lightDirection), 0.1);// Calculate diffuse impact by generating dot product of normal and light
    vec3 fillDiffuse = impact * fillLightColor; // Generate diffuse light color

    // Calculate Fill Lamp Specular lighting
    specularIntensity = 5.0f; // Set specular light strength
    highlightSize = 16.0f; // Set specular highlight size
    viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    reflectDir = reflect(-lightDirection, norm); // Calculate reflection vector
    //Calculate specular component
    specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 fillSpecular = specularIntensity * specularComponent * fillLightColor;

    // Texture holds the color to be used for all three components
    vec3 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale).xyz;

    // Calculate phong result
    vec3 fillResult = (fillAmbient + fillDiffuse + fillSpecular);
    vec3 keyResult = (keyAmbient + keyDiffuse + keySpecular);
    vec3 lightingResult = fillResult + keyResult;
    vec3 phong = lightingResult * textureColor;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object

    // Create the shader programs
    if (!UCreateShaderProgram(tableVertexShaderSource, tableFragmentShaderSource, gTableProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(carpetVertexShaderSource, carpetFragmentShaderSource, gCarpetProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(planeVertexShaderSource, planeFragmentShaderSource, gPlaneProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(ceramicVertexShaderSource, ceramicFragmentShaderSource, gCeramicProgramId))
        return EXIT_FAILURE;

    UCreateTexturePrograms();

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh);

    // Release texture
    UDestroyTexture(gTableTextureId);
    UDestroyTexture(gPlaneTextureId);
    UDestroyTexture(gCarpetTextureId);
    UDestroyTexture(gCeramicTextureId);

    // Release shader programs
    UDestroyShaderProgram(gTableProgramId);
    UDestroyShaderProgram(gPlaneProgramId);
    UDestroyShaderProgram(gLampProgramId);
    UDestroyShaderProgram(gCarpetProgramId);
    UDestroyShaderProgram(gCeramicProgramId);


    exit(EXIT_SUCCESS); // Terminates the program successfully
}

// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime); 
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime); 

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && ortho != true)
        ortho = true;
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && ortho == true)
        ortho = false;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}

int UCreateTexturePrograms()
{
    // Load textures
    // Table
    //--------------
    const char* texFilename = "../resources/textures/darkwood.jpg";

    if (!UCreateTexture(texFilename, gTableTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gTableProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gTableProgramId, "uTexture"), 0);

    // Carpet
    //----------------
    texFilename = "../resources/textures/carpet.jpg";

    if (!UCreateTexture(texFilename, gCarpetTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gCarpetProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gCarpetProgramId, "uTexture"), 0);


    // Table Setting
    //-----------------
    texFilename = "../resources/textures/abstract-texture.jpg";

    if (!UCreateTexture(texFilename, gCeramicTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gCeramicProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gCeramicProgramId, "uTexture"), 0);

    // Floor
    //-----------------
    texFilename = "../resources/textures/brickwall.jpg";

    if (!UCreateTexture(texFilename, gPlaneTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gPlaneProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gPlaneProgramId, "uTexture"), 0);
};

void USetShaderProgram(GLuint programId, glm::vec3 gPos, glm::vec3 gScale)
{
    // Activate Program
    glUseProgram(programId);
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = glm::translate(gPos) * glm::scale(gScale);
    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();
    // Creates a perspective projection
    glm::mat4 projection;

    if (ortho == false)
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    else if (ortho == true)
        projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(programId, "model");
    GLint viewLoc = glGetUniformLocation(programId, "view");
    GLint projLoc = glGetUniformLocation(programId, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    GLint colorLoc = glGetUniformLocation(programId, "objectColor");
    GLint windowLightColorLoc = glGetUniformLocation(programId, "keyLightColor");
    GLint windowLightPositionLoc = glGetUniformLocation(programId, "keyLightPos");
    GLint lampLightColorLoc = glGetUniformLocation(programId, "fillLightColor");
    GLint lampLightPositionLoc = glGetUniformLocation(programId, "fillLightPos");
    GLint viewPositionLoc = glGetUniformLocation(programId, "viewPosition"); 
    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(colorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(windowLightColorLoc, gLampLightColor.r, gLampLightColor.g, gLampLightColor.b);
    glUniform3f(windowLightPositionLoc, gLampLightPosition.x, gLampLightPosition.y, gLampLightPosition.z);
    glUniform3f(lampLightColorLoc, gWindowLightColor.r, gWindowLightColor.g, gWindowLightColor.b);
    glUniform3f(lampLightPositionLoc, gWindowLightPosition.x, gWindowLightPosition.y, gWindowLightPosition.z);
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);
    GLint UVScaleLoc = glGetUniformLocation(programId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));
};

// Functioned called to render a frame
void URender()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // DRAW PLANE
    // ----------
    USetShaderProgram(gPlaneProgramId, gTablePosition, gTableScale);
    // Activate Plane VAO and set the shader to be used
    glBindVertexArray(gMesh.planeVAO);
    glUseProgram(gPlaneProgramId);
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPlaneTextureId);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.planeVertices);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    
    // DRAW CARPET
    // ----------
    USetShaderProgram(gCarpetProgramId, gCarpetPosition, gCarpetScale);
    // Activate Plane VAO and set the shader to be used
    glBindVertexArray(gMesh.carpetVAO);
    glUseProgram(gCarpetProgramId);
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gCarpetTextureId);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.carpetVertices);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    // DRAW TABLE
    // -----------
    USetShaderProgram(gTableProgramId, gTablePosition, gTableScale);
    // Activate the pyramid VAO and set the shader to be used
    glBindVertexArray(gMesh.tableVAO);
    glUseProgram(gTableProgramId);
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTableTextureId);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.tableVertices);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    // DRAW TEACUP
    //------------
    USetShaderProgram(gCeramicProgramId, gTeacupPosition, gTeacupScale);
    // Activate the pyramid VAO and set the shader to be used
    glUseProgram(gCeramicProgramId);
    glBindVertexArray(gMesh.teacupVAO);
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gCeramicTextureId);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.teacupVertices);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    // DRAW SAUCER
    //------------
    USetShaderProgram(gCeramicProgramId, gSaucerPosition, gSaucerScale);
    // Activate the pyramid VAO and set the shader to be used
    glUseProgram(gCeramicProgramId);
    glBindVertexArray(gMesh.saucerVAO);
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gCeramicTextureId);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.teacupVertices);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    // DRAW WINDOW 1
    //-------------
    USetShaderProgram(gLampProgramId, gWindowLightPosition, gTableScale);
    // Activate the pyramid VAO and set the shader to be used
    glUseProgram(gLampProgramId);
    glBindVertexArray(gMesh.windowVAO);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.windowVertices);


    // DRAW WINDOW 2
    //----------------
    USetShaderProgram(gLampProgramId, gLampLightPosition, gTableScale);
    // Activate the pyramid VAO and set the shader to be used
    glUseProgram(gLampProgramId);
    glBindVertexArray(gMesh.windowVAO);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.windowVertices);

    // Deactivate the Vertex Array Object and shader program
    glBindVertexArray(0);
    glUseProgram(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat tableVerts[] = {
        //Positions          //Normals
        // ------------------------------------------------------
        //Positions             //Texture Coordinates
        1.0f,   0.0f,  0.25f,     0.0f,  1.0f,  0.0f,   1.0f, 0.0f, // Table Top 
        1.0f,   0.0f, -0.25f,     0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
       -1.0f,   0.0f, -0.25f,     0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
       -1.0f,   0.0f,  0.25f,     0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
       -1.0f,   0.0f, -0.25f,     0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
        1.0f,   0.0f,  0.25f,     0.0f,  1.0f,  0.0f,   1.0f, 1.0f,

        1.0f,  -0.1f,  0.25f,     0.0f, -1.0f,  0.0f,   1.0f, 0.0f, // Table Top Underside
        1.0f,  -0.1f, -0.25f,     0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
       -1.0f,  -0.1f, -0.25f,     0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
       -1.0f,  -0.1f,  0.25f,     0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
       -1.0f,  -0.1f, -0.25f,     0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
        1.0f,  -0.1f,  0.25f,     0.0f, -1.0f,  0.0f,   1.0f, 1.0f,

        1.0f,   0.0f,  0.25f,     0.0f,  0.0f,  1.0f,   1.0f, 0.0f, // Table Top Side Front 
        1.0f,  -0.1f,  0.25f,     0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
       -1.0f,   0.0f,  0.25f,     0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
       -1.0f,   0.0f,  0.25f,     0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
       -1.0f,  -0.1f,  0.25f,     0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
        1.0f,  -0.1f,  0.25f,     0.0f,  0.0f,  1.0f,   0.0f, 0.0f,

        1.0f,   0.0f, -0.25f,     0.0f,  0.0f, -1.0f,   1.0f, 0.0f, // Table Top Side Back 
        1.0f,  -0.1f, -0.25f,     0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
       -1.0f,   0.0f, -0.25f,     0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
       -1.0f,   0.0f, -0.25f,     0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
       -1.0f,  -0.1f, -0.25f,     0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
        1.0f,  -0.1f, -0.25f,     0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

        1.0f,   0.0f,  0.25f,     1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // Table Top Side Right
        1.0f,   0.0f, -0.25f,     1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        1.0f,  -0.1f,  0.25f,     1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        1.0f,   0.0f, -0.25f,     1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        1.0f,  -0.1f,  0.25f,     1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        1.0f,  -0.1f, -0.25f,     1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

       -1.0f,   0.0f,  0.25f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // Table Top Side Left
       -1.0f,   0.0f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
       -1.0f,  -0.1f,  0.25f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
       -1.0f,   0.0f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
       -1.0f,  -0.1f,  0.25f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
       -1.0f,  -0.1f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

        1.00f, -0.1f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 1 Side 1
        1.00f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        1.00f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        1.00f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        1.00f, -0.9f,  0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        1.00f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        0.95f, -0.1f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 1 Side 2
        0.95f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        0.95f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        0.95f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        0.95f, -0.9f,  0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        0.95f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        1.00f, -0.1f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 1 Side 3
        0.95f, -0.1f,  0.25f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        1.00f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        1.00f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        0.95f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        0.95f, -0.1f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        1.00f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 1 Side 4
        0.95f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        1.00f, -0.9f,  0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        1.00f, -0.9f,  0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        0.95f, -0.9f,  0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        0.95f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

       -1.00f, -0.1f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 2 Side 1
       -1.00f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
       -1.00f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -1.00f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -1.00f, -0.9f,  0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
       -1.00f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

       -0.95f, -0.1f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 2 Side 2
       -0.95f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
       -0.95f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -0.95f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -0.95f, -0.9f,  0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
       -0.95f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

       -1.00f, -0.1f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 2 Side 3
       -0.95f, -0.1f,  0.25f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
       -1.00f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -1.00f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -0.95f, -0.9f,  0.25f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
       -0.95f, -0.1f,  0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

       -1.00f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 2 Side 4
       -0.95f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
       -1.00f, -0.9f,  0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -1.00f, -0.9f,  0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -0.95f, -0.9f,  0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
       -0.95f, -0.1f,  0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

       -1.00f, -0.1f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 3 Side 1
       -1.00f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
       -1.00f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -1.00f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -1.00f, -0.9f, -0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
       -1.00f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

       -0.95f, -0.1f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 3 Side 2
       -0.95f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
       -0.95f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -0.95f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -0.95f, -0.9f, -0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
       -0.95f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

       -1.00f, -0.1f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 3 Side 3
       -0.95f, -0.1f, -0.25f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
       -1.00f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -1.00f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -0.95f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
       -0.95f, -0.1f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

       -1.00f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 3 Side 4
       -0.95f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
       -1.00f, -0.9f, -0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -1.00f, -0.9f, -0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -0.95f, -0.9f, -0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
       -0.95f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        1.00f, -0.1f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 4 Side 1
        1.00f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        1.00f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        1.00f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        1.00f, -0.9f, -0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        1.00f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        0.95f, -0.1f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 4 Side 2
        0.95f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        0.95f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        0.95f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        0.95f, -0.9f, -0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        0.95f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        1.00f, -0.1f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 4 Side 3
        1.00f, -0.1f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, 
        0.95f, -0.1f, -0.25f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        1.00f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        0.95f, -0.9f, -0.25f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        0.95f, -0.1f, -0.25f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        1.00f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Leg 4 Side 4
        0.95f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        1.00f, -0.9f, -0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        1.00f, -0.9f, -0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        0.95f, -0.9f, -0.20f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        0.95f, -0.1f, -0.20f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f

    };
    
    GLfloat planeVerts[] = {
        // Vertex Positions   //Normals             //Texture  
        -5.0f, -0.9f, -5.0f,  0.0f, -1.0f,  0.0f,   0.0, 0.0,
         5.0f, -0.9f, -5.0f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -5.0f, -0.9f,  5.0f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         5.0f, -0.9f, -5.0f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -5.0f, -0.9f,  5.0f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         5.0f, -0.9f,  5.0f,  0.0f, -1.0f,  0.0f,   1.0, 1.0
    };

    GLfloat carpetVerts[] = {
        // Vertex Positions   //Normals             //Texture  
       -1.5f, -0.8f, -1.5f,  0.0f, -1.0f,  0.0f,   0.0, 0.0,
        1.5f, -0.8f, -1.5f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
       -1.5f, -0.8f,  1.5f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        1.5f, -0.8f, -1.5f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
       -1.5f, -0.8f,  1.5f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        1.5f, -0.8f,  1.5f,  0.0f, -1.0f,  0.0f,   1.0, 1.0
    };

    GLfloat windowVerts[] = {
        // Vertex Positions   //Normals             //Texture  
        -1.0f,  0.0f, -5.0f,  0.0f, -1.0f,  0.0f,   0.0, 0.0,
         1.0f,  0.0f, -5.0f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -1.0f,  1.0f, -5.0f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -1.0f,  1.0f, -5.0f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         1.0f,  1.0f, -5.0f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         1.0f,  0.0f, -5.0f,  0.0f, -1.0f,  0.0f,   1.0, 1.0
    };

    GLfloat teacupVerts[] = {
        //base
         0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.1f,  1.0f, -0.2f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.1f,  1.0f, -0.2f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
         0.2f,  1.0f, -0.1f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.1f,  1.0f, -0.2f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
         0.2f,  1.0f,  0.1f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.2f,  1.0f, -0.1f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
         0.2f,  1.0f,  0.1f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.1f,  1.0f,  0.2f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         
         0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.1f,  1.0f,  0.2f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.1f,  1.0f,  0.2f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.2f,  1.0f, -0.1f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.1f,  1.0f, -0.2f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.2f,  1.0f,  0.1f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.2f,  1.0f, -0.1f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.2f,  1.0f,  0.1f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.1f,  1.0f,  0.2f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        //sides 1
         0.00f,  1.1f, -0.25f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //back
        -0.10f,  1.0f, -0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.10f,  1.0f, -0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.17f,  1.1f, -0.17f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, 
         0.20f,  1.0f, -0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.10f,  1.0f, -0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.25f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //right
         0.20f,  1.0f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.20f,  1.0f, -0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.17f,  1.1f,  0.17f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
         0.20f,  1.0f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.10f,  1.0f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.00f,  1.1f,  0.25f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //front
        -0.10f,  1.0f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.10f,  1.0f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.17f,  1.1f,  0.17f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.20f,  1.0f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.10f,  1.0f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.25f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //left
        -0.20f,  1.0f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.20f,  1.0f, -0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.17f,  1.1f, -0.17f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.20f,  1.0f, -0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.10f,  1.0f, -0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         //sides 2
         0.10f,  1.0f, -0.20f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //right
         0.00f,  1.1f, -0.25f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.17f,  1.1f, -0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        
         0.25f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
         0.20f,  1.0f, -0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.17f,  1.1f, -0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.25f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
         0.20f,  1.0f,  0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.17f,  1.1f,  0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.10f,  1.0f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
         0.00f,  1.1f,  0.25f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.17f,  1.1f,  0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.10f,  1.0f, -0.20f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //left
         0.00f,  1.1f, -0.25f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.17f,  1.1f, -0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.25f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.20f,  1.0f, -0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.17f,  1.1f, -0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.25f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.20f,  1.0f,  0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.17f,  1.1f,  0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.10f,  1.0f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
         0.00f,  1.1f,  0.25f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.17f,  1.1f,  0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        //sides 3
         0.10f,  1.6f, -0.20f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, 
         0.00f,  1.1f, -0.25f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.17f,  1.1f, -0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        
         0.25f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
         0.20f,  1.6f, -0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.17f,  1.1f, -0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.25f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
         0.20f,  1.6f,  0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.17f,  1.1f,  0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.10f,  1.6f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
         0.00f,  1.1f,  0.25f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.17f,  1.1f,  0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.10f,  1.6f, -0.20f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, 
        -0.00f,  1.1f, -0.25f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.17f,  1.1f, -0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        
        -0.25f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.20f,  1.6f, -0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.17f,  1.1f, -0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.25f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.20f,  1.6f,  0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.17f,  1.1f,  0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.10f,  1.6f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.00f,  1.1f,  0.25f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.17f,  1.1f,  0.17f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        //sides 4
         0.00f,  1.1f, -0.25f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //back
        -0.10f,  1.6f, -0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.00f,  1.6f, -0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.17f,  1.1f, -0.17f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, 
         0.20f,  1.6f, -0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.10f,  1.6f, -0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.25f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //right
         0.20f,  1.6f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.20f,  1.6f, -0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.17f,  1.1f,  0.17f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
         0.20f,  1.6f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.10f,  1.6f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.00f,  1.1f,  0.25f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //front
        -0.10f,  1.6f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.10f,  1.6f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.17f,  1.1f,  0.17f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.20f,  1.6f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.10f,  1.6f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.25f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //left
        -0.20f,  1.6f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.20f,  1.6f, -0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        -0.17f,  1.1f, -0.17f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
        -0.20f,  1.6f, -0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.10f,  1.6f, -0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

         0.00f,  1.1f, -0.25f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //back
         0.10f,  1.6f, -0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.00f,  1.6f, -0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

        //handle
        -0.05f,  1.20f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.05f,  1.20f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.00f,  1.20f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.05f,  1.20f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.05f,  1.25f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.20f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.05f,  1.25f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.25f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.05f,  1.25f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.00f,  1.25f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.05f,  1.20f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.00f,  1.20f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.20f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.00f,  1.20f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.05f,  1.25f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.00f,  1.25f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.25f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.00f,  1.25f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        //handle upright
        -0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.00f,  1.20f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.22f,  0.37f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.05f,  1.52f,  0.37f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.05f,  1.22f,  0.37f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.52f,  0.37f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.05f,  1.52f,  0.37f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.05f,  1.22f,  0.37f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
            //
         0.05f,  1.22f,  0.37f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.05f,  1.52f,  0.37f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.05f,  1.22f,  0.37f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.22f,  0.37f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.05f,  1.22f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.52f,  0.37f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.05f,  1.22f,  0.37f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
            //
        -0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.00f,  1.55f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        //handle
        -0.05f,  1.50f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.05f,  1.50f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.00f,  1.50f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.05f,  1.50f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.05f,  1.55f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.50f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
        -0.05f,  1.55f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.55f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.05f,  1.55f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.00f,  1.55f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.05f,  1.50f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.00f,  1.50f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.50f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.00f,  1.50f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

         0.05f,  1.55f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.00f,  1.55f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
         0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

        -0.05f,  1.55f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
         0.00f,  1.55f,  0.40f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
        -0.05f,  1.52f,  0.40f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
    };

    GLfloat saucerVerts[] = {
    //base
     0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
    -0.1f,  1.0f, -0.2f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
     0.1f,  1.0f, -0.2f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

     0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
     0.2f,  1.0f, -0.1f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
     0.1f,  1.0f, -0.2f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

     0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
     0.2f,  1.0f,  0.1f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
     0.2f,  1.0f, -0.1f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

     0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
     0.2f,  1.0f,  0.1f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
     0.1f,  1.0f,  0.2f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

     0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
    -0.1f,  1.0f,  0.2f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
     0.1f,  1.0f,  0.2f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

     0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
    -0.2f,  1.0f, -0.1f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
    -0.1f,  1.0f, -0.2f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

     0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
    -0.2f,  1.0f,  0.1f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
    -0.2f,  1.0f, -0.1f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

     0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
    -0.2f,  1.0f,  0.1f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
    -0.1f,  1.0f,  0.2f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

    //sides 1
     0.00f,  1.1f, -0.45f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //back
    -0.10f,  1.0f, -0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
     0.10f,  1.0f, -0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

     0.27f,  1.1f, -0.27f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
     0.20f,  1.0f, -0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
     0.10f,  1.0f, -0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

     0.45f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //right
     0.20f,  1.0f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
     0.20f,  1.0f, -0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

     0.27f,  1.1f,  0.27f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
     0.20f,  1.0f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
     0.10f,  1.0f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

     0.00f,  1.1f,  0.45f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //front
    -0.10f,  1.0f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
     0.10f,  1.0f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

    -0.27f,  1.1f,  0.27f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
    -0.20f,  1.0f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
    -0.10f,  1.0f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

    -0.45f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //left
    -0.20f,  1.0f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
    -0.20f,  1.0f, -0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

    -0.27f,  1.1f, -0.27f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,
    -0.20f,  1.0f, -0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
    -0.10f,  1.0f, -0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,

    //sides 2
     0.00f,  1.1f, -0.45f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //back right
     0.10f,  1.0f, -0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
     0.27f,  1.1f, -0.27f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,

     0.27f,  1.1f, -0.27f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //right back
     0.45f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, 
     0.20f,  1.0f, -0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

     0.45f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //right
     0.20f,  1.0f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
     0.27f,  1.1f,  0.27f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,

     0.27f,  1.1f,  0.27f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //right front
     0.10f,  1.0f,  0.20f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
     0.00f,  1.1f,  0.45f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, 

     0.00f,  1.1f,  0.45f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //front left
    -0.10f,  1.0f,  0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
    -0.27f,  1.1f,  0.27f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,

    -0.27f,  1.1f,  0.27f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //left front
    -0.20f,  1.0f,  0.10f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,
    -0.45f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, 

    -0.45f,  1.1f,  0.00f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //left back
    -0.20f,  1.0f, -0.10f,  0.0f, -1.0f,  0.0f,   0.0, 1.0,
    -0.27f,  1.1f, -0.27f,  0.0f, -1.0f,  0.0f,   1.0, 1.0,

    -0.27f,  1.1f, -0.27f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, //back left
     0.00f,  1.1f, -0.45f,  0.0f, -1.0f,  0.0f,   1.0, 1.0, 
    -0.10f,  1.0f, -0.20f,  0.0f, -1.0f,  0.0f,   1.0, 0.0,

};

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    //Generate Teacup Vertex buffers
    //------------------------------
    mesh.teacupVertices = sizeof(teacupVerts) / (sizeof(teacupVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
    glGenVertexArrays(1, &mesh.teacupVAO); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.teacupVAO);
    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.teacupVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.teacupVBO); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(teacupVerts), teacupVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    //Generate Table Vertex buffers
    //------------------------------
    mesh.tableVertices = sizeof(tableVerts) / (sizeof(tableVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
    glGenVertexArrays(1, &mesh.tableVAO); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.tableVAO);
    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.tableVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.tableVBO); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(tableVerts), tableVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    //Generate Plane Vertex buffers
    //------------------------------
    mesh.planeVertices = sizeof(planeVerts) / (sizeof(planeVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
    glGenVertexArrays(1, &mesh.planeVAO); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.planeVAO);
    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.planeVBO); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts), planeVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
    
    //Generate Window Vertex buffers
    //------------------------------
    mesh.windowVertices = sizeof(windowVerts) / (sizeof(windowVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
    glGenVertexArrays(1, &mesh.windowVAO); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.windowVAO);
    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.windowVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.windowVBO); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(windowVerts), windowVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    //Generate Carpet Vertex buffers
    //------------------------------
    mesh.carpetVertices = sizeof(carpetVerts) / (sizeof(carpetVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
    glGenVertexArrays(1, &mesh.carpetVAO); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.carpetVAO);
    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.carpetVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.carpetVBO); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(carpetVerts), carpetVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    //Generate Saucer Vertex buffers
    //------------------------------
    mesh.saucerVertices = sizeof(saucerVerts) / (sizeof(saucerVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
    glGenVertexArrays(1, &mesh.saucerVAO); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.saucerVAO);
    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.saucerVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.saucerVBO); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(saucerVerts), saucerVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.tableVAO);
    glDeleteBuffers(1, &mesh.tableVBO);
    glDeleteVertexArrays(1, &mesh.planeVAO);
    glDeleteBuffers(1, &mesh.planeVBO);
}

/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}

void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}

// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}

void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}

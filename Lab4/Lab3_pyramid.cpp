#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstring> // For memset and memcpy

// Vertex structure with texture coordinates
struct Vertex {
    float x, y, z;
    float u, v; // Texture coordinates
};

// Vertex Shader Source Code
const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)glsl";

// Fragment Shader Source Code
const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// We will use texture units 0 to 4
uniform sampler2D textures[5];

uniform int textureIndex; // To select which texture to use

void main()
{
    FragColor = texture(textures[textureIndex], TexCoord);
}
)glsl";

// Matrix functions

// Set a 4x4 identity matrix
void setIdentityMatrix(float* mat) {
    memset(mat, 0, 16 * sizeof(float));
    mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
}

// Multiply two 4x4 matrices: result = a * b
void multiplyMatrices(const float* a, const float* b, float* result) {
    float temp[16];
    for(int i = 0; i < 4; i++) { // rows of a
        for(int j = 0; j < 4; j++) { // columns of b
            temp[i * 4 + j] = a[i * 4 + 0] * b[0 * 4 + j] +
                              a[i * 4 + 1] * b[1 * 4 + j] +
                              a[i * 4 + 2] * b[2 * 4 + j] +
                              a[i * 4 + 3] * b[3 * 4 + j];
        }
    }
    memcpy(result, temp, 16 * sizeof(float));
}

// Create a perspective projection matrix
void setPerspectiveMatrix(float* mat, float fov, float aspect, float nearPlane, float farPlane) {
    float f = 1.0f / tanf(fov * 0.5f * (3.14159265358979323846f / 180.0f));
    memset(mat, 0, 16 * sizeof(float));
    mat[0] = f / aspect;
    mat[5] = f;
    mat[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
    mat[11] = -1.0f;
    mat[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
}

// Create a lookAt view matrix
void setLookAtMatrix(float* mat, float eyeX, float eyeY, float eyeZ,
                                 float centerX, float centerY, float centerZ,
                                 float upX, float upY, float upZ) {
    float forward[3], side[3], up[3];
    float fwdLen, sideLen, upLen;

    // Compute forward vector (center - eye)
    forward[0] = centerX - eyeX;
    forward[1] = centerY - eyeY;
    forward[2] = centerZ - eyeZ;
    fwdLen = sqrtf(forward[0]*forward[0] + forward[1]*forward[1] + forward[2]*forward[2]);

    // Normalize forward vector
    forward[0] /= fwdLen;
    forward[1] /= fwdLen;
    forward[2] /= fwdLen;

    // Compute side vector = forward x up
    side[0] = forward[1]*upZ - forward[2]*upY;
    side[1] = forward[2]*upX - forward[0]*upZ;
    side[2] = forward[0]*upY - forward[1]*upX;
    sideLen = sqrtf(side[0]*side[0] + side[1]*side[1] + side[2]*side[2]);

    // Normalize side vector
    side[0] /= sideLen;
    side[1] /= sideLen;
    side[2] /= sideLen;

    // Recompute up vector = side x forward
    up[0] = side[1]*forward[2] - side[2]*forward[1];
    up[1] = side[2]*forward[0] - side[0]*forward[2];
    up[2] = side[0]*forward[1] - side[1]*forward[0];

    // Set the view matrix
    mat[0] = side[0];
    mat[1] = up[0];
    mat[2] = -forward[0];
    mat[3] = 0.0f;

    mat[4] = side[1];
    mat[5] = up[1];
    mat[6] = -forward[1];
    mat[7] = 0.0f;

    mat[8] = side[2];
    mat[9] = up[2];
    mat[10] = -forward[2];
    mat[11] = 0.0f;

    mat[12] = - (side[0]*eyeX + side[1]*eyeY + side[2]*eyeZ);
    mat[13] = - (up[0]*eyeX + up[1]*eyeY + up[2]*eyeZ);
    mat[14] = forward[0]*eyeX + forward[1]*eyeY + forward[2]*eyeZ;
    mat[15] = 1.0f;
}

// Create a rotation matrix around the Y axis
void setRotationYMatrix(float* mat, float angleDegrees) {
    float radians = angleDegrees * (3.14159265358979323846f / 180.0f);
    float cosA = cosf(radians);
    float sinA = sinf(radians);

    setIdentityMatrix(mat);
    mat[0] = cosA;
    mat[2] = sinA;
    mat[8] = -sinA;
    mat[10] = cosA;
}

// Function to create vertices of a textured pyramid
void createTexturedPyramid(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const Vertex& center, float baseSize, float pyramidHeight) {
    float halfBase = baseSize / 2.0f;
    float halfHeight = pyramidHeight / 2.0f;

    // Define the 5 unique positions
    Vertex v0 = { center.x - halfBase, center.y - halfHeight, center.z + halfBase }; // Front-left base
    Vertex v1 = { center.x + halfBase, center.y - halfHeight, center.z + halfBase }; // Front-right base
    Vertex v2 = { center.x + halfBase, center.y - halfHeight, center.z - halfBase }; // Back-right base
    Vertex v3 = { center.x - halfBase, center.y - halfHeight, center.z - halfBase }; // Back-left base
    Vertex v4 = { center.x, center.y + halfHeight, center.z };                       // Apex

    // Texture coordinates
    float texCoords[3][2] = {
        { 0.0f, 0.0f }, // Bottom-left
        { 1.0f, 0.0f }, // Bottom-right
        { 0.5f, 1.0f }  // Top-center
    };

    // Base face (two triangles)
    // Triangle 1
    vertices.push_back({ v0.x, v0.y, v0.z, 0.0f, 0.0f }); // v0
    vertices.push_back({ v1.x, v1.y, v1.z, 1.0f, 0.0f }); // v1
    vertices.push_back({ v2.x, v2.y, v2.z, 1.0f, 1.0f }); // v2

    // Triangle 2
    vertices.push_back({ v2.x, v2.y, v2.z, 1.0f, 1.0f }); // v2
    vertices.push_back({ v3.x, v3.y, v3.z, 0.0f, 1.0f }); // v3
    vertices.push_back({ v0.x, v0.y, v0.z, 0.0f, 0.0f }); // v0

    // Indices for the base
    for (unsigned int i = 0; i < 6; ++i) {
        indices.push_back(i);
    }

    // Side faces
    // Face 1
    vertices.push_back({ v0.x, v0.y, v0.z, texCoords[0][0], texCoords[0][1] }); // v0
    vertices.push_back({ v1.x, v1.y, v1.z, texCoords[1][0], texCoords[1][1] }); // v1
    vertices.push_back({ v4.x, v4.y, v4.z, texCoords[2][0], texCoords[2][1] }); // v4

    // Face 2
    vertices.push_back({ v1.x, v1.y, v1.z, texCoords[0][0], texCoords[0][1] }); // v1
    vertices.push_back({ v2.x, v2.y, v2.z, texCoords[1][0], texCoords[1][1] }); // v2
    vertices.push_back({ v4.x, v4.y, v4.z, texCoords[2][0], texCoords[2][1] }); // v4

    // Face 3
    vertices.push_back({ v2.x, v2.y, v2.z, texCoords[0][0], texCoords[0][1] }); // v2
    vertices.push_back({ v3.x, v3.y, v3.z, texCoords[1][0], texCoords[1][1] }); // v3
    vertices.push_back({ v4.x, v4.y, v4.z, texCoords[2][0], texCoords[2][1] }); // v4

    // Face 4
    vertices.push_back({ v3.x, v3.y, v3.z, texCoords[0][0], texCoords[0][1] }); // v3
    vertices.push_back({ v0.x, v0.y, v0.z, texCoords[1][0], texCoords[1][1] }); // v0
    vertices.push_back({ v4.x, v4.y, v4.z, texCoords[2][0], texCoords[2][1] }); // v4

    // Indices for the sides
    for (unsigned int i = 6; i < 18; ++i) {
        indices.push_back(i);
    }
}

// Function to load a texture from file
unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int texWidth, texHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &texWidth, &texHeight, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping/filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // T axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Minification
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Magnification

        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version to 3.3 and use the core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Textured Pyramid", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set OpenGL context
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set viewport size
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compile and link shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::VERTEX_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::FRAGMENT_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders into program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for shader program linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete shaders after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Get uniform locations
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc  = glGetUniformLocation(shaderProgram, "view");
    int projLoc  = glGetUniformLocation(shaderProgram, "projection");

    // Create pyramid vertices and indices
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Adjust the pyramid size to fit within NDC (-1 to 1)
    Vertex center = { 0.0f, 0.0f, 0.0f }; // Center of the base
    float baseSize = 1.0f; // Width and depth of the base
    float pyramidHeight = 1.0f; // Height of the pyramid

    createTexturedPyramid(vertices, indices, center, baseSize, pyramidHeight);

    // Generate buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind and fill VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Bind and fill EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float))); // Texture Coords
    glEnableVertexAttribArray(1);

    // Unbind VBO (the VAO keeps the EBO binding)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind VAO
    glBindVertexArray(0);

    // Load textures for each face
    unsigned int textures[5];
    textures[0] = loadTexture("brick.jpg"); 
    textures[1] = loadTexture("trees.jpg");
    textures[2] = loadTexture("soil.jpg");
    textures[3] = loadTexture("water.jpg");
    textures[4] = loadTexture("brick.jpg");  

    // Activate texture units and bind textures
    glUseProgram(shaderProgram);
    for (int i = 0; i < 5; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        // Set sampler uniforms
        std::string uniformName = "textures[" + std::to_string(i) + "]";
        glUniform1i(glGetUniformLocation(shaderProgram, uniformName.c_str()), i);
    }

    // Set up the projection matrix
    float projection[16];
    setPerspectiveMatrix(projection, 45.0f, (float)width / height, 0.1f, 100.0f);

    // Set up the view matrix
    float view[16];
    setLookAtMatrix(view,
                    3.0f, 3.0f, 3.0f,   // Camera position
                    0.0f, 0.0f, 0.0f,   // Target position
                    0.0f, 1.0f, 0.0f);  // Up vector

    // Prepare the model matrix (static rotation)
    float model[16];
    float rotation[16];
    setIdentityMatrix(model);
    setRotationYMatrix(rotation, 45.0f); // Rotate by 45 degrees around Y-axis
    multiplyMatrices(model, rotation, model); // model = model * rotation

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the color and depth buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader program
        glUseProgram(shaderProgram);

        // Pass the matrices to the shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

        // Bind VAO
        glBindVertexArray(VAO);

        // Draw base
        glUniform1i(glGetUniformLocation(shaderProgram, "textureIndex"), 0); // Texture unit for base
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Draw sides
        for (int i = 0; i < 4; ++i) {
            glUniform1i(glGetUniformLocation(shaderProgram, "textureIndex"), i + 1); // Texture unit for sides
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * (6 + i * 3)));
        }

        // Unbind VAO
        glBindVertexArray(0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    for (int i = 0; i < 5; ++i) {
        glDeleteTextures(1, &textures[i]);
    }

    glfwTerminate();
    return 0;
}
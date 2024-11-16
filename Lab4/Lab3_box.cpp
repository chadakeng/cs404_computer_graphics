#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>   // For mathematical functions
#include <cstring> // For memset and memcpy

// Vertex structure with texture coordinates
struct Vertex {
    float x, y, z;
    float u, v; // Texture coordinates
};

// Function to create the vertices of a box with texture coordinates
void createBoxVertices(Vertex* vertices, const Vertex& center, float width, float height, float depth) {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float halfDepth = depth / 2.0f;

    // Define texture coordinates
    float uvs[4][2] = {
        {0.0f, 0.0f}, // Bottom-left
        {1.0f, 0.0f}, // Bottom-right
        {1.0f, 1.0f}, // Top-right
        {0.0f, 1.0f}  // Top-left
    };

    // Front face
    vertices[0] = {center.x - halfWidth, center.y - halfHeight, center.z + halfDepth, uvs[0][0], uvs[0][1]};
    vertices[1] = {center.x + halfWidth, center.y - halfHeight, center.z + halfDepth, uvs[1][0], uvs[1][1]};
    vertices[2] = {center.x + halfWidth, center.y + halfHeight, center.z + halfDepth, uvs[2][0], uvs[2][1]};
    vertices[3] = {center.x - halfWidth, center.y + halfHeight, center.z + halfDepth, uvs[3][0], uvs[3][1]};

    // Back face
    vertices[4] = {center.x + halfWidth, center.y - halfHeight, center.z - halfDepth, uvs[0][0], uvs[0][1]};
    vertices[5] = {center.x - halfWidth, center.y - halfHeight, center.z - halfDepth, uvs[1][0], uvs[1][1]};
    vertices[6] = {center.x - halfWidth, center.y + halfHeight, center.z - halfDepth, uvs[2][0], uvs[2][1]};
    vertices[7] = {center.x + halfWidth, center.y + halfHeight, center.z - halfDepth, uvs[3][0], uvs[3][1]};

    // Left face
    vertices[8]  = {center.x - halfWidth, center.y - halfHeight, center.z - halfDepth, uvs[0][0], uvs[0][1]};
    vertices[9]  = {center.x - halfWidth, center.y - halfHeight, center.z + halfDepth, uvs[1][0], uvs[1][1]};
    vertices[10] = {center.x - halfWidth, center.y + halfHeight, center.z + halfDepth, uvs[2][0], uvs[2][1]};
    vertices[11] = {center.x - halfWidth, center.y + halfHeight, center.z - halfDepth, uvs[3][0], uvs[3][1]};

    // Right face
    vertices[12] = {center.x + halfWidth, center.y - halfHeight, center.z + halfDepth, uvs[0][0], uvs[0][1]};
    vertices[13] = {center.x + halfWidth, center.y - halfHeight, center.z - halfDepth, uvs[1][0], uvs[1][1]};
    vertices[14] = {center.x + halfWidth, center.y + halfHeight, center.z - halfDepth, uvs[2][0], uvs[2][1]};
    vertices[15] = {center.x + halfWidth, center.y + halfHeight, center.z + halfDepth, uvs[3][0], uvs[3][1]};

    // Top face
    vertices[16] = {center.x - halfWidth, center.y + halfHeight, center.z + halfDepth, uvs[0][0], uvs[0][1]};
    vertices[17] = {center.x + halfWidth, center.y + halfHeight, center.z + halfDepth, uvs[1][0], uvs[1][1]};
    vertices[18] = {center.x + halfWidth, center.y + halfHeight, center.z - halfDepth, uvs[2][0], uvs[2][1]};
    vertices[19] = {center.x - halfWidth, center.y + halfHeight, center.z - halfDepth, uvs[3][0], uvs[3][1]};

    // Bottom face
    vertices[20] = {center.x - halfWidth, center.y - halfHeight, center.z - halfDepth, uvs[0][0], uvs[0][1]};
    vertices[21] = {center.x + halfWidth, center.y - halfHeight, center.z - halfDepth, uvs[1][0], uvs[1][1]};
    vertices[22] = {center.x + halfWidth, center.y - halfHeight, center.z + halfDepth, uvs[2][0], uvs[2][1]};
    vertices[23] = {center.x - halfWidth, center.y - halfHeight, center.z + halfDepth, uvs[3][0], uvs[3][1]};
}

// Function to create indices for the box with 24 vertices
void createBoxIndices(unsigned int* indices) {
    unsigned int tempIndices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        8, 9, 10, 10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Top face
        16, 17, 18, 18, 19, 16,
        // Bottom face
        20, 21, 22, 22, 23, 20
    };

    for (int i = 0; i < 36; i++) {
        indices[i] = tempIndices[i];
    }
}

// Shader sources (modified to include texture coordinates and transformations)
const char* vertexShaderSource = "#version 330 core\n"
    // Input attributes
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"

    // Uniform matrices
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"

    // Output to fragment shader
    "out vec2 TexCoord;\n"

    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   TexCoord = aTexCoord;\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    // Output color
    "out vec4 FragColor;\n"

    // Input from vertex shader
    "in vec2 TexCoord;\n"

    // Texture sampler
    "uniform sampler2D texture1;\n"

    "void main()\n"
    "{\n"
    "   FragColor = texture(texture1, TexCoord);\n"
    "}\n\0";

// Function to load a texture from file
unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int texWidth, texHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &texWidth, &texHeight, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
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
    upLen = sqrtf(up[0]*up[0] + up[1]*up[1] + up[2]*up[2]);

    // Normalize up vector
    up[0] /= upLen;
    up[1] /= upLen;
    up[2] /= upLen;

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
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Textured Cube at an Angle", NULL, NULL);
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

    // Compile and link shaders (include error checking)
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
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
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Box vertices and indices
    Vertex verticesArr[24];
    unsigned int indicesArr[36];
    Vertex center = {0.0f, 0.0f, 0.0f};  // Center of the box
    createBoxVertices(verticesArr, center, 1.0f, 1.0f, 1.0f);  // Create vertices for the box
    createBoxIndices(indicesArr);  // Create indices for the box

    // Generate buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind and fill VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesArr), verticesArr, GL_STATIC_DRAW);

    // Bind and fill EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesArr), indicesArr, GL_STATIC_DRAW);

    // Set vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Set texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VBO (optional) and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Load and create a texture
    unsigned int texture1 = loadTexture("brick.jpg"); // Replace with your texture file

    // Use shader program and set the texture uniform
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0); // Texture unit 0

    // Get uniform locations
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc  = glGetUniformLocation(shaderProgram, "view");
    int projLoc  = glGetUniformLocation(shaderProgram, "projection");

    // Set up the projection matrix once
    float projection[16];
    setPerspectiveMatrix(projection, 45.0f, (float)width / height, 0.1f, 100.0f);

    // Set up the view matrix once
    float view[16];
    setLookAtMatrix(view,
                    3.0f, 3.0f, 3.0f,   // Camera position
                    0.0f, 0.0f, 0.0f,   // Target position
                    0.0f, 1.0f, 0.0f);  // Up vector

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Prepare the model matrix (static rotation)
    float model[16];
    float rotation[16];
    setIdentityMatrix(model);
    setRotationYMatrix(rotation, 45.0f); // Rotate by 45 degrees around Y-axis
    multiplyMatrices(model, rotation, model); // model = model * rotation

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

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // Bind VAO and draw the box
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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
    glDeleteTextures(1, &texture1);

    glfwTerminate();
    return 0;
}
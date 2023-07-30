#include "./shader.h"

Shader compileShader(const char* vertexShaderPath, const char* fragmentShaderPath) {
    uint32_t id;
    uint32_t vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // open shader files
    FILE* vertexShaderFile, * fragmentShaderFile;
    fopen_s(&vertexShaderFile, vertexShaderPath, "rb");
    fopen_s(&fragmentShaderFile, fragmentShaderPath, "rb");

    // if no files found
    ERROR_CHECK(vertexShaderFile, "[ERROR] No vertex shader found\n");
    ERROR_CHECK(fragmentShaderFile, "[ERROR] No fragment shader found\n");

    char* vertexShaderBuffer, * fragmentShaderBuffer;
    int32_t vtxSrcSize, fragSrcSize;
    fseek(vertexShaderFile, 0, SEEK_END);
    fseek(fragmentShaderFile, 0, SEEK_END);
    vtxSrcSize = ftell(vertexShaderFile);
    fragSrcSize = ftell(fragmentShaderFile);

    fseek(vertexShaderFile, 0, SEEK_SET);
    fseek(fragmentShaderFile, 0, SEEK_SET);

    vertexShaderBuffer = (char*)malloc(vtxSrcSize+1);
    fragmentShaderBuffer = (char*)malloc(fragSrcSize+1);

    fread(vertexShaderBuffer, 1, vtxSrcSize, vertexShaderFile);
    fread(fragmentShaderBuffer, 1, fragSrcSize, fragmentShaderFile);

    vertexShaderBuffer[vtxSrcSize] = 0;
    fragmentShaderBuffer[fragSrcSize] = 0;

    fclose(vertexShaderFile);
    fclose(fragmentShaderFile);


    // set shader sources
    glShaderSource(vertexShader, 1, &vertexShaderBuffer, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderBuffer, NULL);

    glCompileShader(vertexShader);
    {
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "[ERROR] Compilation failed: Vertex shader\n" << infoLog << std::endl;
        }
    }


    glCompileShader(fragmentShader);
    {
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "[ERROR] Compilation failed: Fragment shader\n" << infoLog << std::endl;
        }
    }


    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    {
        int  success;
        char infoLog[512];
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 512, NULL, infoLog);
            std::cout << "[ERROR] Link failed\n" << infoLog << std::endl;
        }
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(vertexShaderBuffer);
    free(fragmentShaderBuffer);

    Shader shader;
    shader.id = id;
    return(shader);
}
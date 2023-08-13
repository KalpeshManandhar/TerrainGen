#include "./shader.h"

Shader compileShader(const char* vertexShaderPath, const char* fragmentShaderPath, const char *geometryShaderPath) {
    auto loadFileToBuffer = [&](const char * filepath) -> char *
    {
        FILE *file;
        fopen_s(&file, filepath, "rb");
        ERROR_CHECK(file, "[ERROR] File %s not found", filepath);
        

        fseek(file, 0, SEEK_END);
        int32_t size = ftell(file);
        fseek(file, 0, SEEK_SET);


        char* buffer = (char*)malloc(size+1);
        fread(buffer, 1, size, file);
        buffer[size] = 0;
        fclose(file);
        return(buffer);
    };
    
    
    uint32_t id;
    uint32_t vertexShader, fragmentShader, geometryShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (geometryShaderPath) geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    // open shader files
    char *vertexShaderBuffer = NULL, *fragmentShaderBuffer = NULL, *geometryShaderBuffer = NULL; 
    vertexShaderBuffer = loadFileToBuffer(vertexShaderPath);
    fragmentShaderBuffer = loadFileToBuffer(fragmentShaderPath);
    if (geometryShaderPath) geometryShaderBuffer = loadFileToBuffer(geometryShaderPath);


    // set shader sources
    glShaderSource(vertexShader, 1, &vertexShaderBuffer, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderBuffer, NULL);
    if (geometryShaderPath) glShaderSource(geometryShader, 1, &geometryShaderBuffer, NULL);
     

    auto checkShaderCompileStatus = [&](uint32_t shader, const char *type){
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            fprintf(stderr, "[ERROR] Compilation failed: %s shader\n%s\n", type, infoLog);
        }
    };

    glCompileShader(vertexShader);
    checkShaderCompileStatus(vertexShader, "vertex");
    
    if(geometryShaderPath){
        glCompileShader(geometryShader);
        checkShaderCompileStatus(geometryShader, "geometry");
    }
    
    glCompileShader(fragmentShader);
    checkShaderCompileStatus(fragmentShader, "fragment");


    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    if (geometryShaderPath) glAttachShader(id, geometryShader);
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
    if (geometryShaderPath) glDeleteShader(geometryShader);
    
    free(vertexShaderBuffer);
    free(fragmentShaderBuffer);
    if (geometryShaderPath) free(geometryShaderBuffer);

    Shader shader;
    shader.id = id;
    return(shader);
}
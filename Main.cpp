
#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// CONFIGURACIÓN
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// CÁMARA
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float cameraSpeed = 0.02f;
// Movimiento de la cámara con el teclado
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(
            glm::cross(cameraFront, cameraUp)
        ) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(
            glm::cross(cameraFront, cameraUp)
        ) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
}

// CARGA DE TEXTURAS
void LoadTexturesFromFiles(
    const char* texturePaths[],
    int count,
    GLuint textures[]
)
{
    stbi_set_flip_vertically_on_load(true);

    for (int i = 0; i < count; i++)
    {
        int width = 0;
        int height = 0;
        int nrChannels = 0;

        std::cout << "\nCargando textura "
            << i + 1 << ": "
            << texturePaths[i] << std::endl;

        unsigned char* data = stbi_load(
            texturePaths[i],
            &width,
            &height,
            &nrChannels,
            0
        );

        if (!data)
        {
            std::cout << "ERROR NO CARGO: "
                << texturePaths[i] << std::endl;

            std::cout << "STB: "
                << stbi_failure_reason()
                << std::endl;

            continue;
        }

        std::cout << "OK CARGO: "
            << width << "x" << height
            << std::endl;

        // Crear y configurar textura
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_REPEAT
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_REPEAT
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR
        );

        // Determinar formato según canales
        GLenum format;

        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else
        {
            std::cout << "ERROR: canales no soportados."
                << std::endl;

            stbi_image_free(data);
            glBindTexture(GL_TEXTURE_2D, 0);
            continue;
        }

        // Enviar imagen a OpenGL
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        GLenum error = glGetError();

        if (error != GL_NO_ERROR)
        {
            std::cout << "ERROR DE OPENGL EN TEXTURA "
                << i + 1 << ": "
                << error << std::endl;
        }
        else
        {
            std::cout << "TEXTURA "
                << i + 1
                << " CARGADA EN OPENGL"
                << std::endl;
        }

        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

// MAIN
int main()
{
    // 1. Inicializar GLFW y crear ventana
    if (!glfwInit())
    {
        std::cout << "ERROR: No se pudo inicializar GLFW."
            << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "OpenGL 3D",
        NULL,
        NULL
    );

    if (window == NULL)
    {
        std::cout << "ERROR: No se pudo crear la ventana."
            << std::endl;

        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    // 2. Inicializar GLAD 
    if (!gladLoadGLLoader(
        (GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "ERROR: No se pudo inicializar GLAD."
            << std::endl;

        glfwTerminate();
        return -1;
    }
    
    // 3. Configurar viewport
    int width;
    int height;

    glfwGetFramebufferSize(
        window,
        &width,
        &height
    );

    glViewport(0, 0, width, height);
    
    // SHADERS
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec4 aColor;\n"
        "layout (location = 3) in vec2 aTexCoord;\n"

        "uniform mat4 uMVP;\n"

        "out vec4 VertexColor;\n"
        "out vec2 TexCoord;\n"

        "void main()\n"
        "{\n"
        "   gl_Position = uMVP * vec4(aPos, 1.0);\n"
        "   VertexColor = aColor;\n"
        "   TexCoord = aTexCoord;\n"
        "}\0";

    const char* fragmentShaderSource =
        "#version 330 core\n"

        "in vec4 VertexColor;\n"
        "in vec2 TexCoord;\n"

        "uniform sampler2D texture1;\n"

        "out vec4 FragColor;\n"

        "void main()\n"
        "{\n"
        "   FragColor = texture(texture1, TexCoord)"
        " * VertexColor;\n"
        "}\n\0";

    // Crear y compilar shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        NULL
    );

    glCompileShader(vertexShader);

   GLuint fragmentShader =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        NULL
    );

    glCompileShader(fragmentShader);

    // Crear programa de shaders
    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // GEOMETRÍA DE LA PIRÁMIDE
    // Cada vértice contiene:
    // posición (3) + color (4) + textura (2) = 9 valores

    GLfloat vertices[] =
    {
        // CARA FRONTAL
        -0.6f, -0.5f, 0.0f,  1,1,1,1,  0.0f,0.0f,
         0.6f, -0.5f, 0.0f,  1,1,1,1,  1.0f,0.0f,
         0.0f,  0.0f, 0.8f,  1,1,1,1,  0.5f,1.0f,

         // CARA DERECHA
          0.6f, -0.5f, 0.0f,  1,1,1,1,  0.0f,0.0f,
          0.0f,  0.5f, 0.0f,  1,1,1,1,  1.0f,0.0f,
          0.0f,  0.0f, 0.8f,  1,1,1,1,  0.5f,1.0f,

          // CARA IZQUIERDA
           0.0f,  0.5f, 0.0f,  1,1,1,1,  0.0f,0.0f,
          -0.6f, -0.5f, 0.0f,  1,1,1,1,  1.0f,0.0f,
           0.0f,  0.0f, 0.8f,  1,1,1,1,  0.5f,1.0f,

           // BASE
           -0.6f, -0.5f, 0.0f,  1,1,1,1,  0.0f,0.0f,
            0.0f,  0.5f, 0.0f,  1,1,1,1,  1.0f,0.0f,
            0.6f, -0.5f, 0.0f,  1,1,1,1,  0.5f,1.0f
    };

    GLuint indices[] =
    {
        0, 1, 2,       // Frontal
        3, 4, 5,       // Derecha
        6, 7, 8,       // Izquierda
        9, 10, 11      // Base
    };

    // VAO, VBO Y EBO 
    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );

    // Posición
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        9 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(
        1,
        4,
        GL_FLOAT,
        GL_FALSE,
        9 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    // Coordenadas de textura
    glVertexAttribPointer(
        3,
        2,
        GL_FLOAT,
        GL_FALSE,
        9 * sizeof(float),
        (void*)(7 * sizeof(float))
    );

    glEnableVertexAttribArray(3);
    
    // TEXTURAS
    GLuint textures[3];

    glGenTextures(3, textures);

    const char* texturePaths[3] =
    {
        "images/texture1.jpeg",
        "images/texture2.jpeg",
        "images/texture3.jpeg"
    };

    LoadTexturesFromFiles(
        texturePaths,
        3,
        textures
    );
    
    // CONFIGURAR SHADER Y TEXTURA
    glUseProgram(shaderProgram);

    GLint texLoc =
        glGetUniformLocation(
            shaderProgram,
            "texture1"
        );

    glActiveTexture(GL_TEXTURE0);

    if (texLoc >= 0)
        glUniform1i(texLoc, 0);

    GLint mvpLoc =
        glGetUniformLocation(
            shaderProgram,
            "uMVP"
        );

    // Activar profundidad
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // BUCLE PRINCIPAL
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float t = (float)glfwGetTime();
        // Limpiar pantalla
        glClearColor(
            0.30f,
            0.126f,
            0.160f,
            1.0f
        );

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        // MATRICES
        // Proyección, Vista y Modelo (MVP)     
        glm::mat4 projection =
            glm::perspective(
                glm::radians(60.0f),
                (float)width / (float)height,
                0.1f,
                100.0f
            );

        glm::mat4 view =
            glm::lookAt(
                cameraPos,
                cameraPos + cameraFront,
                cameraUp
            );

        glm::mat4 model =
            glm::mat4(1.0f);

        model = glm::rotate(
            model,
            t * glm::radians(30.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 mvp =
            projection * view * model;

        // Enviar MVP al shader
        glUseProgram(shaderProgram);

        glUniformMatrix4fv(
            mvpLoc,
            1,
            GL_FALSE,
            glm::value_ptr(mvp)
        );

        glBindVertexArray(VAO);
       
        // DIBUJAR CADA CARA CON UNA TEXTURA        
        // Cara frontal
        glBindTexture(GL_TEXTURE_2D, textures[0]);

        glDrawElements(
            GL_TRIANGLES,
            3,
            GL_UNSIGNED_INT,
            (void*)(0 * sizeof(GLuint))
        );

        // Cara derecha
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        glDrawElements(
            GL_TRIANGLES,
            3,
            GL_UNSIGNED_INT,
            (void*)(3 * sizeof(GLuint))
        );

        // Cara izquierda
        glBindTexture(GL_TEXTURE_2D, textures[2]);

        glDrawElements(
            GL_TRIANGLES,
            3,
            GL_UNSIGNED_INT,
            (void*)(6 * sizeof(GLuint))
        );

        // Base
        glBindTexture(GL_TEXTURE_2D, textures[0]);

        glDrawElements(
            GL_TRIANGLES,
            3,
            GL_UNSIGNED_INT,
            (void*)(9 * sizeof(GLuint))
        );
      
        // Mostrar resultado     
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // LIBERAR RECURSOS
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// ============================================================
// SHADERS
// ============================================================

// VERTEX SHADER
// Se encarga de recibir la posición de cada vértice,
// sus coordenadas de textura y aplicar la transformación
// 3D mediante la matriz MVP.

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


// FRAGMENT SHADER
// Se encarga de determinar el color final de cada fragmento.
// En este caso, toma el color de la textura y lo multiplica
// por el color del vértice.

const char* fragmentShaderSource =
"#version 330 core\n"

"in vec4 VertexColor;\n"
"in vec2 TexCoord;\n"

"uniform sampler2D texture1;\n"

"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"   FragColor = texture(texture1, TexCoord) * VertexColor;\n"
"}\n\0";


// ============================================================
// FUNCIÓN PARA CARGAR TEXTURAS
// ============================================================

void LoadTexturesFromFiles(
    const char* texturePaths[],
    int count,
    GLuint textures[]
)
{
    // Volteamos verticalmente las imágenes para que
    // las coordenadas UV coincidan correctamente.
    stbi_set_flip_vertically_on_load(true);

    for (int i = 0; i < count; i++)
    {
        int width;
        int height;
        int nrChannels;

        // Cargar la imagen utilizando stb_image
        unsigned char* data = stbi_load(
            texturePaths[i],
            &width,
            &height,
            &nrChannels,
            0
        );


        // ----------------------------------------------------
        // COMPROBAR SI LA IMAGEN SE CARGÓ
        // ----------------------------------------------------

        if (!data)
        {
            std::cout
                << "ERROR NO CARGO: "
                << texturePaths[i]
                << std::endl;

            std::cout
                << "STB: "
                << stbi_failure_reason()
                << std::endl;

            // Si no cargó, pasamos a la siguiente textura.
            continue;
        }


        // Mostrar información de la textura cargada
        std::cout
            << "OK CARGO: "
            << texturePaths[i]
            << " ("
            << width
            << "x"
            << height
            << ")"
            << std::endl;


        // ----------------------------------------------------
        // ACTIVAR LA TEXTURA
        // ----------------------------------------------------

        glBindTexture(
            GL_TEXTURE_2D,
            textures[i]
        );


        // ----------------------------------------------------
        // CONFIGURACIÓN DE LA TEXTURA
        // ----------------------------------------------------

        // Repetición horizontal
        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_REPEAT
        );

        // Repetición vertical
        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_REPEAT
        );


        // Filtro cuando la textura se ve pequeña
        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR
        );

        // Filtro cuando la textura se ve grande
        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR
        );


        // ----------------------------------------------------
        // DETERMINAR EL FORMATO DE LA IMAGEN
        // ----------------------------------------------------

        GLenum format;

        if (nrChannels == 1)
        {
            format = GL_RED;
        }
        else if (nrChannels == 3)
        {
            format = GL_RGB;
        }
        else if (nrChannels == 4)
        {
            format = GL_RGBA;
        }
        else
        {
            format = GL_RGB;
        }


        // ----------------------------------------------------
        // ENVIAR LA IMAGEN A OPENGL
        // ----------------------------------------------------

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


        // Crear mipmaps para mejorar la calidad
        // cuando la textura cambia de tamaño.
        glGenerateMipmap(GL_TEXTURE_2D);


        // Liberar la memoria utilizada por stb_image
        stbi_image_free(data);
    }


    // Desactivar la textura
    glBindTexture(GL_TEXTURE_2D, 0);
}


// ============================================================
// FUNCIÓN PRINCIPAL
// ============================================================

int main()
{
    // ========================================================
    // INICIALIZAR GLFW
    // ========================================================

    glfwInit();

    // Configuramos OpenGL 3.3
    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    // Utilizamos el perfil Core
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    // Reservamos 24 bits para el Depth Buffer
    glfwWindowHint(
        GLFW_DEPTH_BITS,
        24
    );


    // ========================================================
    // CREACIÓN DE LA VENTANA
    // ========================================================

    GLFWwindow* window =
        glfwCreateWindow(
            800,
            600,
            "OpenGL 3D",
            nullptr,
            nullptr
        );


    if (window == nullptr)
    {
        std::cerr
            << "Failed to create GLFW window"
            << std::endl;

        glfwTerminate();

        return -1;
    }


    // Activamos el contexto de OpenGL
    glfwMakeContextCurrent(window);


    // ========================================================
    // INICIALIZACIÓN DE GLAD
    // ========================================================

    if (!gladLoadGLLoader(
        (GLADloadproc)glfwGetProcAddress))
    {
        std::cerr
            << "Failed to initialize GLAD"
            << std::endl;

        return -1;
    }


    // ========================================================
    // VIEWPORT
    // ========================================================

    int width;
    int height;

    glfwGetFramebufferSize(
        window,
        &width,
        &height
    );

    glViewport(
        0,
        0,
        width,
        height
    );


    // ========================================================
    // DEPTH TEST
    // ========================================================

    // Permite que OpenGL determine qué objetos están
    // delante y cuáles están detrás.

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);


    // ========================================================
    // VÉRTICES
    // ========================================================

    // Tenemos 3 triángulos planos.
    // Cada triángulo tiene un tamaño diferente.
    //
    // Cada vértice contiene:
    //
    // 3 valores -> posición X,Y,Z
    // 4 valores -> color R,G,B,A
    // 2 valores -> coordenadas de textura U,V
    //
    // Total = 9 valores por vértice.


    GLfloat vertices[] =
    {
        // ====================================================
        // TRIÁNGULO PEQUEÑO
        // posición              color                 UV
        // ====================================================

        -0.30f, -0.20f,  0.20f,
         1, 1, 1, 1,
         0.0f, 0.0f,

         0.10f, -0.20f,  0.20f,
         1, 1, 1, 1,
         1.0f, 0.0f,

        -0.10f,  0.20f,  0.20f,
         1, 1, 1, 1,
         0.5f, 1.0f,


         // ====================================================
         // TRIÁNGULO MEDIANO
         // ====================================================

         -0.40f, -0.35f,  0.00f,
          1, 1, 1, 1,
          0.0f, 0.0f,

          0.40f, -0.35f,  0.00f,
          1, 1, 1, 1,
          1.0f, 0.0f,

          0.00f,  0.40f,  0.00f,
          1, 1, 1, 1,
          0.5f, 1.0f,


          // ====================================================
          // TRIÁNGULO GRANDE
          // ====================================================

          -0.60f, -0.50f, -0.20f,
           1, 1, 1, 1,
           0.0f, 0.0f,

           0.60f, -0.50f, -0.20f,
           1, 1, 1, 1,
           1.0f, 0.0f,

           0.00f,  0.60f, -0.20f,
           1, 1, 1, 1,
           0.5f, 1.0f
    };


    // ========================================================
    // ÍNDICES
    // ========================================================

    // Cada grupo de 3 índices forma un triángulo.

    GLuint indices[] =
    {
        0, 1, 2,       // Triángulo pequeño
        3, 4, 5,       // Triángulo mediano
        6, 7, 8        // Triángulo grande
    };


    // ========================================================
    // CREACIÓN DE LOS SHADERS
    // ========================================================


    // --------------------------------------------------------
    // VERTEX SHADER
    // --------------------------------------------------------

    GLuint vertexShader =
        glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        nullptr
    );

    glCompileShader(vertexShader);


    // --------------------------------------------------------
    // FRAGMENT SHADER
    // --------------------------------------------------------

    GLuint fragmentShader =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        nullptr
    );

    glCompileShader(fragmentShader);


    // ========================================================
    // SHADER PROGRAM
    // ========================================================

    GLuint shaderProgram =
        glCreateProgram();

    glAttachShader(
        shaderProgram,
        vertexShader
    );

    glAttachShader(
        shaderProgram,
        fragmentShader
    );

    glLinkProgram(shaderProgram);


    // Los shaders individuales ya no son necesarios
    // después de enlazarlos al programa.

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // ========================================================
    // VAO, VBO Y EBO
    // ========================================================

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;


    // Generamos los objetos
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    // ========================================================
    // VAO
    // ========================================================

    // Activamos el VAO
    glBindVertexArray(VAO);


    // ========================================================
    // VBO
    // ========================================================

    // Activamos el VBO
    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    // Guardamos los vértices en el VBO
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );


    // ========================================================
    // EBO
    // ========================================================

    // Activamos el EBO
    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        EBO
    );


    // Guardamos los índices
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );


    // ========================================================
    // CONFIGURACIÓN DE LOS ATRIBUTOS DE LOS VÉRTICES
    // ========================================================

    // Cada vértice tiene 9 floats:
    //
    // posición = 3
    // color = 4
    // UV = 2
    //
    // Total = 9


    // --------------------------------------------------------
    // POSICIÓN
    // --------------------------------------------------------

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        9 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);


    // --------------------------------------------------------
    // COLOR
    // --------------------------------------------------------

    glVertexAttribPointer(
        1,
        4,
        GL_FLOAT,
        GL_FALSE,
        9 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);


    // --------------------------------------------------------
    // COORDENADAS DE TEXTURA
    // --------------------------------------------------------

    glVertexAttribPointer(
        3,
        2,
        GL_FLOAT,
        GL_FALSE,
        9 * sizeof(float),
        (void*)(7 * sizeof(float))
    );

    glEnableVertexAttribArray(3);


    // Desactivamos el VBO
    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );

    // Desactivamos el VAO
    glBindVertexArray(0);


    // ========================================================
    // RUTAS DE LAS TEXTURAS
    // ========================================================

    const char* texturePaths[3] =
    {
        "images/texture1.jpeg",
        "images/texture2.jpeg",
        "images/texture3.jpeg"
    };


    // ========================================================
    // CREACIÓN DE LAS TEXTURAS
    // ========================================================

    GLuint textures[3];

    glGenTextures(
        3,
        textures
    );


    // Cargamos las tres imágenes
    LoadTexturesFromFiles(
        texturePaths,
        3,
        textures
    );


    // ========================================================
    // UBICACIÓN DE LOS UNIFORMS
    // ========================================================

    GLint mvpLoc =
        glGetUniformLocation(
            shaderProgram,
            "uMVP"
        );

    GLint texLoc =
        glGetUniformLocation(
            shaderProgram,
            "texture1"
        );


    // Activamos el programa de shaders
    glUseProgram(shaderProgram);


    // Utilizamos la unidad de textura 0
    glActiveTexture(GL_TEXTURE0);


    // Le indicamos al sampler que utilice
    // la unidad de textura 0.

    if (texLoc >= 0)
    {
        glUniform1i(
            texLoc,
            0
        );
    }


    // ========================================================
    // BUCLE PRINCIPAL
    // ========================================================

    while (!glfwWindowShouldClose(window))
    {
        // ----------------------------------------------------
        // LIMPIAR LA PANTALLA
        // ----------------------------------------------------

        glClearColor(
            0.07f,
            0.13f,
            0.17f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        // ----------------------------------------------------
        // ACTIVAR SHADER Y VAO
        // ----------------------------------------------------

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);


        // ----------------------------------------------------
        // MATRIZ DE PROYECCIÓN
        // ----------------------------------------------------

        glm::mat4 projection =
            glm::perspective(
                glm::radians(60.0f),

                // Relación correcta de la ventana
                (float)width / (float)height,

                0.1f,
                100.0f
            );


        // ----------------------------------------------------
        // MATRIZ DE VISTA
        // ----------------------------------------------------

        glm::mat4 view =
            glm::lookAt(
                glm::vec3(
                    0.0f,
                    0.0f,
                    3.0f
                ),

                glm::vec3(
                    0.0f,
                    0.0f,
                    0.0f
                ),

                glm::vec3(
                    0.0f,
                    1.0f,
                    0.0f
                )
            );


        // ----------------------------------------------------
        // MATRIZ DE MODELO
        // ----------------------------------------------------

        glm::mat4 model =
            glm::mat4(1.0f);


        // Tiempo transcurrido
        float t =
            (float)glfwGetTime();


        // Rotación de los tres triángulos
        model =
            glm::rotate(
                model,
                t * glm::radians(30.0f),
                glm::vec3(
                    0.0f,
                    1.0f,
                    0.0f
                )
            );


        // ----------------------------------------------------
        // MATRIZ MVP
        // ----------------------------------------------------

        glm::mat4 mvp =
            projection *
            view *
            model;


        // Enviamos la matriz al Vertex Shader
        glUniformMatrix4fv(
            mvpLoc,
            1,
            GL_FALSE,
            glm::value_ptr(mvp)
        );


        // ====================================================
        // TRIÁNGULO 1
        // ====================================================

        glActiveTexture(GL_TEXTURE0);

        glBindTexture(
            GL_TEXTURE_2D,
            textures[0]
        );

        glDrawElements(
            GL_TRIANGLES,
            3,
            GL_UNSIGNED_INT,
            (void*)0
        );


        // ====================================================
        // TRIÁNGULO 2
        // ====================================================

        glActiveTexture(GL_TEXTURE0);

        glBindTexture(
            GL_TEXTURE_2D,
            textures[1]
        );

        glDrawElements(
            GL_TRIANGLES,
            3,
            GL_UNSIGNED_INT,
            (void*)(3 * sizeof(GLuint))
        );


        // ====================================================
        // TRIÁNGULO 3
        // ====================================================

        glActiveTexture(GL_TEXTURE0);

        glBindTexture(
            GL_TEXTURE_2D,
            textures[2]
        );

        glDrawElements(
            GL_TRIANGLES,
            3,
            GL_UNSIGNED_INT,
            (void*)(6 * sizeof(GLuint))
        );


        // ----------------------------------------------------
        // DESACTIVAR VAO
        // ----------------------------------------------------

        glBindVertexArray(0);


        // ----------------------------------------------------
        // MOSTRAR EL RESULTADO
        // ----------------------------------------------------

        glfwSwapBuffers(window);

        // Procesar eventos del teclado, mouse, ventana, etc.
        glfwPollEvents();
    }


    // ========================================================
    // LIBERACIÓN DE RECURSOS
    // ========================================================

    glDeleteTextures(
        3,
        textures
    );

    glDeleteVertexArrays(
        1,
        &VAO
    );

    glDeleteBuffers(
        1,
        &VBO
    );

    glDeleteBuffers(
        1,
        &EBO
    );

    glDeleteProgram(
        shaderProgram
    );


    // ========================================================
    // FINALIZACIÓN
    // ========================================================

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}

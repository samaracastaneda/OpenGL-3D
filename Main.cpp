#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>



// SHADERS
// Vertex Shader
// Se encarga de recibir la posición de cada vértice
// y aplicar las transformaciones 3D.
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"

// Matriz del objeto
"uniform mat4 model;\n"

// Matriz de la cámara
"uniform mat4 view;\n"

// Matriz de perspectiva
"uniform mat4 projection;\n"

"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\0";


// Fragment Shader
// Se encarga de determinar el color de la figura.
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 color;\n"
"void main()\n"
"{\n"
"   FragColor = color;\n"
"}\n\0";


int main()
{
    // INICIALIZAR GLFW
    glfwInit();

    // Configuramos OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Utilizamos el perfil Core
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    
    // VÉRTICES
// Tenemos 3 triángulos planos.
// Cada triángulo tiene un tamaño diferente.
// La coordenada Z permite colocarlos a diferentes profundidades.

    GLfloat vertices[] = {
        // TRIÁNGULO 1 - PEQUEÑO
        -0.20f, -0.15f,  0.20f,   // 0
         0.20f, -0.15f,  0.20f,   // 1
         0.00f,  0.20f,  0.20f,   // 2        
        // TRIÁNGULO 2 - MEDIANO        
         -0.35f, -0.25f,  0.00f,   // 3
          0.35f, -0.25f,  0.00f,   // 4
          0.00f,  0.35f,  0.00f,   // 5        
        // TRIÁNGULO 3 - GRANDE   
          -0.50f, -0.35f, -0.20f,   // 6
           0.50f, -0.35f, -0.20f,   // 7
           0.00f,  0.50f, -0.20f    // 8
    };

    // CREACIÓN DE LA VENTANA
    GLFWwindow* window = glfwCreateWindow(
        800,
        800,
        "OpenGL 3D",
        NULL,
        NULL
    );

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;

        glfwTerminate();

        return -1;
    }

    // Activamos el contexto de OpenGL
    glfwMakeContextCurrent(window);

    // INICIALIZACIÓN DE GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }


    // Área donde se renderiza
    glViewport(0, 0, 800, 800);  
    // DEPTH TEST
    // Permite que OpenGL sepa qué partes están
    // delante y cuáles están detrás.
    glEnable(GL_DEPTH_TEST);
    // CREACIÓN DE LOS SHADERS
    // Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        NULL
    );

    glCompileShader(vertexShader);


    // Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        NULL
    );

    glCompileShader(fragmentShader);


    
    // SHADER PROGRAM
    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);

    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);


    // IMPORTANTE:
    // Para eliminar shaders usamos glDeleteShader
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ÍNDICES
 // Cada grupo de 3 índices forma un triángulo.

    GLuint indices[] =
    {
        // Triángulo pequeño
        0, 1, 2,

        // Triángulo mediano
        3, 4, 5,

        // Triángulo grande
        6, 7, 8
    };


    
    // VAO, VBO Y EBO
    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    glGenBuffers(1, &EBO);


    // Activamos el VAO
    glBindVertexArray(VAO);


      // VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );


   
    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );


    // CONFIGURACIÓN DE LOS VÉRTICES
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // UBICACIÓN DE LAS MATRICES
    glUseProgram(shaderProgram);

    GLint modelLoc =
        glGetUniformLocation(shaderProgram, "model");

    GLint viewLoc =
        glGetUniformLocation(shaderProgram, "view");

    GLint projectionLoc =
        glGetUniformLocation(shaderProgram, "projection");
   
    // MATRIZ VISTA-CAMERA
    glm::mat4 view = glm::mat4(1.0f);

    view = glm::translate(
        view,
        glm::vec3(0.0f, 0.0f, -2.0f)
    );
   
    // MATRIZ PROJECTION
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        800.0f / 800.0f,
        0.1f,
        100.0f
    );
    
    // BUCLE PRINCIPAL
    while (!glfwWindowShouldClose(window))
    {

        // Limpiamos el color y el Depth Buffer
        glClearColor(
            0.941f,
            0.502f,
            0.502f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        // Activamos el shader
        glUseProgram(shaderProgram);


        
        // MATRIZ MODEL
        glm::mat4 model = glm::mat4(1.0f);


        // Rotación en el eje Y
        model = glm::rotate(
            model,
            (float)glfwGetTime(),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );


        // También podemos inclinar ligeramente
        // el objeto sobre el eje X.
        model = glm::rotate(
            model,
            glm::radians(-15.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );


        // Enviamos MODEL al Vertex Shader
        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );


        // Enviamos VIEW
        glUniformMatrix4fv(
            viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );


        // Enviamos PROJECTION
        glUniformMatrix4fv(
            projectionLoc,
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );
  
        // COLOR
        GLint colorLocation =
            glGetUniformLocation(
                shaderProgram,
                "color"
            );


        // Color naranja
        glUniform4f(
            colorLocation,
            0.949f,
            0.604f,
            0.180f,
            1.0f
        );      
        // DIBUJAR       
       glBindVertexArray(VAO);
        glDrawElements(
            GL_TRIANGLES,
            9,
            GL_UNSIGNED_INT,
            0
        );       
        // MOSTRAR EN PANTALLA       
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
   
    // LIBERACIÓN DE RECURSOS

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
   
    // FINALIZACIÓN
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

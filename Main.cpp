#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// SHADERS
// VERTEX SHADER
// Se encarga de recibir la posición de cada vértice
// y aplicar la transformación 3D mediante la matriz MVP.
const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"

// Matriz Model-View-Projection
"uniform mat4 uMVP;\n"
"void main()\n"
"{\n"
"   gl_Position = uMVP * vec4(aPos, 1.0);\n"
"}\0";


// FRAGMENT SHADER
// Se encarga de determinar el color de los triángulos.
const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"

// Color enviado desde C++
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
    // Reservamos 24 bits para el Depth Buffer
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    
    // CREACIÓN DE LA VENTANA    
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL 3D", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Activamos el contexto de OpenGL
    glfwMakeContextCurrent(window);

    // INICIALIZACIÓN DE GLAD   
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // VIEWPORT
    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
   
    // DEPTH TEST    
    // Permite que OpenGL determine qué objetos están delante y cuáles están detrás.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // VÉRTICES    
    // Tenemos 3 triángulos planos.   
    // Cada triángulo tiene un tamaño diferente.   
    // La coordenada Z permite colocarlos a diferentes
    //profundidades dentro del espacio 3D.   
    // Triángulo 1: 0, 1, 2    
    // Triángulo 2: 3, 4, 5    
    // Triángulo 3: 6, 7, 8 
   
    GLfloat vertices[] =
    {
        // TRIÁNGULO 1 - PEQUEÑO
        -0.35f, -0.25f,  0.25f,    // 0
         0.35f, -0.25f,  0.25f,    // 1
         0.00f,  0.35f,  0.25f,    // 2        
        // TRIÁNGULO 2 - MEDIANO        
        -0.50f, -0.35f,  0.00f,    // 3
         0.50f, -0.35f,  0.00f,    // 4
         0.00f,  0.50f,  0.00f,    // 5       
        // TRIÁNGULO 3 - GRANDE        
        -0.65f, -0.45f, -0.25f,    // 6
         0.65f, -0.45f, -0.25f,    // 7
         0.00f,  0.65f, -0.25f     // 8
    };
    
    // ÍNDICES
    // Cada grupo de 3 índices forma un triángulo.
    GLuint indices[] =
    {       
        0, 1, 2, // Triángulo pequeño       
        3, 4, 5, // Triángulo mediano        
        6, 7, 8 // Triángulo grande
    };
   
    // CREACIÓN DE LOS SHADERS 
    // VERTEX SHADER
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource,nullptr);
    glCompileShader(vertexShader);
 
    // FRAGMENT SHADER    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);    
    glCompileShader(fragmentShader);
    
    // SHADER PROGRAM  
    GLuint shaderProgram =  glCreateProgram();
    glAttachShader( shaderProgram, vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);

    // Los shaders individuales ya no son necesarios después de enlazarlos al programa.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // VAO, VBO Y EBO
    GLuint VAO, VBO, EBO;
    // Generamos los objetos
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
  
    // VAO  
    // Activamos el VAO
    glBindVertexArray(VAO);
    
    // VBO  
    // Activamos el VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Guardamos los vértices en el VBO
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    
    // EBO   
    // Activamos el EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // Guardamos los índices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
    
    // CONFIGURACIÓN DE LOS VÉRTICES  
    // Cada vértice tiene: X,Y , Z
    // Por eso utilizamos 3 componentes.
    glVertexAttribPointer( 0,3,GL_FLOAT,GL_FALSE, 3 * sizeof(float), (void*)0); 
    glEnableVertexAttribArray(0);

    // Desactivamos el VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Desactivamos el VAO
    glBindVertexArray(0);   
    // UBICACIÓN DE LA MATRIZ MVP    
    glUseProgram(shaderProgram);
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");
        
    // BUCLE PRINCIPAL    
    while (!glfwWindowShouldClose(window))
    {     
        // LIMPIAR PANTALLA       
        glClearColor(
          0.05f, 0.07f, 0.12f, 1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );
       
        // ACTIVAR SHADER      
        glUseProgram(shaderProgram);      
        // OBTENER TAMAÑO DE LA VENTANA  
        glfwGetFramebufferSize(
            window,
            &width,
            &height
        );

        // Evitamos dividir entre cero
        float aspect = width > 0 ? (float)width / (float)height: 4.0f / 3.0f;
       
        // MATRIZ PROJECTION       
        // glm::perspective crea una perspectiva 3D.
        // 60 grados = campo de visión
        // aspect    = relación ancho / alto
        // 0.1       = distancia mínima
        // 100       = distancia máxima
        glm::mat4 projection =
            glm::perspective(
                glm::radians(60.0f),
                aspect,
                0.1f,
                100.0f
            );
       
        // MATRIZ VISTA-CAMERA
        // glm::lookAt crea la cámara.       
        // Primer vector: posición de la cámara        
        // Segundo vector: punto hacia donde mira       
        // Tercer vector: dirección que representa "arriba"
        glm::mat4 view =
            glm::lookAt(
                glm::vec3(0.0f, 0.0f, 3.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
        
        // MATRIZ MODELO   
        // glm::mat4(1.0f) crea una matriz identidad.
        glm::mat4 model = glm::mat4(1.0f);

        // Obtenemos el tiempo que lleva ejecutándose el programa.
        float t = (float)glfwGetTime();

        // Rotamos los tres triángulos juntos
        // alrededor del eje Y.
        model =
            glm::rotate(
                model,
                t * glm::radians(15.0f),
                glm::vec3(0.0f, 1.0f,0.0f)
            );
        
        // MATRIZ MVP      
        // Combinamos las tres transformaciones:
        // Projection
        // View
        // Model
        glm::mat4 mvp = projection * view * model;        
        // ENVIAR MVP AL VERTEX SHADER      
        glUniformMatrix4fv(mvpLoc,1, GL_FALSE, glm::value_ptr(mvp));
     
        // COLOR
        GLint colorLocation = glGetUniformLocation(shaderProgram,"color");

        // DIBUJAR TRIÁNGULO PEQUEÑO
        glUniform4f(
            colorLocation,
            1.0f,    // Rojo
            0.3f,    // Verde
            0.2f,    // Azul
            1.0f
        );

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,3, GL_UNSIGNED_INT, (void*)0);
        // DIBUJAR TRIÁNGULO MEDIANO
        glUniform4f(
            colorLocation,
            0.2f,    // Rojo
            0.8f,    // Verde
            0.3f,    // Azul
            1.0f
        );

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3 * sizeof(GLuint)));
        // DIBUJAR TRIÁNGULO GRANDE
        glUniform4f(
            colorLocation,
            0.2f,    // Rojo
            0.5f,    // Verde
            1.0f,    // Azul
            1.0f
        );

       glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));
 
        // MOSTRAR EN PANTALLA      
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
  
    // LIBERACIÓN DE RECURSOS  
    glDeleteVertexArrays( 1, &VAO);
    glDeleteBuffers( 1, &VBO);
    glDeleteBuffers( 1, &EBO);
    glDeleteProgram( shaderProgram );
   
    // FINALIZACIÓN
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <cmath>

//SHADERS
// Vertex Shader:
// Se encarga de recibir la posición de cada vértice y
// determinar dónde se ubicará dentro de la ventana.
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";
// Fragment Shader:
// Se encarga de determinar el color que tendrá cada
// fragmento/píxel de la figura.
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 color;\n"
    "void main()\n"
    "{\n"
    "   FragColor = color;\n"  
	"}\n\0";


int main()
{
    // Inicializamos GLFW (necesario para crear ventanas y manejar OpenGL.
    // GLFW permite crear y administrar la ventana y el
    // contexto de OpenGL.
    glfwInit();


    // Configuramos la versión de OpenGL que queremos usar (3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Indicamos que usaremos el perfil Core de OpenGL
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // VÉRTICES
    // Cada vértice tiene 3 valores:
    // X, Y, Z.
    // Vértices principales del triángulo exterior
    GLfloat vertices[] = {
    -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // vertice izquierdo 0 
     0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // vértice derecho  1
     0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,  //vértice superior  2

     //Vértices internos
     -0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // 3
     0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // 4
     0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f // 5
    };

    // CREACIÓN DE LA VENTANA
    //Creación de la ventana de 800x800 píxeles 
    GLFWwindow* window = glfwCreateWindow(800, 800, "YouTubeOpenGL", NULL, NULL);
    // Se verifica que la ventana se haya creado correctamente
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Activamos el contexto de OpenGL en la ventana creada
    glfwMakeContextCurrent(window);

    // INICIALIZACIÓN DE GLAD
    // Inicializamos GLAD para poder usar las funciones de OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }
    //Área dentro de la ventana donde se renderizará (en este caso, toda la ventana)    
    glViewport(0, 0, 800, 800);

   
    // CREACIÓN DE LOS SHADERS
    // Vertex Shader 
    // Creamos un objeto de tipo Vertex Shader./Posición
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Fragment Shader
    // Creamos un objeto de tipo Fragment Shader.s //Color
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // SHADER PROGRAM
    // Creamos un programa que permitirá utilizar
    // conjuntamente los dos shaders.
    GLuint shaderProgram = glCreateProgram();

    // Adjuntamos el Vertex Shader
    glAttachShader(shaderProgram, vertexShader);
    // Adjuntamos el Fragment Shader.
    glAttachShader(shaderProgram, fragmentShader);
    // Enlazamos ambos shaders en un solo programa.
    glLinkProgram(shaderProgram);

    glDeleteBuffers(1, &vertexShader); // Ya no necesitamos los shaders individuales después de enlazarlos al programa
    glDeleteBuffers(1, &fragmentShader); // Ya no necesitamos los shaders individuales después de enlazarlos al programa

    // ÍNDICES
    // Los índices indican qué vértices se utilizan
    // para construir cada triángulo.
    // Cada grupo de 3 índices forma un triángulo.
    GLuint indices[] =
    {
        0, 3, 5,   // triángulo inferior izquierdo
        3, 2, 4,   // triángulo superior
        5, 4, 1    // triángulo inferior derecho
    };

    // VAO, VBO Y EBO
	GLuint VAO, VBO, EBO;

    // Generamos los objetos de OpenGL.
	glGenVertexArrays(1, &VAO); 
	glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);

    // VBO
    // El VBO almacena los datos de los vértices.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copiamos los vértices desde la memoria del programa
    // hacia la memoria utilizada por OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  

    // EBO
    // El EBO almacena los índices.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // Copiamos los índices hacia el EBO.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // CONFIGURACIÓN DE LOS VÉRTICES
    // Indicamos a OpenGL cómo interpretar los datos
    // almacenados en el VBO.
    // location = 0
    // Cada vértice tiene 3 valores
    // Los valores son float
    // 3 * sizeof(float) = tamaño de un vértice
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
    // BUCLE PRINCIPAL
    // El programa continúa ejecutándose mientras la ventana no se cierre
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.941f, 0.502f, 0.502f, 1.0f); //Color de fondo
        glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram); // Activamos el VAO que contiene la configuración de nuestros vértices.
		glBindVertexArray(VAO);
        // Obtener la ubicación de la variable color
        GLint colorLocation = glGetUniformLocation(shaderProgram, "color");

        // Color naranja para los triángulos
        glUniform4f(colorLocation, 0.949f, 0.604f, 0.180f, 1.0f);

        // Dibujar los triángulos
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
        // Color diferente para los vértices
        glUniform4f(colorLocation, 0.0f, 0.0f, 0.0f, 1.0f);

        // Hacer los puntos más grandes
        glPointSize(10.0f);

        // Dibujar los 6 vértices
        glDrawArrays(GL_POINTS, 0, 6);
        // Mostramos en pantalla el resultado renderizado.
        glfwSwapBuffers(window);


        glfwPollEvents();
    }

    // LIBERACIÓN DE RECURSOS
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
	
	glDeleteProgram(shaderProgram);


    // FINALIZACIÓN
    // Destruimos la ventana antes de cerrar el programa
    glfwDestroyWindow(window);
	// Finalizamos GLFW para liberar los recursos utilizados
    glfwTerminate();
	return 0;
}

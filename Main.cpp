#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.949f, 0.604f, 0.180f, 1.0f);\n"
	"}\n\0";


int main()
{
    // Inicializamos GLFW (necesario para crear ventanas y manejar OpenGL)
	glfwInit();

    // Configuramos la versión de OpenGL que queremos usar (3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Indicamos que usaremos el perfil Core de OpenGL
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLfloat vertices[] = {
	-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // vertice izquierdo  
	 0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // vértice derecho  
	 0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,  //vértice superior  
	 -0.5f /2, 0.5f * float (sqrt(3))  / 6, 0.0f,
	 0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,
	 0.0f, 0.5f * float(sqrt(3)) / 3, 0.0f, //inner down
	};

	GLuint indices[] =
	{
		0, 3, 5, //lower left triangle
		3, 2, 4, //lower right triangle
		5, 4, 1 //uper triangle
	};




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

    // Inicializamos GLAD para poder usar las funciones de OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }
	//Área dentro de la ventana donde se renderizará (en este caso, toda la ventana)    
    glViewport(0, 0, 800, 800);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);  

	glDeleteBuffers(1, &vertexShader); // Ya no necesitamos los shaders individuales después de enlazarlos al programa
	glDeleteBuffers(1, &fragmentShader); // Ya no necesitamos los shaders individuales después de enlazarlos al programa


	GLuint VAO, VBO, EBO;


	glGenVertexArrays(1, &VAO); 
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);


	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);



	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.941f, 0.502f, 0.502f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 9,GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);


        glfwPollEvents();
    }

	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);



    // Destruimos la ventana antes de cerrar el programa
    glfwDestroyWindow(window);
	// Finalizamos GLFW para liberar los recursos utilizados
    glfwTerminate();
	return 0;
}
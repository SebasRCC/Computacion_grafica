#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

//Callback para ajustar el tama�o del framebuffer cuando se redimensiona la ventana.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
// Definici�n de la ventana
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// Aqu� se define el vertex shader ("dibujo")
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   //gl_Position = vec4( x, y, z, dist_cam);\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.2);\n"
"}\0";
// Aqu� se define el fragment shader (colores)
const char* fragmentShaderSource = "#version 330 core\n"
"out vec3 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec3(0.043f, 0.427f, 0.839f);\n"
"}\n\0";

int main()
{
	// Inicializa GLFW
    glfwInit();
	// Configura GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Se crea la ventana (con el tama�o definido arriba).
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sebastian Ramos 1804540159", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
	// Se hace el contexto de OpenGL actual
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Inicializa GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

	// Compilaci�n del vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
		// Aviso de error en la compilaci�n
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
	// Compilaci�n del fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
		// Aviso de error en la compilaci�n
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

	// Se crea el shader program (equivalente a un .exe) 
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
	// Se verifica si hubo error en el linking
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

	/*
	Aqu� se definen del pol�gono. En mi caso es un hex�gono.
	El hex�gono se dibuja con un TRIANGLE_FAN, que es un tri�ngulo que comparte el primer v�rtice con los dem�s, 
	por eso se ponen dos veces el primer v�rtice.
	El primer v�rtice es el centro del hex�gono, y los siguientes son los v�rtices
    */
    float vertices[] = {
		// Centro del hex�gono
		 0.0f, 0.0f, 0.0f,
		 // Vertices del hex�gono
         0.43f, 0.55f, 0.0f,
		-0.26, 0.65f, 0.0f,
        -0.69f,  0.1f, 0.0f,  
        -0.43f, -0.55f, 0.0f,  
		 0.26f, -0.65f, 0.0f,  
		 0.69f, -0.1f, 0.0f,  
		 // De nuevo el primero, para volver a "cerrar" el hex�gono
		 0.43f, 0.55f, 0.0f,
    };

    // Creaci�n del VAO y el VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
	// Se enlaza el buffer de v�rtices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Se enlaza el buffer de v�rtices al VAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);


	// Aqu� empieza el loop de renderizado.
	/* El loop de renderizado es el ciclo infinito que se encarga de dibujar la escena.
	* En este caso, se encarga de dibujar el hex�gono.
	* Se ejecuta hasta que la ventana se cierra.
    */
    while (!glfwWindowShouldClose(window))
    {
        // Procesa la entrada de teclado
        processInput(window);

		// Renderiza
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
		// Activa el shader program
        glUseProgram(shaderProgram);
		// Dibuja el hex�gono usando el VAO
        glBindVertexArray(VAO); 
		// Aqu� se usa el TRIANGLE_FAN para dibujar el hex�gono y 8 puntos (6 v�rtices + 1 centro + 1 repetido)
        glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
       
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	// Limpia los recursos
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    return 0;
}

// Funci�n para procesar la entrada de teclado
void processInput(GLFWwindow* window)
{
	// Si se presiona la tecla ESC, se cierra la ventana
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Callback para ajustar el tama�o del framebuffer cuando se redimensiona la ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

    glViewport(0, 0, width, height);
}
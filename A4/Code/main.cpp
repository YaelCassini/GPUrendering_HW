#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glut.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

// ��������
void framebuffer_size_callback(GLFWwindow * window, int width, int height);
void windowResize_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow * window);
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);


// ��Ļ�ߴ�
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;
GLuint WIDTH = 800, HEIGHT = 600;
// ��Դλ��
GLfloat lightPos[4] = { -30.0,200.0,100.0,0 };
// ������ת
bool isRotate = true;
float rotateangle = 0.0f;
// ����ϸ�ּ���
int times = 3;
// ������ɫ��ʽ
int renderindex = 0;

// ������
int main()
{
     // ��ʼ��GLFW
     glfwInit();
     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // ��������
    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "Homework Four", NULL, NULL);
    if (window == NULL)
    {
		cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // ���ûص�����
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, windowResize_callback);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ��ʼ��GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        cout << "Failed to initialize GLEW!" << endl;
        return -1;
    }


    // ����GLSL���벢����shader program
	Shader testshader("shaders/subdivision.vert", "shaders/subdivision.frag", "shaders/subdivision.geom");
    
    // ��������������λ��
    float vertices[] = {
        1.0f,  0.0f,  0.0f,
        0.0f,  1.0f,  0.0f, 
        0.0f,  0.0f,  1.0f, 
    };

    // VAO��VBO���ü���������
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ��������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);


    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);


    // ����
    while (!glfwWindowShouldClose(window))
    {
        
        processInput(window);
        glfwPollEvents();
		
        // ����
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // ʹ��GLM����������任����
        // ��ʼ��model��view��projection����
        glm::mat4 model = glm::mat4(1.0f); 
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);


        // model���󣺰���rotate��scale��transform
        // ��object�ռ䵽����ռ�
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
        model = glm::rotate(model, glm::radians(-15.0f - rotateangle), glm::vec3(0.0, 1.0, 0.0));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        
        // view����
        // ������ռ䵽�۲�ռ�
        view = glm::lookAt(glm::vec3(0.0f, 0.8f, 2.0f), glm::vec3(0.0f, 0.2f, 2.0f) + glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // glm::mat4 viewMatrix = glm::lookAt(camera.position, camera.position + CameraForward(), glm::vec3(0.0f, 1.0f, 0.0f));


        // projection����
        // ͶӰ����
        projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 300.0f);


        // ʹ��shader
        testshader.use();
        // ����uniform����
        glUniform1i(glGetUniformLocation(testshader.ID, "times"), times);
        glUniform1i(glGetUniformLocation(testshader.ID, "renderindex"), renderindex);
        glUniform4f(glGetUniformLocation(testshader.ID, "lightPos"), lightPos[0], lightPos[1], lightPos[2], lightPos[3]);

        // ����任����
        unsigned int modelLoc = glGetUniformLocation(testshader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        unsigned int viewLoc = glGetUniformLocation(testshader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        unsigned int projectionLoc = glGetUniformLocation(testshader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
        
        
        // ��VAO
        glBindVertexArray(VAO);
        // ����������
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // ������ת
        if (isRotate) rotateangle = rotateangle + 0.03f;;

        glfwSwapBuffers(window);      
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // Terminate GLFW, clearing any resources allocated by GLFW.    
    glfwTerminate();
    return 0;

}

void processInput(GLFWwindow * window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        times = (times + 1) % 4;
        // cout << "times:" << times << endl;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        times = (times + 3) % 4;
        // cout << "times:" << times << endl;
    }

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        renderindex = (renderindex + 1) % 3;
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        isRotate = !isRotate;
    }
}

void windowResize_callback(GLFWwindow* window, int width, int height)
{
    //���½��ǣ�0,0��
    WIDTH = HEIGHT = width > height ? height : width;    
    glViewport((width - WIDTH) / 2, (height - HEIGHT) / 2, WIDTH, HEIGHT);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
     // make sure the viewport matches the new window dimensions; note that width and 
     // height will be significantly larger than specified on retina displays.
     glViewport(0, 0, width, height);
}


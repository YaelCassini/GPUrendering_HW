#define _CRT_SECURE_NO_WARNINGS
// ͷ�ļ�����
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

// ��������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void configureFBO(GLuint* FBO, GLuint textures);
GLuint loadShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
void checkCompileErrors(unsigned int shader, std::string type);
GLuint loadBMPTexture(const char* imagepath);

// ȫ�ֱ���
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;
int numSamples = 4;
bool isOutline = false;
int texindex = 0;


int main(int argc, char** argv)
{
	// ��ʼ��glfw
	glfwInit();

	// ʹ�� OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //�ΰ汾��
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// MSAA ���ز���
	glfwWindowHint(GLFW_SAMPLES, numSamples);

	// ��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Homework Three", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);
	// ���ûص�����
	glfwSetKeyCallback(window, key_callback);

	// ��ʼ�� glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ������ɫ������
	GLuint texShader = loadShader("Shaders/texShading.vert", "Shaders/texShading.frag");
	GLuint sobelShader = loadShader("Shaders/RenderQuad.vert", "Shaders/SobelOutline.frag");

	// ������ͼ
	vector<GLuint> textures;
	GLuint screentex;
	textures.push_back(loadBMPTexture("./Textures/bean.bmp"));
	textures.push_back(loadBMPTexture("./Textures/train.bmp"));
	textures.push_back(loadBMPTexture("./Textures/bush.bmp"));
	textures.push_back(loadBMPTexture("./Textures/women.bmp"));

	screentex = textures[0];

	// һ��������Ļ�ľ��εĶ������꼰��������
	float quadVertices[] = {
		// ��������             // ��������
		-1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,     1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
	};


	// �ܹ�������Ļ��VAO��VBO
	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	// �����ݴ��������VBO��
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


	// ����������frame buffer����
	GLuint intermediateFBO, imageTexture;
	glGenTextures(1, &imageTexture);
	configureFBO(&intermediateFBO, imageTexture);

	// ����
	while (!glfwWindowShouldClose(window))
	{
		screentex = textures[texindex];
		// ����
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// ѡ����ɫ��������ɫ�����ã��򵥵ؽ������ͼ��ƽ�̵�������Ļ
		glUseProgram(texShader);
		// �������VAO
		glBindTexture(GL_TEXTURE_2D, screentex);
		glBindVertexArray(quadVAO);
		//����
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


		// ��ƽ���������Ⱦ��֡ͼ����뻺��
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		if (isOutline)
		{
			// ����
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			// ѡ����ɫ�����������
			glUseProgram(sobelShader);
			glUniform4f(glGetUniformLocation(sobelShader, "_OutlineColor"), 1.0f, 1.0f, 1.0f, 1.0f); // �����ɫ
			glUniform1f(glGetUniformLocation(sobelShader, "_OutlineStrength"), 4.0f);  // ���ǿ��

			glBindTexture(GL_TEXTURE_2D, imageTexture); //ƽ�̵�������Ļ������ͼ��
			// ��VAO
			glBindVertexArray(quadVAO);
			// ����
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// ����VAO��VBO�Լ�Frame Buffer
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	glDeleteFramebuffers(1, &intermediateFBO);

	// ��ֹGLFW
	glfwTerminate();
	return 0;
}


// ���̻ص�����
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		isOutline = !isOutline;
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
		texindex = (texindex + 1) % 4;
}


// ����VBO
void configureFBO(GLuint* FBO, GLuint textures)
{
	glGenFramebuffers(1, FBO);

	glBindFramebuffer(GL_FRAMEBUFFER, *FBO);

	glBindTexture(GL_TEXTURE_2D, textures);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



GLuint loadShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	// ���ļ��ж�ȡshader����
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// ȷ���ļ��������throw�쳣
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// ���ļ�����ȡ
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// �ر��ļ�
		vShaderFile.close();
		fShaderFile.close();
		// ��ʽת��
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// ��������ɫ��
		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// ����shader����
	unsigned int vertex, fragment;

	// ������ɫ��
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	// Ƭ����ɫ��
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	// ������ɫ��
	unsigned int geometry;
	if (geometryPath != nullptr)
	{
		const char* gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}

	// ��ɫ������
	GLuint ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometryPath != nullptr)
		glAttachShader(ID, geometry);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	// ɾ����ɫ��
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr)
		glDeleteShader(geometry);

	return ID;
}

// ������ɫ���������
void checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}


// ����BMP�����ļ�
GLuint loadBMPTexture(const char* imagepath)
{

	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char* data;

	// Open the file
	FILE* file = fopen(imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	// Everything is in memory now, the file wan be closed
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete[] data;

	// ... nice trilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}

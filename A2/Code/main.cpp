#define _CRT_SECURE_NO_WARNINGS

// ͷ�ļ�����
// ʹ��freeglut + glew����
#include <GL/glew.h>
#include <GL/freeglut.h>  

#include <iostream>
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
// �����ռ�
using namespace std;

#pragma comment(lib,"glew32.lib")    

// ��������
void init();
void initShader(GLuint& shaderProgram, const char* VShaderFile, const char* FShaderFile);
void SpecialKey(GLint key, GLint x, GLint y);
void NormalKey(unsigned char key, GLint x, GLint y);
void Reshape(int w, int h);
void display();
void idle();
char* textFileRead(const char* fn);


// ������
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	//��ʼ��OPENGL��ʾ��ʽ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//�趨OPENGL����λ�úʹ�С
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	//�򿪴���
	glutCreateWindow("Homework One");

	// ��ʼ������
	init();

	// ���ڴ�С�仯�ص�����
	glutReshapeFunc(Reshape);

	// ���̻ص�����
	glutSpecialFunc(SpecialKey);
	glutKeyboardFunc(NormalKey);

	// OPENGL����ѭ��
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}



// ȫ�ֱ���
GLfloat lightPos[4] = { 30.0,200,100.0,0 };
GLfloat ambient[4] = { 1.0 , 1.0 , 1.0 , 1.0 };
GLfloat lightcolor[4] = { 1.0 , 1.0 , 1.0 , 1.0 };
GLfloat eyePos[3] = { 0.0 , 0.0 , 50.0 };  // ���λ��
GLfloat center[] = { 0, 0, 0 };  // ��������

GLfloat objectSize = 15.0;
GLuint ivoryShaderProgram;
GLuint goochShaderProgram;
GLuint edgeShaderProgram;
int shaderIndex = 1;

float fRotate = 0.0f;  //��ת�Ƕȿ��Ʊ������弰��ʼ��
bool isRotate = false;  //��ת���Ʊ���

//�����ַ���  
char* textFileRead(const char* fn)
{
	FILE* fp;
	char* content = NULL;
	int count = 0;
	if (fn != NULL)
	{
		fp = fopen(fn, "rt");
		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count > 0)
			{
				content = (char*)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

void initShader(GLuint& shaderProgram, const char* VShaderFile, const char* FShaderFile)
{
	//1���鿴�Կ���GLSL��OpenGL����Ϣ    
	//const GLubyte* vendor = glGetString(GL_VENDOR);
	//const GLubyte* renderer = glGetString(GL_RENDERER);
	//const GLubyte* version = glGetString(GL_VERSION);
	//const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	//cout << "�Կ���Ӧ��   : " << vendor << endl;
	//cout << "�Կ��ͺ�     : " << renderer << endl;
	//cout << "OpenGL�汾   : " << version << endl;
	//cout << "GLSL�汾     : " << glslVersion << endl;


	//2��������ɫ��    
	//������ɫ�����󣺶�����ɫ��    
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	//������    
	if (0 == vShader)
	{
		cerr << "ERROR : Create vertex shader failed" << endl;
		exit(1);
	}
	//����ɫ��Դ�������ɫ�����������    
	const GLchar* vShaderCode = textFileRead(VShaderFile);
	const GLchar* vCodeArray[1] = { vShaderCode };

	//���ַ�����󶨵���Ӧ����ɫ��������  
	//����ɫ��Դ�븽�ӵ�vs��fs����ɫ�������ϣ��ڶ�����ָ���˴��ݵ�Դ���ַ�������
	glShaderSource(vShader, 1, vCodeArray, NULL);

	//������ɫ������    
	glCompileShader(vShader);

	//�������Ƿ�ɹ�    
	GLint compileResult;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &compileResult);
	if (GL_FALSE == compileResult)
	{
		GLint logLen;
		//�õ�������־����    
		glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = (char*)malloc(logLen);
			GLsizei written;
			//�õ���־��Ϣ�����    
			glGetShaderInfoLog(vShader, logLen, &written, log);
			cerr << "vertex shader compile log : " << endl;
			cerr << log << endl;
			free(log);//�ͷſռ�    
		}
	}

	//������ɫ������Ƭ����ɫ��    
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//������    
	if (0 == fShader)
	{
		cerr << "ERROR : Create fragment shader failed" << endl;
		exit(1);
	}

	//����ɫ��Դ�������ɫ�����������    
	const GLchar* fShaderCode = textFileRead(FShaderFile);
	const GLchar* fCodeArray[1] = { fShaderCode };
	glShaderSource(fShader, 1, fCodeArray, NULL);

	//������ɫ������    
	glCompileShader(fShader);

	//�������Ƿ�ɹ�    
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &compileResult);
	if (GL_FALSE == compileResult)
	{
		GLint logLen;
		//�õ�������־����    
		glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = (char*)malloc(logLen);
			GLsizei written;
			//�õ���־��Ϣ�����    
			glGetShaderInfoLog(fShader, logLen, &written, log);
			cerr << "fragment shader compile log : " << endl;
			cerr << log << endl;
			free(log);//�ͷſռ�    
		}
	}
	//3��������ɫ������    
	//������ɫ������    
	shaderProgram = glCreateProgram();
	if (!shaderProgram)
	{
		cerr << "ERROR : create program failed" << endl;
		exit(1);
	}
	//����ɫ���������ӵ��������ĳ�����    
	glAttachShader(shaderProgram, vShader);
	glAttachShader(shaderProgram, fShader);
	//����Щ�������ӳ�һ����ִ�г���    
	glLinkProgram(shaderProgram);
	//��ѯ���ӵĽ��    
	GLint linkStatus;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
	if (GL_FALSE == linkStatus)
	{
		cerr << "ERROR : link shader program failed" << endl;
		GLint logLen;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH,
			&logLen);
		if (logLen > 0)
		{
			char* log = (char*)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(shaderProgram, logLen,
				&written, log);
			cerr << "Program log : " << endl;
			cerr << log << endl;
		}
	}

	//�ڰ���ɫ���������ӵ���������Ժ󣬼ǵ�ɾ����ɫ������
	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

//���glew��ʼ���ͼ��ض��㡢Ƭ����ɫ��
void init()
{
	// ��ʼ��glew��չ��    
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "Error initializing GLEW: " << glewGetErrorString(err) << endl;
	}
	glEnable(GL_DEPTH_TEST);


	// ����Ƿ����ʹ��ARB��չ
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL.\n");
	else
	{
		printf("Not totally ready.\n");
		exit(1);
	}

	glEnable(GL_DEPTH_TEST);

	// ���ض����Ƭ����ɫ���������ӵ�һ�����������  
	initShader(ivoryShaderProgram, "./shaders/ivoryShader.vert", "./shaders/ivoryShader.frag");
	initShader(goochShaderProgram, "./shaders/goochShader.vert", "./shaders/goochShader.frag");
	initShader(edgeShaderProgram, "./shaders/edgeShader.vert", "./shaders/edgeShader.frag");

	// ��ջ��ƴ���
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void Reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	glLoadIdentity();
	gluLookAt(eyePos[0], eyePos[1], eyePos[2], center[0], center[1], center[2], 0, 1, 0);


	// ��һ��Pass
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glUseProgram(edgeShaderProgram);
	glUniform4f(glGetUniformLocation(edgeShaderProgram, "outlineColor"), 0.0, 1.0, 0.0, 1.0);

	// ������ת
	glPushMatrix();
	glRotatef(fRotate, 0, 1.0f, 0);
	glutSolidTeapot(objectSize);
	glPopMatrix();


	// �ڶ���Pass
	glDisable(GL_CULL_FACE);

	if (shaderIndex == 1)
	{
		glUseProgram(ivoryShaderProgram);
		glUniform4f(glGetUniformLocation(ivoryShaderProgram, "lightPos"), lightPos[0], lightPos[1], lightPos[2], lightPos[3]);
	}
	else if (shaderIndex == 2)
	{
		glUseProgram(goochShaderProgram);
		glUniform4f(glGetUniformLocation(goochShaderProgram, "lightPos"), lightPos[0], lightPos[1], lightPos[2], lightPos[3]);
		glUniform4f(glGetUniformLocation(goochShaderProgram, "ambient"), ambient[0], ambient[1], ambient[2], ambient[3]);
	}
	// ������ת
	glPushMatrix();
	glRotatef(fRotate, 0, 1.0f, 0);
	glutSolidTeapot(objectSize);
	glPopMatrix();


	if (isRotate)fRotate += 0.1f;

	glutSwapBuffers();

}


// ���̻ص�����
void SpecialKey(GLint key, GLint x, GLint y)
{
	if (key == GLUT_KEY_UP)
	{
		//do something
	}
	display();
}

void NormalKey(unsigned char key, GLint x, GLint y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	// �ı����ʹ�õ���ɫ��
	case '1':
	{
		shaderIndex = 1;
		break;
	}
	case '2':
	{
		shaderIndex = 2;
		break;
	}
	case ' ':
	{
		isRotate = !isRotate;
		break;
	}
	case 'A': case 'a':
	{
		center[0] += 0.2f;
		eyePos[0] += 0.2f;
		break;
	}
	case 'D': case 'd':
	{
		center[0] -= 0.2f;
		eyePos[0] -= 0.2f;
		break;
	}
	case 'W': case 'w':
	{
		center[1] -= 0.2f;
		eyePos[1] -= 0.2f;
		break;
	}
	case 'S': case 's':
	{
		center[1] += 0.2f;
		eyePos[1] += 0.2f;
		break;
	}
	case 'Z': case 'z':
	{
		center[2] += 0.2f;
		eyePos[2] += 0.2f;
		break;
	}
	case 'C': case 'c':
	{
		center[2] -= 0.2f;
		eyePos[2] -= 0.2f;
		break;
	}
	case 'J': case 'j':
	{
		lightPos[0] -= 5.0f;
		break;
	}
	case 'L': case 'l':
	{
		lightPos[0] += 5.0f;
		break;
	}
	case 'I': case 'i':
	{
		lightPos[1] += 5.0f;
		break;
	}
	case 'K': case 'k':
	{
		lightPos[1] -= 5.0f;
		break;
	}
	case 'M': case 'm':
	{
		lightPos[2] -= 5.0f;
		break;
	}
	case ',': case '<':
	{
		lightPos[2] += 5.0f;
		break;
	}
	}
	display();
}

void idle()
{
	glutPostRedisplay();
}


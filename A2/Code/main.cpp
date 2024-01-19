#define _CRT_SECURE_NO_WARNINGS

// 头文件引用
// 使用freeglut + glew搭建框架
#include <GL/glew.h>
#include <GL/freeglut.h>  

#include <iostream>
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
// 命名空间
using namespace std;

#pragma comment(lib,"glew32.lib")    

// 函数定义
void init();
void initShader(GLuint& shaderProgram, const char* VShaderFile, const char* FShaderFile);
void SpecialKey(GLint key, GLint x, GLint y);
void NormalKey(unsigned char key, GLint x, GLint y);
void Reshape(int w, int h);
void display();
void idle();
char* textFileRead(const char* fn);


// 主函数
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	//初始化OPENGL显示方式
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//设定OPENGL窗口位置和大小
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	//打开窗口
	glutCreateWindow("Homework One");

	// 初始化函数
	init();

	// 窗口大小变化回调函数
	glutReshapeFunc(Reshape);

	// 键盘回调函数
	glutSpecialFunc(SpecialKey);
	glutKeyboardFunc(NormalKey);

	// OPENGL绘制循环
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}



// 全局变量
GLfloat lightPos[4] = { 30.0,200,100.0,0 };
GLfloat ambient[4] = { 1.0 , 1.0 , 1.0 , 1.0 };
GLfloat lightcolor[4] = { 1.0 , 1.0 , 1.0 , 1.0 };
GLfloat eyePos[3] = { 0.0 , 0.0 , 50.0 };  // 相机位置
GLfloat center[] = { 0, 0, 0 };  // 视线中心

GLfloat objectSize = 15.0;
GLuint ivoryShaderProgram;
GLuint goochShaderProgram;
GLuint edgeShaderProgram;
int shaderIndex = 1;

float fRotate = 0.0f;  //旋转角度控制变量定义及初始化
bool isRotate = false;  //旋转控制变量

//读入字符流  
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
	//1、查看显卡、GLSL和OpenGL的信息    
	//const GLubyte* vendor = glGetString(GL_VENDOR);
	//const GLubyte* renderer = glGetString(GL_RENDERER);
	//const GLubyte* version = glGetString(GL_VERSION);
	//const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	//cout << "显卡供应商   : " << vendor << endl;
	//cout << "显卡型号     : " << renderer << endl;
	//cout << "OpenGL版本   : " << version << endl;
	//cout << "GLSL版本     : " << glslVersion << endl;


	//2、编译着色器    
	//创建着色器对象：顶点着色器    
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	//错误检测    
	if (0 == vShader)
	{
		cerr << "ERROR : Create vertex shader failed" << endl;
		exit(1);
	}
	//把着色器源代码和着色器对象相关联    
	const GLchar* vShaderCode = textFileRead(VShaderFile);
	const GLchar* vCodeArray[1] = { vShaderCode };

	//将字符数组绑定到对应的着色器对象上  
	//把着色器源码附加到vs、fs的着色器对象上，第二参数指定了传递的源码字符串数量
	glShaderSource(vShader, 1, vCodeArray, NULL);

	//编译着色器对象    
	glCompileShader(vShader);

	//检查编译是否成功    
	GLint compileResult;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &compileResult);
	if (GL_FALSE == compileResult)
	{
		GLint logLen;
		//得到编译日志长度    
		glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = (char*)malloc(logLen);
			GLsizei written;
			//得到日志信息并输出    
			glGetShaderInfoLog(vShader, logLen, &written, log);
			cerr << "vertex shader compile log : " << endl;
			cerr << log << endl;
			free(log);//释放空间    
		}
	}

	//创建着色器对象：片断着色器    
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//错误检测    
	if (0 == fShader)
	{
		cerr << "ERROR : Create fragment shader failed" << endl;
		exit(1);
	}

	//把着色器源代码和着色器对象相关联    
	const GLchar* fShaderCode = textFileRead(FShaderFile);
	const GLchar* fCodeArray[1] = { fShaderCode };
	glShaderSource(fShader, 1, fCodeArray, NULL);

	//编译着色器对象    
	glCompileShader(fShader);

	//检查编译是否成功    
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &compileResult);
	if (GL_FALSE == compileResult)
	{
		GLint logLen;
		//得到编译日志长度    
		glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = (char*)malloc(logLen);
			GLsizei written;
			//得到日志信息并输出    
			glGetShaderInfoLog(fShader, logLen, &written, log);
			cerr << "fragment shader compile log : " << endl;
			cerr << log << endl;
			free(log);//释放空间    
		}
	}
	//3、链接着色器对象    
	//创建着色器程序    
	shaderProgram = glCreateProgram();
	if (!shaderProgram)
	{
		cerr << "ERROR : create program failed" << endl;
		exit(1);
	}
	//将着色器程序链接到所创建的程序中    
	glAttachShader(shaderProgram, vShader);
	glAttachShader(shaderProgram, fShader);
	//将这些对象链接成一个可执行程序    
	glLinkProgram(shaderProgram);
	//查询链接的结果    
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

	//在把着色器对象链接到程序对象以后，记得删除着色器对象
	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

//完成glew初始化和加载顶点、片段着色器
void init()
{
	// 初始化glew扩展库    
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "Error initializing GLEW: " << glewGetErrorString(err) << endl;
	}
	glEnable(GL_DEPTH_TEST);


	// 检查是否可以使用ARB扩展
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL.\n");
	else
	{
		printf("Not totally ready.\n");
		exit(1);
	}

	glEnable(GL_DEPTH_TEST);

	// 加载顶点和片段着色器对象并链接到一个程序对象上  
	initShader(ivoryShaderProgram, "./shaders/ivoryShader.vert", "./shaders/ivoryShader.frag");
	initShader(goochShaderProgram, "./shaders/goochShader.vert", "./shaders/goochShader.frag");
	initShader(edgeShaderProgram, "./shaders/edgeShader.vert", "./shaders/edgeShader.frag");

	// 清空绘制窗口
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


	// 第一个Pass
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glUseProgram(edgeShaderProgram);
	glUniform4f(glGetUniformLocation(edgeShaderProgram, "outlineColor"), 0.0, 1.0, 0.0, 1.0);

	// 控制旋转
	glPushMatrix();
	glRotatef(fRotate, 0, 1.0f, 0);
	glutSolidTeapot(objectSize);
	glPopMatrix();


	// 第二个Pass
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
	// 控制旋转
	glPushMatrix();
	glRotatef(fRotate, 0, 1.0f, 0);
	glutSolidTeapot(objectSize);
	glPopMatrix();


	if (isRotate)fRotate += 0.1f;

	glutSwapBuffers();

}


// 键盘回调函数
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
	// 改变绘制使用的着色器
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


#version 330 core
layout(location = 0) in vec3 aPos; 

// 由于在几何着色器阶段需要生成新的顶点，并且生成顶点时在局部空间计算比较便利
// 因此使用GLM生成的矩阵将坐标从局部坐标系变换到最终的屏幕坐标这个步骤延迟到几何着色器中做

// uniform int times;
// uniform mat4 model; 
// uniform mat4 view; 
// uniform mat4 projection; 

// 定义输出interface block
out VS_OUT
{
    vec4 vert;
    /*vec4 vertori;
    vec3 normal;
    vec3 lightVec;
    vec3 viewVec;
    vec3 vertColor;*/
}vs_out;

void main()
{
	// gl_Position = projection * view * model * vec4(aPos, 1.0f);
    /*vs_out.vert = view * model * vec4(aPos, 1.0f);
    vs_out.normal = vec3(view * model * vec4(aPos, 0.0f));
    vs_out.lightVec = vec3(lightPos - vs_out.vert);
    vs_out.viewVec = -vec3(vs_out.vert);*/
    vs_out.vert = vec4(aPos, 1.0f);
}
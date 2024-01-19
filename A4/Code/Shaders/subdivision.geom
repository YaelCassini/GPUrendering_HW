#version 330 core

layout(triangles) in ;  // 输入三角形
layout(triangle_strip, max_vertices = 192) out;  // 输出3个代表法向量的直线

// 定义输入interface block
in VS_OUT
{   
    vec4 vert;
}gs_in[];

uniform int times;
uniform vec4 lightPos;
uniform mat4 model; 
uniform mat4 view; 
uniform mat4 projection; 

// 输出到片元着色器的变量
out vec4 fcolor;
out vec3 normal;
out vec3 lightVec;
out vec3 viewVec;

// 仅在本着色器中使用的变量
vec4 vert;
vec4 vertexs[192];

// 输出一个顶点信息到片元着色器
// 包括坐标变换和normal、lightVec及viewVec的计算
void outputVertex(vec4 pos)
{
    gl_Position = projection * view * model * pos;

    vert = view * model * pos;
    normal = vec3(view * model * vec4(pos.xyz,0.0f));
    lightVec = vec3(lightPos - vert);
    viewVec = -vec3(vert);
    EmitVertex();
}

// 输出一个三角形的三个顶点信息到片元着色器
// 其中改变fcolor是为了能够对每个三角形的顶点色赋值
// 从而显示出细分后的每个三角形
void outputTriangle(vec4 posi, vec4 posj, vec4 posk)
{

    fcolor = posi;
    outputVertex(posi);

    fcolor = vec4(0.0,0.5,0.0,1.0);
    outputVertex(posj);

    fcolor = vec4(1.0,1.0,1.0,1.0);
    outputVertex(posk);

    EndPrimitive();
}


// 进行times0次循环，每次循环取出当前数组中所有三角形
// 通过对三个顶点的计算，将每个三角形划分为四个三角形
// 并将变换后的坐标及其他顶点信息输出到片元着色器
void subdivision(int times0, vec4 pos1, vec4 pos2, vec4 pos3)
{
    //vec4 pos4 = normalize((pos1+pos2)/2.0f);
    //vec4 pos5 = normalize((pos2+pos3)/2.0f);
    //vec4 pos6 = normalize((pos3+pos1)/2.0f);

    // 通过思考不难发现，将球心与原点重合
    // 则产生的新顶点即为原三角形两个顶点的坐标相加后做归一化（在单位圆上）
    vec4 pos4 = vec4(normalize((pos1.xyz+pos2.xyz)/2.0f),1.0f);
    vec4 pos5 = vec4(normalize((pos2.xyz+pos3.xyz)/2.0f),1.0f);
    vec4 pos6 = vec4(normalize((pos3.xyz+pos1.xyz)/2.0f),1.0f);


    int totalcnt = 1;
    vec4 vertexs2[192];
    for(int i = 1; i <= times0; i++)
    {

        for(int j = 0; j < totalcnt; j++)
        {
            vec4 pos11 = vertexs[j*3];
            vec4 pos22 = vertexs[j*3+1];
            vec4 pos33 = vertexs[j*3+2];

            vec4 pos44 = vec4(normalize((pos11.xyz+pos22.xyz)/2.0f),1.0f);
            vec4 pos55 = vec4(normalize((pos22.xyz+pos33.xyz)/2.0f),1.0f);
            vec4 pos66 = vec4(normalize((pos33.xyz+pos11.xyz)/2.0f),1.0f);

            
            vertexs2[j*12] = pos11;
            vertexs2[j*12+1] = pos44;
            vertexs2[j*12+2] = pos66;
            vertexs2[j*12+3] = pos44;
            vertexs2[j*12+4] = pos22;
            vertexs2[j*12+5] = pos55;
            vertexs2[j*12+6] = pos55;
            vertexs2[j*12+7] = pos66;
            vertexs2[j*12+8] = pos44;
            vertexs2[j*12+9] = pos66;
            vertexs2[j*12+10] = pos55;
            vertexs2[j*12+11] = pos33;
        }

        vertexs = vertexs2;
        totalcnt = totalcnt * 4;
    }

    for(int i = 0; i < totalcnt; i++)
    {
        outputTriangle(vertexs[i*3], vertexs[i*3+1], vertexs[i*3+2]);
    }

    /*
    if(times==0)
    {
        outputTriangle(pos1, pos4, pos6);
        outputTriangle(pos4, pos2, pos5);
        outputTriangle(pos5, pos6, pos4);
        outputTriangle(pos6, pos5, pos3);
    }*/
}

// 主函数
void main()
{
    // 初始化vertexs数组，将最初的三个顶点坐标存入数组
    vertexs[0] = gs_in[0].vert;
    vertexs[1] = gs_in[1].vert;
    vertexs[2] = gs_in[2].vert;

    // 传入顶点坐标，进行细分
    subdivision(times, gs_in[0].vert, gs_in[1].vert, gs_in[2].vert);
    // subdivision(0, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);
}



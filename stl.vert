#version 460 core
layout (location = 0) in vec3 aPos; // 位置变量的属性位置值为0
layout (location = 1) in vec3 aNormal; // 颜色变量的属性位置值为 1

out vec3 Normal;
out vec3 FragPos;  

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);// 注意我们如何把一个vec3作为vec4的构造器的参数

    // 法线矩阵去乘以法向量 防止不等比缩放的影响
    Normal = mat3(transpose(inverse(model))) * aNormal;

    //计算顶点世界坐标
    FragPos = vec3(model * vec4(aPos, 1.0));

    // 显示法向量的颜色
    //FragColor = vec4(vectors*0.5+0.5, 1.0); 
}
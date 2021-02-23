#ifndef STL_LOADER_H
#define STL_LOADER_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include<fstream>
#include "shader.h"
using namespace std;


//存储顶点的结构
struct Vertex {
    // 位置
    glm::vec3 Position;
    // 法线
    glm::vec3 Normal;
};


class STLloader {
public:
    // mesh Data
    vector<Vertex>   vertices;
    unsigned int VAO;
    //顶点个数
    unsigned int vector_number;

    // constructor
    STLloader(string path);
    // render the mesh
    void Draw(Shader& shader);
private:
    // render data 
    unsigned int VBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
    void readSTL_ASCII(std::ifstream& stl_file);
    void readSTL_Binary(std::ifstream &stl_file);
};
#endif

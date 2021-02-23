#include"STLloader.h"


STLloader::STLloader(string path)
{
    std::ifstream stl_file;
    // ensure ifstream objects can throw exceptions:
    stl_file.exceptions(std::ifstream::badbit);
    try
    {
        // open files
        stl_file.open(path,ios::in|ios:: binary);
        if (!stl_file.is_open())
        {
            cout << "文件打开失败" << endl;
            return;
        }

        stl_file.ignore(79);
        char is_binary;
        stl_file.read((char*)(&is_binary), sizeof(is_binary));


        ios::sync_with_stdio(false);//可以加快速度 
        //判断是二进制还是文本格式
        if (is_binary != '\0' && is_binary!=' ')//判断格式,有的二进制以空格分割，有的以'\0'标志
        {
            readSTL_ASCII(stl_file);
        }
        else
        {
            readSTL_Binary(stl_file);
        }
        ios::sync_with_stdio(true);
        stl_file.close();
        //加载数据到显存
        setupMesh();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::STL_FILE_NOT_SUCCESFULLY_READ" << std::endl; 
            cout << e.what() << endl;
    }
}


void STLloader::readSTL_ASCII(std::ifstream& stl_file)
{
    vector_number = 0;
    stl_file.seekg(0, ios::beg);


    string buf;
    getline(stl_file,buf);
    float tn0, tn1, tn2; //面的法线
    float vector0, vector1, vector2;  //顶点坐标
    Vertex vertex;//顶点结构体

    //这里的速度还可以优化
    while (1)
    {
        stl_file >> buf;
        if (buf == "endsolid")
            break;
        stl_file >> buf;
        stl_file >> tn0; stl_file >> tn1; stl_file >> tn2;
        stl_file >> buf;
        stl_file >> buf;
        for (int i = 0; i < 3; i++)
        {
            stl_file >> buf;
            stl_file >> vector0; stl_file >> vector1; stl_file >> vector2;
            vertex.Normal = glm::vec3(tn0, tn1, tn2);
            vertex.Position = glm::vec3(vector0, vector1, vector2);
            vertices.push_back(vertex);
            vector_number++;  //用来统计顶点的个数
        }
        stl_file >> buf;
        stl_file >> buf;
    }
    cout << "顶点的数量："<<vector_number << endl;
}


void STLloader::readSTL_Binary(std::ifstream &stl_file)
{
    // 面的个数
    int num_tris;
    stl_file.read((char*)(&num_tris), sizeof(int));
    cout << "顶点数量：" << num_tris*3 << endl;
    vector_number = num_tris * 3;

    //给容器预分配内存
    vertices.reserve(num_tris * 3);
    float tn0, tn1, tn2; //面的法线
    float vector0, vector1, vector2;  //顶点坐标
    Vertex vertex;//顶点结构体
    
    for (int i = 0; i < num_tris; i++)
    {
        stl_file.read((char*)(&tn0), sizeof(float));
        stl_file.read((char*)(&tn1), sizeof(float));
        stl_file.read((char*)(&tn2), sizeof(float));

        //如果模型进行坐标变换，需要重新计算法向量
        // faceNrm.push_back(Normal(tn0, tn1, -tn2)); 

        // 每个面3个顶点
        for (int i = 0; i < 3; i++)
        {
            stl_file.read((char*)(&vector0), sizeof(float));
            stl_file.read((char*)(&vector1), sizeof(float));
            stl_file.read((char*)(&vector2), sizeof(float));
            vertex.Normal = glm::vec3(tn0, tn1, tn2);
            vertex.Position = glm::vec3(vector0, vector1, vector2);
            vertices.push_back(vertex);
        }
        // z有的程序可能有用，表示颜色信息，这里先忽略
        stl_file.ignore(2);
    }
}


void STLloader::Draw(Shader& shader)
{
    // 4. 绘制物体
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
}


void STLloader::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glBindVertexArray(0);
}
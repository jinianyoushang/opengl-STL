#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include"STLloader.h"
#include "shader.h"
#include "texture.h"
using namespace std;
#include <Windows.h>

// 调用独显
//extern "C" __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

// settings screen
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 50.0f, 125.0f));
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// 模型的旋转角度
float MODEL_ROTATE_X = -90.0f;
float MODEL_ROTATE_Y = 0.0f;
float MODEL_ROTATE_Z = 0.0f;

// 光源的位置
glm::vec3 lightPos(300.0f, 300.0f, 300.0f);
//定义光源的颜色
glm::vec3 lightColor(1.0, 1.0, 1.0);

// 返回变换后的模型
glm::mat4 get_my_model();
void print_fps();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// 用来检查错误的函数
#pragma region glcheck_error
GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam)
{
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}
#pragma endregion

int main()
{
#pragma region open a window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // 启用调试上下文
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // glfw 创建窗口
// --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MyLearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    //glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  //关闭鼠标指针显示

    // glad: 加载所有 OpenGL 函数指针
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // enable OpenGL debug context if context allows for debug context
    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        cout << "debug model" << endl;
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
        glDebugMessageCallback(glDebugOutput, nullptr);
        //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        // 只输出高等级错误
        glDebugMessageControl(GL_DEBUG_SOURCE_API,
            GL_DEBUG_TYPE_ERROR,
            GL_DEBUG_SEVERITY_HIGH,
            0, nullptr, GL_TRUE);
    }
#pragma endregion

    // 开启深度缓冲
    glEnable(GL_DEPTH_TEST);
    //开启抗锯齿
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);
    //开启面剔除
    glEnable(GL_CULL_FACE);
    
    //加载模型
    STLloader stlloader("stl_file/小黄人.stl");
    //加载shader
    Shader stl_shader("stl.vert","stl.frag");


    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

        stl_shader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        stl_shader.setMat4("projection", projection);
        stl_shader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = get_my_model();
        stl_shader.setMat4("model", model);

        stl_shader.setVec3("viewPos", camera.Position);

        // light properties
        stl_shader.setVec3("light.position", lightPos);
        stl_shader.setVec3("light.ambient", 0.2,0.2,0.2); // note that all light colors are set at full intensity
        stl_shader.setVec3("light.diffuse", 0.4,0.4,0.4);
        stl_shader.setVec3("light.specular", 0.05,0.05,0.05);

        // 物体的材质 properties
        stl_shader.setVec3("material.ambient", 1.0f, 1.0f, 1.0f);
        stl_shader.setVec3("material.diffuse", 1.0f, 1.0, 1.0f);
        stl_shader.setVec3("material.specular", 1.0f, 1.0f, 1.0f);
        stl_shader.setFloat("material.shininess", 32.0f);

        stlloader.Draw(stl_shader);

        // input
        processInput(window);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        //print_fps();
    }

	return 0;
}



// 返回变换后的模型
glm::mat4 get_my_model()
{
    glm::mat4 model = glm::mat4(1.0f);
    // 平移
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    // 旋转
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    // 绕X轴转
    model = glm::rotate(model, glm::radians(MODEL_ROTATE_X), glm::vec3(1.0, 0.0, 0.0));
    // 绕Y轴转
    model = glm::rotate(model, glm::radians(MODEL_ROTATE_Y), glm::vec3(0.0, 1.0, 0.0));
    // 绕Z轴转
    model = glm::rotate(model, glm::radians(MODEL_ROTATE_Z), glm::vec3(0.0, 0.0, 1.0));

    return model;
}

////计算fps,有点问题
float PRO_TIME = 0;
void print_fps()
{
    static float avgDuration = 0.f;
    static float alpha = 1.f / 30.f; // 采样数设置为100
    static int frameCount = 0;
    //print fps
    auto now_time = glfwGetTime();
    auto delta_Time = (float)(now_time - PRO_TIME);
    PRO_TIME = (float)now_time;

    ++frameCount;

    int fps = 0;
    if (1 == frameCount)
    {
        avgDuration = static_cast<float>(delta_Time);
    }
    else
    {
        avgDuration = avgDuration * (1 - alpha) + delta_Time * alpha;
    }

    fps = static_cast<int>(1.f / avgDuration);
    std::cout << fps << std::endl;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement((float)xoffset, (float)yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //对视角的移动
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
      //q是向上，e是向下
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);


    // 对模型的旋转
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        MODEL_ROTATE_Z += deltaTime * 50;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        MODEL_ROTATE_Z -= deltaTime * 50;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        MODEL_ROTATE_X += deltaTime * 50;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        MODEL_ROTATE_X -= deltaTime * 50;


    // 按F2 切换渲染模式
    static GLboolean F2_PRESS = GL_FALSE;
    static GLboolean F2_PROCESSED = GL_FALSE;
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
    {
        F2_PRESS = GL_TRUE;
    }
    else if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_RELEASE)
    {
        F2_PRESS = GL_FALSE;
        F2_PROCESSED = GL_FALSE;
    }

    if (F2_PRESS == GL_TRUE && F2_PROCESSED == GL_FALSE)
    {
        static int render_model = 0;
        if (render_model % 3 == 0)
        {
            // 使用线框模式
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            F2_PROCESSED = GL_TRUE;
            render_model++;

        }
        else if (render_model % 3 == 1)
        {
            // 使用点模式
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            F2_PROCESSED = GL_TRUE;
            render_model++;
        }
        else if (render_model % 3 == 2)
        {
            // 使用填充模式
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            F2_PROCESSED = GL_TRUE;
            render_model++;
        }
    }

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


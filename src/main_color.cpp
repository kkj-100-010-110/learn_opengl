#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <iostream>
#include <glad/glad.h> // this should be before <GLFW/glfw3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui.h>

#include "shader.h"
#include "camera.h"

#define OK_MSG_BEGIN "\033[96m"
#define OK_MSG_END "\033[0m"
#define ERR_MSG_BEGIN "\033[91m"
#define ERR_MSG_END "\033[0m"
#define INFO_MSG_BEGIN "\033[93m"
#define INFO_MSG_END "\033[0m"

void FrameBufferSizeCallback(GLFWwindow *window, int width, int height);
void ProcessInput(GLFWwindow * window);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
void OnMouseButton(GLFWwindow* window, int button, int action, int modifier);
void OnCharEvent(GLFWwindow* window, unsigned int ch);
void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// clear color for immgui
glm::vec4 clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

// object color & light color
glm::vec3 oc = {1.0f, 0.5f, 0.31f};
glm::vec3 lc = {1.0f, 1.0f, 1.0f};

// light pos
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main(int argc, char** argv)
{
    std::cout << OK_MSG_BEGIN << "START THE PROGRAM" << OK_MSG_END << std::endl;
    std::cout << OK_MSG_BEGIN << "INITIALIZE GLFW LIBRARY" << OK_MSG_END << std::endl;
    if (!glfwInit()) {
        const char* description = nullptr;
        glfwGetError(&description);
        std::cout << ERR_MSG_BEGIN << "ERROR::MAIN::FAILED_TO_INITIALIZE_GLFW_LIB::" << description << ERR_MSG_END << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    std::cout << OK_MSG_BEGIN << "CREATE GLFW WINDOW" << OK_MSG_END << std::endl;
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window) {
        std::cout << ERR_MSG_BEGIN << "ERROR::MAIN::FAILED_TO_CREATE_GLFW_WINDOW" << ERR_MSG_END << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetKeyCallback(window, OnKeyEvent);
    glfwSetCharCallback(window, OnCharEvent);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetMouseButtonCallback(window, OnMouseButton);

    std::cout << OK_MSG_BEGIN << "LOAD ALL OPENGL FUNCTION POINTERS" << OK_MSG_END << std::endl;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << ERR_MSG_BEGIN << "ERROR::MAIN::FAILED_TO_INITIALIZE_GLAD" << ERR_MSG_END << std::endl;
        glfwTerminate();
        return -1;
    }
    auto glVersion = glGetString(GL_VERSION);
    std::cout << INFO_MSG_BEGIN << "OPENGL CONTEXT VERSION: " << (char*)glVersion << INFO_MSG_END << std::endl;

    // imgui is set up 
    auto imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    std::cout << OK_MSG_BEGIN << "BUILD AND COMPILE SHADER PROGRAM" << OK_MSG_END << std::endl;
    Shader objectShader("./shader/color.vs", "./shader/color.fs");
    Shader lightShader("./shader/light_resourse.vs", "./shader/light_resourse.fs");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -10.0f),
    };

    uint32_t objectVAO, VBO;
    glGenVertexArrays(1, &objectVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(objectVAO);
    // copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    uint32_t lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // check maximum number of vertex attributes supported
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << INFO_MSG_BEGIN << "MAXIMUM NUMBER OF VERTEX ATTRIBUTES SUPPORTED: " << nrAttributes << INFO_MSG_END << std::endl;

    std::cout << OK_MSG_BEGIN << "START MAIN LOOP" << OK_MSG_END << std::endl;
    // render loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // imgui ui set up
        if (ImGui::Begin("UI window")) {
            ImGui::Text("Color");
            if (ImGui::ColorEdit4("clear", glm::value_ptr(clearColor))) {
                glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
            }
            ImGui::Separator();
            ImGui::Text("Object color");
            ImGui::ColorEdit3("color", glm::value_ptr(oc));
            ImGui::Separator();
            ImGui::Text("Camera");
            ImGui::DragFloat3("Pos", glm::value_ptr(camera.Position), 0.01f);
            ImGui::DragFloat("Pitch", &camera.Front.y, 0.01f, -2.0f, 2.0f);
            ImGui::DragFloat("Yaw", &camera.Front.x, 0.01f);
            ImGui::Separator();
            if (ImGui::Button("Reset Camera")) {
                camera.Front.x = 0.0f;
                camera.Front.y = 0.0f;
                camera.Position = glm::vec3(0.0f, 0.0f, 3.0f);
            }
        }
        ImGui::End();

        // update deltatime
        float currentFrame = static_cast<float>(glfwGetTime());
        camera.deltaTime = currentFrame - camera.lastFrame;
        camera.lastFrame = currentFrame;
        // input
        ProcessInput(window);
        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        objectShader.use();
        objectShader.setVec3("objectColor", oc);
        objectShader.setVec3("lightColor",  lc);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        objectShader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        objectShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        objectShader.setMat4("model", model);

        glBindVertexArray(objectVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffer and poll IO events
        glfwSwapBuffers(window);
    }
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &objectVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(objectShader.ID);
    glDeleteProgram(lightShader.ID);

    // terminate imgui
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);

    // glfw: terminate, clearing all previously allocated GLFW resources
    glfwTerminate();

    return 0;
}

void FrameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    std::cout << INFO_MSG_BEGIN << "FRAMEBUFFER SIZE CHANGED: " << "(" << width << " x " << height << ")" << INFO_MSG_END << std::endl;
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow * window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // right-click
    if (camera.CameraControl == false)
        return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, camera.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, camera.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, camera.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, camera.deltaTime);
    
    // make sure the user stays at the ground level.
    // camera.Position.y = 0.0f;
}

void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (camera.firstMouse) {
        camera.lastX = xpos;
        camera.lastY = ypos;
        camera.firstMouse = false;
    }
    float xoffset = xpos - camera.lastX;
    float yoffset = camera.lastY - ypos;
    camera.lastX = xpos;
    camera.lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void OnMouseButton(GLFWwindow *window, int button, int action, int modifier)
{
    // add this function to adjust imgui windows.
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier); 
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    camera.MouseButton(button, action, x, y);
}

void OnCharEvent(GLFWwindow* window, unsigned int ch) {
    ImGui_ImplGlfw_CharCallback(window, ch);
}

void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
   
    std::string actionStr = action == GLFW_PRESS ? "Pressed"
                       : action == GLFW_RELEASE ? "Released"
                       : action == GLFW_REPEAT ? "Repeat" : "Unknown";
    std::string modsStr = mods & GLFW_MOD_SHIFT ? "S" : "-";
    modsStr += mods & GLFW_MOD_ALT ? "A" : "-";

    // std::cout << INFO_MSG_BEGIN
    //           << "Key: " << key
    //           << ", scancode: " << scancode
    //           << ", action: " << actionStr
    //           << ", mods : " << modsStr
    //           << INFO_MSG_END << std::endl;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}
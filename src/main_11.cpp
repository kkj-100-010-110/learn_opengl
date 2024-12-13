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
    Shader shader("../shader/texture.vs", "../shader/texture.fs");

   

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    uint32_t VAO, VBO;//, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    // copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // check maximum number of vertex attributes supported
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << INFO_MSG_BEGIN << "MAXIMUM NUMBER OF VERTEX ATTRIBUTES SUPPORTED: " << nrAttributes << INFO_MSG_END << std::endl;

    // load and create textures
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping/filtering options (on currently bound texture)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data = stbi_load("../image/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << ERR_MSG_BEGIN << "ERROR::MAIN::FAILED TO LOAD TEXTURE" << ERR_MSG_END << std::endl;
    }
    stbi_image_free(data);

    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping/filtering options (on currently bound texture)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("../image/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << ERR_MSG_BEGIN << "ERROR::MAIN::FAILED TO LOAD TEXTURE" << ERR_MSG_END << std::endl;
    }
    stbi_image_free(data);

    // tell OpenGL for each sampler to which texture unit it belongs to (only has to be done once)
    shader.use();                 // don’t forget to activate the shader first!
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

    // tell OpenGL to enable depth testing
    glEnable(GL_DEPTH_TEST);

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
        // binding textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        // activate shader
        shader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        // render box
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glDrawArrays(GL_TRIANGLES, 0, 36);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffer and poll IO events
        glfwSwapBuffers(window);
    }
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader.ID);

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

    std::cout << INFO_MSG_BEGIN
              << "Key: " << key
              << ", scancode: " << scancode
              << ", action: " << actionStr
              << ", mods : " << modsStr
              << INFO_MSG_END << std::endl;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}
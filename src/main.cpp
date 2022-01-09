#include <iostream>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <cmath>

#include "Wall.h"
#include "Environment.h"
#include "WaterParticle.h"
#include "Water.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    int width = 1280, height = 720;
    float aspectRatio = ((float) width / height);
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glOrtho(0, width, 0, height, -1, 1);

    const char* glsl_version = "#version 100";
    ImGui_ImplOpenGL3_Init(glsl_version);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // Environment setup for walls
    float scale = 1.0f;
    // Ratio between 0.125 and 0.4375 has a distance of 5 for width
    // Ratio between 0.1111 and 0.6666 has a distance of 5 for height
    Wall ground(glm::vec2(0.125 * width, 0.1111 * height), glm::vec2(0.4375 * width, 0.1111 * height));
    Wall leftWall(glm::vec2(0.125 * width, 0.1111 * height), glm::vec2(0.125 * width, 0.6666 * height));
    Wall rightWall(glm::vec2(0.4375 * width, 0.1111 * height), glm::vec2(0.4375 * width, 0.6666 * height));

    Environment environment;
    environment.addWalls(ground);
    environment.addWalls(leftWall);
    environment.addWalls(rightWall);
    environment.updateMinMax();

    const float restDensity = 0.00000000001f;
    const float gasConstant = 3.0f;
    const float miu = 0.16f;
    const float tensionCoef = 0.0f;
    const float radius = 50.0f;

    bool startSimulate = false;
    bool collision = true;
    float restDensityCopy = restDensity;
    float gasConstantCopy = gasConstant;
    float miuCopy = miu;
    float tensionCoefCopy = tensionCoef;
    float radiusCopy = radius;

    float deltaTime = 0.005f;

    // Setup for water particles and blob
    // 0.5555 as the bottom height as it has a distance of 4 from the ground
    // 0.8889 as the top height
    // 0.1875 and 0.3125 for left and right respectivitely
    int particleNumber = 100;
    Water water(particleNumber, width, height);

    // Testing code to see if the blob is creating correctly
    /*Wall bottomBlob(glm::vec2(0.1875 * width, 0.5555 * height), glm::vec2(0.3125 * width, 0.5555 * height));
    Wall topBlob(glm::vec2(0.1875 * width, 0.8889 * height), glm::vec2(0.3125 * width, 0.8889 * height));
    Wall leftBlob(glm::vec2(0.1875 * width, 0.5555 * height), glm::vec2(0.1875 * width, 0.8889 * height));
    Wall rightBlob(glm::vec2(0.3125 * width, 0.5555 * height), glm::vec2(0.3125 * width, 0.8889 * height));
    environment.addWalls(bottomBlob);
    environment.addWalls(topBlob);
    environment.addWalls(leftBlob);
    environment.addWalls(rightBlob);*/

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        // Projection because we are doing 2D stuff here and not playing with 3D
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        /* Poll for and process events */
        glfwPollEvents();

        // ImGui stuffs
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Settings");                          // Create a window called "Settings" and append into it.

            ImGui::Text("Remember to apply variables before simulation");               // Display some text (you can use a format strings too)
            //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            //ImGui::Checkbox("Another Window", &show_another_window);
            //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderInt("Water particles", &particleNumber, 1, 1000);
            
            ImGui::SliderFloat("Rest density", &restDensityCopy, 0.0, 1.0);

            ImGui::SliderFloat("Gas constant", &gasConstantCopy, 0.0, 5.0);

            ImGui::SliderFloat("Miu", &miuCopy, 0.0, 2.0);

            ImGui::SliderFloat("Tension Coefficient", &tensionCoefCopy, 0.0, 3.0);

            ImGui::SliderFloat("Radius", &radiusCopy, 0.0, 100.0);

            ImGui::Checkbox("Collision", &collision);

            if (ImGui::Button("Apply variables"))
            {
                water.updateParticleNumber(particleNumber);
                water.updateVariables();
                water.restDensity = restDensityCopy;
                water.gasConstant = gasConstantCopy;
                water.miu = miuCopy;
                water.tensionCoef = tensionCoefCopy;
                water.radius = radiusCopy;
            }

            ImGui::SameLine();

            if (ImGui::Button("Reset variables"))
            {
                particleNumber = 100;
                restDensityCopy = restDensity;
                gasConstantCopy = gasConstant;
                miuCopy = miu;
                tensionCoefCopy = tensionCoef;
                radiusCopy = radius;

                water.updateParticleNumber(particleNumber);
                water.updateVariables();
                water.restDensity = restDensityCopy;
                water.gasConstant = gasConstantCopy;
                water.miu = miuCopy;
                water.tensionCoef = tensionCoefCopy;
                water.radius = radiusCopy;

                water.findNeighbourParticles();
                water.findRestDensity();
            }

            ImGui::SameLine();

            // Start Simulate Button
            if (!startSimulate)
            {
                if (ImGui::Button("Start Simulation###StartButton"))
                {
                    startSimulate = !startSimulate;
                }
            }
            else
            {
                if (ImGui::Button("End Simulation###StartButton"))
                {
                    startSimulate = !startSimulate;
                }
            }
            ImGui::Text("Simulating: %s", startSimulate ? "true" : "false");

            int currentParticleNumber = water.waterParticles.size();

            ImGui::Text("Number of particles = %d", currentParticleNumber);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // Water Particles Calcuations
        if (startSimulate)
        {
            water.findNeighbourParticles();
            water.findDensityAndPressure();
            water.updateParticleForce();
            water.updateAcceleration();
            water.updateParticlePosition(deltaTime);
            water.checkCollision(environment, collision);

            //water.update(deltaTime);
        }

        // Actual Rendering
        environment.Render();
        water.Render();
        
        // Rendering for ImGui
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
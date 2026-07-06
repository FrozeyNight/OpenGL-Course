#pragma once

#include "Test.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "texture.h"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <vector>
#include <ctime>

namespace test{
    class TestExercise : public Test{
    private:
    // I think we need:
    // vertexArray
    // IndexBuffer
    // Shader
    // MVP? - maybe just the V and P or not even that (cos maybe we can just make it in the OnRender, not as a member)
        //std::vector<Cube> cubes;
        float positions[16] = {
            -50.0f, -50.0f, 0.0f, 0.0f,// 0
             50.0f, -50.0f, 1.0f, 0.0f,// 1
             50.0f,  50.0f, 1.0f, 1.0f,// 2
            -50.0f,  50.0f, 0.0f, 1.0f,// 3
        };
            
        unsigned int indices[6] = {
            0, 1, 2,
            2, 3, 0
        };
        VertexArray va;
        IndexBuffer ib = IndexBuffer(indices, 6);
        Shader shader = Shader("res/shaders/Basic.shader");
        Texture texture = Texture("res/textures/Example.png");
        const glm::mat4 projectionMatrix = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        const glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        size_t objectCount = 0;
        std::vector<glm::vec3> objPositions;

    public:
        TestExercise();
        ~TestExercise();

        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
    };
}
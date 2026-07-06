#include "TestExercise.h"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

#include "imgui.h"

namespace test{    
    TestExercise::TestExercise(){

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        shader.Bind();
        
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();

        std::srand(std::time(NULL));
    };
    TestExercise::~TestExercise(){
    };

    void TestExercise::OnUpdate(float deltaTime){
        // change the mvp using deltaTime / random to move the cubes
        // we need a trajectory of every cube stored in a vector
        // trajectory gets chosen from x: (1-,1) and y: (-1,1)
        // trajectories don't get changed, they are used by multiplying them with a linearly growing number to "move" them in a direction
        // for every cube that linear growing number must be different
    };

    void TestExercise::OnRender(){
        // render all the cubes in the vector
        bool x = std::rand() % 2;
        bool y = std::rand() % 2;

        for(size_t i = 0; i < objectCount; i++){
            x = std::rand() % 2;
            y = std::rand() % 2;

            if(x){
                objPositions.at(i).x = objPositions.at(i).x - 1;
            }
            else{
                objPositions.at(i).x = objPositions.at(i).x + 1;
            }

            if(y){
                objPositions.at(i).y = objPositions.at(i).y - 1;
            }
            else{
                objPositions.at(i).y = objPositions.at(i).y + 1;
            }

            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), objPositions.at(i));
            glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix; // <- we edit the model matrix here: ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f);
            shader.Bind();
            shader.SetUniformMat4f("u_MVP", mvp);
            va.Bind();
            ib.Bind();
            GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
        }
    };

    void TestExercise::OnImGuiRender(){
        // check if the button was pressed and if yes, add another cube to the vector
        if(ImGui::Button("Spawn in an object")){
            objectCount++;
            glm::vec3 position(480, 270, 0);
            objPositions.push_back(position);
        }
    };
}
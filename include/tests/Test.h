#pragma once

#include "Renderer.h"

namespace test
{
    class Test{
        public:
            Test() {}
            virtual ~Test() {}

            virtual void OnUpdate(float deltaTime) {}
            virtual void OnRender() {}
            virtual void OnRender(Renderer& renderer) {}
            virtual void OnImGuiRender() {}
    };
}

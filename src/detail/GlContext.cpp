#include <GLFW/glfw3.h>
#include <etherblocks/detail/GlContext.hpp>
#include <glad/glad.h>
#include <stdexcept>

namespace etherblocks::detail {

   GlContext& GlContext::instance() {
      static GlContext ctx;
      return ctx;
   }

   void GlContext::load() {
      if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
         throw std::runtime_error{"Failed to initialize GLAD"};
      }
   }

   GlContext::GlContext() {
      if (!glfwInit()) {
         throw std::runtime_error{"Failed to initialize GLFW"};
      }
   }

   GlContext::~GlContext() {
      glfwTerminate();
   }

} // namespace etherblocks::detail

#include <etherblocks/system/detail/GlContext.hpp>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace etherblocks::system::detail {

   GlContext& GlContext::instance() {
      static GlContext ctx;
      return ctx;
   }

   void GlContext::load() {
      if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
         throw std::runtime_error{"Failed to initialize GLAD"};
      }
   }

   GlContext::GlContext() {
      if (glfwInit() == GLFW_FALSE) {
         throw std::runtime_error{"Failed to initialize GLFW"};
      }
   }

   GlContext::~GlContext() {
      glfwTerminate();
   }

} // namespace etherblocks::system::detail

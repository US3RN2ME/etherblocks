#include <etherblocks/system/Logger.hpp>
#include <etherblocks/system/detail/GlContext.hpp>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>

namespace etherblocks::system::detail {

   GlContext& GlContext::instance() {
      static GlContext ctx;
      return ctx;
   }

   void GlContext::load() {
      if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
         log(LogLevel::Error, "Failed to initialize GLAD");
         throw std::runtime_error{"Failed to initialize GLAD"};
      }
      log(LogLevel::Debug, "GLAD initialized");
   }

   GlContext::GlContext() {
      glfwSetErrorCallback([](int code, const char* description) {
         log(LogLevel::Error, "GLFW error " + std::to_string(code) + ": " + (description != nullptr ? description : "unknown"));
      });
      if (glfwInit() == GLFW_FALSE) {
         log(LogLevel::Error, "Failed to initialize GLFW");
         throw std::runtime_error{"Failed to initialize GLFW"};
      }
      log(LogLevel::Debug, "GLFW initialized");
   }

   GlContext::~GlContext() {
      glfwTerminate();
      log(LogLevel::Debug, "GLFW terminated");
   }

} // namespace etherblocks::system::detail

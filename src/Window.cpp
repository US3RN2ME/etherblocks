#include <GLFW/glfw3.h>
#include <etherblocks/Window.hpp>
#include <etherblocks/detail/GlContext.hpp>
#include <glad/glad.h>
#include <stdexcept>

namespace etherblocks {
   namespace {

      GLbitfield toGlClearMask(Buffer buffers) {
         GLbitfield mask = 0;
         const auto value = static_cast<unsigned>(buffers);

         if ((value & static_cast<unsigned>(Buffer::Color)) != 0) {
            mask |= GL_COLOR_BUFFER_BIT;
         }
         if ((value & static_cast<unsigned>(Buffer::Depth)) != 0) {
            mask |= GL_DEPTH_BUFFER_BIT;
         }
         if ((value & static_cast<unsigned>(Buffer::Stencil)) != 0) {
            mask |= GL_STENCIL_BUFFER_BIT;
         }

         return mask;
      }

      int toGlCursorMode(CursorMode mode) {
         switch (mode) {
            case CursorMode::Normal:
               return GLFW_CURSOR_NORMAL;
            case CursorMode::Hidden:
               return GLFW_CURSOR_HIDDEN;
            case CursorMode::Disabled:
               return GLFW_CURSOR_DISABLED;
         }
         return GLFW_CURSOR_NORMAL;
      }

      GLenum toGLenum(RenderFeature f) {
         switch (f) {
            case RenderFeature::DepthTest:
               return GL_DEPTH_TEST;
            case RenderFeature::StencilTest:
               return GL_STENCIL_TEST;
            case RenderFeature::Blending:
               return GL_BLEND;
            case RenderFeature::CullFace:
               return GL_CULL_FACE;
            case RenderFeature::Multisample:
               return GL_MULTISAMPLE;
         }
      }
   } // namespace

   Window::Window(WindowConfig cfg) {
      detail::GlContext::instance();

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, cfg.glMajor);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, cfg.glMinor);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

      handle_ = glfwCreateWindow(cfg.width, cfg.height, cfg.title.c_str(), nullptr, nullptr);
      if (handle_ == nullptr) {
         throw std::runtime_error{"Failed to create GLFW window"};
      }

      glfwSetWindowUserPointer(handle_, this);
      setupCallbacks();

      glfwMakeContextCurrent(handle_);
      detail::GlContext::load();

      glfwSwapInterval(cfg.vsync ? 1 : 0);
   }

   Window::~Window() {
      if (handle_ != nullptr) {
         glfwDestroyWindow(handle_);
      }
   }

   Window::Window(Window&& other) noexcept
       : handle_{std::exchange(other.handle_, nullptr)}
       , events_{std::move(other.events_)} {
      if (handle_ != nullptr) {
         glfwSetWindowUserPointer(handle_, this);
      }
   }

   Window& Window::operator=(Window&& other) noexcept {
      if (this != &other) {
         if (handle_ != nullptr) {
            glfwDestroyWindow(handle_);
         }

         handle_ = std::exchange(other.handle_, nullptr);
         events_ = std::move(other.events_);

         if (handle_ != nullptr) {
            glfwSetWindowUserPointer(handle_, this);
         }
      }

      return *this;
   }

   bool Window::isOpen() const {
      return handle_ != nullptr && !glfwWindowShouldClose(handle_);
   }

   void Window::pollEvents() {
      glfwPollEvents();
   }

   std::optional<event::Any> Window::nextEvent() {
      if (events_.empty()) {
         return {};
      }
      auto event = std::move(events_.front());
      events_.pop();
      return event;
   }

   void Window::display() {
      glfwSwapBuffers(handle_);
   }

   void Window::clear(Color color, Buffer buffers) {
      glClearColor(color.r, color.g, color.b, color.a);
      glClear(toGlClearMask(buffers));
   }

   void Window::close() {
      glfwSetWindowShouldClose(handle_, GLFW_TRUE);
   }

   void Window::enable(RenderFeature feature) {
      glEnable(toGLenum(feature));
   }

   void Window::disable(RenderFeature feature) {
      glEnable(toGLenum(feature));
   }

   std::pair<int, int> Window::framebufferSize() const {
      int width{};
      int height{};
      glfwGetFramebufferSize(handle_, &width, &height);
      return {width, height};
   }

   std::pair<int, int> Window::size() const {
      int width{};
      int height{};
      glfwGetWindowSize(handle_, &width, &height);
      return {width, height};
   }

   GLFWwindow* Window::nativeHandle() const noexcept {
      return handle_;
   }

   void Window::setCursorMode(CursorMode mode) {
      const int glfwMode = etherblocks::toGlCursorMode(mode);
      glfwSetInputMode(handle_, GLFW_CURSOR, glfwMode);
   }

   void Window::setStickyKeys(bool enabled) {
      glfwSetInputMode(handle_, GLFW_STICKY_KEYS, enabled ? GLFW_TRUE : GLFW_FALSE);
   }

   void Window::setRawMouseMotion(bool enabled) {
      if (glfwRawMouseMotionSupported())
         glfwSetInputMode(handle_, GLFW_RAW_MOUSE_MOTION, enabled ? GLFW_TRUE : GLFW_FALSE);
   }

   void Window::setupCallbacks() {
      glfwSetWindowCloseCallback(handle_, [](GLFWwindow* window) {
         self(window).events_.emplace(event::Closed{});
      });

      glfwSetFramebufferSizeCallback(handle_, [](GLFWwindow* window, int width, int height) {
         glViewport(0, 0, width, height);
         self(window).events_.emplace(event::Resized{width, height});
      });

      glfwSetKeyCallback(handle_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
         if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            self(window).events_.emplace(event::KeyPressed{key, scancode, mods});
         } else if (action == GLFW_RELEASE) {
            self(window).events_.emplace(event::KeyReleased{key, scancode, mods});
         }
      });

      glfwSetCursorPosCallback(handle_, [](GLFWwindow* window, double x, double y) {
         self(window).events_.emplace(event::MouseMoved{x, y});
      });

      glfwSetMouseButtonCallback(handle_, [](GLFWwindow* window, int button, int action, int mods) {
         if (action == GLFW_PRESS) {
            self(window).events_.emplace(event::MouseButtonPressed{button, mods});
         } else if (action == GLFW_RELEASE) {
            self(window).events_.emplace(event::MouseButtonReleased{button, mods});
         }
      });

      glfwSetScrollCallback(handle_, [](GLFWwindow* window, double xOffset, double yOffset) {
         self(window).events_.emplace(event::MouseScrolled{xOffset, yOffset});
      });
   }

   Window& Window::self(GLFWwindow* window) {
      return *static_cast<Window*>(glfwGetWindowUserPointer(window));
   }

} // namespace etherblocks

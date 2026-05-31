#include <etherblocks/system/Window.hpp>
#include <etherblocks/system/detail/GlContext.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <memory>
#include <queue>
#include <stdexcept>

namespace etherblocks::system {
   namespace {
      struct WindowDeleter {
         void operator()(GLFWwindow* window) const noexcept {
            glfwDestroyWindow(window);
         }
      };
   } // namespace

   struct Window::Impl {
      std::unique_ptr<GLFWwindow, WindowDeleter> handle;
      std::queue<event::Any> events;
      Input input;
   };

   namespace {
      int toGlfw(CursorMode mode) noexcept {
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

      Key toKey(int key) noexcept {
         switch (key) {
            case GLFW_KEY_A:
               return Key::A;
            case GLFW_KEY_B:
               return Key::B;
            case GLFW_KEY_C:
               return Key::C;
            case GLFW_KEY_D:
               return Key::D;
            case GLFW_KEY_E:
               return Key::E;
            case GLFW_KEY_F:
               return Key::F;
            case GLFW_KEY_G:
               return Key::G;
            case GLFW_KEY_H:
               return Key::H;
            case GLFW_KEY_I:
               return Key::I;
            case GLFW_KEY_J:
               return Key::J;
            case GLFW_KEY_K:
               return Key::K;
            case GLFW_KEY_L:
               return Key::L;
            case GLFW_KEY_M:
               return Key::M;
            case GLFW_KEY_N:
               return Key::N;
            case GLFW_KEY_O:
               return Key::O;
            case GLFW_KEY_P:
               return Key::P;
            case GLFW_KEY_Q:
               return Key::Q;
            case GLFW_KEY_R:
               return Key::R;
            case GLFW_KEY_S:
               return Key::S;
            case GLFW_KEY_T:
               return Key::T;
            case GLFW_KEY_U:
               return Key::U;
            case GLFW_KEY_V:
               return Key::V;
            case GLFW_KEY_W:
               return Key::W;
            case GLFW_KEY_X:
               return Key::X;
            case GLFW_KEY_Y:
               return Key::Y;
            case GLFW_KEY_Z:
               return Key::Z;
            case GLFW_KEY_0:
               return Key::Num0;
            case GLFW_KEY_1:
               return Key::Num1;
            case GLFW_KEY_2:
               return Key::Num2;
            case GLFW_KEY_3:
               return Key::Num3;
            case GLFW_KEY_4:
               return Key::Num4;
            case GLFW_KEY_5:
               return Key::Num5;
            case GLFW_KEY_6:
               return Key::Num6;
            case GLFW_KEY_7:
               return Key::Num7;
            case GLFW_KEY_8:
               return Key::Num8;
            case GLFW_KEY_9:
               return Key::Num9;
            case GLFW_KEY_F1:
               return Key::F1;
            case GLFW_KEY_F2:
               return Key::F2;
            case GLFW_KEY_F3:
               return Key::F3;
            case GLFW_KEY_F4:
               return Key::F4;
            case GLFW_KEY_F5:
               return Key::F5;
            case GLFW_KEY_F6:
               return Key::F6;
            case GLFW_KEY_F7:
               return Key::F7;
            case GLFW_KEY_F8:
               return Key::F8;
            case GLFW_KEY_F9:
               return Key::F9;
            case GLFW_KEY_F10:
               return Key::F10;
            case GLFW_KEY_F11:
               return Key::F11;
            case GLFW_KEY_F12:
               return Key::F12;
            case GLFW_KEY_UP:
               return Key::Up;
            case GLFW_KEY_DOWN:
               return Key::Down;
            case GLFW_KEY_LEFT:
               return Key::Left;
            case GLFW_KEY_RIGHT:
               return Key::Right;
            case GLFW_KEY_PAGE_UP:
               return Key::PageUp;
            case GLFW_KEY_PAGE_DOWN:
               return Key::PageDown;
            case GLFW_KEY_HOME:
               return Key::Home;
            case GLFW_KEY_END:
               return Key::End;
            case GLFW_KEY_INSERT:
               return Key::Insert;
            case GLFW_KEY_DELETE:
               return Key::Delete;
            case GLFW_KEY_SPACE:
               return Key::Space;
            case GLFW_KEY_ENTER:
               return Key::Enter;
            case GLFW_KEY_ESCAPE:
               return Key::Escape;
            case GLFW_KEY_TAB:
               return Key::Tab;
            case GLFW_KEY_BACKSPACE:
               return Key::Backspace;
            case GLFW_KEY_CAPS_LOCK:
               return Key::CapsLock;
            case GLFW_KEY_LEFT_SHIFT:
               return Key::LeftShift;
            case GLFW_KEY_RIGHT_SHIFT:
               return Key::RightShift;
            case GLFW_KEY_LEFT_CONTROL:
               return Key::LeftCtrl;
            case GLFW_KEY_RIGHT_CONTROL:
               return Key::RightCtrl;
            case GLFW_KEY_LEFT_ALT:
               return Key::LeftAlt;
            case GLFW_KEY_RIGHT_ALT:
               return Key::RightAlt;
            case GLFW_KEY_KP_0:
               return Key::Numpad0;
            case GLFW_KEY_KP_1:
               return Key::Numpad1;
            case GLFW_KEY_KP_2:
               return Key::Numpad2;
            case GLFW_KEY_KP_3:
               return Key::Numpad3;
            case GLFW_KEY_KP_4:
               return Key::Numpad4;
            case GLFW_KEY_KP_5:
               return Key::Numpad5;
            case GLFW_KEY_KP_6:
               return Key::Numpad6;
            case GLFW_KEY_KP_7:
               return Key::Numpad7;
            case GLFW_KEY_KP_8:
               return Key::Numpad8;
            case GLFW_KEY_KP_9:
               return Key::Numpad9;
            case GLFW_KEY_KP_ENTER:
               return Key::NumpadEnter;
            case GLFW_KEY_KP_ADD:
               return Key::NumpadAdd;
            case GLFW_KEY_KP_SUBTRACT:
               return Key::NumpadSub;
            case GLFW_KEY_KP_MULTIPLY:
               return Key::NumpadMul;
            case GLFW_KEY_KP_DIVIDE:
               return Key::NumpadDiv;
            default:
               return Key::Unknown;
         }
      }

      MouseButton toMouseButton(int button) noexcept {
         switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
               return MouseButton::Left;
            case GLFW_MOUSE_BUTTON_RIGHT:
               return MouseButton::Right;
            case GLFW_MOUSE_BUTTON_MIDDLE:
               return MouseButton::Middle;
            case GLFW_MOUSE_BUTTON_4:
               return MouseButton::Button4;
            case GLFW_MOUSE_BUTTON_5:
               return MouseButton::Button5;
            default:
               return MouseButton::Unknown;
         }
      }

      Window& self(GLFWwindow* window) {
         return *static_cast<Window*>(glfwGetWindowUserPointer(window));
      }
   } // namespace

   Window::Window(const WindowConfig& cfg)
       : impl_(std::make_unique<Impl>()) {
      detail::GlContext::instance();

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, cfg.glMajor);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, cfg.glMinor);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

      impl_->handle.reset(glfwCreateWindow(cfg.size.x, cfg.size.y, cfg.title.c_str(), nullptr, nullptr));
      if (impl_->handle == nullptr) {
         throw std::runtime_error{"Failed to create GLFW window"};
      }

      glfwSetWindowUserPointer(impl_->handle.get(), this);
      setupCallbacks();

      glfwMakeContextCurrent(impl_->handle.get());
      detail::GlContext::load();

      glfwSwapInterval(cfg.vsync ? 1 : 0);
   }

   Window::~Window() = default;

   Window::Window(Window&& other) noexcept
       : impl_(std::move(other.impl_)) {
      if (impl_ != nullptr && impl_->handle != nullptr) {
         glfwSetWindowUserPointer(impl_->handle.get(), this);
      }
   }

   Window& Window::operator=(Window&& other) noexcept {
      if (this != &other) {
         impl_ = std::move(other.impl_);

         if (impl_ != nullptr && impl_->handle != nullptr) {
            glfwSetWindowUserPointer(impl_->handle.get(), this);
         }
      }

      return *this;
   }

   bool Window::isOpen() const {
      return impl_ != nullptr && impl_->handle != nullptr && glfwWindowShouldClose(impl_->handle.get()) == GLFW_FALSE;
   }

   const Input& Window::input() const noexcept {
      return impl_->input;
   }

   void Window::pollEvents() {
      impl_->input.flush();
      glfwPollEvents();
   }

   std::optional<event::Any> Window::nextEvent() {
      if (impl_->events.empty()) {
         return {};
      }
      auto event = impl_->events.front();
      impl_->events.pop();
      return event;
   }

   void Window::display() {
      glfwSwapBuffers(impl_->handle.get());
   }

   void Window::close() {
      glfwSetWindowShouldClose(impl_->handle.get(), GLFW_TRUE);
   }

   glm::ivec2 Window::framebufferSize() const {
      int width{};
      int height{};
      glfwGetFramebufferSize(impl_->handle.get(), &width, &height);
      return {width, height};
   }

   glm::ivec2 Window::size() const {
      int width{};
      int height{};
      glfwGetWindowSize(impl_->handle.get(), &width, &height);
      return {width, height};
   }

   double Window::elapsedTime() noexcept {
      return glfwGetTime();
   }

   void Window::setCursorMode(CursorMode mode) {
      glfwSetInputMode(impl_->handle.get(), GLFW_CURSOR, toGlfw(mode));
   }

   void Window::setStickyKeys(bool enabled) {
      glfwSetInputMode(impl_->handle.get(), GLFW_STICKY_KEYS, enabled ? GLFW_TRUE : GLFW_FALSE);
   }

   void Window::setRawMouseMotion(bool enabled) {
      if (glfwRawMouseMotionSupported() != GLFW_FALSE) {
         glfwSetInputMode(impl_->handle.get(), GLFW_RAW_MOUSE_MOTION, enabled ? GLFW_TRUE : GLFW_FALSE);
      }
   }

   void Window::setupCallbacks() {
      glfwSetWindowCloseCallback(impl_->handle.get(), [](GLFWwindow* window) {
         self(window).impl_->events.emplace(event::Closed{});
      });

      glfwSetFramebufferSizeCallback(impl_->handle.get(), [](GLFWwindow* window, int width, int height) {
         self(window).impl_->events.emplace(event::Resized{{width, height}});
      });

      glfwSetKeyCallback(impl_->handle.get(), [](GLFWwindow* window, int key, int scancode, int action, int mods) {
         auto& impl = *self(window).impl_;
         const auto mappedKey = toKey(key);
         if (action == GLFW_PRESS) {
            impl.input.onKey(mappedKey, Input::KeyAction::Press);
            impl.events.emplace(event::KeyPressed{mappedKey, scancode, mods});
         } else if (action == GLFW_RELEASE) {
            impl.input.onKey(mappedKey, Input::KeyAction::Release);
            impl.events.emplace(event::KeyReleased{mappedKey, scancode, mods});
         }
      });

      glfwSetCursorPosCallback(impl_->handle.get(), [](GLFWwindow* window, double x, double y) {
         auto& impl = *self(window).impl_;
         impl.input.onMouseMove({x, y});
         impl.events.emplace(event::MouseMoved{{x, y}});
      });

      glfwSetMouseButtonCallback(impl_->handle.get(), [](GLFWwindow* window, int button, int action, int mods) {
         auto& impl = *self(window).impl_;
         const auto mappedButton = toMouseButton(button);
         if (action == GLFW_PRESS) {
            impl.input.onMouseButton(mappedButton, Input::KeyAction::Press);
            impl.events.emplace(event::MouseButtonPressed{mappedButton, mods});
         } else if (action == GLFW_RELEASE) {
            impl.input.onMouseButton(mappedButton, Input::KeyAction::Release);
            impl.events.emplace(event::MouseButtonReleased{mappedButton, mods});
         }
      });

      glfwSetScrollCallback(impl_->handle.get(), [](GLFWwindow* window, double xOffset, double yOffset) {
         auto& impl = *self(window).impl_;
         impl.input.onScroll({xOffset, yOffset});
         impl.events.emplace(event::MouseScrolled{{xOffset, yOffset}});
      });
   }

} // namespace etherblocks::system

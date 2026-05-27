#ifndef ETHERBLOCKS_WINDOW_HPP
#define ETHERBLOCKS_WINDOW_HPP

#include <optional>
#include <queue>
#include <string>
#include <utility>
#include <variant>

struct GLFWwindow;

namespace etherblocks {

   struct Color {
      float r{};
      float g{};
      float b{};
      float a{};
   };

   enum class Buffer : unsigned {
      Color = 1 << 0,
      Depth = 1 << 1,
      Stencil = 1 << 2,
   };

   inline Buffer operator|(Buffer a, Buffer b) {
      return static_cast<Buffer>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
   }

   namespace event {

      struct Closed {};

      struct Resized {
         int width{};
         int height{};
      };

      struct KeyPressed {
         int key{};
         int scancode{};
         int mods{};
      };

      struct KeyReleased {
         int key{};
         int scancode{};
         int mods{};
      };

      struct MouseMoved {
         double x{};
         double y{};
      };

      struct MouseButtonPressed {
         int button{};
         int mods{};
      };

      struct MouseButtonReleased {
         int button{};
         int mods{};
      };

      struct MouseScrolled {
         double xOffset{};
         double yOffset{};
      };

      using Any = std::variant<Closed, Resized, KeyPressed, KeyReleased, MouseMoved, MouseButtonPressed, MouseButtonReleased,
                               MouseScrolled>;
   } // namespace event

   enum class CursorMode { Normal, Hidden, Disabled };

   enum class RenderFeature {
      DepthTest,
      StencilTest,
      Blending,
      CullFace,
      Multisample,
   };

   struct WindowConfig {
      int width{800};
      int height{600};
      std::string title{"Window"};
      bool vsync{true};
      int glMajor{3};
      int glMinor{3};
   };

   class Window {
   public:
      // TODO: static create() method - no exceptions?
      explicit Window(WindowConfig cfg = {});
      ~Window();

      Window(const Window&) = delete;
      Window& operator=(const Window&) = delete;

      Window(Window&& other) noexcept;
      Window& operator=(Window&& other) noexcept;

      [[nodiscard]] bool isOpen() const;

      void pollEvents();

      std::optional<event::Any> nextEvent();

      void display();
      void clear(Color color = {}, Buffer buffers = Buffer::Color | Buffer::Depth);
      void close();

      // TODO: move to separate entity?
      void enable(RenderFeature feature);
      void disable(RenderFeature feature);

      [[nodiscard]] std::pair<int, int> framebufferSize() const;
      [[nodiscard]] std::pair<int, int> size() const;
      [[nodiscard]] GLFWwindow* nativeHandle() const noexcept;

      void setCursorMode(CursorMode mode);
      void setStickyKeys(bool enabled);
      void setRawMouseMotion(bool enabled);

   private:
      void setupCallbacks();

      static Window& self(GLFWwindow* window);

   private:
      GLFWwindow* handle_{nullptr};
      std::queue<event::Any> events_;
   };

} // namespace etherblocks

#endif // ETHERBLOCKS_WINDOW_HPP

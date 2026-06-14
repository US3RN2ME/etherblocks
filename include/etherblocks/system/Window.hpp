#ifndef ETHERBLOCKS_SYSTEM_WINDOW_HPP
#define ETHERBLOCKS_SYSTEM_WINDOW_HPP

#include <etherblocks/system/Input.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace etherblocks::system {

   namespace event {

      struct Closed {};

      struct Resized {
         glm::ivec2 size{};
      };

      struct KeyPressed {
         Key key{};
         int scancode{};
         int mods{};
      };

      struct KeyReleased {
         Key key{};
         int scancode{};
         int mods{};
      };

      struct MouseMoved {
         glm::dvec2 position{};
      };

      struct MouseButtonPressed {
         MouseButton button{};
         int mods{};
      };

      struct MouseButtonReleased {
         MouseButton button{};
         int mods{};
      };

      struct MouseScrolled {
         glm::dvec2 offset{};
      };

      using Any = std::variant<Closed, Resized, KeyPressed, KeyReleased, MouseMoved, MouseButtonPressed, MouseButtonReleased,
                               MouseScrolled>;
   } // namespace event

   enum class CursorMode { Normal, Hidden, Disabled };

   struct WindowConfig {
      glm::ivec2 size{800, 600};
      std::string title{"Window"};
      bool vsync{true};
      bool fullscreen{false};
      int glMajor{3};
      int glMinor{3};
   };

   class Window {
   public:
      explicit Window(const WindowConfig& cfg = {});
      ~Window();

      Window(const Window&) = delete;
      Window& operator=(const Window&) = delete;

      Window(Window&& other) noexcept;
      Window& operator=(Window&& other) noexcept;

      [[nodiscard]] bool isOpen() const;
      [[nodiscard]] const Input& input() const noexcept;

      void pollEvents();

      std::optional<event::Any> nextEvent();

      void display();
      void close();

      [[nodiscard]] glm::ivec2 framebufferSize() const;
      [[nodiscard]] glm::ivec2 size() const;
      [[nodiscard]] std::vector<glm::ivec2> availableResolutions() const;
      [[nodiscard]] static double elapsedTime() noexcept;

      void setSize(glm::ivec2 size);
      void setVsync(bool enabled);
      void setFullscreen(bool enabled);
      void setCursorMode(CursorMode mode);
      void setStickyKeys(bool enabled);
      void setRawMouseMotion(bool enabled);

   private:
      struct Impl;

      void setupCallbacks();
      std::unique_ptr<Impl> impl_;
   };

} // namespace etherblocks::system

#endif // ETHERBLOCKS_SYSTEM_WINDOW_HPP

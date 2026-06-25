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

      /**
       * @brief Event emitted when the window requests closing.
       */
      struct Closed {};

      /**
       * @brief Event emitted after the window size changes.
       */
      struct Resized {
         /**
          * @brief New window size in screen coordinates.
          */
         glm::ivec2 size{};
      };

      /**
       * @brief Event emitted when a key is pressed.
       */
      struct KeyPressed {
         /**
          * @brief Logical key identifier.
          */
         Key key{};

         /**
          * @brief Platform scancode reported by GLFW.
          */
         int scancode{};

         /**
          * @brief Platform modifier bitmask reported by GLFW.
          */
         int mods{};
      };

      /**
       * @brief Event emitted when a key is released.
       */
      struct KeyReleased {
         /**
          * @brief Logical key identifier.
          */
         Key key{};

         /**
          * @brief Platform scancode reported by GLFW.
          */
         int scancode{};

         /**
          * @brief Platform modifier bitmask reported by GLFW.
          */
         int mods{};
      };

      /**
       * @brief Event emitted when the cursor moves.
       */
      struct MouseMoved {
         /**
          * @brief Cursor position in window coordinates.
          */
         glm::dvec2 position{};
      };

      /**
       * @brief Event emitted when a mouse button is pressed.
       */
      struct MouseButtonPressed {
         /**
          * @brief Logical mouse button identifier.
          */
         MouseButton button{};

         /**
          * @brief Platform modifier bitmask reported by GLFW.
          */
         int mods{};
      };

      /**
       * @brief Event emitted when a mouse button is released.
       */
      struct MouseButtonReleased {
         /**
          * @brief Logical mouse button identifier.
          */
         MouseButton button{};

         /**
          * @brief Platform modifier bitmask reported by GLFW.
          */
         int mods{};
      };

      /**
       * @brief Event emitted when the mouse wheel or touchpad scrolls.
       */
      struct MouseScrolled {
         /**
          * @brief Scroll delta.
          */
         glm::dvec2 offset{};
      };

      /**
       * @brief Variant containing any window event type.
       */
      using Any = std::variant<Closed, Resized, KeyPressed, KeyReleased, MouseMoved, MouseButtonPressed, MouseButtonReleased,
                               MouseScrolled>;
   } // namespace event

   /**
    * @brief Cursor visibility and capture mode.
    */
   enum class CursorMode { Normal, Hidden, Disabled };

   /**
    * @brief Window creation parameters.
    */
   struct WindowConfig {
      /**
       * @brief Initial window size.
       */
      glm::ivec2 size{800, 600};

      /**
       * @brief Window title.
       */
      std::string title{"Window"};

      /**
       * @brief Whether presentation should synchronize to vertical refresh.
       */
      bool vsync{true};

      /**
       * @brief Whether the window should start fullscreen.
       */
      bool fullscreen{false};

      /**
       * @brief Requested OpenGL major version.
       */
      int glMajor{3};

      /**
       * @brief Requested OpenGL minor version.
       */
      int glMinor{3};
   };

   /**
    * @brief GLFW-backed window and OpenGL context owner.
    */
   class Window {
   public:
      /**
       * @brief Create a window and graphics context.
       *
       * @param cfg Window creation parameters.
       */
      explicit Window(const WindowConfig& cfg = {});

      /**
       * @brief Destroy the window and owned graphics context.
       */
      ~Window();

      Window(const Window&) = delete;
      Window& operator=(const Window&) = delete;

      /**
       * @brief Move construct a window.
       *
       * @param other Window to move from.
       */
      Window(Window&& other) noexcept;

      /**
       * @brief Move assign a window.
       *
       * @param other Window to move from.
       *
       * @return This window.
       */
      Window& operator=(Window&& other) noexcept;

      /**
       * @brief Test whether the window is still open.
       *
       * @return True while the window can continue running.
       */
      [[nodiscard]] bool isOpen() const;

      /**
       * @brief Return the current per-frame input state.
       *
       * @return Input state owned by the window.
       */
      [[nodiscard]] const Input& input() const noexcept;

      /**
       * @brief Poll platform events and update input state.
       */
      void pollEvents();

      /**
       * @brief Pop the next queued window event.
       *
       * @return Event when one is available.
       */
      std::optional<event::Any> nextEvent();

      /**
       * @brief Present the back buffer.
       */
      void display();

      /**
       * @brief Request that the window closes.
       */
      void close();

      /**
       * @brief Return the framebuffer size in physical pixels.
       *
       * @return Framebuffer size.
       */
      [[nodiscard]] glm::ivec2 framebufferSize() const;

      /**
       * @brief Return the window size in screen coordinates.
       *
       * @return Window size.
       */
      [[nodiscard]] glm::ivec2 size() const;

      /**
       * @brief Return fullscreen video mode resolutions available on the primary monitor.
       *
       * @return List of supported resolutions.
       */
      [[nodiscard]] std::vector<glm::ivec2> availableResolutions() const;

      /**
       * @brief Return elapsed application time.
       *
       * @return Time in seconds.
       */
      [[nodiscard]] static double elapsedTime() noexcept;

      /**
       * @brief Resize the window.
       *
       * @param size New window size.
       */
      void setSize(glm::ivec2 size);

      /**
       * @brief Enable or disable vertical sync.
       *
       * @param enabled True to enable vsync.
       */
      void setVsync(bool enabled);

      /**
       * @brief Switch between windowed and fullscreen mode.
       *
       * @param enabled True to use fullscreen mode.
       */
      void setFullscreen(bool enabled);

      /**
       * @brief Set cursor visibility and capture mode.
       *
       * @param mode New cursor mode.
       */
      void setCursorMode(CursorMode mode);

      /**
       * @brief Enable or disable sticky key handling.
       *
       * @param enabled True to enable sticky keys.
       */
      void setStickyKeys(bool enabled);

      /**
       * @brief Enable or disable raw mouse motion when supported.
       *
       * @param enabled True to request raw mouse motion.
       */
      void setRawMouseMotion(bool enabled);

   private:
      struct Impl;

      void setupCallbacks();
      std::unique_ptr<Impl> impl_;
   };

} // namespace etherblocks::system

#endif // ETHERBLOCKS_SYSTEM_WINDOW_HPP

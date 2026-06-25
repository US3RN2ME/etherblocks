#ifndef ETHERBLOCKS_SYSTEM_INPUT_HPP
#define ETHERBLOCKS_SYSTEM_INPUT_HPP

#include <array>
#include <cstdint>
#include <glm/glm.hpp>

namespace etherblocks::system {

   /**
    * @brief Keyboard key identifiers used by the input system.
    */
   enum class Key {
      A,
      B,
      C,
      D,
      E,
      F,
      G,
      H,
      I,
      J,
      K,
      L,
      M,
      N,
      O,
      P,
      Q,
      R,
      S,
      T,
      U,
      V,
      W,
      X,
      Y,
      Z,
      Num0,
      Num1,
      Num2,
      Num3,
      Num4,
      Num5,
      Num6,
      Num7,
      Num8,
      Num9,
      F1,
      F2,
      F3,
      F4,
      F5,
      F6,
      F7,
      F8,
      F9,
      F10,
      F11,
      F12,
      Up,
      Down,
      Left,
      Right,
      PageUp,
      PageDown,
      Home,
      End,
      Insert,
      Delete,
      Space,
      Enter,
      Escape,
      Tab,
      Backspace,
      CapsLock,
      LeftShift,
      RightShift,
      LeftCtrl,
      RightCtrl,
      LeftAlt,
      RightAlt,
      Numpad0,
      Numpad1,
      Numpad2,
      Numpad3,
      Numpad4,
      Numpad5,
      Numpad6,
      Numpad7,
      Numpad8,
      Numpad9,
      NumpadEnter,
      NumpadAdd,
      NumpadSub,
      NumpadMul,
      NumpadDiv,

      Unknown,
   };

   /**
    * @brief Mouse button identifiers used by the input system.
    */
   enum class MouseButton {
      Left,
      Right,
      Middle,
      Button4,
      Button5,
      Unknown,
   };

   /**
    * @brief Per-frame keyboard, mouse button, cursor, and scroll state.
    */
   class Input {
   public:
      /**
       * @brief Test whether a key was pressed during the current frame.
       *
       * @param key Key to test.
 *

       * * @return True when the key transitioned to held this frame.
       */
      [[nodiscard]] bool isKeyPressed(Key key) const noexcept;

      /**
       * @brief Test whether a key is currently held.
       *
       * @param key Key to test.
       *
       *
       * @return True while the key is held.
       */
      [[nodiscard]] bool isKeyHeld(Key key) const noexcept;

      /**
       * @brief Test whether a key was released during the current frame.
       *
       * @param key Key to test.

       * *
       * @return True when the key transitioned to released this frame.
       */
      [[nodiscard]] bool isKeyReleased(Key key) const noexcept;

      /**
       * @brief Test whether a mouse button was pressed during the current frame.
       *
       * @param btn Mouse
       * button to test.
       *
       * @return True when the button transitioned to held this frame.
       */
      [[nodiscard]] bool isMouseButtonPressed(MouseButton btn) const noexcept;

      /**
       * @brief Test whether a mouse button is currently held.
       *
       * @param btn Mouse button to test.
 *

       * * @return True while the button is held.
       */
      [[nodiscard]] bool isMouseButtonHeld(MouseButton btn) const noexcept;

      /**
       * @brief Test whether a mouse button was released during the current frame.
       *
       * @param btn Mouse
       * button to test.
       *
       * @return True when the button transitioned to released this frame.
       */
      [[nodiscard]] bool isMouseButtonReleased(MouseButton btn) const noexcept;

      /**
       * @brief Return the latest cursor position.
       *
       * @return Cursor position in window coordinates.

       */
      [[nodiscard]] glm::dvec2 mousePosition() const noexcept;

      /**
       * @brief Return cursor movement accumulated for the current frame.
       *
       * @return Cursor delta in
       * window coordinates.
       */
      [[nodiscard]] glm::dvec2 mouseDelta() const noexcept;

      /**
       * @brief Return scroll movement accumulated for the current frame.
       *
       * @return Scroll delta.
 */
      [[nodiscard]] glm::dvec2 scrollDelta() const noexcept;

   private:
      friend class Window;

      enum class KeyAction { Press, Release };

      void flush() noexcept;
      void onKey(Key key, KeyAction action) noexcept;
      void onMouseButton(MouseButton btn, KeyAction action) noexcept;
      void onMouseMove(glm::dvec2 position) noexcept;
      void onScroll(glm::dvec2 offset) noexcept;

      static constexpr auto kKeyCount = static_cast<std::size_t>(Key::Unknown);

      static constexpr auto kButtonCount = static_cast<std::size_t>(MouseButton::Unknown);

      enum StateFlags : std::uint8_t {
         Held = 1 << 0,
         Pressed = 1 << 1,
         Released = 1 << 2,
      };

      std::array<std::uint8_t, kKeyCount> keys_{};
      std::array<std::uint8_t, kButtonCount> buttons_{};

      glm::dvec2 mousePosition_{};
      glm::dvec2 previousMousePosition_{};
      glm::dvec2 scrollDelta_{};
      bool hasMousePosition_{};
   };

} // namespace etherblocks::system

#endif // ETHERBLOCKS_SYSTEM_INPUT_HPP

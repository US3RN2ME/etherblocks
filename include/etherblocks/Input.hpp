#ifndef ETHERBLOCKS_INPUT_HPP
#define ETHERBLOCKS_INPUT_HPP

#include <array>
#include <cstdint>
#include <glm/glm.hpp>

namespace etherblocks {

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

   enum class MouseButton {
      Left,
      Right,
      Middle,
      Button4,
      Button5,
      Unknown,
   };

   class Input {
   public:
      [[nodiscard]] bool isKeyPressed(Key key) const noexcept;
      [[nodiscard]] bool isKeyHeld(Key key) const noexcept;
      [[nodiscard]] bool isKeyReleased(Key key) const noexcept;

      [[nodiscard]] bool isMouseButtonPressed(MouseButton btn) const noexcept;
      [[nodiscard]] bool isMouseButtonHeld(MouseButton btn) const noexcept;
      [[nodiscard]] bool isMouseButtonReleased(MouseButton btn) const noexcept;

      [[nodiscard]] glm::dvec2 mousePosition() const noexcept;

      [[nodiscard]] glm::dvec2 mouseDelta() const noexcept;

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

} // namespace etherblocks

#endif // ETHERBLOCKS_INPUT_HPP

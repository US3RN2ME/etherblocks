#include <etherblocks/Input.hpp>

namespace etherblocks {

   namespace {

      constexpr std::size_t idx(Key key) noexcept {
         return static_cast<std::size_t>(key);
      }

      constexpr std::size_t idx(MouseButton btn) noexcept {
         return static_cast<std::size_t>(btn);
      }

   } // namespace

   void Input::flush() noexcept {
      for (auto& k : keys_)
         k &= ~(Pressed | Released);

      for (auto& b : buttons_)
         b &= ~(Pressed | Released);

      previousMousePosition_ = mousePosition_;
      scrollDelta_ = {};
   }

   void Input::onKey(Key key, KeyAction action) noexcept {
      if (key == Key::Unknown)
         return;

      auto& state = keys_[idx(key)];

      if (action == KeyAction::Press) {
         if ((state & Held) == 0)
            state |= Pressed;
         state |= Held;
      } else {
         state &= ~Held;
         state |= Released;
      }
   }

   void Input::onMouseButton(MouseButton btn, KeyAction action) noexcept {
      if (btn == MouseButton::Unknown)
         return;

      auto& state = buttons_[idx(btn)];

      if (action == KeyAction::Press) {
         if ((state & Held) == 0)
            state |= Pressed;
         state |= Held;
      } else {
         state &= ~Held;
         state |= Released;
      }
   }

   void Input::onMouseMove(glm::dvec2 position) noexcept {
      if (!hasMousePosition_) {
         previousMousePosition_ = position;
         hasMousePosition_ = true;
      }
      mousePosition_ = position;
   }

   void Input::onScroll(glm::dvec2 offset) noexcept {
      scrollDelta_ += offset;
   }

   bool Input::isKeyPressed(Key key) const noexcept {
      if (key == Key::Unknown)
         return false;
      return (keys_[idx(key)] & Pressed) != 0;
   }

   bool Input::isKeyHeld(Key key) const noexcept {
      if (key == Key::Unknown)
         return false;
      return (keys_[idx(key)] & Held) != 0;
   }

   bool Input::isKeyReleased(Key key) const noexcept {
      if (key == Key::Unknown)
         return false;
      return (keys_[idx(key)] & Released) != 0;
   }

   bool Input::isMouseButtonPressed(MouseButton btn) const noexcept {
      if (btn == MouseButton::Unknown)
         return false;
      return (buttons_[idx(btn)] & Pressed) != 0;
   }

   bool Input::isMouseButtonHeld(MouseButton btn) const noexcept {
      if (btn == MouseButton::Unknown)
         return false;
      return (buttons_[idx(btn)] & Held) != 0;
   }

   bool Input::isMouseButtonReleased(MouseButton btn) const noexcept {
      if (btn == MouseButton::Unknown)
         return false;
      return (buttons_[idx(btn)] & Released) != 0;
   }

   glm::dvec2 Input::mousePosition() const noexcept {
      return mousePosition_;
   }

   glm::dvec2 Input::mouseDelta() const noexcept {
      return mousePosition_ - previousMousePosition_;
   }

   glm::dvec2 Input::scrollDelta() const noexcept {
      return scrollDelta_;
   }

} // namespace etherblocks

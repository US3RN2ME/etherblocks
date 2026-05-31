#include <etherblocks/system/FrameClock.hpp>

namespace etherblocks::system {

   float FrameClock::tick(double time) noexcept {
      const auto current = static_cast<float>(time);
      const auto delta = initialized_ ? current - previous_ : 0.0f;
      previous_ = current;
      initialized_ = true;
      return delta;
   }

} // namespace etherblocks::system

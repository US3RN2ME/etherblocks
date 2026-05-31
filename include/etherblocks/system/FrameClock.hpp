#ifndef ETHERBLOCKS_SYSTEM_FRAMECLOCK_HPP
#define ETHERBLOCKS_SYSTEM_FRAMECLOCK_HPP

namespace etherblocks::system {

   class FrameClock {
   public:
      [[nodiscard]] float tick(double time) noexcept;

   private:
      float previous_{};
      bool initialized_{};
   };

} // namespace etherblocks::system

#endif // ETHERBLOCKS_SYSTEM_FRAMECLOCK_HPP

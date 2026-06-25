#ifndef ETHERBLOCKS_SYSTEM_FRAMECLOCK_HPP
#define ETHERBLOCKS_SYSTEM_FRAMECLOCK_HPP

namespace etherblocks::system {

   /**
    * @brief Tracks frame-to-frame elapsed time.
    */
   class FrameClock {
   public:
      /**
       * @brief Advance the clock and return the time since the previous tick.
       *
       * @param time Current absolute time in seconds.
       *
       * @return Delta time in seconds.
       */
      [[nodiscard]] float tick(double time) noexcept;

   private:
      float previous_{};
      bool initialized_{};
   };

} // namespace etherblocks::system

#endif // ETHERBLOCKS_SYSTEM_FRAMECLOCK_HPP

#include <etherblocks/system/FrameClock.hpp>

#include "ut_main.hpp"

namespace {
   suite<"[FrameClock]"> _ = [] {
      "[FirstTickReturnsZeroAndSubsequentTicksReturnDelta]"_test = [] {
         etherblocks::system::FrameClock clock;

         expect(eq(clock.tick(10.0), 0.0_f));
         expect(eq(clock.tick(10.25), 0.25_f));
         expect(eq(clock.tick(11.0), 0.75_f));
      };
   };
} // namespace

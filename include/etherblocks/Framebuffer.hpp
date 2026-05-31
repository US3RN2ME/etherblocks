#ifndef ETHERBLOCKS_FRAMEBUFFER_HPP
#define ETHERBLOCKS_FRAMEBUFFER_HPP

#include <etherblocks/detail/GlObject.hpp>

namespace etherblocks {

   class Framebuffer : private detail::GlObject {
   public:
      Framebuffer();
      Framebuffer(Framebuffer&& other) noexcept;
      Framebuffer& operator=(Framebuffer&& other) noexcept;
      ~Framebuffer();

      void bind() const noexcept;
      static void bindDefault() noexcept;

   private:
      void reset() noexcept;
   };

} // namespace etherblocks

#endif // ETHERBLOCKS_FRAMEBUFFER_HPP

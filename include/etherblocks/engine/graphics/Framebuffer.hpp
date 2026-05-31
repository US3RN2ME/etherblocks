#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_FRAMEBUFFER_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_FRAMEBUFFER_HPP

#include <etherblocks/engine/graphics/detail/GlObject.hpp>

namespace etherblocks::engine::graphics {

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

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_FRAMEBUFFER_HPP

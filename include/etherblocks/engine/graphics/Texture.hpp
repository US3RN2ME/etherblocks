#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_TEXTURE_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_TEXTURE_HPP

#include <etherblocks/engine/graphics/detail/GlObject.hpp>
#include <string_view>

namespace etherblocks::engine::graphics {

   class Texture : private detail::GlObject {
   public:
      explicit Texture(std::string_view path);
      Texture(Texture&& other) noexcept;
      Texture& operator=(Texture&& other) noexcept;
      ~Texture();

      void bind(unsigned int unit = 0) const noexcept;

   private:
      void reset() noexcept;
   };

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_TEXTURE_HPP

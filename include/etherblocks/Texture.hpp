#ifndef ETHERBLOCKS_TEXTURE_HPP
#define ETHERBLOCKS_TEXTURE_HPP

#include <etherblocks/detail/GlObject.hpp>
#include <string_view>

namespace etherblocks {

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

} // namespace etherblocks

#endif // ETHERBLOCKS_TEXTURE_HPP

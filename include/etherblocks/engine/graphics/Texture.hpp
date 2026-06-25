#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_TEXTURE_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_TEXTURE_HPP

#include <etherblocks/engine/graphics/detail/GlObject.hpp>
#include <string_view>

namespace etherblocks::engine::graphics {

   /**
    * @brief RAII wrapper for a 2D texture loaded from disk.
    */
   class Texture : private detail::GlObject {
   public:
      /**
       * @brief Load a texture from an image file.
       *
       * @param path Image path.
       */
      explicit Texture(std::string_view path);

      /**
       * @brief Move construct a texture.
       *
       * @param other Texture to move from.
       */
      Texture(Texture&& other) noexcept;

      /**
       * @brief Move assign a texture.
       *
       * @param other Texture to move from.
       *
       * @return This texture.
       */
      Texture& operator=(Texture&& other) noexcept;

      /**
       * @brief Destroy the texture object.
       */
      ~Texture();

      /**
       * @brief Bind the texture to a texture unit.
       *
       * @param unit Texture unit index.
       */
      void bind(unsigned int unit = 0) const noexcept;

   private:
      void reset() noexcept;
   };

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_TEXTURE_HPP

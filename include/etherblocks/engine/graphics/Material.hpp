#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_MATERIAL_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_MATERIAL_HPP

#include <etherblocks/engine/graphics/Shader.hpp>
#include <string_view>

namespace etherblocks::engine::graphics {

   class Texture;

   class Material {
   public:
      Material(std::string_view vertexShaderPath, std::string_view fragmentShaderPath);

      void bind() const noexcept;
      void setTexture(const Texture& texture, unsigned int unit = 0) noexcept;

      [[nodiscard]] ShaderProgram& shader() noexcept;
      [[nodiscard]] const ShaderProgram& shader() const noexcept;

   private:
      ShaderProgram shader_;
      const Texture* texture_{};
      unsigned int textureUnit_{};
   };

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_MATERIAL_HPP

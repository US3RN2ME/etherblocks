#ifndef ETHERBLOCKS_MATERIAL_HPP
#define ETHERBLOCKS_MATERIAL_HPP

#include <etherblocks/Shader.hpp>
#include <string_view>

namespace etherblocks {

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

} // namespace etherblocks

#endif // ETHERBLOCKS_MATERIAL_HPP

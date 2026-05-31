#include <etherblocks/engine/graphics/Material.hpp>
#include <etherblocks/engine/graphics/Texture.hpp>

namespace etherblocks::engine::graphics {

   Material::Material(std::string_view vertexShaderPath, std::string_view fragmentShaderPath)
       : shader_(vertexShaderPath, fragmentShaderPath) {}

   void Material::bind() const noexcept {
      shader_.use();
      if (texture_ != nullptr) {
         texture_->bind(textureUnit_);
      }
   }

   void Material::setTexture(const Texture& texture, unsigned int unit) noexcept {
      texture_ = &texture;
      textureUnit_ = unit;
   }

   ShaderProgram& Material::shader() noexcept {
      return shader_;
   }

   const ShaderProgram& Material::shader() const noexcept {
      return shader_;
   }

} // namespace etherblocks::engine::graphics

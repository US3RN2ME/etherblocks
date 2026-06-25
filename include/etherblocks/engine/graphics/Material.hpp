#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_MATERIAL_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_MATERIAL_HPP

#include <etherblocks/engine/graphics/Shader.hpp>
#include <string_view>

namespace etherblocks::engine::graphics {

   class Texture;

   /**
    * @brief Shader program plus optional bound texture state.
    */
   class Material {
   public:
      /**
       * @brief Construct a material from shader source files.
       *
       * @param vertexShaderPath Vertex shader source path.
       * @param fragmentShaderPath Fragment shader source path.
       */
      Material(std::string_view vertexShaderPath, std::string_view fragmentShaderPath);

      /**
       * @brief Bind the shader and configured texture state.
       */
      void bind() const noexcept;

      /**
       * @brief Assign a texture to bind with the material.
       *
       * @param texture Texture to bind.
       * @param unit Texture unit index.
       */
      void setTexture(const Texture& texture, unsigned int unit = 0) noexcept;

      /**
       * @brief Return the mutable shader program.
       *
       * @return Shader program.
       */
      [[nodiscard]] ShaderProgram& shader() noexcept;

      /**
       * @brief Return the shader program.
       *
       * @return Shader program.
       */
      [[nodiscard]] const ShaderProgram& shader() const noexcept;

   private:
      ShaderProgram shader_;
      const Texture* texture_{};
      unsigned int textureUnit_{};
   };

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_MATERIAL_HPP

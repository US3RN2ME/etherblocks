#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_SHADER_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_SHADER_HPP

#include <etherblocks/engine/graphics/detail/GlObject.hpp>
#include <glm/glm.hpp>
#include <string_view>

namespace etherblocks::engine::graphics {

   /**
    * @brief Supported shader stages.
    */
   enum class ShaderType { Vertex, Fragment };

   /**
    * @brief RAII wrapper for a compiled OpenGL shader object.
    */
   class Shader : private detail::GlObject {
   public:
      /**
       * @brief Load and compile a shader from disk.
       *
       * @param path Shader source path.
       * @param type Shader stage.
       */
      Shader(std::string_view path, ShaderType type);

      /**
       * @brief Move construct a shader.
       *
       * @param other Shader to move from.
       */
      Shader(Shader&& other) noexcept;

      /**
       * @brief Move assign a shader.
       *
       * @param other Shader to move from.
       *
       * @return This shader.
       */
      Shader& operator=(Shader&& other) noexcept;

      /**
       * @brief Destroy the shader object.
       */
      ~Shader();

   private:
      friend class ShaderProgram;

      [[nodiscard]] unsigned int id() const noexcept;
      void reset() noexcept;
   };

   /**
    * @brief Linked OpenGL shader program.
    */
   class ShaderProgram : private detail::GlObject {
   public:
      /**
       * @brief Compile, attach, and link a vertex and fragment shader pair.
       *
       * @param vertexPath Vertex shader source path.
       * @param fragmentPath Fragment shader source path.
       */
      ShaderProgram(std::string_view vertexPath, std::string_view fragmentPath);

      /**
       * @brief Set a boolean uniform.
       *
       * @param variable Uniform name.
       * @param value Uniform value.
       */
      void set(std::string_view variable, bool value) const;

      /**
       * @brief Set an integer uniform.
       *
       * @param variable Uniform name.
       * @param value Uniform value.
       */
      void set(std::string_view variable, int value) const;

      /**
       * @brief Set a floating-point uniform.
       *
       * @param variable Uniform name.
       * @param value Uniform value.
       */
      void set(std::string_view variable, float value) const;

      /**
       * @brief Set a vec4 uniform.
       *
       * @param variable Uniform name.
       * @param value Uniform value.
       */
      void set(std::string_view variable, const glm::vec4& value) const;

      /**
       * @brief Set a mat4 uniform.
       *
       * @param variable Uniform name.
       * @param value Uniform value.
       */
      void set(std::string_view variable, const glm::mat4& value) const;

      /**
       * @brief Move construct a shader program.
       *
       * @param other Shader program to move from.
       */
      ShaderProgram(ShaderProgram&& other) noexcept;

      /**
       * @brief Move assign a shader program.
       *
       * @param other Shader program to move from.
       *
       * @return This shader program.
       */
      ShaderProgram& operator=(ShaderProgram&& other) noexcept;

      /**
       * @brief Destroy the shader program.
       */
      ~ShaderProgram();

      /**
       * @brief Bind the program for subsequent draw calls.
       */
      void use() const noexcept;

   private:
      void reset() noexcept;
   };

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_SHADER_HPP

#ifndef ETHERBLOCKS_SHADER_HPP
#define ETHERBLOCKS_SHADER_HPP

#include <etherblocks/detail/GlObject.hpp>
#include <glm/glm.hpp>
#include <string_view>

namespace etherblocks {

   enum class ShaderType { Vertex, Fragment };

   class Shader : private detail::GlObject {
   public:
      Shader(std::string_view path, ShaderType type);
      Shader(Shader&& other) noexcept;
      Shader& operator=(Shader&& other) noexcept;
      ~Shader();

   private:
      friend class ShaderProgram;

      [[nodiscard]] unsigned int id() const noexcept;
      void reset() noexcept;
   };

   class ShaderProgram : private detail::GlObject {
   public:
      ShaderProgram(std::string_view vertexPath, std::string_view fragmentPath);

      void set(std::string_view variable, bool value) const;
      void set(std::string_view variable, int value) const;
      void set(std::string_view variable, float value) const;
      void set(std::string_view variable, const glm::vec4& value) const;
      void set(std::string_view variable, const glm::mat4& value) const;

      ShaderProgram(ShaderProgram&& other) noexcept;
      ShaderProgram& operator=(ShaderProgram&& other) noexcept;
      ~ShaderProgram();

      void use() const noexcept;

   private:
      void reset() noexcept;
   };

} // namespace etherblocks

#endif // ETHERBLOCKS_SHADER_HPP

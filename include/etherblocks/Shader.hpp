#ifndef ETHERBLOCKS_SHADER_HPP
#define ETHERBLOCKS_SHADER_HPP

#include <glad/glad.h>
#include <string>
#include <string_view>
#include <type_traits>

namespace etherblocks {

   class Object {
   public:
      Object(const Object&) = delete;
      Object& operator=(const Object&) = delete;

      [[nodiscard]] GLuint id() const noexcept;

   protected:
      Object() = default;
      Object(Object&& other) noexcept;

      Object& move_assign(Object&& other) noexcept;

      GLuint id_{0};
   };

   class Shader : public Object {
   public:
      Shader(std::string_view path, GLenum type);
      Shader(Shader&& other) noexcept;
      Shader& operator=(Shader&& other) noexcept;
      ~Shader();

   private:
      void reset() noexcept;
   };

   class ShaderProgram : public Object {
   public:
      ShaderProgram(std::string_view vertexPath, std::string_view fragmentPath);

      template <typename T>
      void set(std::string_view variable, const T& value) const {
         std::string name(variable);
         const GLint location = glGetUniformLocation(id_, name.c_str());
         if (location == -1) {
            return;
         }

         using U = std::remove_cvref_t<T>;

         if constexpr (std::is_same_v<U, bool> || std::is_same_v<U, int>) {
            glUniform1i(location, static_cast<GLint>(value));
         } else if constexpr (std::is_same_v<U, float>) {
            glUniform1f(location, value);
         } else {
            static_assert(false, "Unsupported uniform type");
         }
      }

      ShaderProgram(ShaderProgram&& other) noexcept;
      ShaderProgram& operator=(ShaderProgram&& other) noexcept;
      ~ShaderProgram();

      void use() const noexcept;

   private:
      void reset() noexcept;
   };

} // namespace etherblocks

#endif // ETHERBLOCKS_SHADER_HPP

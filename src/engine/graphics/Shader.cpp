#include <etherblocks/engine/graphics/Shader.hpp>
#include <etherblocks/system/Utils.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <string>
#include <utility>

namespace etherblocks::engine::graphics {

   namespace {
      void deleteShader(unsigned int id) noexcept {
         glDeleteShader(id);
      }

      void deleteProgram(unsigned int id) noexcept {
         glDeleteProgram(id);
      }

      template <void (*Delete)(unsigned int)>
      class GlHandleGuard {
      public:
         explicit GlHandleGuard(unsigned int id) noexcept
             : id_(id) {}

         ~GlHandleGuard() {
            if (id_ != 0) {
               Delete(id_);
            }
         }

         GlHandleGuard(const GlHandleGuard&) = delete;
         GlHandleGuard& operator=(const GlHandleGuard&) = delete;

         [[nodiscard]] unsigned int get() const noexcept {
            return id_;
         }

         [[nodiscard]] unsigned int release() noexcept {
            return std::exchange(id_, 0);
         }

      private:
         unsigned int id_{};
      };

      GLenum toGl(ShaderType type) {
         switch (type) {
            case ShaderType::Vertex:
               return GL_VERTEX_SHADER;
            case ShaderType::Fragment:
               return GL_FRAGMENT_SHADER;
         }
         throw std::invalid_argument{"Unknown shader type"};
      }

      GLint uniformLocation(unsigned int program, std::string_view variable) {
         const std::string name(variable);
         return glGetUniformLocation(program, name.c_str());
      }
   } // namespace

   Shader::Shader(std::string_view path, ShaderType type) {
      const auto source = system::Utils::readFile(path);
      if (source.empty()) {
         throw std::runtime_error("Failed to read shader file");
      }

      GlHandleGuard<deleteShader> shader(glCreateShader(toGl(type)));
      if (shader.get() == 0) {
         throw std::runtime_error("glCreateShader failed");
      }

      const char* data = source.c_str();
      glShaderSource(shader.get(), 1, &data, nullptr);
      glCompileShader(shader.get());

      GLint success = 0;
      glGetShaderiv(shader.get(), GL_COMPILE_STATUS, &success);
      if (success == 0) {
         GLint logLength = 0;
         glGetShaderiv(shader.get(), GL_INFO_LOG_LENGTH, &logLength);

         std::string log;
         if (logLength > 0) {
            log.resize(static_cast<std::size_t>(logLength));
            glGetShaderInfoLog(shader.get(), logLength, nullptr, log.data());
         }

         throw std::runtime_error("Shader compilation failed:\n" + log);
      }
      id_ = shader.release();
   }

   Shader::Shader(Shader&& other) noexcept
       : detail::GlObject(std::move(other)) {}

   Shader& Shader::operator=(Shader&& other) noexcept {
      if (this != &other) {
         reset();
         moveAssign(std::move(other));
      }
      return *this;
   }

   Shader::~Shader() {
      reset();
   }

   unsigned int Shader::id() const noexcept {
      return id_;
   }

   void Shader::reset() noexcept {
      if (id_ != 0) {
         glDeleteShader(id_);
         id_ = 0;
      }
   }

   ShaderProgram::ShaderProgram(std::string_view vertexPath, std::string_view fragmentPath) {
      Shader vertex(vertexPath, ShaderType::Vertex);
      Shader fragment(fragmentPath, ShaderType::Fragment);

      GlHandleGuard<deleteProgram> program(glCreateProgram());
      if (program.get() == 0) {
         throw std::runtime_error("glCreateProgram failed");
      }

      glAttachShader(program.get(), vertex.id());
      glAttachShader(program.get(), fragment.id());
      glLinkProgram(program.get());

      GLint success = 0;
      glGetProgramiv(program.get(), GL_LINK_STATUS, &success);
      if (success == 0) {
         GLint logLength = 0;
         glGetProgramiv(program.get(), GL_INFO_LOG_LENGTH, &logLength);

         std::string log;
         if (logLength > 0) {
            log.resize(static_cast<std::size_t>(logLength));
            glGetProgramInfoLog(program.get(), logLength, nullptr, log.data());
         }

         throw std::runtime_error("Program link failed:\n" + log);
      }
      id_ = program.release();
   }

   ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
       : detail::GlObject(std::move(other)) {}

   ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
      if (this != &other) {
         reset();
         moveAssign(std::move(other));
      }
      return *this;
   }

   ShaderProgram::~ShaderProgram() {
      reset();
   }

   void ShaderProgram::use() const noexcept {
      glUseProgram(id_);
   }

   void ShaderProgram::set(std::string_view variable, bool value) const {
      set(variable, static_cast<int>(value));
   }

   void ShaderProgram::set(std::string_view variable, int value) const {
      if (const auto location = uniformLocation(id_, variable); location != -1) {
         use();
         glUniform1i(location, value);
      }
   }

   void ShaderProgram::set(std::string_view variable, float value) const {
      if (const auto location = uniformLocation(id_, variable); location != -1) {
         use();
         glUniform1f(location, value);
      }
   }

   void ShaderProgram::set(std::string_view variable, const glm::vec4& value) const {
      if (const auto location = uniformLocation(id_, variable); location != -1) {
         use();
         glUniform4f(location, value.x, value.y, value.z, value.w);
      }
   }

   void ShaderProgram::set(std::string_view variable, const glm::mat4& value) const {
      if (const auto location = uniformLocation(id_, variable); location != -1) {
         use();
         glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
      }
   }

   void ShaderProgram::reset() noexcept {
      if (id_ != 0) {
         glDeleteProgram(id_);
         id_ = 0;
      }
   }

} // namespace etherblocks::engine::graphics

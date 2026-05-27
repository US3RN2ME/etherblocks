#include <etherblocks/Shader.hpp>
#include <etherblocks/Utils.hpp>
#include <stdexcept>
#include <utility>

namespace etherblocks {

   GLuint Object::id() const noexcept {
      return id_;
   }

   Object::Object(Object&& other) noexcept
       : id_(std::exchange(other.id_, 0)) {}

   Object& Object::move_assign(Object&& other) noexcept {
      if (this != &other) {
         id_ = std::exchange(other.id_, 0);
      }
      return *this;
   }

   Shader::Shader(std::string_view path, GLenum type) {
      const auto source = Utils::readFile(path);
      if (source.empty()) {
         throw std::runtime_error("Failed to read shader file");
      }

      id_ = glCreateShader(type);
      if (id_ == 0) {
         throw std::runtime_error("glCreateShader failed");
      }

      const char* data = source.c_str();
      glShaderSource(id_, 1, &data, nullptr);
      glCompileShader(id_);

      GLint success = 0;
      glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
      if (!success) {
         GLint logLength = 0;
         glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &logLength);

         std::string log;
         if (logLength > 0) {
            log.resize(static_cast<std::size_t>(logLength));
            glGetShaderInfoLog(id_, logLength, nullptr, log.data());
         }

         glDeleteShader(id_);
         id_ = 0;

         throw std::runtime_error("Shader compilation failed:\n" + log);
      }
   }

   Shader::Shader(Shader&& other) noexcept
       : Object(std::move(other)) {}

   Shader& Shader::operator=(Shader&& other) noexcept {
      if (this != &other) {
         reset();
         move_assign(std::move(other));
      }
      return *this;
   }

   Shader::~Shader() {
      reset();
   }

   void Shader::reset() noexcept {
      if (id_ != 0) {
         glDeleteShader(id_);
         id_ = 0;
      }
   }

   ShaderProgram::ShaderProgram(std::string_view vertexPath, std::string_view fragmentPath) {
      Shader vertex(vertexPath, GL_VERTEX_SHADER);
      Shader fragment(fragmentPath, GL_FRAGMENT_SHADER);

      id_ = glCreateProgram();
      if (id_ == 0) {
         throw std::runtime_error("glCreateProgram failed");
      }

      glAttachShader(id_, vertex.id());
      glAttachShader(id_, fragment.id());
      glLinkProgram(id_);

      GLint success = 0;
      glGetProgramiv(id_, GL_LINK_STATUS, &success);
      if (!success) {
         GLint logLength = 0;
         glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &logLength);

         std::string log;
         if (logLength > 0) {
            log.resize(static_cast<std::size_t>(logLength));
            glGetProgramInfoLog(id_, logLength, nullptr, log.data());
         }

         glDeleteProgram(id_);
         id_ = 0;

         throw std::runtime_error("Program link failed:\n" + log);
      }
   }

   ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
       : Object(std::move(other)) {}

   ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
      if (this != &other) {
         reset();
         move_assign(std::move(other));
      }
      return *this;
   }

   ShaderProgram::~ShaderProgram() {
      reset();
   }

   void ShaderProgram::use() const noexcept {
      glUseProgram(id_);
   }

   void ShaderProgram::reset() noexcept {
      if (id_ != 0) {
         glDeleteProgram(id_);
         id_ = 0;
      }
   }

} // namespace etherblocks

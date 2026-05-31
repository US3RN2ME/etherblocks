#include <etherblocks/engine/graphics/Texture.hpp>
#include <etherblocks/system/Logger.hpp>
#include <glad/glad.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace etherblocks::engine::graphics {

   namespace {
      enum class PixelFormat { Red, Rgb, Rgba };

      struct TextureGuard {
         ~TextureGuard() {
            if (id != 0) {
               glDeleteTextures(1, &id);
            }
         }

         [[nodiscard]] unsigned int release() noexcept {
            return std::exchange(id, 0);
         }

         unsigned int id{};
      };

      GLenum toGl(PixelFormat format) noexcept {
         switch (format) {
            case PixelFormat::Red:
               return GL_RED;
            case PixelFormat::Rgb:
               return GL_RGB;
            case PixelFormat::Rgba:
               return GL_RGBA;
         }
         return GL_RGB;
      }

      PixelFormat pixelFormat(int channelCount) {
         switch (channelCount) {
            case 1:
               return PixelFormat::Red;
            case 3:
               return PixelFormat::Rgb;
            case 4:
               return PixelFormat::Rgba;
            default:
               throw std::runtime_error{"Unsupported texture channel count"};
         }
      }
   } // namespace

   Texture::Texture(std::string_view path) {
      TextureGuard texture;
      glGenTextures(1, &texture.id);
      if (texture.id == 0) {
         system::log(system::LogLevel::Error, "glGenTextures failed");
         throw std::runtime_error{"glGenTextures failed"};
      }
      glBindTexture(GL_TEXTURE_2D, texture.id);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      stbi_set_flip_vertically_on_load(1);
      int width = 0;
      int height = 0;
      int channelCount = 0;
      const std::string filePath(path);
      const std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> data{
          stbi_load(filePath.c_str(), &width, &height, &channelCount, 0), &stbi_image_free};
      if (data == nullptr) {
         const auto* const reason = stbi_failure_reason();
         const auto message = "Failed to load texture: " + filePath + ": " + (reason != nullptr ? reason : "unknown");
         system::log(system::LogLevel::Error, message);
         throw std::runtime_error{message};
      }

      PixelFormat pixelFormatValue;
      try {
         pixelFormatValue = pixelFormat(channelCount);
      } catch (const std::exception& error) {
         system::log(system::LogLevel::Error, "Failed to load texture " + filePath + ": " + error.what());
         throw;
      }
      const auto format = toGl(pixelFormatValue);
      glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data.get());
      glGenerateMipmap(GL_TEXTURE_2D);
      id_ = texture.release();
      system::log(system::LogLevel::Debug, "Texture loaded: " + filePath);
   }

   Texture::Texture(Texture&& other) noexcept
       : detail::GlObject(std::move(other)) {}

   Texture& Texture::operator=(Texture&& other) noexcept {
      if (this != &other) {
         reset();
         moveAssign(std::move(other));
      }
      return *this;
   }

   Texture::~Texture() {
      reset();
   }

   void Texture::bind(unsigned int unit) const noexcept {
      glActiveTexture(GL_TEXTURE0 + unit);
      glBindTexture(GL_TEXTURE_2D, id_);
   }

   void Texture::reset() noexcept {
      if (id_ != 0) {
         glDeleteTextures(1, &id_);
         id_ = 0;
      }
   }

} // namespace etherblocks::engine::graphics

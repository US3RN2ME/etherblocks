#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <etherblocks/engine/graphics/Texture.hpp>
#include <etherblocks/engine/ui/UiLayer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace etherblocks::engine::ui {

   namespace {
      using graphics::Color;

      constexpr auto kFontWidth = 5;
      constexpr auto kFontHeight = 7;

      glm::vec4 toVec4(Color color) noexcept {
         return {color.r, color.g, color.b, color.a};
      }

      std::array<std::string_view, kFontHeight> glyphRows(char character) noexcept {
         switch (static_cast<char>(std::toupper(static_cast<unsigned char>(character)))) {
            case 'A':
               return {"01110", "10001", "10001", "11111", "10001", "10001", "10001"};
            case 'B':
               return {"11110", "10001", "10001", "11110", "10001", "10001", "11110"};
            case 'C':
               return {"01111", "10000", "10000", "10000", "10000", "10000", "01111"};
            case 'D':
               return {"11110", "10001", "10001", "10001", "10001", "10001", "11110"};
            case 'E':
               return {"11111", "10000", "10000", "11110", "10000", "10000", "11111"};
            case 'F':
               return {"11111", "10000", "10000", "11110", "10000", "10000", "10000"};
            case 'G':
               return {"01111", "10000", "10000", "10011", "10001", "10001", "01111"};
            case 'H':
               return {"10001", "10001", "10001", "11111", "10001", "10001", "10001"};
            case 'I':
               return {"11111", "00100", "00100", "00100", "00100", "00100", "11111"};
            case 'J':
               return {"00111", "00010", "00010", "00010", "10010", "10010", "01100"};
            case 'K':
               return {"10001", "10010", "10100", "11000", "10100", "10010", "10001"};
            case 'L':
               return {"10000", "10000", "10000", "10000", "10000", "10000", "11111"};
            case 'M':
               return {"10001", "11011", "10101", "10101", "10001", "10001", "10001"};
            case 'N':
               return {"10001", "11001", "10101", "10011", "10001", "10001", "10001"};
            case 'O':
               return {"01110", "10001", "10001", "10001", "10001", "10001", "01110"};
            case 'P':
               return {"11110", "10001", "10001", "11110", "10000", "10000", "10000"};
            case 'Q':
               return {"01110", "10001", "10001", "10001", "10101", "10010", "01101"};
            case 'R':
               return {"11110", "10001", "10001", "11110", "10100", "10010", "10001"};
            case 'S':
               return {"01111", "10000", "10000", "01110", "00001", "00001", "11110"};
            case 'T':
               return {"11111", "00100", "00100", "00100", "00100", "00100", "00100"};
            case 'U':
               return {"10001", "10001", "10001", "10001", "10001", "10001", "01110"};
            case 'V':
               return {"10001", "10001", "10001", "10001", "10001", "01010", "00100"};
            case 'W':
               return {"10001", "10001", "10001", "10101", "10101", "10101", "01010"};
            case 'X':
               return {"10001", "10001", "01010", "00100", "01010", "10001", "10001"};
            case 'Y':
               return {"10001", "10001", "01010", "00100", "00100", "00100", "00100"};
            case 'Z':
               return {"11111", "00001", "00010", "00100", "01000", "10000", "11111"};
            case '0':
               return {"01110", "10001", "10011", "10101", "11001", "10001", "01110"};
            case '1':
               return {"00100", "01100", "00100", "00100", "00100", "00100", "01110"};
            case '2':
               return {"01110", "10001", "00001", "00010", "00100", "01000", "11111"};
            case '3':
               return {"11110", "00001", "00001", "01110", "00001", "00001", "11110"};
            case '4':
               return {"00010", "00110", "01010", "10010", "11111", "00010", "00010"};
            case '5':
               return {"11111", "10000", "10000", "11110", "00001", "00001", "11110"};
            case '6':
               return {"01110", "10000", "10000", "11110", "10001", "10001", "01110"};
            case '7':
               return {"11111", "00001", "00010", "00100", "01000", "01000", "01000"};
            case '8':
               return {"01110", "10001", "10001", "01110", "10001", "10001", "01110"};
            case '9':
               return {"01110", "10001", "10001", "01111", "00001", "00001", "01110"};
            case '-':
               return {"00000", "00000", "00000", "11111", "00000", "00000", "00000"};
            case '+':
               return {"00000", "00100", "00100", "11111", "00100", "00100", "00000"};
            case ':':
               return {"00000", "00100", "00100", "00000", "00100", "00100", "00000"};
            case '>':
               return {"10000", "01000", "00100", "00010", "00100", "01000", "10000"};
            default:
               return {"00000", "00000", "00000", "00000", "00000", "00000", "00000"};
         }
      }

      std::array<graphics::VertexAttribute, 2> layout() noexcept {
         return {
             graphics::VertexAttribute{0, 2, graphics::VertexAttributeType::Float, false, offsetof(UiLayer::Vertex, position)},
             graphics::VertexAttribute{1, 4, graphics::VertexAttributeType::Float, false, offsetof(UiLayer::Vertex, color)},
         };
      }

      std::array<graphics::VertexAttribute, 3> imageLayout() noexcept {
         return {
             graphics::VertexAttribute{0, 2, graphics::VertexAttributeType::Float, false,
                                       offsetof(UiLayer::ImageVertex, position)},
             graphics::VertexAttribute{1, 2, graphics::VertexAttributeType::Float, false,
                                       offsetof(UiLayer::ImageVertex, textureCoordinate)},
             graphics::VertexAttribute{2, 4, graphics::VertexAttributeType::Float, false,
                                       offsetof(UiLayer::ImageVertex, color)},
         };
      }
   } // namespace

   UiLayer::UiLayer()
       : mesh_(sizeof(Vertex), layout())
       , material_("./shaders/ui.vertex.glsl", "./shaders/ui.fragment.glsl")
       , imageMesh_(sizeof(ImageVertex), imageLayout())
       , imageMaterial_("./shaders/ui_image.vertex.glsl", "./shaders/ui_image.fragment.glsl") {
      imageMaterial_.shader().set("uTexture", 0);
   }

   void UiLayer::begin(glm::ivec2 screenSize, const system::Input& input, bool inputEnabled) {
      screenSize_ = screenSize;
      input_ = &input;
      inputEnabled_ = inputEnabled;
      vertices_.clear();
      imageVertices_.clear();
      imageTexture_ = nullptr;
   }

   void UiLayer::image(const graphics::Texture& texture, glm::vec2 position, glm::vec2 size, Color tint) {
      imageTexture_ = &texture;
      const auto color = toVec4(tint);
      const auto a = position;
      const auto b = glm::vec2{position.x + size.x, position.y};
      const auto c = glm::vec2{position.x + size.x, position.y + size.y};
      const auto d = glm::vec2{position.x, position.y + size.y};

      imageVertices_.push_back({a, {0.0f, 0.0f}, color});
      imageVertices_.push_back({b, {1.0f, 0.0f}, color});
      imageVertices_.push_back({c, {1.0f, 1.0f}, color});
      imageVertices_.push_back({a, {0.0f, 0.0f}, color});
      imageVertices_.push_back({c, {1.0f, 1.0f}, color});
      imageVertices_.push_back({d, {0.0f, 1.0f}, color});
   }

   void UiLayer::panel(glm::vec2 position, glm::vec2 size, Color color) {
      rect(position, size, color);
   }

   bool UiLayer::button(std::string_view label, glm::vec2 position, glm::vec2 size) {
      const auto hovered = contains(position, size);
      const auto pressed = hovered && input_ != nullptr && input_->isMouseButtonPressed(system::MouseButton::Left);
      const auto fill = hovered ? Color{0.27f, 0.36f, 0.42f, 0.95f} : Color{0.18f, 0.25f, 0.30f, 0.92f};
      const auto edge = hovered ? Color{0.73f, 0.87f, 0.94f, 1.0f} : Color{0.42f, 0.55f, 0.62f, 1.0f};
      rect(position, size, fill);
      border(position, size, 2.0f, edge);

      const auto scale = 3.0f;
      const auto textWidth = static_cast<float>(label.size()) * (kFontWidth + 1) * scale;
      const auto textHeight = static_cast<float>(kFontHeight) * scale;
      text(label, {position.x + (size.x - textWidth) * 0.5f, position.y + (size.y - textHeight) * 0.5f}, scale,
           Color{0.88f, 0.95f, 0.98f, 1.0f});
      return pressed;
   }

   void UiLayer::text(std::string_view value, glm::vec2 position, float scale, Color color) {
      auto cursor = position;
      for (const auto character : value) {
         if (character == ' ') {
            cursor.x += static_cast<float>(kFontWidth + 1) * scale;
            continue;
         }
         glyph(character, cursor, scale, color);
         cursor.x += static_cast<float>(kFontWidth + 1) * scale;
      }
   }

   void UiLayer::end(const graphics::Renderer& renderer) {
      const auto projection =
          glm::ortho(0.0f, static_cast<float>(screenSize_.x), static_cast<float>(screenSize_.y), 0.0f, -1.0f, 1.0f);
      if (!imageVertices_.empty() && imageTexture_ != nullptr) {
         imageMesh_.upload(std::span<const ImageVertex>{imageVertices_}, graphics::BufferUsage::DynamicDraw);
         imageMaterial_.setTexture(*imageTexture_);
         imageMaterial_.shader().set("uProjection", projection);
         renderer.draw(imageMesh_, imageMaterial_);
      }

      if (vertices_.empty()) {
         return;
      }
      mesh_.upload(std::span<const Vertex>{vertices_}, graphics::BufferUsage::DynamicDraw);
      material_.shader().set("uProjection", projection);
      renderer.draw(mesh_, material_);
   }

   void UiLayer::rect(glm::vec2 position, glm::vec2 size, Color color) {
      const auto c = toVec4(color);
      const auto a = position;
      const auto b = glm::vec2{position.x + size.x, position.y};
      const auto cpos = glm::vec2{position.x + size.x, position.y + size.y};
      const auto d = glm::vec2{position.x, position.y + size.y};
      vertices_.push_back({a, c});
      vertices_.push_back({b, c});
      vertices_.push_back({cpos, c});
      vertices_.push_back({a, c});
      vertices_.push_back({cpos, c});
      vertices_.push_back({d, c});
   }

   void UiLayer::border(glm::vec2 position, glm::vec2 size, float thickness, Color color) {
      rect(position, {size.x, thickness}, color);
      rect({position.x, position.y + size.y - thickness}, {size.x, thickness}, color);
      rect(position, {thickness, size.y}, color);
      rect({position.x + size.x - thickness, position.y}, {thickness, size.y}, color);
   }

   void UiLayer::glyph(char character, glm::vec2 position, float scale, Color color) {
      const auto rows = glyphRows(character);
      for (auto y = 0; y < kFontHeight; ++y) {
         for (auto x = 0; x < kFontWidth; ++x) {
            if (rows[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] == '1') {
               rect({position.x + static_cast<float>(x) * scale, position.y + static_cast<float>(y) * scale}, {scale, scale},
                    color);
            }
         }
      }
   }

   bool UiLayer::contains(glm::vec2 position, glm::vec2 size) const noexcept {
      if (!inputEnabled_ || input_ == nullptr) {
         return false;
      }
      const auto mouse = input_->mousePosition();
      return mouse.x >= position.x && mouse.x <= position.x + size.x && mouse.y >= position.y && mouse.y <= position.y + size.y;
   }

} // namespace etherblocks::engine::ui

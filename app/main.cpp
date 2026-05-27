#include <GLFW/glfw3.h>
#include <array>
#include <etherblocks/Camera.hpp>
#include <etherblocks/Shader.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <variant>

#define STB_IMAGE_IMPLEMENTATION
#include "etherblocks/Window.hpp"
#include "stb_image.h"

etherblocks::FreeCamera fpsCamera(glm::vec3(0.0f, 0.0f, 3.0f));
etherblocks::CameraBase& camera = fpsCamera;

static auto deltaTime = 0.0f; // Time between current frame and last frame
static auto lastFrame = 0.0f; // Time of last frame

static auto screenHeight = 600;
static auto screenWidth = 800;

static auto lastX = static_cast<double>(screenWidth) / 2.0;
static auto lastY = static_cast<double>(screenHeight) / 2.0;

static auto isFirstMouseMove = true;

void loadTexture(std::string_view path);

struct InputState {
   bool moveForward{};
   bool moveBackward{};
   bool moveLeft{};
   bool moveRight{};
};

template <typename... Ts>
struct Overloaded : Ts... {
   using Ts::operator()...;
};

template <typename... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

void handleEvent(etherblocks::Window& window, InputState& input, const etherblocks::event::Any& event);
void applyMovement(const InputState& input);

int main() {
   etherblocks::Window window(etherblocks::WindowConfig{800, 600, "etherblocks"});
   window.setCursorMode(etherblocks::CursorMode::Disabled);
   window.setRawMouseMotion(true);
   window.enable(etherblocks::RenderFeature::DepthTest);

   InputState input;

   constexpr std::array vertices = {
       -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
       0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,

       -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
       0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,

       -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,

       0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,
       0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,

       -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,
       0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,

       -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,
       0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,
   };

   constexpr std::array textureCoordinates = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

                                              0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

                                              1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

                                              1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

                                              0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

                                              0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

   unsigned int VAO;
   glGenVertexArrays(1, &VAO);
   glBindVertexArray(VAO);

   unsigned int verticesBuffer;
   glGenBuffers(1, &verticesBuffer);
   glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
   glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
   glEnableVertexAttribArray(0);

   unsigned int textureCoordinatesBuffer;
   glGenBuffers(1, &textureCoordinatesBuffer);
   glBindBuffer(GL_ARRAY_BUFFER, textureCoordinatesBuffer);
   glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(textureCoordinates.size() * sizeof(float)), textureCoordinates.data(),
                GL_STATIC_DRAW);

   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
   glEnableVertexAttribArray(1);

   glBindVertexArray(0);

   etherblocks::ShaderProgram shaderProgram("./shaders/basic.vertex.glsl", "./shaders/basic.fragment.glsl");

   stbi_set_flip_vertically_on_load(true);

   unsigned int texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   loadTexture("./assets/container.png");

   shaderProgram.use();
   shaderProgram.set("texture1", 0);

   while (window.isOpen()) {
      auto time = static_cast<float>(glfwGetTime());

      float currentFrame = time;
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;

      window.pollEvents();

      while (auto event = window.nextEvent()) {
         handleEvent(window, input, *event);
      }

      applyMovement(input);

      window.clear({0.2f, 0.3f, 0.3f, 1.0f});

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);

      shaderProgram.use();

      glBindVertexArray(VAO);

      auto view = camera.createViewMatrix();
      int viewLoc = glGetUniformLocation(shaderProgram.id(), "view");
      glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

      auto projection =
          glm::perspective(glm::radians(camera.zoom()), static_cast<float>(screenWidth) / screenHeight, 0.1f, 100.0f);
      int projectionLoc = glGetUniformLocation(shaderProgram.id(), "projection");
      glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

      int modelLoc = glGetUniformLocation(shaderProgram.id(), "model");
      glm::mat4 model = glm::mat4(1.0f);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
      glDrawArrays(GL_TRIANGLES, 0, 36);

      window.display();
   }

   glDeleteVertexArrays(1, &VAO);
   glDeleteBuffers(1, &verticesBuffer);
   glDeleteBuffers(1, &textureCoordinatesBuffer);

   return 0;
}

void handleEvent(etherblocks::Window& window, InputState& input, const etherblocks::event::Any& event) {
   std::visit(Overloaded{
                  [&window](const etherblocks::event::Closed&) {
                     window.close();
                  },
                  [](const etherblocks::event::Resized& resized) {
                     screenWidth = resized.width;
                     screenHeight = resized.height;
                  },
                  [&window, &input](const etherblocks::event::KeyPressed& key) {
                     if (key.key == GLFW_KEY_ESCAPE) {
                        window.close();
                     } else if (key.key == GLFW_KEY_W) {
                        input.moveForward = true;
                     } else if (key.key == GLFW_KEY_S) {
                        input.moveBackward = true;
                     } else if (key.key == GLFW_KEY_A) {
                        input.moveLeft = true;
                     } else if (key.key == GLFW_KEY_D) {
                        input.moveRight = true;
                     }
                  },
                  [&input](const etherblocks::event::KeyReleased& key) {
                     if (key.key == GLFW_KEY_W) {
                        input.moveForward = false;
                     } else if (key.key == GLFW_KEY_S) {
                        input.moveBackward = false;
                     } else if (key.key == GLFW_KEY_A) {
                        input.moveLeft = false;
                     } else if (key.key == GLFW_KEY_D) {
                        input.moveRight = false;
                     }
                  },
                  [](const etherblocks::event::MouseMoved& mouse) {
                     if (isFirstMouseMove) {
                        lastX = mouse.x;
                        lastY = mouse.y;
                        isFirstMouseMove = false;
                     }

                     const float xOffset = static_cast<float>(mouse.x - lastX);
                     const float yOffset = static_cast<float>(lastY - mouse.y);
                     lastX = mouse.x;
                     lastY = mouse.y;

                     camera.rotateByMouseOffset(xOffset, yOffset);
                  },
                  [](const etherblocks::event::MouseScrolled& scroll) {
                     camera.zoomByScrollOffset(static_cast<float>(scroll.yOffset));
                  },
                  [](const auto&) {},
              },
              event);
}

void applyMovement(const InputState& input) {
   if (input.moveForward) {
      camera.move(etherblocks::CameraMovement::Forward, deltaTime);
   }
   if (input.moveBackward) {
      camera.move(etherblocks::CameraMovement::Backward, deltaTime);
   }
   if (input.moveLeft) {
      camera.move(etherblocks::CameraMovement::Left, deltaTime);
   }
   if (input.moveRight) {
      camera.move(etherblocks::CameraMovement::Right, deltaTime);
   }
}

void loadTexture(std::string_view path) {
   int width, height, channelCount;
   auto* data = stbi_load(path.data(), &width, &height, &channelCount, 0);
   if (data) {
      GLenum format = GL_RGB;
      if (channelCount == 1) {
         format = GL_RED;
      } else if (channelCount == 4) {
         format = GL_RGBA;
      }

      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
      stbi_image_free(data);
   } else {
      std::cout << "Failed to load texture " << path << std::endl;
   }
}

#ifndef APP_APPLICATION_HPP
#define APP_APPLICATION_HPP

#include <memory>

namespace etherblocks::app {

   class Application {
   public:
      Application();
      ~Application();

      Application(const Application&) = delete;
      Application& operator=(const Application&) = delete;

      void run();

   private:
      class Impl;
      std::unique_ptr<Impl> impl_;
   };

} // namespace etherblocks::app

#endif // APP_APPLICATION_HPP

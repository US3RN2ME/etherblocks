#include <algorithm>
#include <cstdio>
#include <etherblocks/system/Logger.hpp>
#include <iostream>
#include <utility>

namespace etherblocks::system {

   namespace {
      void fallback(std::string_view text) noexcept {
         std::fputs("[LOGGER ERROR] ", stderr);
         std::fwrite(text.data(), sizeof(char), text.size(), stderr);
         std::fputc('\n', stderr);
      }

      const char* label(LogLevel level) noexcept {
         switch (level) {
            case LogLevel::Debug:
               return "DEBUG";
            case LogLevel::Info:
               return "INFO";
            case LogLevel::Warning:
               return "WARNING";
            case LogLevel::Error:
               return "ERROR";
         }
         return "UNKNOWN";
      }
   } // namespace

   Logger::Logger() {
      static_cast<void>(addSink([](const LogMessage& message) {
         std::clog << '[' << label(message.level) << "] " << message.text << '\n';
      }));
   }

   Logger::SinkId Logger::addSink(Sink sink) {
      std::scoped_lock lock(mutex_);
      const auto id = nextSinkId_++;
      sinks_.emplace_back(id, std::move(sink));
      return id;
   }

   void Logger::removeSink(SinkId id) {
      std::scoped_lock lock(mutex_);
      std::erase_if(sinks_, [id](const auto& entry) {
         return entry.first == id;
      });
   }

   void Logger::log(LogLevel level, std::string_view text) const noexcept {
      try {
         const LogMessage message{level, std::string(text)};
         std::vector<Sink> sinks;

         {
            std::scoped_lock lock(mutex_);
            sinks.reserve(sinks_.size());
            for (const auto& entry : sinks_) {
               sinks.push_back(entry.second);
            }
         }

         for (const auto& sink : sinks) {
            try {
               sink(message);
            } catch (...) {
               fallback("Log sink threw an exception");
            }
         }
      } catch (...) {
         fallback(text);
      }
   }

   Logger& logger() {
      static Logger instance;
      return instance;
   }

   void log(LogLevel level, std::string_view text) noexcept {
      try {
         logger().log(level, text);
      } catch (...) {
         fallback(text);
      }
   }

} // namespace etherblocks::system

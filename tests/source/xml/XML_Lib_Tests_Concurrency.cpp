#include "XML_Lib_Tests.hpp"
#include <atomic>
#include <future>
#include <string>
#include <thread>
#include <vector>

TEST_CASE("Concurrency: Concurrent parsing across multiple threads", "[concurrency]")
{
  constexpr int kNumThreads = 16;
  constexpr int kIterationsPerThread = 25;
  std::atomic<int> successCount{ 0 };
  std::vector<std::future<void>> futures;

  for (int t = 0; t < kNumThreads; ++t) {
    futures.push_back(std::async(std::launch::async, [t, &successCount]() {
      for (int i = 0; i < kIterationsPerThread; ++i) {
        const std::string xmlString =
          "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
          "<thread id=\"" + std::to_string(t) + "\">\n"
          "  <item index=\"" + std::to_string(i) + "\">Value " + std::to_string(t * 1000 + i) + "</item>\n"
          "  <data active=\"true\">Payload</data>\n"
          "</thread>";

        BufferSource source(xmlString);
        XML xml;
        xml.parse(source);

        const auto &root = NRef<Root>(xml.root());
        if (root.name() == "thread" && root["id"].getParsed() == std::to_string(t)) {
          XPath xpath(xml.root());
          auto matches = xpath.evaluate("//item[@index='" + std::to_string(i) + "']");
          if (matches.size() == 1) {
            successCount.fetch_add(1, std::memory_order_relaxed);
          }
        }
      }
    }));
  }

  for (auto &f : futures) {
    f.get();
  }

  REQUIRE(successCount.load() == kNumThreads * kIterationsPerThread);
}

TEST_CASE("Concurrency: Isolated thread parser options and standalone states", "[concurrency]")
{
  constexpr int kNumThreads = 8;
  std::atomic<bool> allPassed{ true };
  std::vector<std::thread> threads;

  for (int t = 0; t < kNumThreads; ++t) {
    threads.emplace_back([t, &allPassed]() {
      const bool isOdd = (t % 2 == 1);
      const std::string xmlString = isOdd
        ? "<?xml version=\"1.0\" standalone=\"yes\"?><root><flag>odd</flag></root>"
        : "<?xml version=\"1.0\" standalone=\"no\"?><root><flag>even</flag></root>";

      for (int i = 0; i < 50; ++i) {
        BufferSource source(xmlString);
        XML xml;
        ParseOptions options;
        options.strictNamespaces = isOdd;
        options.enableNamespaces = !isOdd;
        xml.parse(source, options);

        const auto &decl = NRef<Declaration>(xml.declaration());
        const bool standalone = (decl.standalone() == "yes");
        if (standalone != isOdd) {
          allPassed.store(false);
        }
      }
    });
  }

  for (auto &th : threads) {
    th.join();
  }

  REQUIRE(allPassed.load() == true);
}

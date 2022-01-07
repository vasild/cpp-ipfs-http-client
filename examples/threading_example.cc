/**
 * Example use-case of using IPFS HTTP Client with threading.
 * By: Melroy van den Berg <melroy@melroy.org>
 *
 * Important note: Never create multiple threads! That should NOT be needed
 * in order to have parallel requests, since cURL multi API supports multiple
 * simultaneous requests out-of-the-box (it's called multi API for a reason).
 * However, the IPFS Client does not yet implement multiple requests in
 * parallel.
 *
 * This single thread example is useful in application with a GUI as its main
 * thread. For example with apps that are using Qt or GTK GUI toolkits.
 *
 * First be sure you install the IPFS client locally (as a static lib), using:
 *
 * mkdir build && cd build && cmake .. && make -j10 && sudo make install
 *
 * Then go to the example folder:
 *
 * cd examples
 *
 * Finally, build the example as follows:
 *
 * g++ -std=c++17 threading_example.cc -o threading_app \
   /usr/local/lib/libipfs-http-client.a \
   /usr/lib/x86_64-linux-gnu/libcurl.so -pthread
 *
 */
#include <ipfs/client.h>
// Or when you use IPFS locally:
//#include "ipfs/client.h"

#include <iostream>
#include <sstream>
#include <thread>

void startThread() {
  try {
    // Create IPFS Client object, with 2 minutes time-out.
    ipfs::Client client("localhost", 5001, "2m");

    // Only start a single thread
    std::thread thread([&client]() {
      std::stringstream contents;
      try {
        // File should not exists, takes forever (until time-out)
        client.FilesGet("QmZp1rrtGTictR2rpNcx4673R7qU9Jdr9DQ6Z7F6Wgo2bQ",
                        &contents);
      } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
      }
    });

    if (thread.joinable()) {
      std::cout << "Directly try to abort the request and stop the thread."
                << std::endl;
      // Try to remove the Abort() and Reset() calls,
      // and see the difference yourself :)
      client.Abort();
      thread.join(); // Should not be blocking now
      client.Reset();
    }

  } catch (const std::exception& e) {
    std::cerr << "General error: " << e.what() << std::endl;
  }
}

int main() {
  std::cout << "Starting thread.." << std::endl;

  // Start a request inside a thread
  startThread();

  std::cout << "Done!" << std::endl;
  return 0;
}
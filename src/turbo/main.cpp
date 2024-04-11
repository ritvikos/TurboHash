#include <iostream>
#include <thread>

#include "turbo_hash.h"

using namespace turbo;

void minimal() {
  using HashTable = turbo::unordered_map<std::string, size_t>;

  HashTable tree;
  std::string* keys = new std::string[10];
  ThreadInfo tinfo = tree.getThreadInfo();

  size_t vbuf;
  auto callback = [&](HashTable::RecordType record) { vbuf = record.value(); };

  std::string key = "key";
  size_t range = 11;

  // Put
  for (size_t i = 0; i < range; i++) {
    tree.Put("k", 100 + i, tinfo);
  }

  // Find
  for (size_t i = 0; i < range; i++) {
    bool res = tree.Find("k", tinfo, callback);
    std::cout << "k: " << i << " v: " << vbuf << std::endl;
  }

  // Delete
  for (size_t i = 0; i < range; i++) {
    bool res = tree.Delete("k", tinfo);
  }

  // Find after deletion
  size_t vbuf2;
  auto callback2 = [&](HashTable::RecordType record) {
    vbuf2 = record.value();
  };
  for (size_t i = 0; i < range; i++) {
    bool res = tree.Find("k", tinfo, callback2);

    // it shouldn't run
    if (res == true) {
      std::cout << "k: " << i << " v: " << vbuf2 << std::endl;
    }
  }
}

int main(int argc, char** argv) {
  minimal();
  return 0;
}

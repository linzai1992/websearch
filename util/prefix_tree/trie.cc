// Copyright 2014. All Rights Reserved.
// Author: shunpingye@gmail.com (Shunping Ye)

#include "util/prefix_tree/trie.h"

#include "base/es.h"

namespace util {
Trie::Trie() {
  root = new TrieNode();
}

Trie::~Trie() {
  delete root;
}

void Trie::insert(const std::string& str) {
  TrieNode* n = root;

  for (unsigned int i = 0; i < str.length(); i++) {
    n = n->put(str[i]);
  }

  n->increment();
}

bool Trie::has(const std::string& str) const {
  TrieNode* n = reach(str);
  return n != 0 && n != root && n->get_count() > 0;
}

void Trie::remove(const std::string& str) {
  TrieNode* n = reach(str);
  // TODO Cleanup! See if this node is still needed, or any of its parents, etc...
  if (n) {
    n->decrement();
  }
}

deque<string> Trie::search(const std::string& str) const {
  return search(str, 0);
}

deque<string> Trie::search(const std::string& str, unsigned int cap) const {
  deque<string> results;

  TrieNode* n = reach(str);

  if (n == NULL) {
    return results;
  }

  search_recursively(str, n, &results, cap);

  return results;
}

void Trie::search_recursively(const string& s, TrieNode* n, deque<string>* results,
                              unsigned int cap) const {
  if (n->get_count() > 0) {
    results->push_back(s);
  }

  if (cap && results->size() >= cap) {
    return;
  }

  deque<char> edges = n->edges();

  for (unsigned int i = 0; i < edges.size(); i++) {
    char next = edges[i];
    search_recursively(s + next, n->get(next), results, cap);
  }
}

TrieNode* Trie::reach(const string& str) const {
  TrieNode* n = root;

  for (unsigned int i = 0; i < str.length(); i++) {
    char c = str[i];

    if (!n->has(c)) {
      return 0;
    }

    n = n->get(c);
  }

  return n;
}
}

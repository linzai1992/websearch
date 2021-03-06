// Copyright 2014. All Rights Reserved.
// Author: shunpingye@gmail.com (Shunping Ye)

#include <string>
#include <vector>

#include "util/prefix_tree/prefix_tree.h"
#include "base/flags.h"
#include "base/logging.h"
#include "base/es.h"

int main(int argc, char **argv) {
  google::ParseCommandLineFlags(&argc, &argv, false);
  util::PrefixTree tree;
  tree.AddString("a");
  tree.AddString("yes");
  tree.AddString("zlib");
  tree.AddString("ab");
  tree.AddString("abd");
  tree.AddString("abcd");
  tree.AddString("abcde");
  tree.AddString("aaaa");

  CHECK(tree.Build());
  LOG(INFO) << "record number:" << tree.RecordNumber();
  string key = "abcde";
  LOG(INFO) << "CommonMatch result:";
  vector<string> results;
  CHECK(tree.CommonMatch(key, &results));
  for (size_t i = 0; i < results.size(); ++i) {
    LOG(INFO) << results[i];
  }

  vector<const char*> ptr_results;
  CHECK(tree.CommonMatch(key, &ptr_results));
  for (size_t i = 0; i < ptr_results.size(); ++i) {
    LOG(INFO) << ptr_results[i];
  }

  LOG(INFO) << "ExactMatch result:";
  if (tree.ExactMatch(key)) {
    LOG(INFO) << "matched for " << key;
  } else {
    LOG(INFO) << "not match for " << key;
  }

  string bad_key = "yesp";
  if (tree.ExactMatch(bad_key)) {
    LOG(INFO) << "matched for " << bad_key;
  } else {
    LOG(INFO) << "not match for " << bad_key;
  }
  return 0;
}

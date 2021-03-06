// Copyright 2014. All Rights Reserved.
// Author: shunpingye@gmail.com (Shunping Ye)

#include "util/prefix_tree/prefix_tree.h"

#include "base/logging.h"
#include "base/es.h"
#include "base/stl_util-inl.h"

namespace {
const int kMaxMatchNumber = 1024;
}

namespace util {

void PrefixTree::AddString(const string& str) {
  data_.push_back(str);
}


bool PrefixTree::Build() {
  sort(data_.begin(), data_.end());
  for (size_t i = 0; i < data_.size(); ++i) {
    keys_.push_back(data_[i].c_str());
  }
  double_array_.clear();
  return double_array_.build(keys_.size(), &keys_[0]) == 0;
}

bool PrefixTree::SuffixFirstCommonMatch(const string& key, string* out) const {
  vector<string> result;
  for (size_t i = 0; i < key.size(); ++i) {
    result.clear();
    if (CommonMatch(key.substr(i), &result)) {
      out->assign(result[0]);
      return true;
    }
  }
  return false;
}

bool PrefixTree::SuffixCommonMatchAll(const string& key,
                                      vector<string>* result) const{
  vector<string> res;
  for (size_t i = 0; i < key.size(); ++i) {
    res.clear();
    if (CommonMatch(key.substr(i), &res)) {
      result->insert(result->end(), res.begin(), res.end());
    }
  }
  return !result->empty();
}

bool PrefixTree::CommonMatch(const string& key,
                                vector<string>* result) const {
  Darts::DoubleArray::result_pair_type  result_pair[kMaxMatchNumber];
  size_t num = double_array_.commonPrefixSearch(key.c_str(),
                                                result_pair,
                                                sizeof(result_pair));
  if (num == 0) return false;

  for (size_t i = 0; i < num; ++i) {
    result->push_back(data_[result_pair[i].value]);
  }
  return true;
}

bool PrefixTree::CommonMatchKeyValue(const string& key,
                                     vector<string*>* result) {
  Darts::DoubleArray::result_pair_type  result_pair[kMaxMatchNumber];
  size_t num = double_array_.commonPrefixSearch(key.c_str(),
                                                result_pair,
                                                sizeof(result_pair));
  if (num == 0) {
    return false;
  }

  for (size_t i = 0; i < num; ++i) {
    result->push_back(&data_[result_pair[i].value]);
  }
  return true;
}


bool PrefixTree::CommonMatch(const string& key,
                                vector<const char*>* result) {
  Darts::DoubleArray::result_pair_type  result_pair[kMaxMatchNumber];
  size_t num = double_array_.commonPrefixSearch(key.c_str(),
                                                result_pair,
                                                sizeof(result_pair));
  if (num == 0) return false;

  for (size_t i = 0; i < num; ++i) {
    result->push_back(data_[result_pair[i].value].c_str());
  }
  return true;
}


bool PrefixTree::ExactMatch(const string& key) {
  Darts::DoubleArray::result_pair_type result_pair;
  double_array_.exactMatchSearch(key.c_str(), result_pair);
  return result_pair.value != -1;
}


bool PrefixTree::ExactMatchValue(const string& key) {
  Darts::DoubleArray::result_pair_type result_pair;
  double_array_.exactMatchSearch(key.c_str(), result_pair);
  if (result_pair.value == -1) {
    return false;
  }
  return true;
}


bool PrefixTree::ExactMatchKeyValue(const string& key,
                                    string* result) {
  Darts::DoubleArray::result_pair_type result_pair;
  double_array_.exactMatchSearch(key.c_str(), result_pair);
  if (result_pair.value == -1) {
    return false;
  }
  *result = data_[result_pair.value];
  return true;
}


uint32 PrefixTree::RecordNumber() {
  return data_.size();
}
}

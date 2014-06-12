// Copyright 2014 Easou Inc. All Rights Reserved.
// Author: shunping_ye@staff.easou.com (Ye Shunping)

#include <list>
#include <vector>

#include "base/string_util.h"
#include "thirdparty/gtest/gtest.h"
#include "util/htmlparser/xpath/xpath_extractor.h"
#include "util/htmlparser/htmlparser/html_extractor.h"

DEFINE_int32(run_times, 1, "");

namespace util {

void test() {
  XpathExtractor xpath_extarctor;
  string html = "<p id=\"nv\">"
      "<a href=\"http://news.baidu.com\">����</a>"
      "<a href=\"http://tieba.baidu.com\">����</a>"
      "<a href=\"http://zhidao.baidu.com\">֪��</a>"
      "<a href=\"http://music.baidu.com\">����</a>"
      "<a href=\"http://image.baidu.com\">ͼƬ</a>"
      "<a href=\"http://v.baidu.com\">��Ƶ</a>"
      "<a href=\"http://map.baidu.com\">��ͼ</a>"
      "<a href=\"http://baike.baidu.com\">�ٿ�</a>"
      "<a href=\"http://wenku.baidu.com\">�Ŀ�</a></p>";
  {
    string xpath = "//a";
    vector<string> result;
    xpath_extarctor.ExtractorXpath(html, xpath, &result);
    EXPECT_EQ(result.size(), 9UL);
    EXPECT_EQ(result[0], "����");
    EXPECT_EQ(result[1], "����");
    EXPECT_EQ(result[2], "֪��");
    EXPECT_EQ(result[3], "����");
    EXPECT_EQ(result[4], "ͼƬ");
    EXPECT_EQ(result[5], "��Ƶ");
    EXPECT_EQ(result[6], "��ͼ");
    EXPECT_EQ(result[7], "�ٿ�");
    EXPECT_EQ(result[8], "�Ŀ�");
  }
  {
    string xpath = "//a/@href";
    vector<string> result;
    xpath_extarctor.ExtractorXpath(html, xpath, &result);
    EXPECT_EQ(result.size(), 9UL);
    EXPECT_EQ(result[0], "http://news.baidu.com");
    EXPECT_EQ(result[1], "http://tieba.baidu.com");
    EXPECT_EQ(result[2], "http://zhidao.baidu.com");
    EXPECT_EQ(result[3], "http://music.baidu.com");
    EXPECT_EQ(result[4], "http://image.baidu.com");
    EXPECT_EQ(result[5], "http://v.baidu.com");
    EXPECT_EQ(result[6], "http://map.baidu.com");
    EXPECT_EQ(result[7], "http://baike.baidu.com");
    EXPECT_EQ(result[8], "http://wenku.baidu.com");
  }
  {
    string xpath = "//a";
    char* page = const_cast<char*>(html.c_str());
    int page_len = html.size();

    html_tree_t *html_tree = html_tree_create(html.size());
    if (0 == html_tree_parse(html_tree, page, page_len, true)) {
      LOG(ERROR)<< "html_tree_parse fail\n";
      html_tree_del(html_tree);
      return;
    }

    vector<XPATH_RESULT_NODE> nodes;
    xpath_extarctor.ExtractorXpathNode(&html_tree->root, xpath, &nodes);
    EXPECT_EQ(nodes.size(), 9UL);
    char buff[100];
    html_node_t* node = static_cast<html_node_t*>(nodes[0].node);
    html_node_extract_content(node, buff, 100);
    string name(buff);
    EXPECT_EQ(name, "����");
    LOG(INFO) << node->html_tag.tag_name;
    html_attribute_t *attr = node->html_tag.attribute;
    while (attr) {
      string key(node->html_tag.attribute->name);
      string value(node->html_tag.attribute->value, node->html_tag.attribute->valuelength);
      LOG(INFO) << "attribute key:" << key << ", value:" << value;
      attr = node->html_tag.attribute->next;
    }
    html_tree_del(html_tree);
  }
}

TEST(XpathExtractorUnittest, ExtractorXpath) {
  for (int i = 0; i < FLAGS_run_times; ++i) {
    test();
  }
}
}


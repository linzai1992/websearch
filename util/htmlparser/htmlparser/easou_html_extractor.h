/*
 * easou_html_extractor.h
 *
 *  Created on: 2012-1-9
 *      Author: xunwu
 */

#ifndef EASOU_HTML_EXTRACTOR_H_
#define EASOU_HTML_EXTRACTOR_H_

#include "easou_html_tree.h"

/**
 * @brief link�ĳ���
 **/
typedef struct _link_t
{
	char url[UL_MAX_URL_LEN];
	char text[UL_MAX_TEXT_LEN];
} link_t;

/**
 * @brief ��ȡ����tagtitle
 **/
int html_tree_extract_title(html_tree_t *html_tree, char* title, int size);

/**
 * @brief ��ȡժҪ
 **/
int html_tree_extract_abstract(html_tree_t *tree, char *abstract, int size, int merge = 1);

/**
 * @brief ��ȡ����
 * @return the number of links extracted
 **/
int html_tree_extract_link(html_tree_t *html_tree, char* baseUrl, link_t* link, int& num);

int html_tree_extract_link(html_node_list_t* list, char* baseUrl, link_t* link, int& num);

/**
 * @brief ��ȡժҪ
 **/
int html_tree_extract_keywords(html_tree_t *tree, char *keywords, int size, int merge = 1);

/**
 * @brief ��ȡcss����
 **/
int html_tree_extract_csslink(html_tree_t *html_tree, const char* baseUrl, link_t* link, int& num);

/**
 * @brief ��ȡĳһ�ڵ㼰���µ�����
 **/
int html_node_extract_content(html_node_t *html_node, char* content, int size);
int html_combine_url(char *result_url, const char *src, char *base_domain, char *base_path, char *base_port);
//��ȡҳ����base tag��ָ����URL
int get_base_url(char *base_url, html_tree_t *html_tree);
#endif /* EASOU_HTML_EXTRACTOR_H_ */

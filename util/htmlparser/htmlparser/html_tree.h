/**
 * easou_html_tree.h
 *
 *  Created on: 2011-11-8
 *      Author: xunwu
 */

#ifndef EASOU_HTML_TREE_H_
#define EASOU_HTML_TREE_H_

#include "easou_html_dom.h"

struct printDomS
{
	char *psrccode;
	int availpos;
	int buflen;
};

/**
 * @brief ��ʼ��dom��
 **/
html_tree_t *html_tree_create(int max_page_len);

/**
 * @brief ���dom��
 void html_tree_clean(html_tree_t *html_tree);
 **/

/**
 * @brief ����dom��
 **/
void html_tree_del(html_tree_t *html_tree);

/**
 * @brief �����Ƿ����js
 **/
void html_tree_set_script_parsing(html_tree_t *tree, int enable);

/**
 * @brief �ж��Ƿ����js
 **/
int html_tree_is_script_parsing(html_tree_t *tree);

/**
 * @brief ��html��ҳ������dom��
 * @param [in/out] html_tree   : html_tree_t*	dom���ṹ
 * @param [in] page   : char*	ҳ��Դ����
 * @param [in] page_len   : int	ҳ��Դ����ĳ���
 * @return  int
 * @retval  0:����ʧ��;1:�����ɹ�.
 * @author xunwu
 * @date 2011/08/02
 **/
int html_tree_parse(html_tree_t *html_tree, char *page, int page_len, bool ignore_space = true);

/**
 * @brief Pre visitor
 **/
typedef int (*start_visit_t)(html_tag_t* html_tag, void* result, int flag);

/**
 * @brief Post visitor
 **/
typedef int (*finish_visit_t)(html_tag_t* html_tag, void* result);

/**
 * @brief ����dom��
 * @param html_tree :�����õ�dom��
 * @param start_visit,����ָ�룬������ȱ���dom��������ʱʹ��
 * @param finish_visit,����ָ�룬������ȱ���dom�������ʽڵ㼰���������ʱʹ��
 **/
int html_tree_visit(html_tree_t *html_tree, start_visit_t start_visit, finish_visit_t finish_visit, void *result, int flag);

/**
 * @brief �����ڵ�
 *  @param node :�����õ�dom���Ľڵ�
 * @param start_visit,����ָ�룬������ȱ���dom��������ʱʹ��
 * @param finish_visit,����ָ�룬������ȱ���dom�������ʽڵ㼰���������ʱʹ��
 **/
int html_node_visit(html_node_t *node, start_visit_t start_visit, finish_visit_t finish_visit, void *result, int flag);

/**
 * @brief ������,���ͷſռ�
 **/
int html_tree_reset_no_destroy(struct html_tree_impl_t *self);

/**
 * ��ӡ�ڵ㼰������
 */
void printNode(html_node_t *html_node);

/**
 * ������ȷ���ĵ�����
 * @param:
 * html_tree, [in], �����õ�html��
 * url, [in], URL��ַ
 * @return:
 * 0, �ɹ���-1��ʧ��
 */
int determine_doctype(html_tree_t *html_tree, const char* url);

/**
 * ��ӡ�ڵ㼰������
 */
void printTree(html_tree_t *html_tree);

void PrintNode(html_node_t *html_node, int level);

/**
 * ��עdom����ÿ���ڵ㺬�е��ӽڵ�����
 * html_tree��in
 * �����-1����������0���ɹ���>0:��������
 *
 */
int markDomTreeSubType(html_tree_t *html_tree);

int PrintNodeSrc(html_node_t *html_node, printDomS *pPrintSrc, int level);

int printTreeSrc(html_tree_t *html_tree, char *srcodebuf, int buflen);

#endif /* EASOU_HTML_TREE_H_ */

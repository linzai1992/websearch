/*
 * easou_extractor_com.h
 *
 *  Created on: 2012-1-11
 *      Author: xunwu
 */

#ifndef EASOU_EXTRACTOR_COM_H_
#define EASOU_EXTRACTOR_COM_H_

#include "easou_mark_srctype.h"
#include "easou_mark_func.h"
#include "easou_mark_sem.h"
#include "easou_mark_parser.h"

/**
 * @brief anchor�ĳ���
 **/
typedef struct _anchor_info
{
	char url[UL_MAX_URL_LEN];
	char text[UL_MAX_TEXT_LEN];
	unsigned long linktype;
} anchor_info;

#define IS_LINK_COPYRIGHT(flag) ((flag) & 0x01)
#define SET_LINK_COPYRIGHT(flag) ((flag)|= 0x01)
#define IS_LINK_NAV(flag) ((flag) & 0x02)
#define SET_LINK_NAV(flag) ((flag)|= 0x02)
#define IS_LINK_FRIEND(flag) ((flag) & 0x04)
#define SET_LINK_FRIEND(flag) ((flag)|= 0x04)
#define IS_LINK_RELATE_LINK(flag) ((flag) & 0x08)
#define SET_LINK_RELATE_LINK(flag) ((flag)|= 0x08)
#define IS_LINK_MYPOS(flag) ((flag) & 0x10)
#define SET_LINK_MYPOS(flag) ((flag)|= 0x10)
#define IS_LINK_ARTICLE_SIGN(flag) ((flag) & 0x20)
#define SET_LINK_ARTICLE_SIGN(flag) ((flag)|= 0x20)

/**
 * @brief ��ȡĳ��Դ���ͷֿ������.
 **/
char *get_area_content(char *buf, int size, area_tree_t *atree, html_area_srctype_t func);

/**
 * @brief ��ȡĳ�������͵ķֿ������.
 **/
char *get_area_content(char *buf, int size, area_tree_t *atree, html_area_func_t func);

/**
 * @brief ��ȡĳ�������͵ķֿ������.
 **/
char *get_area_content(char *buf, int size, area_tree_t *atree, html_area_sem_t func);

/**
 * @brief ��Ӷϵ�.
 **/
int addBreakInfo(char *buffer, int available, int end, const char *break_info);

/**
 * @brief ����������
 */
int destroyTree(html_tree_t *&html_tree, vtree_in_t *&vtree_in, html_vtree_t *&vtree, area_tree_t * &atree);

/**
 * @brief ����������
 */
int createTree(html_tree_t *&html_tree, vtree_in_t *&vtree_in, html_vtree_t *&vtree, area_tree_t * &atree);

/**
 * @brief ��λ������
 */
int resetTree(html_tree_t *&html_tree, vtree_in_t *&vtree_in, html_vtree_t *&vtree, area_tree_t * &atree);

/**
 * @brief ��ȡĳ�������͵����зֿ������.
 **/
char *get_all_area_content(char *buf, int size, area_tree_t *atree, html_area_sem_t sem);

/**
 * @brief ��ȡĳ�������͵����зֿ������.
 **/
char *get_all_area_content(char *buf, int size, area_tree_t *atree, html_area_func_t func);

/**
 * @brief ��ȡĳ��Դ�������зֿ������.
 **/
char *get_all_area_content(char *buf, int size, area_tree_t *atree, html_area_srctype_t srctype);

int getAnchorInfos(area_tree_t *atree, anchor_info * anchors, int anchorsize, const char *baseUrl);

#endif /* EASOU_EXTRACTOR_COM_H_ */


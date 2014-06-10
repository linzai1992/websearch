/***************************************************************************
 * 
 * Copyright (c) 2012 Easou.com, Inc. All Rights Reserved
 * $Id: easou_link_common.h,v 1.2 2012/09/01 pageparse Exp $
 * 
 **************************************************************************/

/**
 * @file easou_link_common.h
 * @author (pageparse@staff.easou.com)
 * @date 2012/09/01
 * @version $Revision: 1.0 $
 * @brief 	���ӱ�עʱʹ�õĹ�������
 **/

#ifndef  __EASOU_LINK_COMMON_H_
#define  __EASOU_LINK_COMMON_H_

#include "easou_link.h"
#include "easou_mark_markinfo.h"

/**
 * @brief short description ������Ӷ�Ӧ�Ľṹ
 */
typedef struct _html_area_link_t
{
	html_area_t *html_area; //��ǰ��
	vlink_t *vlink; //���е����Ӽ���
	int link_count; //���е����Ӹ���

	vlink_t * next_vlink; //������Ϣ�����ڲ�����һ���������, ָ����һ����Ŀ�ʼλ��
	int next_count; //������Ϣ
} html_area_link_t;

// Input : url, Output : trunk
// Return : 1, success; 0, otherwise
int get_trunk_from_url(const char *url, char *trunk);

/**
 * @brief ���һ�������������ӵ���Ϣ
 * @param [in] area   : html_area_t*	Ҫ��ȡ���ӵĿ�
 * @param [in] vlink   : vlink_t*	��ҳ���������Ӽ���
 * @param [in] link_count   : int	��ҳ���������Ӹ���
 * @param [in/out] parea_link   : html_area_link_t*	�������������Ϣ
 * @return  void 
 **/
void get_area_link(html_area_t *area, vlink_t *vlink, int link_count, html_area_link_t *parea_link);

// Function : check url to see if in same trunk
// Return : 1, OK; 0, Not same trunk
int check_url_trunk(char *url, char *base_trunk);

int get_valid_text_len(html_vnode_t *pnode, int strict = 0);

/**
 * @brief �ж�url�Ƿ���Э��ͷ
 * @param [in] url   : char*	������url
 * @return  int 
 * @retval   	0	����Э��ͷ
 * @retval	1	��Э��ͷ
 **/
int is_url_has_protocol_head(const char *url);

/**
 * @brief ���urlЭ��ͷ�ĳ��ȣ� ��//�ģ� ���� http://
 * @param [in/out] url   : const char*	��ҳurl
 * @return  int 
 * @retval   	>0	����
 * @retval	0	����Э��ͷ������Ĭ��Ϊ0
 **/
int url_protocol_head_len(const char *url);

/**
 * @brief ��ý���tagcode
 */
int get_vnode_tag_code(html_vnode_t *vnode);

/**
 * @brief �����������tag code
 */
int get_sub_tree_max_tag_code(html_vnode_t *start_vnode);

/**
 * @brief �Ƿ���ҳ
 */
bool is_homepage(const char *str);

/**
 * @brief �����Ƿ���area��
 */
int in_area(vlink_info_t *vlink, const area_list_t * plist);

/**
 * @brief �����Ƿ���area��
 */
int in_one_area(vlink_info_t *vlink, const html_area_t *parea);

/**
 * @brief ������ֵ����ĸ���
 */
int get_chword_len(const char *text);

/**
 * @brief ��ӡ�ֿ�
 */
void print_area(FILE *fp, html_area_t *area);

/**
 * @brief ���������Ӧ��tag code�����ֳ��ȵ�ӳ���
 */
void get_tag_code_text_len(html_vnode_t *root, int *tag_code_len, int size);

/**
 * @brief ���������С��tag_code
 */
int get_sub_tree_min_tag_code(html_vnode_t *start_vnode);

#endif

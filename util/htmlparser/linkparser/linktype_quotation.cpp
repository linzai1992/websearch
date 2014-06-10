/***************************************************************************
 * 
 * Copyright (c) 2012 Easou.com, Inc. All Rights Reserved
 * $Id: easou_linktype_quotation.cpp,v 1.0 2012/09/01 pageparse Exp $
 * 
 **************************************************************************/

/**
 * @file easou_linktype_quotation.cpp
 * @author (pageparse@staff.easou.com)
 * @date 2012/09/01
 * @version $Revision: 1.0 $
 * @brief �����������
 *  
 **/
#include "PageType.h"
#include "easou_link.h"
#include "easou_link_mark.h"
#include "easou_link_common.h"
#include "easou_mark_parser.h"
#include "easou_mark_srctype.h"
#include <ctype.h>

#define QI_NEXT_IS_NOT_GBK 0x1		//���ĵ���һ���ַ�����Ϊ����
#define QI_CAN_INHERIT	0x2		//���ĵĿ��Դ���
#define QI_NONE		0x0		//�����⹦��
#define NORMAL_QUOTATION_LINK	2	//������������
#define INHERIT_QUOTATION_LINK	1	//�ɴ��ݵ���������
#define NOT_QUOTATION_LINK	0	//������������
/**
 * @brief short description ������Ϣ�ṹ
 */
typedef struct QUOTATION_INFO_T
{
	const char *text; //�����ַ���
	const int len_limit; //����������֮������ּ������
	const int tag_count_limit; //����������֮��Ľ��������
	const int attribute; //�����ĵ�����
} quotation_info_t;

static const quotation_info_t QUOTATION_WORD[] =
{ //������Ϣ
		{ "ת����", 25, 3, QI_NONE },
		{ "ת��", 25, 3, QI_NONE },
		{ "ת����", 25, 3, QI_NONE },
		{ "ת����", 25, 3, QI_NONE },
		{ "�ο�����", 20, 4, QI_CAN_INHERIT | QI_NEXT_IS_NOT_GBK },
		{ "�ο�����", 20, 4, QI_CAN_INHERIT | QI_NEXT_IS_NOT_GBK },
		{ "�ο���վ", 20, 4, QI_CAN_INHERIT | QI_NEXT_IS_NOT_GBK },
		{ "�ο�", 10, 2, QI_CAN_INHERIT | QI_NEXT_IS_NOT_GBK },
		{ "��Դ", 6, 2, QI_NEXT_IS_NOT_GBK },
		{ "����", 6, 2, QI_NEXT_IS_NOT_GBK },
		{ "ת��", 20, 2, QI_NEXT_IS_NOT_GBK },
		{ "ת��", 20, 2, QI_NEXT_IS_NOT_GBK },
		{ "ԭ�ĵ�ַ", 25, 3, QI_NEXT_IS_NOT_GBK },
		{ "����", 20, 2, QI_NEXT_IS_NOT_GBK },
		{ "����", 20, 2, QI_NEXT_IS_NOT_GBK },
		{ "ԭ��", 6, 3, QI_NEXT_IS_NOT_GBK },
		{ "ԭ������", 25, 3, QI_NEXT_IS_NOT_GBK }, };

/**
 * @brief short description ������Ϣ�ṹ
 */
typedef struct VISIT_INFO_T
{
	int begin_code; //������Χ
	int end_code; //�����ҷ�Χ
	int range; //�����������ֳ���
	int tag_count; //��ǰ�����Ľ�����
	int route; //��ǰ��·�����
	int ypos; //���ӵ�������
	int xpos; //���ӵĺ�����
} visit_info_t;

/**
 * @brief ��õ�ǰ����ǰһ�����, ����Ǹ��ڵ㣬��Ҫ��ʱ����·�����
 * @param [in/out] pnode   : html_vnode_t*	��ǰ���
 * @return  html_vnode_t* 
 * @retval   
 **/
static html_vnode_t *get_front_node(html_vnode_t *pnode, int *route)
{
	html_vnode_t *retnode = NULL;
	retnode = pnode->prevNode;
	if (retnode == NULL)
	{
		retnode = pnode->upperNode;
		if (retnode != NULL)
			*route -= pnode->upperNode->hpNode->html_tag.tag_type;
	}
	return retnode;
}

typedef int (*CHECK_FUNC)(html_vnode_t *, int, visit_info_t *);

static int has_quotation_word(html_vnode_t *pnode, int, visit_info_t *);
static int has_time(html_vnode_t *pnode, int, visit_info_t *);

/**
 * @brief ��ȡ��ǰ�������һ������tag_code��ָ����Χ�ڵ��ӽ��
 * @param [in] pnode   : html_vnode_t*	��ǰҪ��ȡ�Ľ��
 * @param [in] end_code   : int		�ҷ�Χ
 * @param [in] begin_code   : int		��Χ
 * @return  html_vnode_t* 
 * @retval   	NULL	δ�ҵ��������ӽڵ�
 * @retval	NOT NULL	����Ҫ����ӽڵ�
 **/
static html_vnode_t * get_last_child(html_vnode_t *pnode, int end_code, int begin_code)
{
	html_vnode_t *retnode = NULL;
	html_vnode_t *prevnode = NULL;

	retnode = pnode->firstChild;
	while (retnode && retnode->hpNode->html_tag.tag_code < end_code && retnode->hpNode->html_tag.tag_code >= begin_code)
	{
		prevnode = retnode;
		retnode = retnode->nextNode;
	}
	return prevnode;
}

/**
 * @brief �鿴��ǰ����Ƿ���Ҫ�����ӽڵ�
 * @param [in] pnode   : html_vnode_t*	��ǰ���
 * @return  int 
 * @retval   	0	��Ҫ�����ӽڵ�
 * @retval	1	����Ҫ�����ӽڵ�
 **/
static int skip_child_node(html_vnode_t *pnode)
{
	html_tag_type_t tag_type = pnode->hpNode->html_tag.tag_type;
	int ret = 0;
	switch (tag_type)
	{
	case TAG_SCRIPT:
	case TAG_IFRAME:
	case TAG_HEAD:
		ret = 1;
		break;
	default:
		break;
	}
	return ret;
}

/**
 * @brief ��һ���Ľ�㷶Χ�ڣ�������㣬��ȡ�Ƿ����������ӵ���Ϣ
 * @param [in] pnode   : html_vnode_t*	��ǰ���
 * @param [in/out] result   : int*	�Ƿ�Ϊ����
 * @param [in] check_func   : CHECK_FUNC	����Ƿ�Ϊ���ĵĺ���
 * @param [in/out] pvinfo   : visit_info_t*	ȫ�ֱ�����Ϣ
 * @return  int 
 * @retval   -1		��������
 * @retval	>=0	�����ý������ֳ���
 **/
static int traverse_node_in_range(html_vnode_t *pnode, int *result, CHECK_FUNC check_func, visit_info_t *pvinfo)
{
	int cur_traverse_len = 0;
	int cur_node_len = 0;
	html_vnode_t *pchildNode = NULL;

	if (skip_child_node(pnode))
	{
		return cur_traverse_len;
	}

	if (pnode->hpNode->html_tag.tag_type == TAG_A)
		return -1;

	pchildNode = get_last_child(pnode, pvinfo->end_code, pvinfo->begin_code);
	pvinfo->route += pnode->hpNode->html_tag.tag_type;

	while (pchildNode)
	{ //�����ӽڵ�
		cur_node_len = traverse_node_in_range(pchildNode, result, check_func, pvinfo);
		if (cur_node_len < 0)
			return cur_node_len;
		cur_traverse_len += cur_node_len;
		if (cur_node_len >= 4)
			pvinfo->tag_count++;

		if (*result == NOT_QUOTATION_LINK)
			*result = (*check_func)(pchildNode, cur_traverse_len, pvinfo); //���ý���Ƿ�����������

		if (cur_traverse_len > pvinfo->range)
		{
			pvinfo->route -= pnode->hpNode->html_tag.tag_type;
			return cur_traverse_len;
		}

		if (*result != NOT_QUOTATION_LINK)
		{
			break;
		}

		pchildNode = pchildNode->prevNode;
	}
	pvinfo->route -= pnode->hpNode->html_tag.tag_type;

	//��鸸�ڵ��Ƿ�������������
	if (*result != NOT_QUOTATION_LINK)
		return cur_traverse_len;
	else
	{
		cur_node_len = get_valid_text_len(pnode);
		cur_traverse_len += cur_node_len;
		if (cur_node_len >= 4)
			pvinfo->tag_count++;

		if (*result == NOT_QUOTATION_LINK)
			*result = (*check_func)(pnode, cur_traverse_len, pvinfo);

		if (cur_traverse_len > pvinfo->range)
		{
			return cur_traverse_len;
		}

		if (*result != NOT_QUOTATION_LINK)
			return cur_traverse_len;
	}
	return cur_traverse_len;
}

/**
 * @brief �ж��Ƿ�Ϊ������������������
 * @param [in] pnode   : html_vnode_t*	��ǰ����ָ��Ľ��
 * @param [in] len   : int	��ǰ��������
 * @param [in] pvinfo   : visit_info_t*	ȫ��������Ϣ
 * @return  int 
 * @retval   	NORMAL_QUOTATION_LINK	��ͨ������
 * @retval	INHERT_QUOTATION_LINK	�ɼ̳е�����
 * @retval	NOT_QUOTATION_LINK	��������
 **/
static int has_quotation_word(html_vnode_t *pnode, int cur_traverse_len, visit_info_t *pvinfo)
{
	char *text = pnode->hpNode->html_tag.text;
	char *p = NULL;
	int tmplen = 0;

	if (!text || pnode->hpNode->html_tag.tag_type != TAG_PURETEXT)
		return NOT_QUOTATION_LINK;

	for (unsigned int i = 0; i < sizeof(QUOTATION_WORD) / sizeof(quotation_info_t); i++)
	{
		if ((p = strstr(text, QUOTATION_WORD[i].text)) != NULL)
		{
			int ch;
			int offset = 0;
			ch = *p;
			*p = '\0';
			offset = get_valid_text_len(pnode);
			*p = ch;

			tmplen = cur_traverse_len - offset;
			if (tmplen <= QUOTATION_WORD[i].len_limit && pvinfo->tag_count <= QUOTATION_WORD[i].tag_count_limit)
			{
				p = p + strlen(QUOTATION_WORD[i].text);
				if (QUOTATION_WORD[i].attribute & QI_NEXT_IS_NOT_GBK)
				{
					if (IS_GB_CODE(p))
						return NOT_QUOTATION_LINK;
				}

				if (QUOTATION_WORD[i].attribute & QI_CAN_INHERIT)
					return INHERIT_QUOTATION_LINK;
				else
					return NORMAL_QUOTATION_LINK;
			}
		}
	}
	return NOT_QUOTATION_LINK;
}

/**
 * @brief ������Ƿ���ʱ��
 * @param [in] pnode   : html_vnode_t*	���
 * @param [in] len   : int	��ǰ��������
 * @param [in] pvinfo   : visit_info_t*	ȫ��������Ϣ
 * @return  int 
 * @retval   	NORMAL_QUOTATION_LINK	����ʱ�������
 * @retval	NOT_QUOTATION_LINK	���Ǻ���ʱ�������
 **/
static int has_time(html_vnode_t *pnode, int cur_traverse_len, visit_info_t *pvinfo)
{
	char *text = pnode->hpNode->html_tag.text;
	char *year = NULL;
	char *month = NULL;
	char *day = NULL;
	if (!text || pnode->hpNode->html_tag.tag_type != TAG_PURETEXT)
		return NOT_QUOTATION_LINK;

	year = strstr(text, "��");
	month = strstr(text, "��");
	day = strstr(text, "��");

	if (year == NULL || month == NULL || day == NULL)
		return NOT_QUOTATION_LINK;
	if (month < year || month - year >= 7)
		return NOT_QUOTATION_LINK;
	if (day < month || day - month >= 5)
		return NOT_QUOTATION_LINK;
	cur_traverse_len -= (year - text);
	if (cur_traverse_len < 30 && pvinfo->tag_count < 4 && abs(pnode->ypos - pvinfo->ypos) < 5)
		return NORMAL_QUOTATION_LINK;

	return NOT_QUOTATION_LINK;
}

/**
 * @brief �Ƿ�Ϊ��Ч����������
 * @param [in/out] html_area   : html_area_t*	�����
 * @return  int 	
 * @retval   	0	��Ч
 * @retval	1	��Ч
 **/
static int is_valid_quotation_area(html_area_t *html_area)
{
	int type_mark_ok = 0;
	int mark_ok = 0;

	if (is_srctype_area(html_area, AREA_SRCTYPE_TEXT)
	/* TODO
	 #ifdef _VAREA_MARK_EXIST_
	 || IS_VOID_AREA_MARK(html_area->srctype_mark)
	 #else
	 || VOID_AREA_MARK == html_area->srctype_mark
	 #endif
	 */
	|| is_srctype_area(html_area, AREA_SRCTYPE_LINK))
	{
		type_mark_ok = 1;
	}

	switch (html_area->abspos_mark)
	{
	case PAGE_HEADER:
	case PAGE_MAIN:
		mark_ok = 1;
		break;
	default:
		break;
	}
	return type_mark_ok && mark_ok;
}

/**
 * @brief ��Ǹ�html_area���е���������
 * @param [in] html_area   : html_area_t*	html_area��
 * @param [in/out] vlink   : vlink_t*		���е�����
 * @param [in] link_count   : int		���ӵĸ���
 * @return  int 
 * @retval   	0	������������
 * @retval	-1	����ʶ�����
 **/
static int area_mark_quotation_type(html_area_t *html_area, vlink_t *vlink, int link_count)
{
	if (link_count == 0)
		return 0;

	if (!is_valid_quotation_area(html_area))
		return 0;
	visit_info_t vinfo;
	int prev_link_can_inherit = 0;
	int is_quotation_link = 0;

	for (int i = 0; i < link_count; i++)
	{
		if (vlink[i].inner.vnode == NULL)
			continue;
		if (vlink[i].inner.is_goodlink)
		{
			//prev link is a quotation link
			if (prev_link_can_inherit)
			{
				if (abs(vlink[i].inner.ypos - vlink[i - 1].inner.ypos) < 40)
				{
					vlink[i].linkFunc |= VLINK_QUOTATION;
					continue;
				}
			}
			is_quotation_link = 0;

			//���ؼ��ֵ�����
			vinfo.range = 50;
			vinfo.route = 0;
			vinfo.end_code = vlink[i].inner.vnode->hpNode->html_tag.tag_code;
			vinfo.begin_code = html_area->begin->hpNode->html_tag.tag_code;
			vinfo.tag_count = 0;

			html_vnode_t *pnode = NULL;
			pnode = get_front_node(vlink[i].inner.vnode, &vinfo.route);
			int ret = 0;
			while (vinfo.range > 0 && pnode != NULL)
			{
				ret = traverse_node_in_range(pnode, &is_quotation_link, has_quotation_word, &vinfo);
				if (ret < 0)
					break;
				vinfo.range -= ret;
				if (vinfo.range < 0)
					break;

				if (is_quotation_link != NOT_QUOTATION_LINK)
				{
					break;
				}
				pnode = get_front_node(pnode, &vinfo.route);
			}
			if (is_quotation_link != NOT_QUOTATION_LINK)
			{
				if (is_quotation_link == INHERIT_QUOTATION_LINK)
					prev_link_can_inherit = 1;
				vlink[i].linkFunc |= VLINK_QUOTATION;
				continue;
			}

			//��ʱ�������
			vinfo.route = 0;
			vinfo.tag_count = 0;
			vinfo.range = 50;
			vinfo.ypos = vlink[i].inner.ypos;
			vinfo.xpos = vlink[i].inner.xpos;
			pnode = get_front_node(vlink[i].inner.vnode, &vinfo.route);
			while (vinfo.range > 0 && pnode != NULL)
			{
				ret = traverse_node_in_range(pnode, &is_quotation_link, has_time, &vinfo);
				if (ret < 0)
					break;
				vinfo.range -= ret;
				if (vinfo.range < 0)
				{
					break;
				}
				if (is_quotation_link != NOT_QUOTATION_LINK)
				{
					break;
				}
				pnode = get_front_node(pnode, &vinfo.route);
			}
			if (is_quotation_link != NOT_QUOTATION_LINK)
			{
				vlink[i].linkFunc |= VLINK_QUOTATION;
				continue;
			}
		}
		prev_link_can_inherit = 0;
	}
	return 0;
}

/**
 * @brief �������
 * @param [in/out] pargs   : vhp_args_t* �������
 * @param [in/out] pres   : vhp_res_t*	������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @retval	<0	��������
 **/
int mark_linktype_quotation(lt_args_t *pargs, lt_res_t *pres)
{
	if (PT_IS_HUB(pargs->pagetype))
		return 0;

	html_area_link_t area_link;
	memset(&area_link, 0, sizeof(html_area_link_t));

	html_area_t *subArea = pargs->atree->root->subArea;
	if (!pargs->atree->root->subArea)
		subArea = pargs->atree->root;
	for (; subArea; subArea = subArea->nextArea)
	{
		get_area_link(subArea, pargs->vlink, pargs->link_count, &area_link);

		area_mark_quotation_type(area_link.html_area, area_link.vlink, area_link.link_count);
	}

	int link_count = pargs->link_count;
	vlink_t *vlink = pargs->vlink;
	for (int i = 0; i < link_count; i++)
	{
		if (vlink[i].linkFunc & VLINK_COPYRIGHT)
		{
			vlink[i].linkFunc = vlink[i].linkFunc & (~VLINK_QUOTATION);
		}
	}
	return 1;
}

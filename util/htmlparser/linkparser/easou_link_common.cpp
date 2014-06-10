/***************************************************************************
 * 
 * Copyright (c) 2012 Easou.com, Inc. All Rights Reserved
 * $Id: easou_link_common.cpp,v 1.0 2012/09/01 pageparse Exp $
 * 
 **************************************************************************/

/**
 * @file easou_link_common.cpp
 * @author (pageparse@staff.easou.com)
 * @date 2012/09/01
 * @version $Revision: 1.0 $
 * @brief ��������
 **/

#include "easou_url.h"
#include "easou_link_common.h"

#include <string.h>
#include <assert.h>
#include <ctype.h>

typedef struct _protocol_t
{
	const char *name;
	int len;
} protocol_t;

static const protocol_t protocol_head[] =
{
{ "http://", 7 }, {"https://", 8 } };

// Input : url, Output : trunk
// Return : 1, success; 0, otherwise
int get_trunk_from_url(const char *url, char *trunk)
{
	assert(url);
	char site_name[UL_MAX_SITE_LEN] = "";
	*trunk = 0;

	if (!easou_get_site(url, site_name))
		return 0; // fail to get site name

	if (easou_fetch_maindomain(site_name, trunk, UL_MAX_SITE_LEN) < 0)
		return 0;

	return 1;
}

int get_vnode_tag_code(html_vnode_t *vnode)
{
	return vnode->hpNode->html_tag.tag_code;
}

/**
 * @brief �����������tag code
 */
int get_sub_tree_max_tag_code(html_vnode_t *vnode)
{
	html_node_t *iter_node = vnode->hpNode;
	while (iter_node->last_child != NULL)
	{
		iter_node = iter_node->last_child;
	}
	return iter_node->html_tag.tag_code;
}

/**
 * @brief ���������С��tag code
 */
int get_sub_tree_min_tag_code(html_vnode_t *vnode)
{
	return get_vnode_tag_code(vnode);
}

/**
 * @brief ��������Ƿ��ڷֿ���
 */
int in_area(vlink_info_t *vlink, const area_list_t * plist)
{
	if (vlink->vnode == NULL)
		return 0;
	if (plist == NULL)
		return 0;
	for (area_list_node_t *p = plist->head; p; p = p->next)
	{
		int min = p->area->begin->hpNode->html_tag.tag_code;
		int max = get_sub_tree_max_tag_code(p->area->end);
		if (vlink->node->html_tag.tag_code >= min && vlink->node->html_tag.tag_code <= max)
			return 1;
	}
	return 0;
}

/**
 * @brief ��������Ƿ��ڷֿ���
 */
int in_one_area(vlink_info_t *vlink, const html_area_t * area)
{
	int min = area->begin->hpNode->html_tag.tag_code;
	int max = get_sub_tree_max_tag_code(area->end);
	if (vlink->node->html_tag.tag_code >= min && vlink->node->html_tag.tag_code <= max)
		return 1;
	return 0;
}

// Function : check url to see if in same trunk
// Return : 1, OK; 0, Not same trunk
int check_url_trunk(char *url, char *base_trunk)
{
	char chk_trunk[UL_MAX_SITE_LEN] = "";

	if (!get_trunk_from_url(url, chk_trunk))
		return 0;

	if (strcasecmp(base_trunk, chk_trunk) != 0)
		return 0;
	return 1;
}

/**
 * @brief ���һ�������������ӵ���Ϣ
 * @param [in] area   : html_area_t*	Ҫ��ȡ���ӵĿ�
 * @param [in] vlink   : vlink_t*	��ҳ���������Ӽ���
 * @param [in] link_count   : int	��ҳ���������Ӹ���
 * @param [in/out] parea_link   : html_area_link_t*	�������������Ϣ
 * @return  void 
 **/
void get_area_link(html_area_t *area, vlink_t *vlink, int link_count, html_area_link_t *parea_link)
{
	//Ϊ��Ч�ʣ��ڱ�����ʱ����ǰһ�ο����Ϣ
	vlink_t *cand_link_begin = vlink;
	parea_link->vlink = NULL;
	parea_link->html_area = area;
	parea_link->link_count = 0;

	if (parea_link->next_vlink != NULL)
		cand_link_begin = parea_link->next_vlink;

	if (parea_link->next_count >= link_count || cand_link_begin->inner.html_area != area)
		return;

	parea_link->vlink = cand_link_begin;
	parea_link->link_count = cand_link_begin->inner.area_left_link_count + 1;
	parea_link->next_vlink = cand_link_begin + parea_link->link_count;
	parea_link->next_count += parea_link->link_count;
	return;
}

/**
 * @brief �ж�url�Ƿ���Э��ͷ
 * @param [in] url   : char*	������url
 * @return  int 
 * @retval   	0	����Э��ͷ
 * @retval	1	��Э��ͷ
 **/
int is_url_has_protocol_head(const char *url)
{
	for (unsigned int i = 0; i < sizeof(protocol_head) / sizeof(protocol_t); i++)
	{
		if (strncasecmp(url, protocol_head[i].name, protocol_head[i].len) == 0)
			return 1;
	}
	return 0;
}

/**
 * @brief ���urlЭ��ͷ�ĳ��ȣ� ��//�ģ� ���� http://
 * @param [in/out] url   : const char*	��ҳurl
 * @return  int 
 * @retval   	>0	����
 * @retval	0	����Э��ͷ������Ĭ��Ϊ0
 **/
int url_protocol_head_len(const char *url)
{
	for (unsigned int i = 0; i < sizeof(protocol_head) / sizeof(protocol_t); i++)
	{
		if (strncasecmp(url, protocol_head[i].name, protocol_head[i].len) == 0)
			return protocol_head[i].len;
	}
	return 0;
}

/**
 * @brief ���Э��ͷ����
 */
int get_protocal_head_len(const char *url)
{
	if (strncmp(url, "http://", 7) == 0)
		return 7;
	//add
	else if(strncmp(url, "https://", 8) == 0)
		return 8;
	return 0;
}

/**
 * @brief �Ƿ���ҳurl
 */
bool is_homepage(const char *purl)
{
	const char *begin;
	const char *slashpos;

	begin = purl + get_protocal_head_len(purl);

	if ((slashpos = strchr(begin, '/')) == NULL)
	{
		return true;
	}
	else
	{
		if ((*(slashpos + 1)) == 0)
		{
			return true;
		}
		else if (strncmp(slashpos + 1, "index", 5) == 0 || strncmp(slashpos + 1, "main", 4) == 0
				|| strncmp(slashpos + 1, "default", 7) == 0)
		{
			if (strchr(slashpos + 1, '?') == NULL && strchr(slashpos + 1, '/') == NULL)
				return true;
		}
	}
	return false;
}

/**
 * @brief �����������Ч�ַ��ĳ���
 */
int get_valid_text_len(html_vnode_t *pnode, int strict)
{
	char *text = pnode->hpNode->html_tag.text;
	if (text == NULL || pnode->hpNode->html_tag.tag_type != TAG_PURETEXT)
	{
		return 0;
	}

	int len = 0;
	char *p = text;
	while (*p != '\0')
	{
		if ((unsigned char) *p >= 128)
		{
			if (IS_GB_CODE(p))
				len += 2;
			p += GET_CHR_LEN(p);
		}
		else
		{
			if (*p == '&')
			{
				char *tmp = p;
				p = strchr(p, ';');
				if (!p || p - tmp > 7)
				{
					p = tmp;
				}
			}
			else if (isalnum(*p))
			{
				if (!strict || isalpha(*p))
					len++;
			}
			p++;
		}
	}
	return len;
}

/**
 * @brief ��������к��ֵĸ���
 */
int get_chword_len(const char *text)
{
	const char *p = text;
	int ret = 0;
	while (*p != '\0')
	{
		if ((unsigned char) *p >= 128)
		{
			ret += 2;
		}
		p += GET_CHR_LEN(p);
	}
	return ret;
}

/**
 * @brief ��ӡ���
 */
static int print_vnode(html_vnode_t *vnode, void *fp)
{
	FILE *fpp = (FILE*) fp;
	html_tag_t *ptag = &vnode->hpNode->html_tag;
	if (ptag->text)
	{
		fprintf(fpp, "%s ", ptag->text);
	}
	return 0;
}

/**
 * @brief ��ӡ�ֿ�
 */
void print_area(FILE *fp, html_area_t *area)
{
	for (html_vnode_t *vnode = area->begin; vnode; vnode = vnode->nextNode)
	{
		html_vnode_visit(vnode, print_vnode, NULL, fp);
		if (vnode == area->end)
			break;
	}
}

/**
 * @brief short description ������ȡ���ֳ��ȵ���ʱ�ṹ
 */
typedef struct
{
	int *tag_code_len; /**<  �����������tag code����      */
	int size; /**<  ���������С      */
	int in_anchor; /**<  �Ƿ���������      */
} len_info_t;

static int start_visit_for_len(html_vnode_t * vnode, void *data)
{
	html_tag_t *ptag = &vnode->hpNode->html_tag;
	len_info_t *pli = (len_info_t *) data;
	if (ptag->tag_type == TAG_A)
		pli->in_anchor = 1;

	if (get_vnode_tag_code(vnode) >= pli->size)
		return VISIT_NORMAL;

	int len = get_valid_text_len(vnode);
	if (pli->in_anchor == 0)
	{
		pli->tag_code_len[get_vnode_tag_code(vnode)] = len;
	}
	else
	{
		pli->tag_code_len[get_vnode_tag_code(vnode)] = -len;
	}
	return VISIT_NORMAL;
}

/**
 * @brief ��������
 */
static int finish_visit_for_len(html_vnode_t *vnode, void*data)
{
	html_tag_t *ptag = &vnode->hpNode->html_tag;
	len_info_t *pli = (len_info_t *) data;
	if (ptag->tag_type == TAG_A)
		pli->in_anchor = 0;
	return VISIT_NORMAL;
}

/**
 * @brief ���tag code�����ֳ��ȵ�ӳ���
 */
void get_tag_code_text_len(html_vnode_t *vnode, int *tag_code_len, int size)
{
	len_info_t li;
	li.tag_code_len = tag_code_len;
	li.size = size;
	li.in_anchor = 0;
	html_vnode_visit(vnode, start_visit_for_len, finish_visit_for_len, &li);
}

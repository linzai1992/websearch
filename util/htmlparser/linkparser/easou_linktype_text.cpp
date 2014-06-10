/***************************************************************************
 * 
 * Copyright (c) 2012 Easou.com, Inc. All Rights Reserved
 * $Id: easou_linktype_text.cpp,v 1.0 2012/09/01 pageparse Exp $
 * 
 **************************************************************************/

/**
 * @file easou_linktype_text.cpp
 * @author (pageparse@staff.easou.com)
 * @date 2012/09/01
 * @version $Revision: 1.0 $
 * @brief ��Ǵ����ı�����
 **/

#include "easou_link_mark.h"
#include "easou_mark_parser.h"
#include "easou_link_common.h"
#include "easou_vhtml_parser.h"
#include "html_text_utils.h"
#include <ctype.h>

/*
 * ��ȡ�ڵ���ߵ��ı��ڵ�
 */
static html_vnode_t *get_left_text_vnode(html_vnode_t * vnode)
{
	html_vnode_t *iter = vnode->prevNode;
	while (iter && iter->textSize <= 0)
		iter = iter->prevNode;

	if (iter)
		return iter;
	if (vnode->upperNode == NULL)
		return NULL;

	iter = vnode->upperNode->prevNode;
	while (iter && iter->textSize <= 0)
		iter = iter->prevNode;
	return iter;
}

/*
 * ��ȡ�ڵ��ұߵ��ı��ڵ�
 */
static html_vnode_t *get_right_text_vnode(html_vnode_t *vnode)
{
	html_vnode_t *iter = vnode->nextNode;
	while (iter && iter->textSize <= 0)
		iter = iter->nextNode;

	if (iter)
		return iter;

	if (vnode->upperNode == NULL)
		return NULL;

	iter = vnode->upperNode->nextNode;
	while (iter && iter->textSize <= 0)
		iter = iter->nextNode;
	return iter;
}

/*
 * ��ȡ�ڵ����������
 */
static font_t get_vnode_text_font(html_vnode_t *vnode)
{
	html_vnode_t *iter = vnode;
	while (iter->hpNode->html_tag.tag_type != TAG_PURETEXT && iter->firstChild)
	{
		iter = iter->firstChild;
	}
	return iter->font;
}

/*
 * ��ȡ�Ƿ���ɫ���ܱ�һ��
 */
static bool is_same_color_around(html_vnode_t *vnode)
{
	html_vnode_t *left_vnode = get_left_text_vnode(vnode);
	html_vnode_t *right_vnode = get_right_text_vnode(vnode);
	font_t f = get_vnode_text_font(vnode);

	if (left_vnode && f.color != left_vnode->font.color)
	{
		return false;
	}
	if (right_vnode && f.color != right_vnode->font.color)
	{
		return false;
	}
	if (left_vnode || right_vnode)
	{
		return true;
	}

	return false;
}

/**
 * @brief short description ��Ǵ����ı�����
 */
int mark_linktype_text(lt_args_t *pargs, lt_res_t *pres)
{
	for (int i = 0; i < pargs->link_count; i++)
	{
		html_vnode_t *vnode = pargs->vlink[i].inner.vnode;
		if (vnode == NULL)
			continue;
		if (!(pargs->vlink[i].linkFunc & VLINK_CONT_INTERLUDE))
		{
			continue;
		}

		css_prop_node_t *css_prop = vnode->css_prop;
		while (css_prop)
		{
			if (css_prop->type == CSS_PROP_TEXT_DECORATION && strcmp(css_prop->value, "none") == 0
					&& is_same_color_around(vnode))
			{
				pargs->vlink[i].linkFunc |= VLINK_TEXT_LINK;
				break;
			}
			css_prop = css_prop->next;
		}
	}
	return 1;
}

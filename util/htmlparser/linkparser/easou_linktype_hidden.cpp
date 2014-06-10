/***************************************************************************
 * 
 * Copyright (c) 2012 Easou.com, Inc. All Rights Reserved
 * $Id: easou_linktype_hidden.cpp,v 1.0 2012/09/01 pageparse Exp $
 * 
 **************************************************************************/

/**
 * @file easou_linktype_hidden.cpp
 * @author (pageparse@staff.easou.com)
 * @date 2012/09/01
 * @version $Revision: 1.0 $
 * @brief �����������
 **/
#include "easou_link_mark.h"
#include "easou_mark_parser.h"
#include "easou_link_common.h"
#include "easou_vhtml_parser.h"
#include "html_text_utils.h"
#include "easou_html_attr.h"
#include <ctype.h>
#include <string.h>

//��Щ������Ȼ��SpecialTagϢϢ��أ����������ط�û��Ҫ֪�����ǣ����Ծ�û���������Ϊ�ӿڣ������Ծ�̬��������ʽ�������
static const int MARQUEEWIDTH_THRESHOLD = 30; //��������ֿ��С�ڵ��ڴ���ֵ������Ϊ��������
static const int MARQUEEHEIGHT_THRESHOLD = 13; //��������ָ߶�С�ڵ��ڴ���ֵ����Ϊ���¹���������Ϊ��������
static const int CHECK_PARENT_THRESHOLD = 10;

/**
 * @brief �������ֵ
 */
static int get_int_value(const char* pValue)
{
	if (pValue == NULL)
		return 0;

	int rlt = 0;
	if (sscanf(pValue, "%d", &rlt) == 0) //��������޷�����������Ϊ0
		rlt = 0;

	return rlt;
}

/**
 * @brief �Ƿ��������Էֱ��С����
 */
static bool is_small_size(const char *value)
{
	const char *skip = value;
	while (*skip == '\t' || *skip == ' ' || *skip == ':')
		skip++;
	if (strncmp(skip, "0.", 2) == 0)
		return true;
	return false;
}

/**
 * @brief �Ƿ�С��������������
 */
static bool is_small_contain(html_vnode_t *pvnode, style_attr_t *attrs, int attr_count)
{
	html_tag_t *ptag = &pvnode->hpNode->html_tag;
	char *width = get_attribute_value(ptag, ATTR_WIDTH);
	char *height = get_attribute_value(ptag, ATTR_HEIGHT);

	if (width && is_small_size(width))
		return true;
	if (height && is_small_size(height))
		return true;

	if (attr_count > 0)
	{
		char value[MAX_ATTR_VALUE_LENGTH];
		if (get_style_attr_value("width", value, sizeof(value), attrs, attr_count) && is_small_size(value))
			return true;
		if (get_style_attr_value("height", value, sizeof(value), attrs, attr_count) && is_small_size(value))
			return true;
		if (get_style_attr_value("line-height", value, sizeof(value), attrs, attr_count) && is_small_size(value))
			return true;
	}

	return false;
}

/**
 * @brief �Ƿ񳬹���Ļ����
 */
static bool is_outer_screen(html_vnode_t *pvnode, style_attr_t *attrs, int attr_count)
{
	if (attr_count == 0)
		return false;
	char value[MAX_ATTR_VALUE_LENGTH];

	bool absolute = false;
	if (get_style_attr_value("position", value, sizeof(value), attrs, attr_count))
		if (is_attr_value(value, "absolute", strlen("absolute")))
			absolute = true;

	if (!get_style_attr_value("top", value, sizeof(value), attrs, attr_count)
			&& !get_style_attr_value("left", value, sizeof(value), attrs, attr_count))
		return false;

	char *fuhao = strchr(value, '-');
	if (!fuhao)
		return false;

	int size = strtol(fuhao + 1, NULL, 10);
	if (size >= 999 || (size >= 99 && absolute))
		return true;

	return false;
}

/**
 * @brief �Ƿ������ع�����
 */
static bool is_hidden_marquee(html_vnode_t *pvnode, style_attr_t *attrs, int attr_count)
{
	static const int DEFAULT_SIZE = 100;
	html_tag_t *ptag = &pvnode->hpNode->html_tag;
	if (ptag == NULL || ptag->tag_type != TAG_MARQUEE)
		return false;

	int width = pvnode->wx;
	int height = pvnode->hx;
	bool bUpDown = false;

	char value[MAX_ATTR_VALUE_LENGTH];
	if (attr_count > 0 && get_style_attr_value("direction", value, sizeof(value), attrs, attr_count))
	{
		if (is_attr_value(value, "down", strlen("down")) || is_attr_value(value, "up", strlen("up")))
			bUpDown = true;
	}

	char* pWidth = get_attribute_value(ptag, ATTR_WIDTH);
	char* pHeight = get_attribute_value(ptag, ATTR_HEIGHT);

	if (pWidth) //���û����width���ԣ�����Ϊwidth����
		width = get_int_value(pWidth);
	else
		width = DEFAULT_SIZE;

	if (pHeight) //���û����height���ԣ�����Ϊheight����
		height = get_int_value(pHeight);
	else
		height = DEFAULT_SIZE;

	//��ȹ�С
	if (width >= 0 && width <= MARQUEEWIDTH_THRESHOLD)
		return true;

	//���¹����Ҹ߶ȹ�С
	if (height >= 0 && height <= MARQUEEHEIGHT_THRESHOLD && bUpDown)
		return true;

	return false;
}

static const char* get_vnode_style(html_vnode_t * vnode)
{
	for (html_attribute_t *attr = vnode->hpNode->html_tag.attribute; attr; attr = attr->next)
	{
		switch (attr->type)
		{
		case ATTR_STYLE:
			return attr->value;
		default:
			break;
		}
	}
	return NULL;
}

/**
 * @brief �����������
 */
int mark_linktype_hidden(lt_args_t *pargs, lt_res_t *pres)
{
	static const int MAX_ATTR_COUNT = 30;
	style_attr_t attrs[MAX_ATTR_COUNT];
	int attr_count = 0;

	for (int i = 0; i < pargs->link_count; i++)
	{
		if (pargs->vlink[i].inner.vnode == NULL)
			continue;
		if (pargs->vlink[i].inner.is_goodlink == 0)
			continue;
		html_vnode_t *vnode = pargs->vlink[i].inner.vnode;
		if (vnode == NULL)
			continue;
		html_vnode_t *iter_vnode = vnode;
		bool is_hidden = false;

		int upper_cnt = 0;
		int in_option = 0;
		while (upper_cnt < CHECK_PARENT_THRESHOLD && iter_vnode)
		{
			const char *style_value = get_vnode_style(iter_vnode);
			if (style_value)
			{
				attr_count = parse_style_attr(style_value, attrs, sizeof(attrs) / sizeof(style_attr_t));
			}
			else
				attr_count = 0;

			if (is_hidden_marquee(iter_vnode, attrs, attr_count) || is_outer_screen(iter_vnode, attrs, attr_count)
					|| is_small_contain(iter_vnode, attrs, attr_count))
			{
				is_hidden = true;
				break;
			}

			if (iter_vnode->hpNode->html_tag.tag_type == TAG_OPTION)
				in_option = 1;

			iter_vnode = iter_vnode->upperNode;
			upper_cnt++;
		}

		if (!in_option
				&& (!vnode->isValid || vnode->font.color == vnode->font.bgcolor || (vnode->xpos + vnode->wx < 0)
						|| (vnode->ypos + vnode->hx < 0) || vnode->wx <= 2 || vnode->hx <= 2))
		{
			is_hidden = true;
		}
		int linkfunc = pargs->vlink[i].linkFunc;
		if (is_hidden && !(linkfunc & VLINK_IMG_SRC) && !(linkfunc & VLINK_CSS) && !(linkfunc & VLINK_IMAGE)
				&& pargs->vlink[i].inner.is_goodlink)
		{
			pargs->vlink[i].linkFunc |= VLINK_HIDDEN;
		}
	}

	return 1;
}

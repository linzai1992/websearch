/***************************************************************************
 * 
 * Copyright (c) 2012 Easou.com, Inc. All Rights Reserved
 * $Id: easou_link_area.h,v 1.2 2012/09/01 pageparse Exp $
 * 
 **************************************************************************/

/**
 * @file easou_link_area.h
 * @author (pageparse@staff.easou.com)
 * @date 2012/09/01
 * @version $Revision: 1.0 $
 * @brief �ֿ���Ϣͳ��
 **/

#ifndef _EASOU_LINK_AREA_H
#define _EASOU_LINK_AREA_H

#include "easou_vhtml_parser.h"
#include "easou_ahtml_area.h"
#include "easou_link_timematch.h"

#define MAX_AREA_NUM 	2500
#define MAX_TIME_NUM	1000
#define MAX_AREA_TIME_NUM	60
#define MAX_VNODE_NUM	50000

/**
 * @brief short description ʱ����
 */
typedef struct
{
	html_vnode_t *vnode; /**<  ���      */
	unsigned int next_gap; /**<  ��㵽��һ��ʱ��������ּ��      */
	unsigned int last_gap; /**<  ��㵽��һ��ʱ��������ּ��      */
	bool next_has_cont; /**<  �������һ��ʱ����֮���Ƿ�������      */
	bool last_has_cont; /**<  �������һ��ʱ����֮���Ƿ�������      */
	unsigned int route_sign; /**<  ·��ǩ��      */
} time_node_info;

/**
 * @brief short description �ֿ��ڲ���Ϣ��ʱ����Ϣ
 */
typedef struct
{
	time_node_info *ptime_nodes[MAX_AREA_TIME_NUM]; /**< ʱ����     */
	unsigned int time_node_cnt; /**<  ʱ�����      */
} area_node_inner_info;

/**
 * @brief short description  blog bbs����
 */
typedef struct
{
	int comment_point; /**<  ���۷���      */
	int comment_area_cnt; /**<  ���ۿ�ĸ���      */
} blog_bbs_info;

/**
 * @brief short description �����ֿ���Ϣ
 */
typedef struct
{
	area_node_inner_info inner; /**<   �ֿ��ڲ�����      */
	int time_cnt; /**<  ʱ�����      */
	int inner_link_cnt; /**<  ��������      */
	int outer_link_cnt; /**<  ��������      */
	int inner_anchor_len; /**<  ����anchor����      */
	int anchor_len; /**<  anchor�ܳ���      */
	int notanchor_text_len; /**<  ���ĳ���      */
	int max_time_gap_len; /**<  ʱ��ڵ�֮����������ֳ���      */
	int anchor_cnt; /**<  anchorg����      */
	blog_bbs_info binfo; /**<  blog bbs����      */
} area_node_info;

/**
 * @brief short description ҳ���ܵķֿ���Ϣ
 */
typedef struct _lt_area_info_t
{
	time_node_info time_nodes[MAX_TIME_NUM]; /**<  ʱ����buffer      */
	area_node_info *area_nodes; /**<  �ֿ�����  */
	int area_node_size;
	unsigned int time_nodes_cnt; /**<  ʱ�������      */
} lt_area_info_t;

static inline void del_area_info(lt_area_info_t *parea_info)
{
	if (parea_info)
	{
		free(parea_info->area_nodes);
		free(parea_info);
	}
}

/**
 * @brief �����ֿ���Ϣbuffer
 */
static inline lt_area_info_t * create_area_info()
{
	lt_area_info_t *parea_info = (lt_area_info_t *) calloc(1, sizeof(lt_area_info_t));
	if (parea_info == NULL)
	{
		Error("calloc area_info error.");
		del_area_info(parea_info);
		return NULL;
	}

	parea_info->area_nodes = (area_node_info *) calloc(MAX_AREA_NUM, sizeof(area_node_info));
	if (parea_info->area_nodes == NULL)
	{
		Error("calloc vhplk_area_node_info error.");
		del_area_info(parea_info);
		return NULL;
	}

	parea_info->area_node_size = MAX_AREA_NUM;

	return parea_info;
}

/**
 * @brief ���ָ���ֿ��Ӧ�ķֿ�ͳ����Ϣ
 */
static inline area_node_info *get_area_node(lt_area_info_t *pai, html_area_t *area)
{
	if (!area)
		return NULL;
	if (area->no >= MAX_AREA_NUM)
	{
		Info("parea->no %d too big", area->no);
		return NULL;
	}
	return &pai->area_nodes[area->no];
}

/**
 * @brief ��÷ֿ���Ϣ
 */
void get_area_info(area_tree_t *pareas, lt_area_info_t *pai, const char * url, timematch_pack_t *ptimematch);

/**
 * @brief ͳ�Ƶ�ǰ�ֿ鰴·��ǩ����ͬ��ʱ��������
 */
unsigned int cacu_max_depth_time_count(area_node_info *pcur_node);

/**
 * @brief ͳ�Ƶ�ǰ�ֿ鰴·��ǩ����ͬ��ʱ��������������ʱ������
 */
unsigned int cacu_max_depth_time_count_with_linktime_filter(area_node_info *pcur_node);

#endif

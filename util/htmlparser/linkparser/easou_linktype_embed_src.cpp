/***************************************************************************
 * 
 * Copyright (c) 2012 Easou.com, Inc. All Rights Reserved
 * $Id: easou_linktype_embed_src.cpp,v 1.0 2012/09/01 pageparse Exp $
 * 
 **************************************************************************/

/**
 * @file easou_linktype_embed_src.cpp
 * @author (pageparse@staff.easou.com)
 * @date 2012/09/01
 * @version $Revision: 1.0 $
 * @brief ���Ƕ����Դ���ӡ�
 **/

#include "easou_link.h"
#include "easou_link_mark.h"
#include "easou_link_common.h"

int mark_linktype_embed_src(lt_args_t *pargs, lt_res_t *pres)
{
	vlink_t *vlink = pargs->vlink;
	int link_count = pargs->link_count;

	for (int i = 0; i < link_count; i++)
	{
		if (vlink[i].inner.vnode == NULL)
			continue;
		html_tag_t *html_tag = &(vlink[i].inner.node->html_tag);
		if (html_tag->tag_type == TAG_EMBED)
		{
			vlink[i].linkFunc |= VLINK_EMBED_SRC;
		}
	}

	return 1;
}

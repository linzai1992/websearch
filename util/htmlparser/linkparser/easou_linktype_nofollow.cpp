/***************************************************************************
 * 
 * Copyright (c) 2012 Easou.com, Inc. All Rights Reserved
 * $Id: easou_linktype_nofollow.cpp,v 1.0 2012/09/01 pageparse Exp $
 * 
 **************************************************************************/

/**
 * @file easou_linktype_nofollow.cpp
 * @author (pageparse@staff.easou.com)
 * @date 2012/09/01
 * @version $Revision: 1.0 $
 * @brief ���nofollow����
 **/

#include "easou_link_mark.h"
#include "easou_mark_parser.h"
#include "easou_vhtml_parser.h"
#include "easou_link_common.h"
#include "html_text_utils.h"
#include <ctype.h>

/**
 * @brief ���nofollow����
 */
int mark_linktype_nofollow(lt_args_t *pargs, lt_res_t *pres)
{
	for (int i = 0; i < pargs->link_count; i++)
	{
		if (pargs->vlink[i].nofollow)
			pargs->vlink[i].linkFunc |= VLINK_NOFOLLOW;
	}
	return 1;
}

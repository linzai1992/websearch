/**
 *  easou_extractor_stdata_video.h
 *  �ṹ����ȡ-��Ƶ��
 *
 *  Created on: 2013-7-19
 *      Author: round
 */

#ifndef EASOU_EXTRACTOR_STDATA_VIDEO_H_
#define EASOU_EXTRACTOR_STDATA_VIDEO_H_

#include "StructData_constants.h"
#include "StructData_types.h"

#include "easou_html_dom.h"
#include "easou_vhtml_basic.h"
#include "easou_ahtml_tree.h"




/**
 * @brief ��ȡ��Ƶ��ṹ������
 * @param url [in], ҳ��url
 * @param url_len [in], url�ĳ���
 * @param page_type [in], ҳ������
 * @param html_tree [in], �Ѿ������õ�dom��
 * @param html_vtree [in], �Ѿ������õ�vtree��
 * @param html_vtree [in], �Ѿ������õ�atree��
 * @param stdata [out], ��ų�ȡ�������
 * @return 0 ��ʾ�ɹ���������ʾʧ��
 * @author round
 * @date 2013-09-06      
 **/

int html_tree_extract_video_stdata(const char *url, int url_len, unsigned int pagetype, html_tree_t *html_tree, html_vtree_t *html_vtree, area_tree_t *atree, StructureData *stdata);


#endif /* EASOU_EXTRACTOR_STDATA_VIDEO_H_ */

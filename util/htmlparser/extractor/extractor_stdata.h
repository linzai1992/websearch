/**
 *  easou_extractor_stdata.h
 *  �ṹ����ȡ
 *
 *  Created on: 2013-7-19
 *      Author: round
 */

#ifndef EASOU_EXTRACTOR_STDATA_H_
#define EASOU_EXTRACTOR_STDATA_H_

#include "StructData_constants.h"
#include "StructData_types.h"

#include "easou_html_dom.h"
#include "easou_vhtml_basic.h"
#include "easou_ahtml_tree.h"

/**
 * @brief ��ȡ�ṹ������
 * @param url [in], ҳ��url
 * @param url_len [in], url�ĳ���
 * @param page_type [in], ҳ������
 * @param html_tree [in], �Ѿ������õ�dom��
 * @param stdata [out], ��ų�ȡ�������
 * @return 0 ��ʾ�ɹ���������ʾʧ��
 * @author round
 * @date 2013-07-20      
 **/
int html_tree_extract_stdata(const char *url, int url_len, unsigned int pagetype, html_tree_t *html_tree, StructureData *stdata);



/**
 * @brief ��ȡС˵��ṹ������
 * @param url [in], ҳ��url
 * @param url_len [in], url�ĳ���
 * @param page_type [in], ҳ������
 * @param html_tree [in], �Ѿ������õ�dom��
 * @param html_vtree [in], �Ѿ������õ�vtree��
 * @param html_vtree [in], �Ѿ������õ�atree��
 * @param stdata [out], ��ų�ȡ�������
 * @return 0 ��ʾ�ɹ���������ʾʧ��
 * @author round
 * @date 2013-08-08      
 **/
int html_tree_extract_download_stdata(const char *url, int url_len, unsigned int pagetype, html_tree_t *html_tree, html_vtree_t *html_vtree, area_tree_t *atree, StructureData *stdata);


int html_tree_extract_music_stdata(const char *url, int url_len, unsigned int pagetype, html_tree_t *html_tree, html_vtree_t *html_vtree, area_tree_t *atree, StructureData *stdata);

/**
 * @brief ���л��ṹ������
 * @param stdata [in], �Ѿ������õĽṹ������
 * @param buf_ptr [in/out], ���ڱ������л������ݵĻ���
 * @param sz [in], buf_ptr�Ĵ�С
 * @return -1 ��ʾʧ�ܣ�������ʾ���л���ĳ���
 * @author round
 * @date 2013-07-20      
 **/
int html_tree_extract_serial(StructureData *stdata, uint8_t *buf_ptr, uint32_t size);


/**
 * @brief �����л��ṹ������
 * @param stdata [out], ��Ž�����Ľṹ������
 * @param buf_ptr [in], ���л����ݵĻ���
 * @param sz [in], buf_ptr�Ĵ�С
 * @return -1 ��ʾʧ�ܣ�0��ʾ�����л��ɹ�.
 * @author round
 * @date 2013-07-20      
 **/
int html_tree_extract_deserial(StructureData *stdata, uint8_t *buf_ptr, uint32_t sz);


#endif /* EASOU_EXTRACTOR_STDATA_H_ */

/**
 * easou_extractor_title.h
 *
 *  Created on: 2012-1-11
 *      Author: xunwu
 */

#ifndef EASOU_EXTRACTOR_TITLE_H_
#define EASOU_EXTRACTOR_TITLE_H_

#include "easou_html_tree.h"
#include "easou_ahtml_tree.h"

/**
 * @brief ��ȡ��ҳ��tagtitle��������ȡ���ĳ���
 */
int html_tree_extract_tagtitle(html_tree_t *html_tree, char* title, int size);

/**
 * @brief ��ȡ��ҳ��realtitle��Ҫ����ҳ�Ѿ����mark��������ȡ���ĳ���
 */
int html_atree_extract_realtitle(area_tree_t *area_tree, char *realtitle, int size);

/**
 * @brief ��ȡ��ҳ��subtitle��Ҫ����ҳ�Ѿ����mark��������ȡ���ĳ���
 */
int html_atree_extract_subtitle(area_tree_t *area_tree, char *subtitle, int size);

/**
 * @brief easou_extract_realtitle����������
 * @author sue
 * @date 2013-07-22
 */
struct realtitle_input_t
{
	area_tree_t* atree;	//�Ѿ���Ǻõķֿ���
	const char* url;	//��ҳ��url
	int url_len;		//url�ĳ���
	const char* tagtitle;	//��ҳ��tagtitle
	int tagtitle_len;	//tagtitle�ĳ���
};

/**
 * @brief ��ȡ��ҳ��realtitle��Ҫ����ҳ�Ѿ����mark��ͬhtml_atree_extract_realtitle������ȣ��÷���
 * 	�����һЩ���Զ��ض���������Ż�������ȫ���������ı�ǡ�
 * @param input [in], ��ȡ��Ҫ�õ�����Ϣ
 * @param realtitle [in/out], ������ȡ���ı���
 * @param size [in], realtitle�Ĵ�С
 * @return �ɹ�ʱ������ȡ���ĳ���(>=0)��<0��ʾʧ�ܡ�
 * @author sue
 * @date 2013-07-22
 */
int easou_extract_realtitle(realtitle_input_t* input, char* realtitle, int size);

#endif /* EASOU_EXTRACTOR_TITLE_H_ */

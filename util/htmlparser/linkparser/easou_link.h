/***************************************************************************
 *
 * Copyright (c) 2012 Easou.com, Inc. All Rights Reserved
 * $Id: easou_link.h,v 1.0 2012/09/01 pageparse Exp $
 *
 **************************************************************************/

#ifndef __EASOU_LINK_H__
#define __EASOU_LINK_H__

#include "log.h"
#include "easou_url.h"
#include "easou_string.h"
#include "easou_vhtml_tree.h"
#include "easou_link_mark.h"
#include "easou_ahtml_tree.h"

#define EXTRACT_MERGE           0	//�ϲ�anchor�еĿո�
#define EXTRACT_NOMERGE         1	//���ϲ�anchor�еĿո�
typedef int vlink_func_t;

#define VLINK_UNDEFINED 0x00		//δ��������	0
#define VLINK_NAV	0x01		//��������	1<<0
#define VLINK_FRIEND	0x02		//��������	1<<1
#define VLINK_INVALID	0x04		//���ɼ�������	1<<2
#define VLINK_CONTROLED	0x08		//ͨ��css�����Ӿ�������	1<<3
#define VLINK_IMAGE	0x10		//ͼƬ����	1<<4
#define VLINK_FRIEND_EX 0x20		//������������	1<<5
#define VLINK_SELFHELP	0x40		//��������	1<<6
#define VLINK_BLOG_MAIN	0x80		//��������	1<<7
#define VLINK_CSS	0x100		//css����	1<<8
#define VLINK_QUOTATION	0x200		//����		1<<9
#define VLINK_CONT_INTERLUDE 0x400	//���ݴ���	1<<10
#define VLINK_BBSRE	0x800		//bbs�ظ�	1<<11
#define VLINK_BLOGRE	0x1000		//blog�ظ�	1<<12
#define VLINK_IMG_SRC   0x2000      //ͼƬ��Դ����	1<<13
#define VLINK_EMBED_SRC 0x4000      //Ƕ����Դ����	1<<14
#define VLINK_FROM_CONT 0x8000		//!!!!!!!!!!!ec�ж����ʵ�ֵ��������ͣ���ֹ��ͻ������	1<<15
#define VLINK_BBSCONT	0x10000		//	1<<16
#define VLINK_BBSSIG	0X20000		//	1<<17
#define VLINK_COPYRIGHT	0x40000		//	1<<18
#define VLINK_NOFOLLOW	0x80000		//nofollow	1<<19
#define VLINK_MYPOS	0x100000		//mypos	1<<20
#define VLINK_HIDDEN	0x200000	//	1<<21
#define VLINK_TEXT_LINK	0x400000	//��������ӣ�αװ���ı���ʽ	1<<22
#define VLINK_FRIEND_SPAM	0x1000000	//Ԥ��������ϵͳ	1<<24
#define VLINK_IN_LINK	0x2000000		//Ԥ��������ϵͳ	1<<25
#define VLINK_TEXT  0x4000000   // ��������
#define VLINK_NEWS_ANCHOR 0x8000000	//����ҳ�����н��ܵ���վ����(Ԥ����EC)
//add
#define VLINK_IFRAME 0x10000000 //iframe��ǩ������
//modify
#define VLINK_ALL	0x3fffffff		//����
//#define VLINK_ALL	0xfffffff		//����
/**
 * @brief short description �����ڲ�����
 */
typedef struct _vlink_info_t
{
	int xpos; //������
	int ypos; //������
	int width; //���
	int height; //�߶�
	html_tag_type_t tag_type; //tag����
	html_node_t *node; //����ָ��Ľ��
	html_vnode_t *vnode; //����ָ���v���
	int is_goodlink :1; //�Ƿ���Ч����
	int is_outlink :1; //�Ƿ�����
	int link_set; // only for VLINK_BLOG_MAIN mark
	html_area_t *html_area; //���������ĸ���
	int route;
	int area_left_link_count; //�ÿ��л��ж�������
	int anchor_from_alt; //���ӵ�anchor�Ƿ����ͼƬ��alt
	int text_len;
	int is_for_screen :1; /**< for css link : �Ƿ���������Ļý�� */
} vlink_info_t;

/**
 * @brief short description ��������
 */
typedef struct _vlink_t
{
	char url[UL_MAX_URL_LEN]; //���ӵ�url
	char text[UL_MAX_TEXT_LEN]; //���ӵ�anchor
	char realanchor[UL_MAX_TEXT_LEN];
	// vlink marks
	html_area_abspos_mark_t position; //�������ڵ�λ��
	int linkFunc; //���ӵ���������
	char nofollow; //�Ƿ�Ϊnofollow����
	int group;
	// inner
	vlink_info_t inner; //�����ڲ�����
	int tag_code;
} vlink_t;

/**
 * @brief ͼƬ����
 */
enum img_type_t
{
	IMG_IN_ANCHOR,			//ê���е�ͼƬ
	IMG_NEAR_ANCHOR, 	//ê����ص�ͼƬ
	IMG_IN_CONTENT,		//������ص�ͼƬ
};

/**
 * @brief ͼƬ��Ϣ
 */
struct img_info_t
{
	img_type_t type;	//ͼƬ������
	char img_url[UL_MAX_URL_LEN]; //ͼƬ��url
	int img_wx;	//ͼƬ�Ŀ�ȣ�û������ʱΪ0
	int img_hx;	//ͼƬ�ĸ߶ȣ�û������ʱΪ0
	int trust;		//ͼƬ�Ŀ�߿��Ŷȣ�0�����ţ�10�����
	vlink_t *owner;	//ͼƬ��Ӧ��vlink����ΪNULL����ʾ�͵�ǰ����ҳ���url��Ӧ
};

/**
 * @brief short description ��ҳ��׼url��Ϣ
 */
typedef struct _base_info_t
{
	char base_domain[UL_MAX_SITE_LEN]; //����
	char base_port[UL_MAX_PORT_LEN]; //�˿�
	char base_path[UL_MAX_PATH_LEN]; //·��
} base_info_t;

/**
 * description : 
 * 	extract all of vlink in this area, every vlink marked postion and function
 * input :
 * 	root, the root vnode of htm vtree;
 * 	html_area, the area struct of html area;
 * 	base_url, the url of this web page;
 * 	base_info, the base url info, may be diff with base_url;
 * 	vlink : the array to store vlink;
 * 	num : the max num of vlink limit;
 * 	flag : merge or nomerge for anchor text
 * output :
 * 	vlink;
 * 	extra_info:
 * 		1.nofollow, if <meta content="nofollow"> defined  0x00000001
 * 		2.is_selfhelp_page, 0x00000002
 * return :
 * 	success : the number of vlinks
 * 	failed : -1
 * 	
 * Pre-required:
 * 	html_area : this area HAS marked position
 */
int html_area_extract_vlink(html_vtree_t *vtree, html_area_t *area, char *base_url, base_info_t *base_info,
		vlink_t *vlink, int num, char flag, char &extra_info);

/**
 * @brief ��html_vtree�н������ӣ����ұ����������, ����anchor�ϲ��ո�
 * @param [in] vtree   : html_vtree_t*	������
 * @param [in] atree   : area_tree_t*	�ֿ���
 * @param [in] base_url   : char*		��ҳ��url
 * @param [in/out] vlink   : vlink_t*		���ڴ洢��������link��buf
 * @param [in] maxnum   : int		��������link����
 * @return  int 
 * @retval   >=0 ����������link����
 **/
int html_vtree_extract_vlink(html_vtree_t *vtree, area_tree_t *atree, char *base_url, vlink_t *vlink, int maxnum);

/**
 * @brief ��CSS����ȡ���ӡ�
 * @param [in] css_text   : const char*	CSS���ı�.
 * @param [in] base_url   : const char*	ҳ��url,������ƴ�ӵ�base url.
 * @param [out] vlink   : vlink_t*	���ڴ洢��������link������.
 * @param [in] maxnum   : int	������ɵ�LINK����.
 * @param [bool] is_mark   : int	�Ƿ�����������Ա�ע��Ĭ��Ϊ��.
 * @return  int -1:error; >=0:��ȡ������������.
 **/
int css_extract_vlink(const char *css_text, const char *base_url, vlink_t *vlink, int maxnum, bool is_mark = true);

/**
 * @brief ��һ��html��������ȡ����������
 *
 * @param [in] text   : const char* html����
 * @param [in] text_len   : int html���ֳ���
 * @param [in/out] vlink   : vlink_t* �������ӵ�����
 * @param [in] maxnum   : int ����ܱ����������
 * @return  int 
 * @retval  >=0 �ɹ���ȡ��������������-1 ����
 **/
int text_extract_vlink(const char* text, int text_len, vlink_t *vlink, int maxnum);

/**
 * @brief ��html_vtree_t�н������ӣ����ұ����������, ����anchor���ϲ��ո�
 *
 * @param [in] vtree   : html_vtree_t*	��ҳ�ķֿ���Ϣ
 * @param [in] atree   : area_tree_t		��ҳ�ķֿ�����
 * @param [in] base_url   : char*		��ҳ��url
 * @param [in/out] vlink   : vlink_t*		���ڴ洢��������link��buf
 * @param [in] maxnum   : int		��������link����
 * @return  int 
 * @retval   >=0 ����������link����
 **/
int html_vtree_extract_vlink_nomerge(html_vtree_t *vtree, area_tree_t *atree, char *base_url, vlink_t *vlink,
		int maxnum);

/**
 * @brief �����������
 * @param [in] vtree   : html_vtree_t* 	������
 * @param [in] atree   : area_tree_t*	�ֿ���
 * @param [in] base_url   : char*	��ҳurl
 * @param [in/out] vlink   : vlink_t*	��ȡ����������
 * @param [in] link_count   : int	���Ӹ���
 * @param [in] pagetype   : unsigned int	ҳ������
 * @param [in] marktype_flag   : unsigned int	��Ҫ��ǵ���������
 * @param [in] pvres   : vhp_res_t*	������������buffer
 * @return  void 
 **/
void html_vtree_mark_vlink(html_vtree_t *vtree, area_tree_t *atree, char *base_url, vlink_t *vlink, int link_count,
		unsigned int pagetype, unsigned int marktype_flag, lt_res_t *pvres);

/**
 * @brief ��ȡͼƬ��Ϣ
 * @param [in] url, ��ǰ����ҳ���url
 * @param [in] url_len, url�ĳ���
 * @param [in] atree, �����õķֿ���
 * @param [in] vlinks, �Ѿ���ȡ�õ�vlink
 * @param [in] num, ��ȡ����vlink�ĸ���
 * @param [in/out] imgs, ����ͼƬ��Ϣ
 * @param [in] size, imgs������
 * @return ʵ�ʳ�ȡ����ͼƬ��Ϣ����
 * @author sue
 * @date 2013/04/14
 */
int extract_img_infos(const char *url, int url_len, area_tree_t* atree, vlink_t *vlinks, int num, img_info_t *imgs, int size);

/**
 * @brief ��ʼ�������������Դ
 *
 * @param [in] marktype_flag   : int	��Ҫ��ǵ��������Ϳ���
 * @return  vhp_res_t* 
 * @retval   	NOT NULL	������Դ
 * @retval	NULL		��ʼ����Դʧ��
 **/
lt_res_t * linktype_res_create(unsigned int marktype_flag);

/**
 * @brief ɾ����Դ
 *
 * @return  void 
 * @retval   
 **/
void linktype_res_del(lt_res_t *);

/**
 * @brief ������Դ
 */
void linktype_res_reset(lt_res_t *pvres);

/**
 * @brief �ͷ�������ʽ�ṹ��
 * @return  void 
 * @retval   
 **/
void fin_url_pcre();

#endif

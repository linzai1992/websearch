/***************************************************************************
 * 
 * Copyright (c) 2012 Easou.com, Inc. All Rights Reserved
 * $Id: easou_link_mark.h,v 1.2 2012/09/01 pageparse Exp $
 * 
 **************************************************************************/

/**
 * @file easou_link_mark.h
 * @author (pageparse@staff.easou.com)
 * @date 2012/09/01
 * @version $Revision: 1.0 $
 * @brief 
 * 	�����������ͱ�ע��ͷ�ļ�, �ڲ�ʹ��
 **/

#ifndef  __EASOU_LINK_MARK_H_
#define  __EASOU_LINK_MARK_H_

#include "bbsparser.h"
#include "easou_url.h"
#include "easou_ahtml_tree.h"
#include "easou_vhtml_parser.h"
#include "easou_extract_blogtime.h"
#include "easou_link_timematch.h"

struct _vlink_t;

/**
 * @brief short description ������������õ�������Դ
 */
typedef struct _lt_args_t
{
	html_vnode_t *root; //root ���

	//html_area_t *html_area;	//��ҳ�ֿ���Ϣ
	//int area_count;		//��ҳ�ֿ�����
	area_tree_t *atree;

	_vlink_t *vlink; //��ҳ���Ӽ���
	int link_count; //��ҳ���Ӹ���

	char *url; //��ҳ��url
	unsigned int pagetype; //��ҳ����
	html_vtree_t *vtree;
} lt_args_t;

/**
 * @brief short description ���������������õ���Դ
 */
typedef struct _blogmain_res_t
{
	extract_time_paras *pparas; //Ԥ�����ʱ��������ʽ
} blogmain_res_t;

/**
 * @brief short description ���ݴ�������ʹ�õ���Դ
 */
typedef struct _cont_interlude_res_t
{
	html_vnode_t **vnode_set; //vnode ����
	int vnode_set_used; //vnode_set ʹ�õ�����
	int vnode_set_size; //vnode_set �Ĵ�С
} cont_interlude_res_t;

/**
 * @brief short description ��������Ϣ
 */
typedef struct _group_link_t
{
	int text_len; /**<  ���ֳ���      */
	int anchor_len; /**<  anchor����      */
	int homepage_count; /**<  ��ҳ���Ӹ���      */
	int link_count; /**<  ���Ӹ���      */
	int outer_count; /**<  ��������      */
	int inner_count; /**<  ��������      */
} group_link_t;

#define MAX_GROUP_NUM 1000

/**
 * @brief short description ��ҳ������
 */
typedef struct _group_t
{
	group_link_t groups[MAX_GROUP_NUM]; /**<  ���ӷ���      */
	int group_num; /**<  ���ӷ������      */
} group_t;

struct _lt_area_info_t;

/**
 * @brief short description ����������õ���Դ
 */
typedef struct _lt_res_t
{
	unsigned int flag; //��Ҫ��ע����������
	blogmain_res_t * res_blogmain; //����������Դ
	cont_interlude_res_t *res_cont_interlude; //���ݴ���������Դ
	timematch_pack_t *ptime_match; /**<  ʱ���жϴʵ�      */
	struct _lt_area_info_t *area_info; /**<  �ֿ�ͳ����Ϣ      */
	int *tag_code_len; /**<  ����tagcode���������ֳ���      */
	int tag_code_size; /**<  tagcode���������size      */
	bbs_post_list_t *post_list; /**<  bbsparser�����ṹ   */
	group_t group; /**<  ���ӷ���ṹ      */
} lt_res_t;

/**
 * @brief ���������������Ӱ���Դ
 * @return  blogmain_res_t* 
 * @retval   NOT NULL	��Դ
 * @retval	NULL	����ʧ��
 **/
blogmain_res_t *blogmain_res_create();

/**
 * @brief ɾ����������������Դ
 * @return  void 
 **/
void blogmain_res_del(blogmain_res_t *);

/**
 * @brief ��ǲ�����������
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_blogmain(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief ��ǲ��ͻظ�����
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_blogre(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief ���bbs�ظ�����
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_bbsre(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief �������ݴ���������Դ
 * @return  cont_interlude_res_t* 
 * @retval   
 **/
cont_interlude_res_t *cont_interlude_res_create();

/**
 * @brief ɾ�����ݴ�����Դ
 * @return  void 
 **/
void cont_interlude_res_del(cont_interlude_res_t *);

/**
 * @brief ������ݴ�������
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_cont_interlude(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief ��ǵ�������
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_nav(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief ��ǰ�Ȩ����
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_copyright(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief ���nofollow����
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_nofollow(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief ���mypos����
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_mypos(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief �����������
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_hidden(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief ���css����
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_css(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief ���ͼƬ����
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_image(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief ���ͼƬ��Դ���ӡ�
 *
 * @param [in/out] pargs   : lt_args_t*    ���������
 * @param [in] pres   : lt_res_t*      ������Դ
 * @return  int 
 * @retval      >=0 ������������
 * @see 	=-1	��������
 **/
int mark_linktype_img_src(lt_args_t *pargs, lt_res_t *pres);
/**
 * @brief ���Ƕ����Դ���ӡ�
 *
 * @param [in/out] pargs   : lt_args_t*    ���������
 * @param [in] pres   : lt_res_t*      ������Դ
 * @return  int 
 * @retval      >=0 ������������
 * @see 	=-1	��������
 **/
int mark_linktype_embed_src(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief �����������
 *
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_friend(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief ͨ�����ӷ�����Ϣ�����������
 *
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_friend_by_group(lt_args_t *pargs, lt_res_t *pvres);

/**
 * @brief ���������������
 *
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_friendex(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief �����������
 *
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_selfhelp(lt_args_t *pargs, lt_res_t *pres);
/**
 * @brief ��ǲ��ɼ�����
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_invalid_and_control(lt_args_t *pargs, lt_res_t *pres);
/**
 * @brief �����������
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_quotation(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief ��Ǵ����ı�����
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int 
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_text(lt_args_t *pargs, lt_res_t *pres);

/**
 * @brief ���IFRAME����
 * @param [in/out] pargs   : lt_args_t*	���������
 * @param [in] pres   : lt_res_t*		������Դ
 * @return  int
 * @retval   	>=0	������������
 * @see 	=-1	��������
 **/
int mark_linktype_iframe(lt_args_t *pargs, lt_res_t *pres);

#endif  

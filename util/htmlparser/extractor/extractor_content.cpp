/*
 * easou_extractor_content.cpp
 *
 *  Created on: 2012-1-11
 *      Author: xunwu
 */
#include "easou_url.h"
#include "html_text_utils.h"
#include "easou_html_attr.h"
#include "easou_mark_baseinfo.h"
#include "easou_extractor_com.h"
#include "easou_extractor_content.h"
#include "debuginfo.h"

#define EXTRACT_BREAK           0
#define EXTRACT_NOBREAK         1
#define EXTRACT_NOBREAK_EX		2
/*
//Ϊ�ж�ҳ���Ƿ�������
//
const char* UrlInfo[]={
    "content",
    "detail",
    "Detail",
    "newspage",
    "thread=",
    "id=",
    "tid="
};
int URL_INFO_SIZE =sizeof(UrlInfo)/sizeof(UrlInfo[0]);



const char* MyPos[] = {
    "����",
    "����",
    "����",
    "��Ƶ",
    "¥����ҳ",
    "ȫ��",
    "����",
    "����",
    "ͼƬ��Ѷ",
    "�鿴����",
    "ժҪ",
    "����ͼ��",
};
int MY_POS_SIZE=sizeof(MyPos)/sizeof(MyPos[0]);
#define PAGETYPE_URL_DEPTH  4


const char* ContentInfo[] = {
     "��Դ",
    "����"
    "��Ҫ����",
    "����",
    "����˵����",
    "��ӡ",
    "����˵˵",
    "����",
    "����",
    "���α༭",
    "��������",
    "�ɷñ༭",
    "����",
    "�Ƽ�",
    "�ֺ�",
    "�ֻ�������",
    "ý�����",
    "��Ҫ����",
    "��ӡԤ��",
    "��������",
    "�����Դ",
    "����ʱ��",
    "����ʱ��",
    "�ظ�ʱ��",
    "�༭",
    "�Ķ�ȫ��",
    "���Ĺؼ���",
    "�ϴ�ʱ��",
    "һ������",
    "���Ŵ���",
    "���ص�ַ",
    "ǰһƪ",
    "��һƪ",
    "�Ƽ���",
    "�������ѽ���",
    "�����",
    "��Ѵ�",
    "�������",
    "������",
    "��ϸ����",
    "��������",
    "��Ҫ�ش�",
    "��Ʒ����",
    "�������",
    "����¥",
    "Ͷ������",
    "�汾",
    "������",
    "����",
    "רҵ�ظ�",
    "�ظ�/",
    "1¥",
    "���ش���",
    "��������",
    "Ӧ��ƽ̨",
    "�������",
    "��һ����",
    "��һ����",
    "��������",
    "��ַ��",
    "��ϵ��ʽ��",
    "�������Ѿ�������",
    "�����������",
    "���ƣ�",
    "����",
};
int CONTENT_INFO_SZE=sizeof(ContentInfo)/sizeof(ContentInfo[0]);
//
// add over
*/
typedef struct _vnode_attribute_t {
	int content_len;
	int anchor_len;
	char need;			// 0, not need; 1, need
	char in_oneline;	// 0 or 1, if text in one line
} vnode_attribute_t;

typedef struct _vcontent_data_t {
	area_tree_t *area_tree;
	char *content;
	int available;
	int end;
	char flag;	// indict if reserve line break
	html_vnode_t *prev_vnode;
	int prev_cont_len;
	int is_dir_url:1;
	int main_con_size;
	int main_anchor_size;
	bool ignore_unavail;  //�Ƿ���Բ��ɼ�Ԫ�s1�1�77
} vcontent_data_t;
/*
typedef struct _vhub_data_t {
	area_tree_t *area_tree;
	char *content;
	int available;
	int end;
	char flag;	// indict if reserve line break
	html_vnode_t *prev_vnode;
	int prev_cont_len;
	int is_dir_url:1;
	int main_con_size;
	int main_anchor_size;
	bool ignore_unavail;  //�Ƿ���Բ��ɼ�Ԫ�s1�1�77
	int total_txt_size;
	int max_txt_size;
	int max_link_size;
	int max_pic_area;
} vhub_data_t;
*/
static void area_add_seperator(html_area_t *area, vcontent_data_t *data)
{
	vcontent_data_t *cont_data = (vcontent_data_t *)data;
	char *content = cont_data->content;

	const char *seperator = NULL;
	switch(cont_data->flag){
		case EXTRACT_BREAK:
			seperator = "\n\n";
			break;
		case EXTRACT_NOBREAK:
			seperator = "\n";
			break;
		case EXTRACT_NOBREAK_EX:
			seperator = " ";
			break;
		default:
			assert(0);
	}

	if(area->depth == 1){
		if(cont_data->available > cont_data->prev_cont_len){
			cont_data->available = addBreakInfo(content, cont_data->available, cont_data->end, seperator);
		}
		cont_data->prev_cont_len = cont_data->available;
	}
}

static int start_visit_for_vcont(html_vnode_t *html_vnode, void *result)
{
	if (html_vnode->hx <= 0 || html_vnode->wx <= 0)
	{
		return VISIT_SKIP_CHILD;
	}
	vcontent_data_t *content_data = (vcontent_data_t *)result;
	if(!html_vnode->isValid && content_data->ignore_unavail && html_vnode->hpNode->html_tag.tag_type != TAG_PURETEXT){
		return VISIT_SKIP_CHILD;
	}
	char *src;
	char *space_str = " ";
	char *break_char = "\n";

	char flag = content_data->flag;
	html_tag_t *html_tag = &(html_vnode->hpNode->html_tag);

	if (html_tag->tag_type==TAG_TITLE || html_tag->tag_type==TAG_OPTION ||
	    html_tag->tag_type==TAG_NOSCRIPT || html_tag->tag_type==TAG_NOEMBED||html_tag->tag_type==TAG_IFRAME) {
		return VISIT_SKIP_CHILD;

	} else if (html_tag->tag_type == TAG_PURETEXT) {
		src = html_tag->text;
	} else if (html_tag->tag_type == TAG_TD ) {
		src = space_str;
	} else 	if (IS_BLOCK_TAG(html_vnode->property)) {
				src = break_char;
				content_data->available = addBreakInfo(content_data->content, content_data->available, content_data->end, break_char);
	}else
	{
		src = NULL;
	}

	int isBreak = 0;
	int hx_diff = 0;
	if (flag == EXTRACT_BREAK) {
		if (html_tag->tag_type == TAG_PURETEXT && content_data->prev_vnode && content_data->prev_vnode->depth == html_vnode->depth) {
			hx_diff = html_vnode->ypos - content_data->prev_vnode->ypos;
			if (hx_diff>=15 || hx_diff<-30){
				isBreak = 1;
			}
		}
	}

	if (flag == EXTRACT_NOBREAK || flag == EXTRACT_NOBREAK_EX) {
		if (src != NULL){
			content_data->available = copy_html_text(content_data->content, content_data->available, content_data->end, src);
		}
	} else if (flag == EXTRACT_BREAK) {
		if (isBreak){
			content_data->available = addBreakInfo(content_data->content, content_data->available, content_data->end, break_char);
		}
		if (src != NULL){
			content_data->available = copy_html_text(content_data->content, content_data->available, content_data->end, src);
		}
	}
	if (html_tag->tag_type==TAG_PURETEXT && (html_vnode->isValid || !content_data->ignore_unavail)){
		if(content_data->prev_vnode&&content_data->prev_vnode->inLink&&html_vnode->inLink){
			content_data->available = addBreakInfo(content_data->content, content_data->available, content_data->end, space_str);
		}
		content_data->prev_vnode = html_vnode;
	}
	return VISIT_NORMAL;
}
/**
 * ��ǰ������
 */
static void extract_area_content(html_area_t *area, vcontent_data_t *cont_data)
{
	for(html_vnode_t *vnode = area->begin; vnode; vnode=vnode->nextNode){
		if(vnode->isValid || vnode->hpNode->html_tag.tag_type == TAG_PURETEXT){
			if(VISIT_ERROR == html_vnode_visit(vnode, start_visit_for_vcont, NULL, cont_data)){
				return;
			}
			if(cont_data->available >= cont_data->end){
				break;
			}
		}
		if(vnode == area->end){
			break;
		}
	}
}

static int visit_for_cont_area(html_area_t *area, void *data)
{
	vcontent_data_t *cont_data = (vcontent_data_t *)data;
	if(!area->isValid && cont_data->ignore_unavail){
		return AREA_VISIT_SKIP;
	}
	if(is_func_area(area, AREA_FUNC_COPYRIGHT)){
		return AREA_VISIT_SKIP;
	}
	if(area->depth == 1 || area->valid_subArea_num == 0){
		cont_data->prev_vnode = NULL;
		area_add_seperator(area, cont_data);
		extract_area_content(area, cont_data);
		return AREA_VISIT_SKIP;
	}
	return AREA_VISIT_NORMAL;
}

/**
 * ��ǰ��ҳ����Ȩ��������������ݣ�����Ҷ�ӿ�
 */
int html_atree_extract_content(area_tree_t *area_tree, char *content, int size, bool ignore_unavail)
{
	vcontent_data_t cont_data;
	cont_data.area_tree = area_tree;
	cont_data.content = content;
	cont_data.available = 0;
	cont_data.end = size - 1;
	cont_data.prev_cont_len = 0;
	cont_data.prev_vnode = NULL;
	cont_data.flag = EXTRACT_NOBREAK;
	cont_data.ignore_unavail = ignore_unavail;

	html_area_t *root = area_tree->root;

	int ret = areatree_visit(root, visit_for_cont_area, NULL, &cont_data);

	content[cont_data.available] = '\0';

	if(ret == AREA_VISIT_ERR){
		return 0;
	}
	return cont_data.available;
}

/**
 * @brief �ӷֿ�����ȡ����.
**/
int html_area_extract_content(html_area_t *area, char *content, int size)
{
	vcontent_data_t cont_data;
	cont_data.area_tree = NULL;
	cont_data.content = content;
	cont_data.available = 0;
	cont_data.end = size - 1;
	cont_data.prev_cont_len = 0;
	cont_data.prev_vnode = NULL;
	cont_data.flag = EXTRACT_NOBREAK;

	int ret = areatree_visit(area,visit_for_cont_area,NULL,&cont_data);
	content[cont_data.available] = '\0';
	if(ret == AREA_VISIT_ERR){
		return 0;
	}
	return cont_data.available;
}

/**
 * �жϸÿ��Ƿ�����߻��ұߣ�true������
 */
static bool is_pagesider(html_area_t *area)
{
	if(area->depth > 2){
		return false;
	}

	if(area->pos_plus != IN_PAGE_LEFT && area->pos_plus != IN_PAGE_RIGHT){
		return false;
	}

	int con_size = area->baseinfo->text_info.con_size;
	int anchor_size = area->baseinfo->link_info.anchor_size;

	if(con_size - anchor_size >= 200){
		return false;
	}

	if(con_size >= 2 * anchor_size){
		return false;
	}

	return true;
}


/**
 * @brief ��������ӿ���жϽ���ٽ��й��ˣ����׼ȷ�ʣ���������badcase�9�61�1�77
**/
static bool is_right_rela_link(html_area_t *area, vcontent_data_t *vcont_data)
{
	if(!is_func_area(area, AREA_FUNC_RELATE_LINK)){
		return false;
	}

	if(area->pos_plus == IN_PAGE_LEFT
			|| area->pos_plus == IN_PAGE_RIGHT){
		return true;
	}

	if(area->area_info.width >= 600
			&& area->area_info.height >= 300
			&& area->area_info.height * 2 > vcont_data->area_tree->root->area_info.height){
		return false;
	}

	if(vcont_data->is_dir_url){
		return false;
	}

	return true;
}

/**
 * @brief �԰�Ȩ����жϽ���ٽ��й���.���׼ȷ�ʣ���������badcase�9�61�1�77
 * �жϸÿ��ǲ��ǰ�Ȩ�飻true����Ȩ��
**/
static bool is_right_copyright(html_area_t *area, area_tree_t *area_tree)
{
	if(!is_func_area(area, AREA_FUNC_COPYRIGHT)){
		marktreeprintfs(MARK_CENTRAL,"the area is not marked for copyright  at %s(%d)-%s\r\n",__FILE__,__LINE__,__FUNCTION__);
		return false;
	}

	if(area->pos_plus == IN_PAGE_FOOTER){
		return true;
	}

	if(area->area_info.height >= 300){
		marktreeprintfs(MARK_CENTRAL,"the area is marked by copyright,but the height of the area >300 ,it is not copyright at %s(%d)-%s\r\n",__FILE__,__LINE__,__FUNCTION__);
		return false;
	}

	if(area->area_info.ypos <= area_tree->root->area_info.height * 7 / 10){
		marktreeprintfs(MARK_CENTRAL,"the area is marked by copyright,but the ypos of the area <= the height of page *0.7 ,it is not copyright at %s(%d)-%s\r\n",__FILE__,__LINE__,__FUNCTION__);
		return false;
	}

	if(area->area_info.height > area_tree->root->area_info.height / 2){
		marktreeprintfs(MARK_CENTRAL,"the area is marked by copyright,but the height of the area > the height of page *0.5,it is not copyright at %s(%d)-%s\r\n",__FILE__,__LINE__,__FUNCTION__);
		return false;
	}

	return true;
}

/**
 * @brief ����Ҫ�����ӵ�ҳ����ȡ��Ҫ���݄1�71�1�77�1�71�1�77
**/
static int visit_mainly_link_page(html_area_t *area, void *data)
{
	if(!area->isValid){
		return AREA_VISIT_SKIP;
	}

	vcontent_data_t *cont_data = (vcontent_data_t *)data;

	if(area->depth == 1){
		area_add_seperator(area,cont_data);
	}

	if((!cont_data->is_dir_url && is_in_func_area(area,AREA_FUNC_NAV))
			|| (!cont_data->is_dir_url && is_in_func_area(area,AREA_FUNC_FRIEND))
			|| is_right_rela_link(area,cont_data)
			|| is_right_copyright(area,cont_data->area_tree)
			|| is_in_srctype_area(area,AREA_SRCTYPE_INTERACTION)){
		return AREA_VISIT_SKIP;
	}

	if(!cont_data->is_dir_url && is_pagesider(area)){
		return AREA_VISIT_SKIP;
	}

	//��Ȩ����λ����ҳ��ײ�����Ϊ��Ӧ����ȡ shuangwei add 20120510
	if(is_in_func_area(area,AREA_FUNC_COPYRIGHT)){
		if(area->area_info.ypos*100>area->area_tree->root->area_info.height*80){
			return AREA_VISIT_SKIP;
		}
	}
	if(area->valid_subArea_num == 0){
		extract_area_content(area,cont_data);
	}

	return AREA_VISIT_NORMAL;
}

/**
 * @brief �Ƿ�τ1�71�1�77�1�7���ȵĿ飬�������Ϊ��������ж��Ƿ���Ҫ���݄1�71�1�77�1�71�1�77
**/
static int is_proper_main_cont_area_size(const html_area_t *area, int page_width, int page_height)
{
	int area_width = area->area_info.width;
	int area_height = area->area_info.height;
//	if(area_width==1200 && area_height==2435){
//		printf("hello world!\n");
//	}
	//��̫��1�1�77
	if(area_width < 15 || area_height < 10){
		return 0;
	}
	if(area_width >= page_width/2 && area_height >= page_height/2 && area_height >= 300){
		//����̫��
		return 0;
	}

	if(area->depth == 2){
		return 1;
	}

	if(area_width >= 400 && area->valid_subArea_num > 0 && area_height >= 100
			&& (area_height <= 20*(int)area->valid_subArea_num)){
		//�б��Ϳ�
		return 1;
	}

	if(area->depth == 1){
		if(area->pos_mark == RELA_LEFT || area->pos_mark == RELA_RIGHT){
		//���ұ߿�
			return 1;
		}
	}

	if(area_width >= 100 && area_width <= 300 && area_height >= 100 && area_height <= 300){
		//����
		return 1;
	}

	if((area_width >= 500 || area_width >= page_width/3) && area_height <= 170){
		//����
		if(area_height >= 30){
			return 1;
		}
		if(area->area_info.ypos <= page_height/3 && area_height >= 15){
			return 1;
		}
	}

	if((area_width >= 500 || area_width >= page_width/3) && area_height > page_height/4){
		if(area_height >= 30){
			return 1;
		}
		if(area->area_info.ypos <= page_height/3 && area_height >= 15){
			return 1;
		}
	}

	if(area->area_info.ypos <= page_height/6 && area->area_info.ypos <= 200){
		//����Ϊrealtitle
		if(area_width >= 45 && area->depth <= 3 && area_height >= 15)
			return 0;
	}
	return 0;
}

typedef struct _text_tag_info_t{
	int tot_text_len;
	int max_text_len;
	int text_tag_num;
	int is_has_container_tag;
}text_tag_info_t;

static void text_tag_info_clean(text_tag_info_t *tt_info)
{
	memset(tt_info,0,sizeof(text_tag_info_t));
}

static int visit_for_text_tag_info(html_vnode_t *vnode, void *data)
{
	if(!vnode->isValid)
		return VISIT_SKIP_CHILD;

	text_tag_info_t *tt_info = (text_tag_info_t *)data;
	if(vnode->hpNode->html_tag.tag_type == TAG_PURETEXT){
		tt_info->tot_text_len += vnode->textSize;
		tt_info->text_tag_num ++;
		if(vnode->textSize > tt_info->max_text_len)
			tt_info->max_text_len = vnode->textSize;
	}
	else if(vnode->hpNode->html_tag.tag_type == TAG_A
			&& get_attribute_value(&vnode->hpNode->html_tag,ATTR_HREF) != NULL){
		return VISIT_SKIP_CHILD;
	}

	return VISIT_NORMAL;
}
static int visit_for_container_tag(html_vnode_t *vnode, void *data)
{
	if(vnode->isValid){

		text_tag_info_t *tt_info = (text_tag_info_t *)data;
		html_tag_type_t tag_type=vnode->hpNode->html_tag.tag_type;
		if(tag_type == TAG_ROOT
			|| tag_type == TAG_HTML
			|| tag_type == TAG_BODY
			|| tag_type == TAG_TABLE
			|| tag_type == TAG_TBODY
			|| tag_type == TAG_DIV
			|| tag_type == TAG_COLGROUP
			|| tag_type == TAG_CENTER
			|| tag_type == TAG_FORM){
		tt_info->is_has_container_tag=1;
		}
	}
	return VISIT_NORMAL;
}
static int has_proper_tag_len(html_area_t *area, int page_width, int page_height, int page_text_len)
{
	text_tag_info_t tt_info;
	text_tag_info_clean(&tt_info);

	/**
	 * ��ȡ�ı���ǩ���Ő_1�1�77
	 */
	for(html_vnode_t *vnode = area->begin; vnode; vnode=vnode->nextNode){
		if(vnode->isValid){
			html_vnode_visit(vnode,visit_for_text_tag_info,visit_for_container_tag,&tt_info);
		}
		if(vnode == area->end)
			break;
	}
	if(tt_info.is_has_container_tag==0&&area->begin->hpNode->html_tag.tag_type==TAG_TABLE&&area->baseinfo->text_info.con_size>area->baseinfo->link_info.anchor_size*2){
			return 1;
		}
	/* �����ı���ǩ�ĳ��Ȅ1�71�1�77�1�7��������Ϣ�ж��Ƿ���Ҫ���݁9�61�1�77
	 * �A1�1�77��˵�������ĵ��ı���ǩ���Ƚϴ�������Խ��ف9�61�1�77
	 * ������ı��Ƚ�֧�����飬�����ǰ�Ȩ��Ϣ����߿��ϵ����ք1�71�1�77�1�71�1�77
	 * */
	if(tt_info.max_text_len >= 100 || (tt_info.text_tag_num > 0 && tt_info.tot_text_len/tt_info.text_tag_num >= 70)){
		return 1;
	}

	int doctype=area->area_tree->hp_vtree->hpTree->doctype;
	if(area->area_info.ypos < page_height/3){
		if(tt_info.max_text_len >= 22 || (tt_info.text_tag_num <= 3 && tt_info.max_text_len >= 4)&&!(doctype>=doctype_xhtml_BP&&doctype<=doctype_html5))
			return 1;
	}

	if(tt_info.max_text_len >= page_text_len/3){
		//�������ע��׼ȷ
		return 1;
	}

	return 0;
}

/**
 * @brief �ֿ����ײ�ֿ��Ƿ����˱�ǁ9�61�1�77
**/
int has_area_marked(html_area_t *area)
{
	if(is_contain_func_area(area, AREA_FUNC_NAV)
			|| is_contain_func_area(area, AREA_FUNC_RELATE_LINK)
			|| is_contain_func_area(area, AREA_FUNC_FRIEND)
			|| is_contain_func_area(area, AREA_FUNC_MYPOS)
//			|| !IS_VOID_AREA_MARK(area->subtree_sem_mark)
			|| is_contain_srctype_area(area,AREA_SRCTYPE_INTERACTION)){
		return 1;
	}

	return 0;
}

/**
 * @brief ��1�1�77��ֿ��Ƿ����Ϊ��Ҫ���݁9�61�1�77
**/
static int is_main_cont_area(html_area_t *area, html_area_t *root)
{
	int pg_width = root->area_info.width;
	int pg_height = root->area_info.height;

	if(!is_proper_main_cont_area_size(area, pg_width, pg_height)){
		/** ���Ȳ�����1�1�77.*/
		return 0;
	}

	if(has_area_marked(area)){
		/*���ײ�ֿ�������ǣ�����Ϊ����̫��1�71�1�77�1�71�1�77*/
		return 0;
	}

	area_baseinfo_t *root_aoi = (area_baseinfo_t *)(root->baseinfo);
	int page_text_len = root_aoi->text_info.con_size;

	area_baseinfo_t *area_aoi = (area_baseinfo_t*)(area->baseinfo);
	int cont_size = area_aoi->text_info.con_size;
	int anchor_size = area_aoi->link_info.anchor_size;
	int link_num = area_aoi->link_info.num;
	//if(cont_size== 1)
	if(cont_size<= 10){
		return 0;
	}

	if(cont_size >= page_text_len / 2){
		if(pg_height >= 600 && area->area_info.ypos >= 7*pg_height/10){
			//����Ϊҳ��ײ��İ�Ȩ��Ϣ
			;
		}
		else{
			//����ռ����ҳ��ı����ϴ󣬿���Ϊ��Ҫ���݁9�61�1�77
			return 1;
		}
	}

	if(has_proper_tag_len(area,pg_width,pg_height,page_text_len)){
		if(cont_size >= 3*anchor_size && link_num <= 8){
			return 1;
		}
		if(link_num <= 2){
			return 1;
		}
	}
	if((area->area_info.ypos <= pg_height/4 || pg_height <= 160) && cont_size >= 5 * anchor_size){
			return 1;
		}
//	int doctype=area->area_tree->hp_vtree->hpTree->doctype;//����doctye����ֹhtml��ҳ����ɢ�ַ������� http://news.qq.com/a/20120507/000509.htm��΢���Ƽ� ��һ��
//	if((area->area_info.ypos <= pg_height/4 || pg_height <= 160) && cont_size >= 5 * anchor_size&&!(doctype>=doctype_xhtml_BP&&doctype<=doctype_html5)){
//		return 1;
//	}
	return 0;
}

/**
 * @brief ��ҳ��������ݿ���ȡ��Ҫ���݁9�61�1�77
**/
int visit_for_central_area(html_area_t *area, void *data)
{
	if(!area->isValid){
		return AREA_VISIT_SKIP;
	}

	vcontent_data_t *cont_data = (vcontent_data_t *)data;

	if(is_in_func_area(area,AREA_FUNC_NAV)
			|| is_in_func_area(area,AREA_FUNC_FRIEND)
			|| is_in_func_area(area,AREA_FUNC_RELATE_LINK)
			|| is_in_srctype_area(area,AREA_SRCTYPE_INTERACTION)
			|| (is_in_func_area(area,AREA_FUNC_COPYRIGHT) && area->pos_plus == IN_PAGE_FOOTER)){
		return AREA_VISIT_SKIP;
	}
	if(area->valid_subArea_num == 0){
		extract_area_content(area,cont_data);
	}
	return AREA_VISIT_NORMAL;
}

/**
 * ��ǰ��ҳ������
 */
static int visit_for_main_cont_area(html_area_t *area, void *data)
{
	if(!area->isValid){
		return AREA_VISIT_SKIP;
	}

	vcontent_data_t *cont_data = (vcontent_data_t *)data;
	if(area->depth == 1){
		cont_data->prev_vnode = NULL;
		area_add_seperator(area,cont_data);
	}
	if(MARK_CENTRAL==g_EASOU_DEBUG){
						printline();
						printNode(area->begin->hpNode);
	}
	/**��Щ���Ǳ߿����ɳ���1�1�77 */
	if(is_in_func_area(area,AREA_FUNC_NAV) || is_func_area(area,AREA_FUNC_NAV)
			|| is_in_func_area(area,AREA_FUNC_RELATE_LINK) || is_func_area(area,AREA_FUNC_RELATE_LINK)
			|| is_in_func_area(area,AREA_FUNC_FRIEND) || is_func_area(area,AREA_FUNC_FRIEND)
			|| is_right_copyright(area, cont_data->area_tree) || is_in_srctype_area(area,AREA_SRCTYPE_INTERACTION)){
		marktreeprintfs(MARK_CENTRAL,"get main content ,skip the area for it is func area or interaction or copyright at %s(%d)-%s\r\n",__FILE__,__LINE__,__FUNCTION__);
		return AREA_VISIT_SKIP;
	}

	/**MYPOS,������Դ,realtitle����Ҫ������Ҫ������ */
	if(is_in_func_area(area,AREA_FUNC_MYPOS) || is_func_area(area,AREA_FUNC_MYPOS)
			|| is_in_func_area(area, AREA_FUNC_ARTICLE_SIGN) || is_func_area(area, AREA_FUNC_ARTICLE_SIGN)
			|| is_in_sem_area(area, AREA_SEM_REALTITLE) || is_sem_area(area, AREA_SEM_REALTITLE)){
		marktreeprintfs(MARK_CENTRAL,"the area is in (mypos or article sign or realtitle),skip it at %s(%d)-%s\r\n",__FILE__,__LINE__,__FUNCTION__);
		return AREA_VISIT_SKIP;
	}
	if(is_pagesider(area)){
		marktreeprintfs(MARK_CENTRAL,"the area is at side,skip it at %s(%d)-%s\r\n",__FILE__,__LINE__,__FUNCTION__);
		return AREA_VISIT_SKIP;
	}

	//�԰�Ȩ�����λ������ shuangwei add 20120510
	if(is_in_sem_area(area,AREA_SEM_CENTRAL) || is_in_func_area(area, AREA_FUNC_COPYRIGHT)&&area->area_info.ypos*10<area->area_tree->root->area_info.height*8){ /*�޲���Ȩ��ʶ�����1�71�1�77�1�7�ɵ�����badcase*/
			/** �Ժ������ݿ� */

		marktreeprintfs(MARK_CENTRAL,"the area is fit at extracting ,is_main_cont_area() at %s(%d)-%s\r\n",__FILE__,__LINE__,__FUNCTION__);
			areatree_visit(area, visit_for_central_area, NULL, cont_data);
			return AREA_VISIT_SKIP;
		}
	/*
	if(is_main_cont_area(area, cont_data->area_tree->root)){

		marktreeprintfs(MARK_CENTRAL,"the area is fit at extracting ,is_main_cont_area() at %s(%d)-%s\r\n",__FILE__,__LINE__,__FUNCTION__);
		extract_area_content(area, cont_data);
		return AREA_VISIT_SKIP;
	}
	*/
	return AREA_VISIT_NORMAL;
}

/**
 * @brief ��ȡ��������
 */
static int visit_for_article_cont_area(html_area_t *area, void *data)
{
	if (!area->isValid)
	{
		return AREA_VISIT_SKIP;
	}
	vcontent_data_t *cont_data = (vcontent_data_t *) data;
	if (area->depth == 1)
	{
		cont_data->prev_vnode = NULL;
		area_add_seperator(area, cont_data);
	}
	if (area->valid_subArea_num == 0 && (is_func_area(area, AREA_FUNC_ARTICLE_CONTENT) || is_in_func_area(area, AREA_FUNC_ARTICLE_CONTENT)))
	{
		extract_area_content(area, cont_data);
	}
	return AREA_VISIT_NORMAL;
}

static int visit_for_link_text_area(html_area_t *area, void *data)
{
	if(!area->isValid){
		return AREA_VISIT_SKIP;
	}

	vcontent_data_t *vcont_data = (vcontent_data_t *)data;

	int page_height = vcont_data->area_tree->root->area_info.height;

	if(is_func_area(area, AREA_FUNC_NAV)){
		return AREA_VISIT_SKIP;
	}

	if(area->area_info.ypos >= page_height * 2 / 3){
		return AREA_VISIT_SKIP;
	}

	if(is_srctype_area(area, AREA_SRCTYPE_LINK)){
		vcont_data->main_anchor_size += area->baseinfo->text_info.con_size;
		vcont_data->main_con_size += area->baseinfo->text_info.con_size;

		return AREA_VISIT_SKIP;
	}
	else if(is_srctype_area(area, AREA_SRCTYPE_TEXT)){
		vcont_data->main_con_size += area->baseinfo->text_info.con_size;

		return AREA_VISIT_SKIP;
	}

	return AREA_VISIT_NORMAL;
}

/**
 * @brief ҳ���Ƿ���Ҫ��������Ʉ1�71�1�77�1�71�1�77
**/
static int is_mainly_link(html_area_t *root, const char *url, vcontent_data_t *vcont_data)
{
	int index_ratio = 0;
	if (url != NULL && is_dir_url (url)){
		vcont_data->is_dir_url = 1;
		index_ratio = 75;
	}else{
		vcont_data->is_dir_url = 0;
		index_ratio = 90;
	}
	vcont_data->main_con_size = 0;
	vcont_data->main_anchor_size = 0;
	if(root->valid_subArea_num == 0){
		areatree_visit(root, visit_for_link_text_area, NULL, vcont_data);
	}
	else{
		for(html_area_t *subarea = root->subArea; subarea; subarea = subarea->nextArea){
			if(subarea->isValid && subarea->pos_mark == RELA_MAIN){
				areatree_visit(subarea, visit_for_link_text_area, NULL, vcont_data);
			}
		}
	}

	int cont_size = vcont_data->main_con_size;
	int anchor_size = vcont_data->main_anchor_size;

	if (100*anchor_size > index_ratio*cont_size) {
		return 1;
	} else{
		return 0;
	}
}

int html_atree_extract_main_content(area_tree_t *area_tree, char *content, int size, const char *url)
{
	vcontent_data_t cont_data;
	cont_data.area_tree = area_tree;
	cont_data.content = content;
	cont_data.available = 0;
	cont_data.end = size - 1;
	cont_data.prev_cont_len = 0;
	cont_data.prev_vnode = NULL;
	cont_data.flag = EXTRACT_BREAK;

	html_area_t *root = area_tree->root;

	if(url == NULL || is_mainly_link(root, url, &cont_data)){
		areatree_visit(root, visit_mainly_link_page, NULL, &cont_data);
	}
	else{
		areatree_visit(root, visit_for_main_cont_area, NULL, &cont_data);
	}
	content[cont_data.available] = '\0';
	return cont_data.available;
}

int html_atree_extract_article_content(area_tree_t *area_tree, char *content, int size)
{
	vcontent_data_t cont_data;
	cont_data.area_tree = area_tree;
	cont_data.content = content;
	cont_data.available = 0;
	cont_data.end = size - 1;
	cont_data.prev_cont_len = 0;
	cont_data.prev_vnode = NULL;
	cont_data.flag = EXTRACT_BREAK;

	html_area_t *root = area_tree->root;

	areatree_visit(root, visit_for_article_cont_area, NULL, &cont_data);
	content[cont_data.available] = '\0';
	return cont_data.available;
}

void printAreaTitle(html_area_t * area,int level){
	if(area&&area->isValid){
		myprintf("***********************************\n");
		myprintf("the area id=%d,depth=%d\n",area->no,area->depth);
		char content[512000]={0};
		vcontent_data_t cont_data;
			cont_data.area_tree = NULL;
			cont_data.content = content;
			cont_data.available = 0;
			cont_data.end = 512000 - 1;
			cont_data.prev_cont_len = 0;
			cont_data.prev_vnode = NULL;
			cont_data.flag = EXTRACT_NOBREAK;
		extract_area_content(area, &cont_data);
		myprintf("area content:%s\n",content);
		if(area->titleArea){
			myprintf("the title area id of the area =%d\n",area->titleArea->no);
			memset(content,0,sizeof(content));
			cont_data.area_tree = NULL;
						cont_data.content = content;
						cont_data.available = 0;
						cont_data.end = 512000 - 1;
						cont_data.prev_cont_len = 0;
						cont_data.prev_vnode = NULL;
						cont_data.flag = EXTRACT_NOBREAK;
			extract_area_content(area->titleArea, &cont_data);
			myprintf("area title:%s\n",content);
		}
		else{
			myprintf("area title is null\n");
		}


		for(html_area_t * subarea=area->subArea;subarea;subarea=subarea->nextArea){
			printAreaTitle( subarea,level+1);
		}

	}
}

//void printAreaWithTitle(html_area_t * area,int level,char *buf,int bufsize,int& available){
//	if(!buf||(bufsize-available)<1000){
//				return;
//	}
//	if(area&&area->isValid){
//
//
//		if(area->titleArea){
//			int tmpsize=0;
//			char *pbuf=buf;
//			tmpsize=sprintf(pbuf+available,"***********************************\n");
//			available+=tmpsize;
//			if((bufsize-available)<1000){
//				return;
//			}
//			tmpsize=sprintf(pbuf+available,"the area id=%d,depth=%d\n",area->no,area->depth);
//			available+=tmpsize;
//			if((bufsize-available)<1000){
//					return;
//			}
//			char content[512000]={0};
//			vcontent_data_t cont_data;
//				cont_data.area_tree = NULL;
//				cont_data.content = content;
//				cont_data.available = 0;
//				cont_data.end = 512000 - 1;
//				cont_data.prev_cont_len = 0;
//				cont_data.prev_vnode = NULL;
//				cont_data.flag = EXTRACT_NOBREAK;
//			extract_area_content(area, &cont_data);
//			if((bufsize-available)<cont_data.available){
//				return;
//			}
//			tmpsize=sprintf(pbuf+available,"area content:%s\n",content);
//			available+=tmpsize;
//			if((bufsize-available)<1000){
//				return;
//			}
//			tmpsize=sprintf(pbuf+available,"the title area id of the area =%d\n",area->titleArea->no);
//			available+=tmpsize;
//			if((bufsize-available)<1000){
//						return;
//			}
//			memset(content,0,sizeof(content));
//			cont_data.area_tree = NULL;
//						cont_data.content = content;
//						cont_data.available = 0;
//						cont_data.end = 512000 - 1;
//						cont_data.prev_cont_len = 0;
//						cont_data.prev_vnode = NULL;
//						cont_data.flag = EXTRACT_NOBREAK;
//			extract_area_content(area->titleArea, &cont_data);
//			if((bufsize-available)<cont_data.available){
//				return;
//			}
//
//			tmpsize=sprintf(pbuf+available,"area title:%s\n",content);
//			available+=tmpsize;
//			if((bufsize-available)<1000){
//				return;
//			}
//		}
//
//
//
//		for(html_area_t * subarea=area->subArea;subarea;subarea=subarea->nextArea){
//			printAreaWithTitle( subarea,level+1,buf, bufsize,available);
//		}
//
//	}
//}

static int printAreaWithTitle(html_area_t *area, void *data)
{
	if(!area->isValid){
		return AREA_VISIT_SKIP;
	}

	vcontent_data_t *cont_data = (vcontent_data_t *)data;
	if(area&&area->isValid){


		if(area->titleArea){
			int tmpsize=0;
			char *pbuf=cont_data->content;
			tmpsize=sprintf(pbuf+cont_data->available,"\n***********************************\n");
			cont_data->available+=tmpsize;
			if((cont_data->end-cont_data->available)<1000){
				return AREA_VISIT_FINISH;
			}
			tmpsize=sprintf(pbuf+cont_data->available,"the area id=%d,depth=%d\n",area->no,area->depth);
			cont_data->available+=tmpsize;
			if((cont_data->end-cont_data->available)<1000){
					return AREA_VISIT_FINISH;
			}
			tmpsize=sprintf(pbuf+cont_data->available,"area content:\n");
			cont_data->available+=tmpsize;
			if((cont_data->end-cont_data->available)<1000){
				return AREA_VISIT_FINISH;
			}
			extract_area_content(area, cont_data);
			if((cont_data->end-cont_data->available)<1000){
				return AREA_VISIT_FINISH;
			}
			tmpsize=sprintf(pbuf+cont_data->available,"\nthe title area id of the area =%d\n",area->titleArea->no);
			cont_data->available+=tmpsize;
			if((cont_data->end-cont_data->available)<1000){
				return AREA_VISIT_FINISH;
			}
			tmpsize=sprintf(pbuf+cont_data->available,"area title:\n");
			cont_data->available+=tmpsize;
			if((cont_data->end-cont_data->available)<1000){
					return AREA_VISIT_FINISH;
			}
			extract_area_content(area->titleArea, cont_data);

		}


	}
	return AREA_VISIT_NORMAL;

}
void printAtreeTitle(area_tree_t * atree){
	if(atree){
		printAreaTitle(atree->root,0);
	}

}

//void printAtreeWithTitle(area_tree_t * atree,char *buf,int bufsize){
//	if(atree){
//		int available=0;
//		buf[bufsize-1]=0;
//		//available=sprintf(buf+available,"<html><head><title>content</title></head><body><div>\n<textarea wrap=\"wrap\" rows=\"30\" style=\"width:100%; word-break: break-all; word-wrap:break-word; border:1px solid #555; white-space:-moz-pre-wrap;\" readonly=\"readonly\">\n");
//
//		printAreaWithTitle(atree->root,0,buf, bufsize-1,available);
//		//available=sprintf(buf+available,"</textarea></div></body></html>\n");
//	}
//
//}
void printAtreeWithTitle(area_tree_t * atree,char *buf,int bufsize){
	if(atree){
		int available=0;
		buf[bufsize-1]=0;
		vcontent_data_t cont_data;
		available=sprintf(buf+available,"<html><head><title>area title</title></head><body><div>\n<textarea wrap=\"wrap\" rows=\"30\" style=\"width:100%; word-break: break-all; word-wrap:break-word; border:1px solid #555; white-space:-moz-pre-wrap;\" readonly=\"readonly\">\n");

		cont_data.area_tree = atree;
		cont_data.content = buf;
		cont_data.available = available;
		cont_data.end = bufsize - 1;
		cont_data.prev_cont_len = 0;
		cont_data.prev_vnode = NULL;
		cont_data.flag = EXTRACT_NOBREAK;

		html_area_t *root = atree->root;

		areatree_visit(root, printAreaWithTitle, NULL, &cont_data);

		//content[cont_data.available] = '\0';
		available=sprintf(buf+cont_data.available,"\n</textarea></div></body></html>\n");
	}

}
/*
//���Ӵ���Ϊ�ж�ҳ���Ƿ�����ҳ

static int getUrlPath(const char *pcUrl)
{
    char *p = (char *)pcUrl;
    int iDepth = 0;
   	if (strncasecmp (pcUrl, "http://", 7) == 0)
        p += 7;
    else
    {
        return PAGETYPE_URL_DEPTH;
    }
   	char *lastslap=NULL;
    for(; *p != '\0'; p++)
    {
        if(*p == '/')
        {
            iDepth++;
            lastslap=p;
        }
    }


    return iDepth;
}

static bool isInCenter(html_area_t *area){
	bool isCenter=true;
	int page_height=area->area_tree->root->area_info.height;
	int page_width=area->area_tree->root->area_info.width;
		int areax=area->area_info.xpos;
		int areay=area->area_info.ypos;
		int areaheigth=area->area_info.height;
		int areawidth=area->area_info.width;
	if(areax*3>page_width*2){
		isCenter=false;
	}
	int acenterx=areax+areawidth/2;
	int acentery=areay+areaheigth/2;
	if(acenterx*6>page_width*5||acenterx*6<page_width*1&&area->nextArea&&area->nextArea->area_info.xpos==areax){
		isCenter=false;
	}
	if(acentery*100>page_height*90||acentery*100<page_height*15){
		isCenter=false;
	}
	marktreeprintfs(MARK_HUB_PAGE,"area id=%d is %s\n",area->no,isCenter ? "center":" not center");
	return isCenter;
}
static int pagetype_visit_for_link_text_area(html_area_t *area, void *data)
{
	if(!area->isValid)
    {
		return AREA_VISIT_SKIP;
	}
//	if(area->valid_subArea_num>0&&is_srctype_area(area, AREA_SRCTYPE_LINK)&&is_contain_srctype_area(area, AREA_SRCTYPE_TEXT)){
//		return AREA_VISIT_NORMAL;
//	}
//	if(area->valid_subArea_num>0&&is_srctype_area(area, AREA_SRCTYPE_TEXT)&&is_contain_srctype_area(area, AREA_SRCTYPE_LINK)){
//			return AREA_VISIT_NORMAL;
//		}

//	if(area->valid_subArea_num>0){
//			return AREA_VISIT_NORMAL;
//	}
	// printf("process area111 no=%d\n",area->no);
	_vhub_data_t *vcont_data = (_vhub_data_t *)data;

	int page_height = vcont_data->area_tree->root->area_info.height;

	if(is_func_area(area, AREA_FUNC_NAV)||is_func_area(area, AREA_FUNC_COPYRIGHT)||is_func_area(area, AREA_FUNC_FRIEND)||is_in_func_area(area,AREA_FUNC_RELATE_LINK)&&(area->baseinfo->text_info.con_size-area->baseinfo->link_info.anchor_size)<100)
    {
		return AREA_VISIT_SKIP;
	}
    if(area->baseinfo->inter_info.is_have_form){
    	return AREA_VISIT_NORMAL;
    }
	if(area->area_info.ypos*5 >= page_height * 4 )
    {
		return AREA_VISIT_SKIP;
	}

	// printf("process area no=%d\n",area->no);
	if(is_srctype_area(area, AREA_SRCTYPE_LINK)){
		vcont_data->main_anchor_size += area->baseinfo->text_info.con_size;
		//vcont_data->main_con_size += area->baseinfo->text_info.con_size;
		// printf("process area no=%d,main_anchor_size=%d\n",area->no,vcont_data->main_anchor_size);
		if(isInCenter(area)){
			if(area->baseinfo->link_info.anchor_size>vcont_data->max_link_size){
				vcont_data->max_link_size=area->baseinfo->link_info.anchor_size;
			}
		}
		return AREA_VISIT_SKIP;
	}
	else if(is_srctype_area(area, AREA_SRCTYPE_TEXT)||area->valid_subArea_num<1&&(area->baseinfo->text_info.con_size>20&&area->baseinfo->text_info.con_size>area->baseinfo->link_info.anchor_size*3||area->baseinfo->pic_info.pic_area>20000||area->area_info.width*area->area_info.height>20000)){

		vcont_data->main_con_size += area->baseinfo->text_info.con_size;
		 if((area->baseinfo->text_info.con_size>=area->baseinfo->link_info.anchor_size*2)&&isInCenter(area)){
			 vcont_data->total_txt_size+=area->baseinfo->text_info.con_size;
		      if(vcont_data->max_txt_size<area->baseinfo->text_info.con_size){
		    	  vcont_data->max_txt_size=area->baseinfo->text_info.con_size;
		      }

		  }
		 if(area->baseinfo->pic_info.pic_area>vcont_data->max_pic_area){
			 vcont_data->max_pic_area=area->baseinfo->pic_info.pic_area;

		 }
		 int areasize=area->area_info.width*area->area_info.height;
		 if(areasize>20000&&areasize>vcont_data->max_pic_area&&area->valid_subArea_num<1&&area->baseinfo->text_info.con_size<10&&isInCenter(area)){

			 vcont_data->max_pic_area=areasize;

				 }
		 //printf("process area no=%d,main_con_size=%d\n",area->no,vcont_data->main_con_size);
		return AREA_VISIT_SKIP;
	}


	return AREA_VISIT_NORMAL;
}

static int getStatic(html_area_t *root, const char *url, _vhub_data_t *vcont_data)
{
	int index_ratio = 0;
	vcont_data->main_con_size = 0;
	vcont_data->main_anchor_size = 0;

	areatree_visit(root, pagetype_visit_for_link_text_area, NULL, vcont_data);


    return 0;
}

static bool isContentPage(const char *pcUrl,int &lastsegcount){
	 char *p = (char *)pcUrl;
	   	if (strncasecmp (pcUrl, "http://", 7) == 0)
	        p += 7;
	    else
	    {
	    	//printf("not begin http\n");
	        return true;
	    }
	   	int iDepth=0;
	   	char *lastslap=NULL;
	   	int firstseglen=0;
	   	int firstdigtalcount=0;
	   	char *firstseg=NULL;

		    for(; *p != '\0'; p++)
		    {
		        if(*p == '/')
		        {
		            //iDepth++;
		            lastslap=p;
		        }
		    }
		    if(lastslap&&*lastslap=='/'){
		    	lastslap++;
		    }
		    char *ptemp=lastslap;
		    bool isthread=true;
		    if(ptemp&&strncasecmp (ptemp, "thread", 6) == 0){
		    	ptemp=ptemp+6;
		    	while(ptemp&&(*ptemp!='\0')&&(*ptemp!='.')){

		    		    	if((*ptemp>='0')&&(*ptemp)<='9'||(*ptemp)=='-'){

		    		    	}
		    		    	else{
		    		    		isthread=false;
		    		    		break;
		    		    	}

		    		    	ptemp++;
		    	}
		    	if(isthread&&ptemp&&strcasecmp (ptemp, ".html") == 0){
		    		return true;
		    	}
		    }
	    bool isdigit=false;
	    int digtalcount=0;
	    if(!lastslap||*lastslap==0){
	    	isdigit=false;
	    	return false;
	    }

	    marktreeprintfs(MARK_HUB_PAGE,"lastslap %s\n",lastslap?:lastslap,"is null");
       // int lastsegcount=0;
	    char *point=strstr(lastslap,".");
	    if(point){
	    	lastsegcount=point-lastslap;
	    }
	    else{
	    	point=strstr(lastslap,"?");
	    	 if(point){
	    		    	lastsegcount=point-lastslap;
	    	}
	    	 else{
	    		 point=strstr(lastslap,"=");
	    		  if(point){
	    		 	    		    	lastsegcount=point-lastslap;
	    		 }
	    		  else{
	    		     lastsegcount=strlen(lastslap);
	    		  }
	    	 }

	    }
	    while(lastslap&&*lastslap!='\0'){
            if(*lastslap=='.'||*lastslap=='?'){
            	isdigit=true;
            	break;
            }
	    	if(!((*lastslap>='0')&&(*lastslap)<='9'||(*lastslap>='a')&&(*lastslap)<='f'||(*lastslap)=='-')){

	    		if(digtalcount>2){
	    			break;
	    		}
	    		//break;

	    	}
	    	else{
	    		if((*lastslap>='0')&&(*lastslap)<='9'||(*lastslap>='a')&&(*lastslap)<='f'){

	    			digtalcount++;
	    		}

	    	}

	    	lastslap++;
	    }
        if(lastslap&&*lastslap=='\0'){
        	isdigit=true;
        }
	    if(digtalcount>7&&isdigit){
	    	//printf("not for isdigit=%d||digtalcount(%d)>5\n",isdigit,digtalcount);
	    	return true;;
	    }

	    return false;
}

int checkContent(const char *pcContent,int iContentLen)
{
    char acHead[2048] = {0 };
    int iNum = 0;
    char *pTmp = NULL;
    int iLen = iContentLen < 2048? iContentLen:2048;
    memcpy(acHead,pcContent,iLen);
    for(int i=0; i<CONTENT_INFO_SZE; i++)
    {
       pTmp = strstr(acHead,ContentInfo[i]);
        if(pTmp != NULL)
        {
        	marktreeprintfs(MARK_HUB_PAGE,"content contain %s\n",ContentInfo[i]);
            iNum++;
        }
    }
    return iNum;
}
bool isMyHomePage(const char* url) {
        const char *begin = NULL;
        const char *slashpos = NULL;

        assert(url != NULL);
        begin = url;
        if (strncmp(begin, "http://", 7) == 0) {
            begin += 7;
        }
        // some  not invaild url is not suited
        if ((slashpos = strchr(begin, '/')) == NULL) {
            return true;
        }
        else {
            if ((*(slashpos + 1)) == 0) {
                return true;
            }
            else if (strncmp(slashpos + 1, "index", 5) == 0
                    || strncmp(slashpos + 1, "main", 4) == 0
                    || strncmp(slashpos + 1, "default", 7) == 0) {

                if (strchr(slashpos + 1, '?') == NULL
                        && strchr(slashpos + 1, '/') == NULL)
                    return true;
            }
        }
        return false;
    }
int isHubType(const char *pcUrl,area_tree_t * atree,const link_t* pLink,int iLinkNum,
                         const char *pcContent,int iContentLen,
                         const mypos_t *pMypos,const char *pcTitle,int iTitlenLen)
{
    char *pTmp = NULL;
    int iRealLinkNum = 0;
    int iRet = 0;
    _vhub_data_t stVcontent;
    int i = 0;
    int iDepth =0;
    int iHaveContent = 0;
    int iScore = 0;
    stVcontent.area_tree=atree;
    char urldomain[1024]={0};
    char mydomain[1024]={0};
    fetch_maindomain_from_url(pcUrl,mydomain,1024);
    memset(&stVcontent,0,sizeof(_vhub_data_t));
    stVcontent.area_tree=atree;
   bool ishome= (isMyHomePage(pcUrl));
   int lastsegcount=0;
 bool isdir= is_dir_url(pcUrl);
   getStatic(atree->root,pcUrl,&stVcontent);
   if(stVcontent.max_pic_area>=60000&&!ishome){

	   if(stVcontent.max_txt_size<300){
		   stVcontent.max_txt_size=300 ;
	   }
	   stVcontent.total_txt_size+=300;
	   marktreeprintfs(MARK_HUB_PAGE,"modify 300\n");

   }
   else{
	   if(stVcontent.max_pic_area>20000){
	 	   if(stVcontent.max_txt_size<100){
	 		   stVcontent.max_txt_size=100 ;
	 	   }
	 	   stVcontent.total_txt_size+=100;
	 	  marktreeprintfs(MARK_HUB_PAGE,"modify 100\n");

	    }
   }
    // printf("not hub for max_txt_size=%d\n",stVcontent.max_txt_size);
     if(stVcontent.main_anchor_size < 40)
   {
    	 marktreeprintfs(MARK_HUB_PAGE,"not hub for anchor size(%d)<40\n",stVcontent.main_anchor_size);
           return 0;
   }
     if(stVcontent.max_txt_size>300){
    	 marktreeprintfs(MARK_HUB_PAGE,"not hub for max_txt_size>300\n");
          	 return 0;
          }
     if(stVcontent.total_txt_size>450&&!ishome){
     	 marktreeprintfs(MARK_HUB_PAGE,"not hub for total_txt_size>450\n");
           	 return 0;
           }
   if(isContentPage(pcUrl,lastsegcount)){
	   marktreeprintfs(MARK_HUB_PAGE,"not hub for contentpage\n");
	   return 0;
   }
   marktreeprintfs(MARK_HUB_PAGE,"lastsegcount=%d,isdir=%d,max_txt_size=%d\n",lastsegcount,isdir,stVcontent.max_txt_size);

    // find content_strint in breadcrum,if find ,the is not hub type
    for(i =0; i<pMypos->item_num; i++)
    {
        for(int j=0; j< MY_POS_SIZE;j++)
        {
            pTmp = strstr((char *)(pMypos->items[i].text), MyPos[j]);

            if(pTmp != NULL)
            {
            	marktreeprintfs(MARK_HUB_PAGE,"not hub for mypos\n");
                return 0;
            }

        }
        if(i>4&&strlen(pMypos->items[i].url)<1&&i==pMypos->item_num-1&&!isdir){
        	marktreeprintfs(MARK_HUB_PAGE,"not hub for mypos depth >3 and last item is not link\n");
        	return 0;
        }
    }
    if(stVcontent.max_link_size>200&&ishome&&stVcontent.main_con_size<200){
       	  marktreeprintfs(MARK_HUB_PAGE,"stVcontent.max_link_size(%d)>200&&stVcontent.main_con_size(%d)<200\n",stVcontent.max_link_size,stVcontent.main_con_size);
       	  return 1;
         }
    iDepth = getUrlPath(pcUrl);
    if(iDepth > PAGETYPE_URL_DEPTH&&!isdir)
    {
    	marktreeprintfs(MARK_HUB_PAGE,"not hub for path depth\n");
        return 0;
    }


    // url info have content,detail,....
    for(i=0; i<URL_INFO_SIZE&&pcUrl; i++)
    {
        pTmp = strstr((char*)pcUrl,UrlInfo[i]);
        if(pTmp != NULL)
        {
            iHaveContent = 1;
            break;
        }
    }
    if(iHaveContent )
    {
    	marktreeprintfs(MARK_HUB_PAGE,"not hub for url have content\n");
    	 return 0;
    }




     ////    // get the link num
     int link_txt_size=0;
             for(i=0; i<iLinkNum; i++)
             {
            	 //memset(urldomain,0,1024);
            	 //fetch_maindomain_from_url(pLink[i].url,urldomain,1024);
                 if(strlen(pLink[i].url) != 0 && (strlen(pLink[i].text) >20))
                 {
                     iRealLinkNum++;
                     //link_txt_size+=strlen(pLink[i].text);
                 }
             }

             if(iRealLinkNum*100>iLinkNum*70&&iLinkNum>0)//
             {
            	 marktreeprintfs(MARK_HUB_PAGE,"is hub for iRealLinkNum*100<iLinkNum*10&&iLinkNum>0,iRealLinkNum=%d,linknum=%d\n",iRealLinkNum,iLinkNum);
                 return 0;
             }



      if(stVcontent.max_txt_size>220&&isdir){
    	  marktreeprintfs(MARK_HUB_PAGE,"stVcontent.max_txt_size>220&&isdir\n");
    	  return 0;
      }

      if((lastsegcount>14||!isdir)&&(stVcontent.max_txt_size>250||stVcontent.main_con_size>350)){
    	  marktreeprintfs(MARK_HUB_PAGE,"(lastsegcount>14||!isdir)&&stVcontent.max_txt_size>250\n");
         	  return 0;
           }
      if(stVcontent.max_link_size<100&&stVcontent.main_anchor_size<300){
    	  marktreeprintfs(MARK_HUB_PAGE,"stVcontent.max_link_size(%d)<100&&stVcontent.main_anchor_size(%d)e<300\n",stVcontent.max_link_size,stVcontent.main_anchor_size);
    	  return 0;
      }
      if(lastsegcount>30&&iTitlenLen>100){
    	  return 0;
      }
      if(stVcontent.max_txt_size<100&&ishome&&stVcontent.main_anchor_size>250){
    	  marktreeprintfs(MARK_HUB_PAGE,"stVcontent.max_txt_size<100&&ishome&&stVcontent.main_anchor_size>150\n");
    	  return 1;
      }


//      if(stVcontent.total_txt_size>400){
//          	  marktreeprintfs(MARK_HUB_PAGE,"stVcontent.total_txt_size>400\n");
//          	  return 0;
//            }

    if(stVcontent.main_con_size >= stVcontent.main_anchor_size*6)
    {
    	marktreeprintfs(MARK_HUB_PAGE,"not hub for stVcontent.main_con_size >= stVcontent.main_anchor_size*3\n");
        return 0;
    }
    iRet = checkContent(pcContent,iContentLen);
        if(iRet >= 2)
        {
       	 marktreeprintfs(MARK_HUB_PAGE,"not hub for check content\n");
            return 0;
        }
        if(iRet >= 1&&stVcontent.max_txt_size>200)
           {
          	 marktreeprintfs(MARK_HUB_PAGE,"not hub for check content,stVcontent.max_txt_size>200\n");
               return 0;
           }

    return 1;


}

//add over
*/

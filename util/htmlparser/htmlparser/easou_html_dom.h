/*
 * easou_html_dom.h
 *
 *  Created on: 2011-11-8
 *      Author: ddt
 */

#ifndef EASOU_HTML_DOM_H_
#define EASOU_HTML_DOM_H_

#include <stddef.h>
#include "queue.h"
#include "easou_html_pool.h"
#include "easou_html_dtd.h"

#define VISIT_ERROR -1
#define VISIT_NORMAL 1
#define VISIT_FINISH 2
#define VISIT_SKIP_CHILD 3

#define	MAX_TITLE_SIZE         1024            //title����󳤶�
#define	MAX_PAGE_SIZE          128000          //ҳ�����󳤶�
#define	MAX_CONTENT_SIZE       128000          //content����󳤶�
#define MAX_LINK_NUM           300             //ҳ����link���������
#define MAX_URL_SIZE           1024            //URL����󳤶�
#define UL_MAX_URL_LEN 2048
#define UL_MAX_TEXT_LEN 1024
#define UL_MAX_PAGE_LEN 128*1024
#define MAX_ANNOTATION_LEN (64 * 1024)
#define MAX_START_TAG_NAME_LEN 16  /* ��ʼ��ǩ����󳤶� */
#define MAX_END_TAG_NAME_LEN 32    /* ������ǩ����󳤶� */

#define POOLSZ 128*1024

#define MARK_DOMTREE_SUBTYPE(x)	((x)|=(0x1<<31))
#define IS_DOMTREE_SUBTYPE(x)	((x)&(0x1<<31))
#define MARK_DOMTREE_P_TAG(x)	((x)|=(0x1))
#define MARK_DOMTREE_DIV_TAG(x)	((x)|=(0x1<<1))
#define MARK_DOMTREE_H_TAG(x)	((x)|=(0x1<<2))
#define MARK_DOMTREE_TABLE_TAG(x)	((x)|=(0x1<<3))
#define MARK_DOMTREE_LIST_TAG(x)	((x)|=(0x1<<4))
#define MARK_DOMTREE_FORM_TAG(x)	((x)|=(0x1<<5))
#define MARK_DOMTREE_IMG_TAG(x)	((x)|=(0x1<<6))
#define IS_DOMTREE_P_TAG(x)	((x)&(0x1))
#define IS_DOMTREE_DIV_TAG(x)	((x)&(0x1<<1))
#define IS_DOMTREE_H_TAG(x)	((x)&(0x1<<2))
#define IS_DOMTREE_TABLE_TAG(x)	((x)&(0x1<<3))
#define IS_DOMTREE_LIST_TAG(x)	((x)&(0x1<<4))
#define IS_DOMTREE_FORM_TAG(x)	((x)&(0x1<<5))
#define IS_DOMTREE_IMG_TAG(x)	((x)&(0x1<<6))
/**
 * �ĵ����Ͷ���
 */
enum html_doctype
{
	doctype_unknown = 0, doctype_wml = 1, // wml/wap1.0
	doctype_xhtml_MP = 2, // xhtml moible/wap2.0
	doctype_xhtml_BP = 3, //xhtml basic/web
	doctype_xhtml = 4, // xhtml/web
	doctype_html4 = 5, // html4.1/web
	doctype_html5 = 6,
// html5/web
};

/**
 * @brief Node type enum
 **/
enum dom_node_type_t
{
	ELEMENT_NODE = 1,
	ATTRIBUTE_NODE,
	TEXT_NODE,
	CDATA_SECTION_NODE,
	ENTITY_REFERENCE_NODE,
	ENTITY_NODE,
	PROCESSING_INSTRUCTION_NODE,
	COMMENT_NODE,
	DOCUMENT_NODE,
	DOCUMENT_TYPE_NODE,
	DOCUMENT_FRAGMENT_NODE,
	NOTATION_NODE
};

struct _html_tree_t;

/**
 * @brief HTML attribute node
 **/
typedef struct _html_attribute_t
{
	html_attr_type_t type; /**< �������� */
	char *value; /**< ����ֵ */
	int valuelength; //����ֵ����
	const char *name; /**< �������� */
	struct _html_attribute_t *next; /**< ��һ������ */
} html_attribute_t;

typedef struct _html_node_t;

/**
 * @brief HTML��ǩ�ĳ���
 **/
typedef struct _html_tag_t
{
	_html_node_t* html_node;
	const char *tag_name; /* ��ǩ���� */
	char *text; /* ��ǩ��Դ�����еı�ʾ */
	int textlength; //�ڵ��text����
	html_attribute_t *attribute; /* �����б� */
	html_attribute_t *attr_class;
	html_attribute_t *attr_id;
	int tag_code; /* ��ǩ���� */
	int page_offset; /* ��ǩ��ҳ���е�ƫ����,����script���ɵ�TAG,ƫ����Ϊ-1 */
	int nodelength; //�ڵ�Դ�볤��
	html_tag_type_t tag_type :16; /* ��ǩ���� */
	unsigned is_close_tag :1; /* �Ƿ��ǽ�����ǩ */
	unsigned is_self_closed :1; /* �Ƿ����Խ�����ǩ */
} html_tag_t;

/**
 * @brief HTML�ڵ�ĳ���
 **/
typedef struct _html_node_t
{
	html_tag_t html_tag;
	struct _html_tree_t *owner; /* ӵ���� */
	struct _html_node_t *parent; /* ���ڵ� */
	struct _html_node_t *next; /* ��һ���ڵ� */
	struct _html_node_t *prev; /* ǰһ���ڵ�*/
	struct _html_node_t *child; /* ��һ�����ӽڵ� */
	struct _html_node_t *last_child; /* ���һ�����ӽڵ� */
	unsigned int subnodetype; //��ʾ�ýڵ�ĺ���ڵ��type�����һλ�Ƿ���P���ұ�2�Ƿ���DIV���ұ�3�Ƿ���H1-H6,�ұ�4λ�Ƿ���table;��5�Ƿ���ul��ol;��6�Ƿ���form,��7 IMG
	unsigned int childnodetype; //��ʾ�ýڵ�Ķ��ӽڵ��type�����һλ�Ƿ���P���ұ�2�Ƿ���DIV���ұ�3�Ƿ���H1-H6,�ұ�4λ�Ƿ���table;��5�Ƿ���ul��ol;��6�Ƿ���form,��7 IMG
	void *user_ptr;//�û��Զ���ָ�룬����ָ���Լ���Ҫ�Ľṹ
} html_node_t;

/**
 * @brief html�ڵ���б�
 * @author sue
 * @date 2013-06-19
 */
typedef struct _html_node_list_t
{
	html_node_t* html_node;
	_html_node_list_t* next;
	_html_node_list_t* prev;
} html_node_list_t;

/**
 * @brief ����tag������.
 */
typedef enum
{
	HTML_NORMAL_TAG, /**< Ҫ������tag */
	HTML_IGNORE_TAG /**< Ҫ���Ե�tag, ��������tag��Χ�ڵ��������ݺ���(�������п��ܰ���Ҫ������tag), ����ע�ͱ�ǩ����һ��*/
} html_tag_config_type_t;

/**
 * @brief ���ĵ�tag����.
 */
typedef struct
{
	char start_tag_name[MAX_START_TAG_NAME_LEN]; /**< ��"<link","<!--" */
	char end_tag_name[MAX_END_TAG_NAME_LEN]; /**< ��"</link>", "-->".�����������|�ָ��� ��"</iframe>|/>" */
	html_tag_config_type_t tag_type; /**< �������õ�����,��Ҫ��������Ҫȫ��ȥ�� */
} html_interest_tag_t;

typedef struct _html_tree_debug_info_t
{
	//��¼�رյ�ǰ�ڵ�����ȵ��ݴ���ԵĴ������
	int close_tag_err[HTML_TREE_TAG_TYPE_NUM][HTML_TREE_TAG_TYPE_NUM];
	//��¼���ӹ�ϵ�ݴ���ԵĴ������
	int child_parent_err[HTML_TREE_TAG_TYPE_NUM][HTML_TREE_TAG_TYPE_NUM];
	int tag_count[HTML_TREE_TAG_TYPE_NUM];
	int cte_count;
	int cpe_count;
	int css_link_count;
	int script_count;
	int img_count;
} html_tree_debug_info_t;

/**
 *��dom���ĳ������а�������dom��������е�һЩ�����ֶ�
 **/
typedef struct _html_tree_t
{
	html_node_t root; // HTML <root> node
	html_node_t *html; // HTML <html> node
	html_node_t *head; // HTML <head> node
	html_node_t *body; // HTML <body> node
	html_doctype doctype; /**< �ĵ����� */
	unsigned int treeAttr; //dom�����е����ԣ����һλ��ʾ�Ƿ���div�ڵ㣻2λ�Ƿ����p�ڵ㣻3λ�Ƿ���ڱ���ڵ�H1-H6�����λ��ʾ�Ƿ��ʾ�ڵ������ڵ�����
} html_tree_t;

struct _html_tokenizer_t;
typedef int (*token_state_t)(struct _html_tokenizer_t*, html_tree_t*);

/*htmlԴ�����ȡ��*/
typedef struct _html_tokenizer_t
{
	const char *ht_source; /*��ҳԴ��*/
	const char *ht_begin; /*��ҳcode�п�ʼ����ʱ��λ��*/
	const char *ht_current; /*��ǰ���ڱ�����λ��*/
	const char *ht_end; /*��ҳcode�Ľ���λ��*/
	token_state_t ht_state; /*�α��һ��״������ָ��*/
	html_node_t *ht_node; /*ɨ�������node��ָ��*/
	html_attribute_t *ht_attr; /*��ǰ����ɨ���attribute��ָ��*/
	html_node_t *ht_opening; /*��ǰ���ڵȴ��رյ�node*/
} html_tokenizer_t;

/**
 * @brief ����װ�ص�ǰ�ȴ��رյĽڵ��ջ��ջ�ڵ�
 **/
struct stack_item_t
{
	html_node_t *si_node;SLIST_ENTRY(stack_item_t) si_entries;
};
SLIST_HEAD(html_stack_t, stack_item_t);

/**
 * @brief Foster Stack Item
 **/
struct foster_item_t
{
	struct html_stack_t fi_stack;SLIST_ENTRY(foster_item_t) fi_entries;
};
SLIST_HEAD(html_foster_t, foster_item_t);

/**
 * @brief State handler
 **/
typedef int (*state_handler_t)(struct html_parser_t*, html_node_t*);

/**
 * @brief HTML5 Parser
 **/
struct html_parser_t
{
	struct mem_pool_t *hp_pool; //parser��Ӧ���ڴ��
	html_tree_t *hp_tree; //��ǰ���ڱ���������
	html_tokenizer_t *hp_tokenizer; //��ǰ����ҳԴ�����ȡ��
	html_tokenizer_t *hp_nest_tokenizer; //���õ�
	struct html_stack_t *hp_stack; //�������ɸ��ӹ�ϵ��ջ
	struct html_stack_t *hp_foster_stack;
	struct html_stack_t *hp_actfmt_list;
	struct html_foster_t *hp_foster;
	html_node_t *hp_html; //html��ǩ����wml��ǩ
	html_node_t *hp_body; //body��ǩ����card��ǩ
	html_node_t *hp_form; //form��ǩ
	char *hp_document_write; //��̬���ɵ���ҳ���룬��Ҫ��js����ʱ���õ�
	state_handler_t hp_handler; //��ǰ�ݴ����
	state_handler_t hp_last_handler; //����handler
	struct slab_t *hp_stack_slab;
	struct slab_t *hp_foster_slab;
	unsigned hp_use_nest_tokenizer :1; //�Ƿ�����nest  tokenizer
	unsigned hp_ignore_space :1; //�Ƿ���Կո�
	unsigned hp_xml_compatible :1; //�Ƿ����xml�ļ�
	unsigned hp_wml_compatible :1; //�Ƿ����wml�ļ�
	unsigned hp_script_parsing :1; //�Ƿ��script���з���
};

/**
 * @brief html dom����ʵ�ֻ���
 **/
struct html_tree_impl_t
{
	html_tree_t ht_tree; /* public interface */
	struct mem_pool_t *ht_pool; /* memory pool */
	struct slab_t *ht_node_slab; /* node slab */
	struct html_parser_t *ht_parser; /* parser */
	int ht_tag_code; /* tag code variable */
};

/**
 * @brief Pre visitor
 **/
typedef int (*pre_visitor_t)(html_tag_t*, void*, int);

/**
 * @brief Post visitor
 **/
typedef int (*post_visitor_t)(html_tag_t*, void*);

/**
 * @brief Visit context
 **/
struct html_node_visit_ctx_t
{
	pre_visitor_t vc_pre_visitor;
	post_visitor_t vc_post_visitor;
	void *vc_data;
};

/**
 * @brief latin map
 **/
extern char g_latin_map[];
/**
 * @brief tag name special character map
 **/
extern char g_tag_name_map[];
/**
 * @brief attribute name special character map
 **/
extern char g_attribute_name_map[];
/**
 * @brief attribute value special character map
 **/
extern char g_attribute_value_uq_map[];

#endif /* EASOU_HTML_DOM_H_ */

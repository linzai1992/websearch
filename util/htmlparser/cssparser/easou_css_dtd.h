/**
 * easou_css_dtd.h
 * Description: CSS��׼���Ͷ���
 *  Created on: 2011-06-20
 * Last modify: 2012-10-31 sue_zhang@staff.easou.com shuangwei_zhang@staff.easou.com
 *      Author: xunwu_chen@staff.easoucom
 *     Version: 1.2
 */
#ifndef EASOU_CSS_DTD_H_
#define EASOU_CSS_DTD_H_

#define UL_MAX_SITE_LEN 1024
#define UL_MAX_PORT_LEN 1024
#define UL_MAX_PATH_LEN 1024

#include "easou_html_dom.h"
#include "simplehashmap.h"

/**
 * @brief CSS���԰����ܻ�������
 */
typedef enum _css_prop_func_type
{
	CSS_PROP_HUNC_FONT, /* �������� */
	CSS_PROP_HUNC_GEO, /* �������� */
	CSS_PROP_HUNC_OTHER /* ��������  */
} css_prop_func_type;

extern short css_prop_first_char_map[]; /* CSS���԰�����ĸ�������� */

/* css_property_name_array[]��prop_type_array[]ÿ��Ԫ��һһ��Ӧ */
extern const char *css_property_name_array[];

extern short css_other_usefull_prop[]; /* �����壬λ�ô�С�������������õ����� */

/**
 * @brief ���е�CSS����
 */
typedef enum _easou_css_prop_type_t
{
	CSS_PROP_ACCELERATOR,
	CSS_PROP_AZIMUTH,
	CSS_PROP_BACKGROUND,
	CSS_PROP_BACKGROUND_ATTACHMENT,
	CSS_PROP_BACKGROUND_COLOR,
	CSS_PROP_BACKGROUND_IMAGE,
	CSS_PROP_BACKGROUND_POSITION,
	CSS_PROP_BACKGROUND_POSITION_X,
	CSS_PROP_BACKGROUND_POSITION_Y,
	CSS_PROP_BACKGROUND_REPEAT,
	CSS_PROP_BEHAVIOR,
	CSS_PROP_BORDER,
	CSS_PROP_BORDER_BOTTOM,
	CSS_PROP_BORDER_BOTTOM_COLOR,
	CSS_PROP_BORDER_BOTTOM_STYLE,
	CSS_PROP_BORDER_BOTTOM_WIDTH,
	CSS_PROP_BORDER_COLLAPSE,
	CSS_PROP_BORDER_COLOR,
	CSS_PROP_BORDER_LEFT,
	CSS_PROP_BORDER_LEFT_COLOR,
	CSS_PROP_BORDER_LEFT_STYLE,
	CSS_PROP_BORDER_LEFT_WIDTH,
	CSS_PROP_BORDER_RIGHT,
	CSS_PROP_BORDER_RIGHT_COLOR,
	CSS_PROP_BORDER_RIGHT_STYLE,
	CSS_PROP_BORDER_RIGHT_WIDTH,
	CSS_PROP_BORDER_SPACING,
	CSS_PROP_BORDER_STYLE,
	CSS_PROP_BORDER_TOP,
	CSS_PROP_BORDER_TOP_COLOR,
	CSS_PROP_BORDER_TOP_STYLE,
	CSS_PROP_BORDER_TOP_WIDTH,
	CSS_PROP_BORDER_WIDTH,
	CSS_PROP_BOTTOM,
	CSS_PROP_CAPTION_SIDE,
	CSS_PROP_CLEAR,
	CSS_PROP_CLIP,
	CSS_PROP_COLOR,
	CSS_PROP_CONTENT,
	CSS_PROP_COUNTER_INCREMENT,
	CSS_PROP_COUNTER_RESET,
	CSS_PROP_CUE,
	CSS_PROP_CUE_AFTER,
	CSS_PROP_CUE_BEFORE,
	CSS_PROP_CURSOR,
	CSS_PROP_DIRECTION,
	CSS_PROP_DISPLAY,
	CSS_PROP_ELEVATION,
	CSS_PROP_EMPTY_CELLS,
	CSS_PROP_FILTER,
	CSS_PROP_FLOAT,
	CSS_PROP_FONT,
	CSS_PROP_FONT_FAMILY,
	CSS_PROP_FONT_SIZE,
	CSS_PROP_FONT_SIZE_ADJUST,
	CSS_PROP_FONT_STRETCH,
	CSS_PROP_FONT_STYLE,
	CSS_PROP_FONT_VARIANT,
	CSS_PROP_FONT_WEIGHT,
	CSS_PROP_HEIGHT,
	CSS_PROP_IME_MODE,
	CSS_PROP_INCLUDE_SOURCE,
	CSS_PROP_LAYER_BACKGROUND_COLOR,
	CSS_PROP_LAYER_BACKGROUND_IMAGE,
	CSS_PROP_LAYOUT_FLOW,
	CSS_PROP_LAYOUT_GRID,
	CSS_PROP_LAYOUT_GRID_CHAR,
	CSS_PROP_LAYOUT_GRID_CHAR_SPACING,
	CSS_PROP_LAYOUT_GRID_LINE,
	CSS_PROP_LAYOUT_GRID_MODE,
	CSS_PROP_LAYOUT_GRID_TYPE,
	CSS_PROP_LEFT,
	CSS_PROP_LETTER_SPACING,
	CSS_PROP_LINE_BREAK,
	CSS_PROP_LINE_HEIGHT,
	CSS_PROP_LIST_STYLE,
	CSS_PROP_LIST_STYLE_IMAGE,
	CSS_PROP_LIST_STYLE_POSITION,
	CSS_PROP_LIST_STYLE_TYPE,
	CSS_PROP_MARGIN,
	CSS_PROP_MARGIN_BOTTOM,
	CSS_PROP_MARGIN_LEFT,
	CSS_PROP_MARGIN_RIGHT,
	CSS_PROP_MARGIN_TOP,
	CSS_PROP_MARKER_OFFSET,
	CSS_PROP_MARKS,
	CSS_PROP_MAX_HEIGHT,
	CSS_PROP_MAX_WIDTH,
	CSS_PROP_MIN_HEIGHT,
	CSS_PROP_MIN_WIDTH,
	CSS_PROP__MOZ_BINDING,
	CSS_PROP__MOZ_BORDER_BOTTOM_COLORS,
	CSS_PROP__MOZ_BORDER_LEFT_COLORS,
	CSS_PROP__MOZ_BORDER_RADIUS,
	CSS_PROP__MOZ_BORDER_RADIUS_BOTTOMLEFT,
	CSS_PROP__MOZ_BORDER_RADIUS_BOTTOMRIGHT,
	CSS_PROP__MOZ_BORDER_RADIUS_TOPLEFT,
	CSS_PROP__MOZ_BORDER_RADIUS_TOPRIGHT,
	CSS_PROP__MOZ_BORDER_RIGHT_COLORS,
	CSS_PROP__MOZ_BORDER_TOP_COLORS,
	CSS_PROP__MOZ_OPACITY,
	CSS_PROP__MOZ_OUTLINE,
	CSS_PROP__MOZ_OUTLINE_COLOR,
	CSS_PROP__MOZ_OUTLINE_STYLE,
	CSS_PROP__MOZ_OUTLINE_WIDTH,
	CSS_PROP__MOZ_USER_FOCUS,
	CSS_PROP__MOZ_USER_INPUT,
	CSS_PROP__MOZ_USER_MODIFY,
	CSS_PROP__MOZ_USER_SELECT,
	CSS_PROP_ORPHANS,
	CSS_PROP_OUTLINE,
	CSS_PROP_OUTLINE_COLOR,
	CSS_PROP_OUTLINE_STYLE,
	CSS_PROP_OUTLINE_WIDTH,
	CSS_PROP_OVERFLOW,
	CSS_PROP_OVERFLOW_X,
	CSS_PROP_OVERFLOW_Y,
	CSS_PROP_PADDING,
	CSS_PROP_PADDING_BOTTOM,
	CSS_PROP_PADDING_LEFT,
	CSS_PROP_PADDING_RIGHT,
	CSS_PROP_PADDING_TOP,
	CSS_PROP_PAGE,
	CSS_PROP_PAGE_BREAK_AFTER,
	CSS_PROP_PAGE_BREAK_BEFORE,
	CSS_PROP_PAGE_BREAK_INSIDE,
	CSS_PROP_PAUSE,
	CSS_PROP_PAUSE_AFTER,
	CSS_PROP_PAUSE_BEFORE,
	CSS_PROP_PITCH,
	CSS_PROP_PITCH_RANGE,
	CSS_PROP_PLAY_DURING,
	CSS_PROP_POSITION,
	CSS_PROP_QUOTES,
	CSS_PROP__REPLACE,
	CSS_PROP_RICHNESS,
	CSS_PROP_RIGHT,
	CSS_PROP_RUBY_ALIGN,
	CSS_PROP_RUBY_OVERHANG,
	CSS_PROP_RUBY_POSITION ,
	CSS_PROP_SCROLLBAR_3D_LIGHT_COLOR,
	CSS_PROP_SCROLLBAR_ARROW_COLOR,
	CSS_PROP_SCROLLBAR_BASE_COLOR,
	CSS_PROP_SCROLLBAR_DARK_SHADOW_COLOR,
	CSS_PROP_SCROLLBAR_FACE_COLOR,
	CSS_PROP_SCROLLBAR_HIGHLIGHT_COLOR,
	CSS_PROP_SCROLLBAR_SHADOW_COLOR,
	CSS_PROP_SCROLLBAR_TRACK_COLOR,
	CSS_PROP__SET_LINK_SOURCE,
	CSS_PROP_SIZE,
	CSS_PROP_SPEAK,
	CSS_PROP_SPEAK_HEADER,
	CSS_PROP_SPEAK_NUMERAL,
	CSS_PROP_SPEAK_PUNCTUATION,
	CSS_PROP_SPEECH_RATE,
	CSS_PROP_STRESS,
	CSS_PROP_TABLE_LAYOUT,
	CSS_PROP_TEXT_ALIGN,
	CSS_PROP_TEXT_ALIGN_LAST,
	CSS_PROP_TEXT_AUTOSPACE,
	CSS_PROP_TEXT_DECORATION,
	CSS_PROP_TEXT_INDENT,
	CSS_PROP_TEXT_JUSTIFY,
	CSS_PROP_TEXT_KASHIDA_SPACE,
	CSS_PROP_TEXT_OVERFLOW,
	CSS_PROP_TEXT_SHADOW,
	CSS_PROP_TEXT_TRANSFORM,
	CSS_PROP_TEXT_UNDERLINE_POSITION,
	CSS_PROP_TOP,
	CSS_PROP_UNICODE_BIDI,
	CSS_PROP__USE_LINK_SOURCE,
	CSS_PROP_VERTICAL_ALIGN,
	CSS_PROP_VISIBILITY,
	CSS_PROP_VOICE_FAMILY,
	CSS_PROP_VOLUME ,
	CSS_PROP_WHITE_SPACE,
	CSS_PROP_WIDOWS,
	CSS_PROP_WIDTH,
	CSS_PROP_WORD_BREAK,
	CSS_PROP_WORD_SPACING,
	CSS_PROP_WORD_WRAP,
	CSS_PROP_WRITING_MODE,
	CSS_PROP_UNKNOWN
} easou_css_prop_type_t;

#define CSS_PROPERTY_NUM (CSS_PROP_UNKNOWN-CSS_PROP_ACCELERATOR)		  /**< CSS���Ը���  */

/**
 * @brief CSS����������Ϣ
 */
typedef struct _easou_css_prop_type_info_t
{
	_easou_css_prop_type_t type; /* ��������  */
	bool is_font_prop; /* �Ƿ���������  */
	bool is_geo_prop; /* �Ƿ񼸺�����  */
} easou_css_prop_type_info_t;

/* css_property_name_array[]��prop_type_array[]ÿ��Ԫ��һһ��Ӧ */
extern const easou_css_prop_type_info_t prop_typeinfo_array[];

#define CSS_UNIVERSAL_SELECTOR	((html_tag_type_t)(-1))		  /**< ȫ��ѡ���ӣ���������ÿ����ǩ�����Ա���Ⱦ  */

/**
 * @brief CSS����ѡ��������.
 */
typedef enum _easou_css_attr_selector_type_t
{
	CSS_ATTR_SELECT_MATCH_NOVALUE, /* ������ֵƥ��  */
	CSS_ATTR_SELECT_MATCH_EXACTLY, /* �����ϸ�ƥ��  */
	CSS_ATTR_SELECT_MATCH_ANY, /* ƥ������һ�� */
	CSS_ATTR_SELECT_MATCH_BEGIN, /* ƥ��ǰ׺  */
	CSS_ATTR_SELECT_OTHER /* ����  */
} easou_css_attr_selector_type_t;

/**
 * @brief CSS����ѡ���.
 */
typedef struct _easou_css_attr_selector_t
{
	easou_css_attr_selector_type_t type; /* ����ѡ��������  */
	html_attribute_t attr; /* ����/ֵ  */
	struct _easou_css_attr_selector_t *next; /* ��һ������ѡ����  */
} easou_css_attr_selector_t;

/**< CLASSѡ����  */
typedef char* easou_css_class_selector_t;
/**< IDѡ����  */
typedef char* easou_css_id_selector_t;
/**< α��ѡ����  */
typedef char* easou_css_pseudo_selector_t;

/**
 * @brief CSSѡ�����������
 */
typedef enum _easou_css_selector_combinator_t
{
	CSS_NON_COMBINATOR, /**< �����  */
	CSS_DESCEND_COMBINATOR, /**< ���ѡ����  */
	CSS_CHILD_COMBINATOR, /**< ��ѡ����  */
	CSS_ADJACENT_COMBINATOR /**< ����ѡ����  */
} easou_css_selector_combinator_t;

enum simple_selector_type_t
{
	SIMPLE_SELECTOR_ALL,
	SIMPLE_SELECTOR_TAG,
	SIMPLE_SELECTOR_CLASS,
	SIMPLE_SELECTOR_ID,
	SIMPLE_SELECTOR_ATTR,
	SIMPLE_SELECTOR_TAGCLASS,
	SIMPLE_SELECTOR_TAGATTR,
	SIMPLE_SELECTOR_TAGID,
	SIMPLE_SELECTOR_PSEUDO,
	SIMPLE_SELECTOR_OTHER
};

/**
 * @brief һ��ѡ����.
 */
typedef struct _easou_css_simple_selector_t
{
	simple_selector_type_t type;
	html_tag_type_t tag_type; /**< ����ѡ����(ת��ΪHTML_TAG����)  */
	char *name; /**< ����ѡ���Ӷ�Ӧ��HTML_TAG��  */
	easou_css_attr_selector_t *attr_selector; /**< ����ѡ����  */
	easou_css_class_selector_t class_selector; /**< CLASSѡ����  */
	easou_css_id_selector_t id_selector; /**<  IDѡ���� */
	easou_css_pseudo_selector_t pseudo_selector; /**< α��ѡ����  */
} easou_css_simple_selector_t;

/**
 * @brief CSSѡ����,�ɼ�ѡ������϶���.
 */
typedef struct _easou_css_selector_t
{
	struct _easou_css_selector_t *pre_selector; /**< ǰѡ����  */
	easou_css_selector_combinator_t combinator; /**< ��Ϸ��� */
	easou_css_simple_selector_t simple_selector; /**< ��ѡ����  */
} easou_css_selector_t;

/**
 * @brief CSS����
 */
typedef struct _easou_css_property_t
{
	easou_css_prop_type_t type; /**< ��������  */
	char *name; /**< ����  */
	char *value; /**< ֵ  */
	struct _easou_css_property_t *next;
} easou_css_property_t;

/**
 * @brief CSS����
 */
typedef struct _easou_css_ruleset_t
{
	int id;
	easou_css_selector_t *selector; /**< ѡ����  */
	easou_css_property_t *all_property_list; /**< ���������б�  */
	easou_css_property_t *font_prop_begin, *font_prop_end; /**< �������ԵĿ�ʼ�ͽ�βָ��  */
	easou_css_property_t *geo_prop_begin, *geo_prop_end; /**< �������ԵĿ�ʼ�ͽ�βָ��  */
	easou_css_property_t *other_prop_begin, *other_prop_end; /**< �������ԵĿ�ʼ�ͽ�βָ��  */
	struct _easou_css_ruleset_t *next; /**< ��CSS�ṹ�ϵ���һ������  */
	struct _easou_css_ruleset_t *index_next; /**< �������ϵ���һ������  */
} easou_css_ruleset_t;

/**
 * @brief �ַ����ѽṹ,���ڴ��CSS�������ַ���.
 */
typedef struct _easou_css_str_heap_t
{
	char *p_heap; /**< ������ڴ濪ʼָ��  */
	size_t heap_size; /**< ������ڴ��С  */
	char *p_heap_avail; /**< ��ǰ���õ��ڴ�ָ��  */
} easou_css_str_heap_t;

/**
 * @brief �ڴ����ڵ�.
 */
typedef struct _easou_css_mem_node_t
{
	void *p_mem; /**< �ڴ��ָ��  */
	size_t mem_size; /**< �ڴ���С  */
	struct _easou_css_mem_node_t *next;
} easou_css_mem_node_t;

/**
 * @brief �ڵ��.
 *	���ڹ����ѷ����һϵ���ڴ��,���������ÿһ���ڵ���ڴ�.
 */
typedef struct _easou_css_nodepool_t
{
	easou_css_mem_node_t *mem_node_list; /**< �ѷ�����ڴ���  */
	void *p_curr_mem; /**< ��ǰʹ�õ��ڴ��  */
	size_t p_curr_mem_size; /**< ��ǰ�ڴ�Ĵ�С  */
	void *p_pool_avail; /**< ��ǰ���õ��ڴ濪ͷ��ָ��  */
} easou_css_nodepool_t;

/**
 * @brief CSS����ʹ�õ��ڲ��ṹ
 */
typedef struct _easou_css_inner_t
{
	easou_css_str_heap_t str_heap; /**< �ַ����ѽṹ  */
	easou_css_nodepool_t nodepool; /**< �ڵ��  */
} easou_css_inner_t;

/**
 * @brief CSS�������
 */
typedef struct _easou_css_index_node_t
{
	const char *key; /**< ������ֵ  */
	void *data; /**< ����������  */
	struct _easou_css_index_node_t *next;
} easou_css_index_node_t;

/**
 * @brief CSS��ѡ���ӵ��������
 */
typedef struct _simple_selector_index_node_t
{
	int selector_id;
	short start_pos;
	short pos;
	easou_css_ruleset_t *ruleset;
	struct _simple_selector_index_node_t *next;
} simple_selector_index_node_t;

#define TAG_TYPE_NUM_LIMIT	256 		  /**< TAG���͵�����  */

/**
 * @brief CSS�����ṹ
 */
typedef struct _easou_css_index_t
{
	easou_css_index_node_t *type_index[TAG_TYPE_NUM_LIMIT]; /**< ÿ��TAG���Ͷ�Ӧһ������  */
	hashmap_t *class_map;
} easou_css_index_t;

/**
 * @brief CSS���������Ľṹ
 */
typedef struct _easou_css_t
{
	easou_css_inner_t css_inner; /**< CSS�ڲ��ṹ,�����ڴ����  */
	easou_css_ruleset_t *all_ruleset_list; /**< ���й����б�  */
	easou_css_index_t index; /**< CSS�����ṹ  */
} easou_css_t;

/**
 * @brief ɨ��״̬��¼��
 *
 */
typedef enum _easou_css_token_type_t
{
	CSS_TOKEN_URI, /**< URI token  */
	CSS_TOKEN_STRING, /**< ɨ�赽�ַ���  */
	CSS_TOKEN_NORMAL, /**< һ��ɨ��  */
	CSS_TOKEN_UNKNOWN /**< δ֪ɨ�� */
} easou_css_token_type_t;

/**
 * @brief ɨ��״̬��¼��
 */
typedef enum _easou_css_state_t
{
	CSS_STAT_SCAN_SELECTOR, /**< ɨ��ѡ����  */
	CSS_STAT_SCAN_PROP_NAME, /**< ɨ�����Ե����� */
	CSS_STAT_SCAN_PROP_VALUE /**< ɨ�����Ե�ֵ */
} easou_css_state_t;

/**
 * @brief cssɨ����
 *
 */
typedef struct _easou_css_scan_t
{
	char *p_curr_token; /**< ��ǰɨ�赽���ַ�  */
	int p_curr_token_len; /**< ��ǰɨ�赽���ַ�����  */
	const char *p_next; /**<   */
	easou_css_token_type_t type; /**< ��ǰ����ɨ����ַ�����  */
	easou_css_state_t state; /**< ��ǰ����ɨ���css����  */
	const char *css_url; /**< ��ǰ����ɨ���css��url  */
	bool test_import; /**< �Ƿ�����css�ļ���import��css  */
} easou_css_scan_t;

/**
 * @brief CSS������Ϣ
 */
typedef struct _easou_css_prop_info_t
{
	char *value; /**< ����ֵ  */
	int prio_val; /**< ���Ե����ȼ�ֵ  */
} easou_css_prop_info_t;

/**
 * @brief CSS���Լ�
 */
typedef struct _easou_css_property_set_t
{
	easou_css_prop_info_t prop[CSS_PROPERTY_NUM];
} easou_css_property_set_t;

/**
 * @brief �ڵ��css���ԵĽ����
 */
typedef struct _prop_set_param_t
{
	easou_css_property_set_t *prop_set;
	const html_node_t *html_node;
	int prop_num;
	int order; //Ϊcss�����������ȼ���orderΪ���ֵ�˳��2012-05-02
} prop_set_param_t;

#endif /*EASOU_CSS_DTD_H_*/

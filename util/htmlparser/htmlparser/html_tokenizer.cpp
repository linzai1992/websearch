/**
 * @file easou_html_tokenizer.cpp
 * @author xunwu
 * @date 2011/08/02
 * @version 1.0
 * @brief htmlԴ�����ȡ��
 *
 **/

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "queue.h"
#include "easou_html_pool.h"
#include "easou_html_dtd.h"
#include "easou_html_dom.h"
#include "easou_html_attr.h"
#include "easou_html_node.h"
#include "easou_html_tokenizer.h"
#include "easou_string.h"

#define SCANNER_ASSERT() \
    assert(tk); \
    assert(doc);

/**����������assert��ȡ��ǰ�ַ�*/
#define SCANNER_COMMON(ch) \
    int (ch) = '\0'; \
    SCANNER_ASSERT() \
    (ch) = (tk)->ht_current[0];

/**
 * @brief �ж��Ƿ��ǿո��
 **/
#define IS_WS(ch) (g_whitespace_map[(unsigned char)(ch)])
/**
 * @brief �ж��Ƿ��������ַ�
 **/
#define IS_LATIN(ch) (g_latin_map[(unsigned char)(ch)])

/**
 *	@brief ����һ��ע�ͽڵ�
 */
static int emit_comment(html_tokenizer_t *tk, html_tree_t *doc, const char *end)
{
	char *text = NULL;
	int len = 0;
	SCANNER_ASSERT();
	len = end - tk->ht_current;
	if (len == 0)
	{
		text = (char*) "";
	}
	else
	{
		text = html_tree_strndup(doc, tk->ht_current, len);
		if (text == NULL)
		{
			return -1;
		}
	}
	tk->ht_node = html_tree_create_comment(doc, text);
	if (tk->ht_node == NULL)
	{
		return -1;
	}
	tk->ht_node->html_tag.page_offset = tk->ht_current - 4 - tk->ht_source;
	return 0;
}

/**
 *	@brief ����һ��doctype�ڵ�
 */
static int emit_doctype(html_tokenizer_t *tk, html_tree_t *doc, const char *end)
{
	char *text = NULL;
	int len = 0;
	SCANNER_ASSERT();
	len = end - tk->ht_current;
	if (len == 0)
	{
		text = (char*) "";
	}
	else
	{
		text = html_tree_strndup(doc, tk->ht_current, len);
		if (text == NULL)
		{
			return -1;
		}
	}
	tk->ht_node = html_tree_create_doctype(doc, text);
	if (tk->ht_node == NULL)
	{
		return -1;
	}
	tk->ht_node->html_tag.page_offset = tk->ht_current - 9 - tk->ht_source;
	return 0;
}

/**
 *	@brief ����һ��text�ڵ�
 */
static int emit_text(html_tokenizer_t *tk, html_tree_t *doc, const char *end)
{
	char *text = NULL;
	html_node_t *node = NULL;
	int i = 0;
	int len = 0;
	SCANNER_ASSERT();
	len = end - tk->ht_begin;
	if (len > 0)
	{
		text = html_tree_strndup(doc, tk->ht_begin, len);
		if (text == NULL)
		{
			return -1;
		}
	}
	else
	{
		text = (char*) "";
	}
	node = html_tree_create_text_node(doc, text);
	if (node == NULL)
	{
		return -1;
	}

	node->html_tag.nodelength = len;
	node->html_tag.textlength = len; //shuangwei add 20120529
	node->html_tag.tag_type = TAG_WHITESPACE;
	node->html_tag.page_offset = tk->ht_begin - tk->ht_source;
	for (i = 0; i < len; i++)
	{
		if (!IS_WS(text[i]))
		{
			node->html_tag.tag_type = TAG_PURETEXT;
			break;
		}
	}
	tk->ht_node = node;
	return 0;
}

/**
 *	@brief ������Ե�name
 */
static int fill_attr_name(html_tokenizer_t *tk, html_tree_t *doc, const char *end)
{
	char *text = NULL;
	html_attr_type_t type = ATTR_UNKNOWN;
	int len = 0;
	int i = 0;
	SCANNER_ASSERT();
	assert(tk->ht_attr);
	len = end - tk->ht_attr->name;
	type = get_attr_type(tk->ht_attr->name, len);

	tk->ht_attr->type = type;
	if (type != ATTR_UNKNOWN)
	{
		tk->ht_attr->name = get_attr_name(type);
	}
	else
	{
		text = html_tree_strndup(doc, tk->ht_attr->name, len);
		if (text == NULL)
		{
			return -1;
		}
		for (i = 0; i < len; i++)
		{
			text[i] = (char) tolower(text[i]);
		}
		tk->ht_attr->name = text;
	}

	return 0;
}

/**
 *	@brief ������Ե�value
 */
static int fill_attr_value(html_tokenizer_t *tk, html_tree_t *doc, const char *end)
{
	char *text = NULL;
	SCANNER_ASSERT();
	assert(tk->ht_attr);

	text = html_tree_strndup(doc, tk->ht_attr->value, end - tk->ht_attr->value);
	if (text == NULL)
	{
		return -1;
	}
	tk->ht_attr->valuelength = end - tk->ht_attr->value; //shuangwei add 20120529
	tk->ht_attr->value = text;

	return 0;
}

/**
 *	@brief ���ַ������ҵ���һ��Ŀ���ַ�������Ҳ�����ֱ�������ַ���ĩβ����begin==0
 */
static inline const char* strchr_range(const char *begin, const char *end, int ch)
{
	while (*begin && begin < end)
	{
		if (*begin == ch)
		{
			return begin;
		}
		begin++;
	}
	return end;
}

/**
 *	@brief �����հ׷�������Ҳ����հ׷���ֱ�������ַ���ĩβ����begin==0
 */
static inline const char* skip_ws(const char *src, const char *end)
{
	while (*src && src < end)
	{
		if (!IS_WS(*src))
		{
			return src;
		}
		src++;
	}
	return end;
}

/**
 *	@brief �����ַ���ֱ�������ض��ַ�������Ҳ����հ׷���ֱ�������ַ���ĩβ����begin==0
 */
static inline const char* skip_to(const char *src, const char *end, char *map)
{
	while (*src && src < end)
	{
		if (map[(unsigned char) *src])
		{
			return src;
		}
		src++;
	}
	return end;
}

/**tokenizer������ҳԴ�ļ��ļ���״̬ */
static int scan_tag_open(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_data(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_bogus_comment(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_rcdata(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_doctype(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_comment(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_self_closing_start_tag(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_attribute_value(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_after_attribute_name(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_attribute_name(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_before_attribute_name(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_tag_name(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_end_tag_open(html_tokenizer_t* tk, html_tree_t* doc);
static int scan_markup_decl_open(html_tokenizer_t* tk, html_tree_t* doc);

/**
 * @brief HTML Tokenizer �����������Ǳ�ǩ��λ�ã��Խ�һ��'<'
 **/
static int scan_tag_open(html_tokenizer_t* tk, html_tree_t* doc)
{
	SCANNER_COMMON(ch);
	if (tk->ht_current >= tk->ht_end || ch == '\0')
	{
		tk->ht_state = scan_data;
		return scan_data(tk, doc);
	}
	switch (ch)
	{
	case '!': /**ע�ͣ�Doc��*/
		tk->ht_current++;
		return scan_markup_decl_open(tk, doc);
	case '/': /**������ǩ*/
		tk->ht_current++;
		return scan_end_tag_open(tk, doc);
	case '?': /**һ����<?xml>��Ҳ���������ǡ���ע�͡�*/
		return scan_bogus_comment(tk, doc);
	default:
		if (IS_LATIN(ch))
		{ /**�����ַ�*/
			/**��ǰ�ַ�����ҳ��ʼ�ľ������1������һ��'<'֮ǰ�����ַ�����Ҫ����text node*/
			if (tk->ht_current - tk->ht_begin > 1)
			{
				if (emit_text(tk, doc, tk->ht_current - 1) != 0)
				{
					return -1;
				}
				tk->ht_state = scan_tag_open;
				return 0;
			}
			/**����һ���ڵ�*/
			tk->ht_node = html_tree_create_element_by_tag(doc, TAG_UNKNOWN);
			if (tk->ht_node == NULL)
			{
				return -1;
			}
			tk->ht_node->html_tag.page_offset = tk->ht_current - 1 - tk->ht_source;
			tk->ht_node->html_tag.tag_name = (char*) tk->ht_current;
			tk->ht_current++;
			/**��ʼ������ǩ��name*/
			return scan_tag_name(tk, doc);
		}
		else
		{/**�������������ַ���ֱ�ӽ�֮ǰ��'<'������ͨ�ַ��������ո��ڴ˴�������˴���*/
			tk->ht_state = scan_data;
			return scan_data(tk, doc);
		}
	}
}

/**
 * @brief HTML Tokenizer ������ҳ�Ŀ�ʼ״̬��Ҳ�����һ���ڵ���Ĭ��״̬
 **/
static int scan_data(html_tokenizer_t* tk, html_tree_t* doc)
{
	const char *ch = NULL;
	assert(tk);
	assert(doc);
	/**�ҵ���һ��'<'*/
	ch = strchr_range(tk->ht_current, tk->ht_end, '<');
	/**���ص��ǽ���ָ��*/
	if (ch == tk->ht_end)
	{
		/**��ǰ�ڵ�ͽ����ڵ�֮�仹���ַ����������һ��text�ڵ�*/
		if (tk->ht_end - tk->ht_current > 0)
		{
			if (emit_text(tk, doc, tk->ht_end) != 0)
			{
				return -1;
			}
			/**��ǰ������������*/
			tk->ht_current = tk->ht_end;
			return 0;
		}
		/**��ǰ�ڵ�ͽ����ڵ��غ��ˣ����ָ����һ���ǿ���ҳ��ɵ�*/
		return 1;
	}
	else
	{
		/**��ǰ������ָ����һ���ַ�������ת����һ��״̬���ҵ�һ�������Ǳ�ǩ�ĵ�*/
		tk->ht_current = ch + 1;
		return scan_tag_open(tk, doc);
	}
}

/**
 * @brief HTML Tokenizer ������ע�͵�״̬ '<!' or '<?' ���ǵõ���ע��������
 **/
static int scan_bogus_comment(html_tokenizer_t* tk, html_tree_t* doc)
{
	const char *gt = NULL;
	SCANNER_ASSERT();

	gt = strchr_range(tk->ht_current, tk->ht_end, '>');
	if (emit_comment(tk, doc, gt) != 0)
	{
		return -1;
	}
	tk->ht_node->html_tag.page_offset += 3; /* bogus comment start as '<!' or '<?' not '<!--' */
	if (gt == tk->ht_end)
	{
		tk->ht_current = tk->ht_end;
	}
	else
	{
		tk->ht_current = gt + 1;
	}
	tk->ht_state = scan_data;
	return 0;
}

/*
 * RCDATA state will parse text until an appropriate end tag,
 * a.k.a <xyz>...</xyz> pattern
 * Find this tag, emit #text and return DATA state.
 */
/**
 * @brief HTML Tokenizer Scanner rcdata  �൱���Զ����ǩ
 **/
static int scan_rcdata(html_tokenizer_t* tk, html_tree_t* doc)
{
	const char *ch = NULL;
	int len = 0;
	SCANNER_ASSERT();

	if (!tk->ht_opening)
	{
		return -1;
	}
	len = strlen(tk->ht_opening->html_tag.tag_name);
	ch = tk->ht_current;
	while (1)
	{
		ch = strchr_range(ch, tk->ht_end, '<');
		if (ch == tk->ht_end)
		{
			tk->ht_current = tk->ht_end;
			if (emit_text(tk, doc, tk->ht_current) != 0)
			{
				return -1;
			}
			tk->ht_state = scan_data;
			return 0;
		}
		if (tk->ht_end - ch > len + 2 && ch[1] == '/')
		{
			/**�ҵ����ڵȴ��رյı�ǩ��ƥ���ǩ*/
			if (strncasecmp(ch + 2, tk->ht_opening->html_tag.tag_name, len) == 0)
			{
				tk->ht_current = ch; /* record the text end boundary */
				ch += 2 + len;
				if (ch[0] == '>')
				{
					if (emit_text(tk, doc, tk->ht_current))
					{
						return -1;
					}
					tk->ht_current = ch + 1;
					tk->ht_state = scan_data;
					return 0;
				}
				else if (IS_WS(ch[0]) || ch[0] == '/')
				{
					ch = strchr_range(ch, tk->ht_end, '>');
					if (ch == tk->ht_end)
					{
						if (emit_text(tk, doc, tk->ht_end) != 0)
						{
							return -1;
						}
						tk->ht_current = tk->ht_end;
					}
					else
					{
						if (emit_text(tk, doc, tk->ht_current) != 0)
						{
							return -1;
						}
						tk->ht_current = ch + 1;
					}
					tk->ht_state = scan_data;
					return 0;
				}
			}
			else
			{
				ch += 2;
			}
		}
		else
		{
			ch += 1;
		}
	}
	return -1;
}

/**
 * @brief HTML Tokenizer ɨ�赽doctype�ڵ�
 **/
static int scan_doctype(html_tokenizer_t* tk, html_tree_t* doc)
{
	const char *gt = NULL;
	SCANNER_ASSERT();
	gt = strchr_range(tk->ht_current, tk->ht_end, '>');
	if (emit_doctype(tk, doc, gt) != 0)
	{
		return -1;
	}
	if (gt == tk->ht_end)
	{
		tk->ht_current = tk->ht_end;
	}
	else
	{
		tk->ht_current = gt + 1;
	}
	tk->ht_state = scan_data;
	return 0;
}

/*
 * A COMMENT generally follows the pattern <!--comment-->,
 * '-->' can also be '--!>' or '--(whitespace)>',
 * or to the end of input stream
 */
/**
 * @brief HTML Tokenizerɨ�赽ע�͵�״̬ <!--comment-->, -->' ������ '--!>' or '-- >
 **/
static int scan_comment(html_tokenizer_t* tk, html_tree_t* doc)
{
	const char *ch = NULL;
	const char *minus = NULL;
	const char *gt = NULL;
	SCANNER_ASSERT();

	/* tk->ht_current points to the first character after <!-- */
	ch = tk->ht_current;
	while ((minus = strchr_range(ch, tk->ht_end, '-')) != tk->ht_end)
	{
		gt = strchr_range(minus, tk->ht_end, '>');
		if (gt == tk->ht_end)
		{
			break;
		}
		else if (minus[1] == '-')
		{
			if (minus + 2 == gt)
			{
				/* --> */
				if (emit_comment(tk, doc, minus) != 0)
				{
					return -1;
				}
				tk->ht_current = gt + 1;
				tk->ht_state = scan_data;
				return 0;
			}
			else if (minus + 3 == gt)
			{
				if (minus[2] == '!' || IS_WS(minus[2]))
				{
					/* --!> or --(whitespace)> */
					if (emit_comment(tk, doc, minus) != 0)
					{
						return -1;
					}
					tk->ht_current = gt + 1;
					tk->ht_state = scan_data;
					return 0;
				}
				else
				{
					/* --(other)>, consume these as comment, go after gt */
					ch = minus + 1;
					continue;
				}
			}
			else
			{
				/* --(.{2,})>, go forward one minus */
				ch = minus + 1;
				continue;
			}
		}
		else
		{
			/* -(^-)(.*)>, go forward */
			ch = minus + 2;
			continue;
		}
	}
	ch = tk->ht_end; /* ch is eof */
	if (ch != tk->ht_current)
	{
		if (*(ch - 1) == '-')
		{
			ch -= 2;
			if (*ch == '-' && ch >= tk->ht_current)
			{
				/* pattern 1 */
				gt = ch;
			}
			else
			{
				/* pattern 2 */
				gt = ch + 1;
			}
		}
		else
		{
			/* pattern 3 */
			gt = ch;
		}
	}
	else
	{
		/* pattern 4 */
		gt = ch;
	}
	if (emit_comment(tk, doc, gt) != 0)
	{
		return -1;
	}
	tk->ht_current = ch;
	tk->ht_state = scan_data;
	return 0;
}

/**
 * @brief HTML Tokenizer ɨ�赽�Թرձ�ǩ�Ľ�β��
 **/
static int scan_self_closing_start_tag(html_tokenizer_t* tk, html_tree_t* doc)
{
	SCANNER_COMMON(ch);
	if (tk->ht_current >= tk->ht_end || ch == '\0')
	{
		return 1;
	}
	/**ֻ��"/>"��ϲ����ж������Թرձ�ǩ��*/
	if (ch == '>')
	{
		assert(tk->ht_node);
		tk->ht_node->html_tag.is_self_closed = 1;
		if (tk->ht_node->html_tag.page_offset >= 0)
		{
			tk->ht_node->html_tag.nodelength = tk->ht_current + 1 - tk->ht_source;
		}

		tk->ht_current++;
		tk->ht_state = scan_data;
		return 0;
	}
	else
	{/**�������ȴ�attribute��״̬*/
		return scan_before_attribute_name(tk, doc);
	}
}

/**
 * @brief HTML Tokenizer ���������Ե�value��
 **/
static int scan_attribute_value(html_tokenizer_t* tk, html_tree_t* doc)
{
	const char *ch = NULL;
	char chr = '\0';
	SCANNER_ASSERT();

	tk->ht_current = skip_ws(tk->ht_current, tk->ht_end);
	/**���겻��Ķ��ɣ���ĩβ�˾ͽ���*/
	if (tk->ht_current == tk->ht_end)
	{
		return 1;
	}

	chr = tk->ht_current[0];
	/**��������ţ���ֱ�������ԳƷ��ų�*/
	if (chr == '"' || chr == '\'')
	{
		//shuangwei modify,skip the " or ' in the value
		ch = strchr_range(tk->ht_current + 1, tk->ht_end, chr);
		if (ch == tk->ht_end)
		{
			return 1;
		}
//		const char * beginpot=tk->ht_current + 1;
//		//const char *pgt=strchr_range(beginpot,tk->ht_end,'>');
//
//		do{
//			ch = strchr_range(beginpot, tk->ht_end, chr);
//
//			if (ch == tk->ht_end) {
//			 return 1;
//			}
//			beginpot=ch+1;
//		}while(((ch+1) < tk->ht_end)&&!(IS_WS(*(ch+1))||*(ch+1) == '>'||*(ch+1) == '/')||((ch+2) < tk->ht_end&&(*(ch+2))=='>'));
//		//shuangwei modify,skip the " or ' in the value
		assert(tk->ht_attr);
		tk->ht_attr->value = (char*) tk->ht_current + 1;
		/**������Ե�value*/
		if (fill_attr_value(tk, doc, ch) != 0)
		{
			return -1;
		}
		assert(tk->ht_node);
		html_node_set_attribute_by_name(tk->ht_node, tk->ht_attr);
		tk->ht_attr = NULL;
		tk->ht_current = ch + 1;

		if (tk->ht_current >= tk->ht_end)
		{
			return 1;
		}
		chr = tk->ht_current[0];

		if (IS_WS(chr))
		{
			tk->ht_current++;
			return scan_before_attribute_name(tk, doc);
		}
		else if (chr == '>')
		{
			assert(tk->ht_node);
			tk->ht_state = scan_data;
			tk->ht_current++;
			return 0;
		}
		else if (chr == '/')
		{
			tk->ht_current++;
			return scan_self_closing_start_tag(tk, doc);
		}
		else
		{
			return scan_before_attribute_name(tk, doc);
		}
	}
	else if (chr == '>')
	{
		assert(tk->ht_node);
		assert(tk->ht_attr);
		html_node_set_attribute_by_name(tk->ht_node, tk->ht_attr);
		tk->ht_current++;
		tk->ht_state = scan_data;
		return 0;
	}
	else
	{
		tk->ht_attr->value = (char*) tk->ht_current;
		ch = skip_to(tk->ht_current, tk->ht_end, g_attribute_value_uq_map);
		if (ch == tk->ht_end)
		{
			return 1;
		}
		assert(tk->ht_attr);
		if (fill_attr_value(tk, doc, ch) != 0)
		{
			return -1;
		}
		assert(tk->ht_node);
		html_node_set_attribute_by_name(tk->ht_node, tk->ht_attr);
		tk->ht_attr = NULL;
		tk->ht_current = ch + 1;
		chr = ch[0];
		if (IS_WS(chr))
		{
			return scan_before_attribute_name(tk, doc);
		}
		else
		{
			assert(chr == '>');
			tk->ht_state = scan_data;
			return 0;
		}
	}
}

/**
 * @brief HTML Tokenizer Scanner after_attribute_name
 **/
static int scan_after_attribute_name(html_tokenizer_t* tk, html_tree_t* doc)
{
	char ch = '\0';
	SCANNER_ASSERT();

	tk->ht_current = skip_ws(tk->ht_current, tk->ht_end);
	if (tk->ht_current == tk->ht_end)
	{
		return 1;
	}

	ch = tk->ht_current[0];
	tk->ht_current++;
	if (ch == '=')
	{
		return scan_attribute_value(tk, doc);
	}
	else if (ch == '>')
	{
		assert(tk->ht_node);
		assert(tk->ht_attr);
		html_node_set_attribute_by_name(tk->ht_node, tk->ht_attr);
		tk->ht_state = scan_data;
		return 0;
	}
	else if (ch == '/')
	{
		assert(tk->ht_node);
		assert(tk->ht_attr);
		html_node_set_attribute_by_name(tk->ht_node, tk->ht_attr);
		return scan_self_closing_start_tag(tk, doc);
	}
	else
	{
		assert(tk->ht_node);
		assert(tk->ht_attr);
		html_node_set_attribute_by_name(tk->ht_node, tk->ht_attr);
		tk->ht_attr = html_tree_create_attribute_by_tag(doc, ATTR_UNKNOWN);
		if (tk->ht_attr == NULL)
		{
			return -1;
		}
		tk->ht_attr->name = tk->ht_current - 1;
		return scan_attribute_name(tk, doc);
	}
}

/**
 * @brief HTML Tokenizer ɨ��attribute��name��״̬
 **/
static int scan_attribute_name(html_tokenizer_t* tk, html_tree_t* doc)
{
	const char *ch = NULL;
	char chr = '\0';
	SCANNER_ASSERT();
	/**��  whitespace / = > ֹͣ*/
	ch = skip_to(tk->ht_current, tk->ht_end, g_attribute_name_map);
	if (ch == tk->ht_end)
	{
		return 1;
	}
	else
	{
		/**����attribute������*/
		assert(tk->ht_attr);
		if (fill_attr_name(tk, doc, ch) != 0)
		{
			return -1;
		}
		tk->ht_current = ch + 1;
		chr = ch[0];
		if (IS_WS(chr))
		{ /**�ո�ֻ��˵���������Ʊ������*/
			return scan_after_attribute_name(tk, doc);
		}
		else if (chr == '=')
		{ /**'='˵����attribute=value��ģʽ*/
			return scan_attribute_value(tk, doc);
		}
		else if (chr == '>')
		{ /**'>'��˵�������Ա���������������ӵ��ڵ���*/
			html_node_set_attribute_by_name(tk->ht_node, tk->ht_attr);
			tk->ht_state = scan_data;
			return 0;
		}
		else
		{
			assert(chr == '/');
			/**ͬ'>'һ��������ͬʱ˵���˸ýڵ���һ���Թرսڵ�*/
			html_node_set_attribute_by_name(tk->ht_node, tk->ht_attr);
			return scan_self_closing_start_tag(tk, doc);
		}
	}
}

/**
 * @brief HTML Tokenizer ����attribute����֮ǰ��׼������attribute���Ƶ�״̬
 **/
static int scan_before_attribute_name(html_tokenizer_t* tk, html_tree_t* doc)
{
	char ch = '\0';
	SCANNER_ASSERT();
	/**�����ո�*/
	tk->ht_current = skip_ws(tk->ht_current, tk->ht_end);
	if (tk->ht_current == tk->ht_end)
	{
		return 1;
	}
	ch = tk->ht_current[0];
	/**��ǩ���ƺ����������'/'��˵���п�����һ���Թرձ�ǩ*/
	if (ch == '/')
	{
		tk->ht_current++;
		return scan_self_closing_start_tag(tk, doc);
	}
	else if (ch == '>')
	{
		tk->ht_current++;
		tk->ht_state = scan_data;
		return 0;
	}
	else
	{
		/**����һ��unknow����*/
		tk->ht_attr = html_tree_create_attribute_by_tag(doc, ATTR_UNKNOWN);
		tk->ht_attr->name = tk->ht_current;
		tk->ht_current++;
		return scan_attribute_name(tk, doc);
	}
}

/**
 * @brief HTML Tokenizer ��ʼ������ǩ��name
 **/
static int scan_tag_name(html_tokenizer_t* tk, html_tree_t* doc)
{
	const char *ch = NULL;
	html_tag_type_t type = TAG_UNKNOWN;
	char *text = NULL;
	int len = 0;
	int i = 0;
	char chr = '\0';
	SCANNER_ASSERT();
	/**������ָ���ַ���*/
	ch = skip_to(tk->ht_current, tk->ht_end, g_tag_name_map);
	if (ch == tk->ht_end)
	{
		return 1;
	}
	else
	{
		assert(tk->ht_node);
		len = ch - tk->ht_node->html_tag.tag_name;
		//todo
		/**��ȡ��ǩ����*/
		/**ȱ�ٵ�һ��map*/
		type = get_tag_type(tk->ht_node->html_tag.tag_name, len);
		//todo
		tk->ht_node->html_tag.tag_type = type;
		/**��ȡ��ǩname*/
		if (type != TAG_UNKNOWN)
		{
			tk->ht_node->html_tag.tag_name = get_tag_name(type);
		}
		else
		{
			text = html_tree_strndup(doc, tk->ht_node->html_tag.tag_name, len);
			if (text == NULL)
			{
				return -1;
			}
			for (i = 0; i < len; i++)
			{/**��ʼ��ΪСд*/
				text[i] = tolower(text[i]);
			}
			tk->ht_node->html_tag.tag_name = text;
		}
		tk->ht_current = ch + 1;
		chr = ch[0];
		if (IS_WS(chr))
		{/**��ǩ���ƺ���������ǿո���˵���ñ�ǩ��������*/
			return scan_before_attribute_name(tk, doc);
		}
		else if (chr == '>')
		{ /**��ǩ���ƺ����������'>'��˵���ñ�ǩ�������Ѿ�����*/
			if (tk->ht_node->html_tag.is_close_tag && tk->ht_node->html_tag.page_offset >= 0)
			{
				tk->ht_node->html_tag.page_offset = ch + 1 - tk->ht_source;
			}
			tk->ht_state = scan_data;
			return 0;
		}
		else
		{ /**��ǩ���ƺ����������'/'��˵���п�����һ���Թرձ�ǩ*/
			assert(chr == '/');
			return scan_self_closing_start_tag(tk, doc);
		}
	}
}

/**
 * @brief HTML Tokenizer ɨ�赽��ǩ������
 **/
static int scan_end_tag_open(html_tokenizer_t* tk, html_tree_t* doc)
{
	SCANNER_COMMON(ch);

	if (tk->ht_current >= tk->ht_end || ch == '\0')
	{
		if (tk->ht_current - tk->ht_begin > 0)
		{
			if (emit_text(tk, doc, tk->ht_current) != 0)
			{
				return -1;
			};
			tk->ht_state = scan_end_tag_open;
			return 0;
		}
		return scan_data(tk, doc);
	}
	else if (IS_LATIN(ch))
	{/**�����ַ���������</a>֮��������������Ҫ�ж�<֮ǰ�Ƿ����ַ�*/
		if (tk->ht_current - tk->ht_begin > 2)
		{
			if (emit_text(tk, doc, tk->ht_current - 2) != 0)
			{
				return -1;
			}
			tk->ht_state = scan_end_tag_open;
			return 0;
		}
		tk->ht_node = html_tree_create_element_by_tag(doc, TAG_UNKNOWN);
		if (tk->ht_node == NULL)
		{
			return -1;
		}
		tk->ht_node->html_tag.page_offset = tk->ht_current - 2 - tk->ht_source;
		tk->ht_node->html_tag.tag_name = (char*) tk->ht_current;
		tk->ht_node->html_tag.is_close_tag = 1;
		return scan_tag_name(tk, doc);
	}
	else if (ch == '>')
	{/**</> ֱ�Ӻ���*/
		tk->ht_current++;
		return scan_data(tk, doc);
	}
	else
	{
		tk->ht_current--; /* preserve the first character */
		return scan_bogus_comment(tk, doc);
	}
}

/**
 * @brief HTML Tokenizer ��������ǩ���������ǩ
 **/
static int scan_markup_decl_open(html_tokenizer_t* tk, html_tree_t* doc)
{
	SCANNER_ASSERT();
	/**<!-- ע��*/
	if (tk->ht_end - tk->ht_current >= 2 && strncmp(tk->ht_current, "--", 2) == 0)
	{
		if (tk->ht_current - tk->ht_begin > 2)
		{
			if (emit_text(tk, doc, tk->ht_current - 2) != 0)
			{
			};
			tk->ht_state = scan_markup_decl_open;
			return 0;
		}
		tk->ht_current += 2;
		return scan_comment(tk, doc);
	}
	else if (tk->ht_end - tk->ht_current >= 7 && strncasecmp(tk->ht_current, "DOCTYPE", 7) == 0)
	{ /**<!DOCTYPE */
		/**�ı��ڵ���Զ������ȷ����һ���ڵ�֮�󡰲�����*/
		if (tk->ht_current - tk->ht_begin > 2)
		{
			if (emit_text(tk, doc, tk->ht_current - 2) != 0)
			{
				return -1;
			};
			tk->ht_state = scan_markup_decl_open;
			return 0;
		}
		tk->ht_current += 7;
		return scan_doctype(tk, doc);
	}
	else
	{
		if (tk->ht_current - tk->ht_begin > 2)
		{
			if (emit_text(tk, doc, tk->ht_current - 2) != 0)
			{
				return -1;
			};
			tk->ht_state = scan_markup_decl_open;
			return 0;
		}
		tk->ht_current--; /* preserve the '!' from '<!' */
		return scan_bogus_comment(tk, doc);
	}
}

html_tokenizer_t* html_tokenizer_create(struct mem_pool_t *pool)
{
	html_tokenizer_t *tokenizer = NULL;
	assert(pool);
	tokenizer = (html_tokenizer_t*) palloc(pool, sizeof(*tokenizer));
	if (tokenizer == NULL)
	{
		return NULL;
	}
	memset(tokenizer, 0, sizeof(*tokenizer));
	return tokenizer;
}

void html_tokenizer_reset(html_tokenizer_t *tokenizer, const char *html, size_t size)
{
	assert(tokenizer);
	tokenizer->ht_source = html;
	tokenizer->ht_begin = html;
	tokenizer->ht_current = html;
	tokenizer->ht_end = tokenizer->ht_source + size;
	tokenizer->ht_state = scan_data;
	tokenizer->ht_node = NULL;
	tokenizer->ht_attr = NULL;
	tokenizer->ht_opening = NULL;
}

/**
 * @brief ����ҳ��Դ����
 **/
html_node_t* html_tokenize(html_tokenizer_t *tokenizer, html_tree_t *doc)
{
	html_node_t *node = NULL;
	assert(tokenizer);
	assert(doc);
	while (1)
	{
		/**���������Ϊ0����Ϊ����*/
		if (tokenizer->ht_state(tokenizer, doc) != 0)
		{
			return NULL;
		}
		node = tokenizer->ht_node;
		assert(node);
		tokenizer->ht_begin = tokenizer->ht_current;
		tokenizer->ht_node = NULL;
		tokenizer->ht_attr = NULL;
		if (!node->html_tag.is_close_tag && node->html_tag.tag_name)
		{
			tokenizer->ht_opening = node;
		}
		return node;
	}
	return NULL;
}

void html_tokenizer_switch_to_rcdata(html_tokenizer_t *tokenizer)
{
	assert(tokenizer);
	tokenizer->ht_state = scan_rcdata;
}

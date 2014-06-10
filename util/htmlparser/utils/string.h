/**
 * easou_string.h
 * Description: �ַ�����صĹ��ߺ���
 *  Created on: 2011-11-23
 * Last modify: 2012-10-26 sue_zhang@staff.easou.com shuangwei_zhang@staff.easou.com
 *      Author: xunwu_chen@staff.easoucom
 *     Version: 1.2
 */
#ifndef EASOU_STRING_H_
#define EASOU_STRING_H_

#include <stddef.h>

#define MAX_TIME_STR_LEN 128
#define MAX_TIME_UNIT_NUM 8

extern int CHAR_SPACE[];
extern char legal_char_set[];
extern char url_eng_set[];
extern char legal_word_set[];

/**
 * @brief whitespace map
 **/
extern char g_whitespace_map[];

/**
 *  �жϸ������ַ��Ƿ�Ϊ�ո�\n
 *  �ַ�����ŵ�dststr�У�srcstr���ֲ���
 *  @param[in]  ch �������ַ�
 *  @return �����жϽ��
 *  - 0 ��ʾ���ǿո�
 *  - 1 ��ʾ�ǿո�
 *  @note ע��space�ַ�����'\0',���뺯��isspace��ͬ
 */
#define easou_isspace(ch) CHAR_SPACE[(unsigned char)(ch)]

/*
 * �ַ���Сдת��
 */
char easou_tolower(unsigned char chr);

/**
 * �ַ�����Сдת��
 */
int easou_trans2lower(char* lower, char* upper);

/**
 * @brief ת��Ϊ���. ���������buffer����ͬ.
 * @return int, ת����ĳ���
 **/
int easou_trans2bj(const char *in, char *out);

/**
 * @brief ת��Ϊ��Ǻ�Сд. ���������buffer����ͬ.
**/
void trans2bj_lower(const char *in, char *out);

/**
 * @brief �����ı���С�����.
 *		 	��Ϊһ��ASCII�ַ�ռһ���ı���С��λ,һ������ռ�����ı���С��λ.
 * 		�ı���С��������������ı��Ŀ��.
 * 		���ֵ������й�.
* 	@param cn_num [in/out], ���ĺ��ָ���
 * @author xunwu
 * @date 2011/06/27
 **/
int getTextSize(const char *src, int &cn_num);

/**
 * @brief	���ݱ��������ж��Ƿ����õ���. ���������.
**/
int is_valid_word(const char *p);

/**
 * @brief �ַ��Ƿ�����. ʵ�����������д���Թ���������ʽҪ��.
**/
bool q_isdigit(const char ch);

/**
 * @brief	�Ƿ�հ��ַ���.
**/
bool is_space_text(const char *text);

/**
 * @brief �ж������ַ�֮���Ƿ�ȫ�ǿո�
 */
bool is_only_space_between(const char *begin, const char *end);

/**
 * @brief ɨ��ո�
 * @param [in] pstr   : const char* �ַ�ָ��.
 * @return  const char* ������ָ�뿪ʼ�ĵ�һ���ǿո��ַ�ָ��.
 * @retval
 * @see
 * @author xunwu
 * @date 2011/06/20
 **/
const char *easou_skip_space(const char *pstr);

/**
 * @brief	�Ƿ���һ����ʾʱ����ַ���
**/
bool is_time_str(const char * str_time );


/**
 * @brief 	�ַ�������
 * @param [out] dst   : char*	Ŀ�껺����
 * @param [int] src   : const char*	src Դ������
 * @param [in] siz   : size_t Ŀ�껺�����Ĵ�С
 * @return  size_t ����Ӧ���������ַ������ȣ�ע����ܻᳬ����������С
**/
size_t easou_strlcpy(char *dst, const char *src, size_t siz);

/**
 * @brief ������Ӵ�. ���ع����Ӵ��ĳ��ȣ�����������й�.
**/
int longest_common_substring(const char *l, const char *r);

/**
 * @brief 	���Դ�Сд�����ַ���.
**/
char *easou_strcasestr(const char *haystack, const char *needle);

/**
 * @brief �����ַ����ڵĿհף�Ӣ�ĵ��ʼ�հ׳���. ����ת����ĳ���.
**/
int easou_trim_space(char *buf);

/**
 * @brief �ü��ַ������ߵĿո� ����м��пո�����ȡ���м�ĵ�1���ո񴦡���ԭ�ַ������޸ġ�
 * @return int, ����ת����ĳ���
 **/
int str_trim_side_space(char *str);

/**
 * @brief	�Ƿ��пհ��ַ�
**/
bool is_has_special_word(const char * spec_words[] , const char * word );

/**
 * @brief ȥ���ַ�����β����\n, \r, \t
 * @return ����ֵ< 0��ʧ��
 */
int remove_tail_rnt(char *line);

/**
 * @brief ���ַ����е�ȫ���ַ�ת��Ϊ���,����������
 * @return int, >=0 ת����ĳ��ȣ� <0  src is NULL
 */
int trans_full_to_half(const char *src, const int srcLen, char *dest, int *destLen);

/**
 * @brief ��ȡ��һ��gb18030�ַ��ĳ��ȣ����Ϊ����ַ������֣�����1�����Ϊȫ���ַ�����2�����Ϊ���֣�����2����4
 */
int get_next_gb18030_bytes(unsigned char* s);

/**
 * @brief ֻ�������ĺ���
 * @param src [in], �����ַ���
 * @param srcLen [in], src�ĳ���
 * @param dest [in/out], ����ֻ�������ֵ��ַ���
 * @param destLen [out], dest�ĳ���
 * @return dest������ַ����ĳ���
 */
int remain_only_chinese(const char *src, const int srcLen, char *dest, int *destLen);

#endif /* EASOU_STRING_H_ */

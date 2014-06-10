/*
 * easou_url.h
 *
 *  Created on: 2011-11-21
 *      Author: xunwu
 */

#ifndef EASOU_URL_H_
#define EASOU_URL_H_

#include "easou_string.h"

/**
 * @brief href���Ե�����
 **/
#define IS_LINK_INNER 1 /**վ��url*/
#define IS_LINK_OUT 2	/**վ��url*/
#define IS_LINK_ANCHOR 3	/**վ��url*/
#define IS_LINK_ERR (-1)/**������һ��ָjs*/

#define MAX_URL_LEN			2048
#define MAX_SITE_LEN		256
#define MAX_PATH_LEN		1600
#define MAX_PORT_LEN		7 //64k 5+:+1
#define MAX_ANCHOR_LEN		256

/*
 * @breif �Ƿ��Ǿ���url
 */
int easou_is_url(const char *url);

/**
 * @brief ��ȡ������������������com
 */
const char * fetch_maindomain_from_url(const char * url, char * domain, int size);

/**
 * @brief ��ȡ����������������com
 */
const char * fetch_domain_l_from_url(const char * url, char * domain, int size);

/**
 * @brief �ж�href�����ͣ�վ�ڣ�վ�⣬js��ERR��
 **/
int get_href_type(const char *phref, const char *base_url);

/**
 * @brief ����url���������еĸ�������
 * @param input �����url
 * @param[out] site վ����������
 * @param[out] port �˿�
 * @param[out] path ·��
 * @return 1������0��Чurl��ʽ
 */
int easou_parse_url(const char *input, char *site, char *port, char *path);

/**
 * @brief ��һ��URL·��
 * @param[in,out] path ·��
 * @return 1������0��Чurl��ʽ
 */
int easou_single_path(char *path);

/**
 * @brief ����url���������е�·������
 * @param url �����url
 * @param[out] path ·��
 * @return NULLʧ�ܣ�����Ϊָ��path��ָ��
 */
char *easou_get_path(const char *url, char *path);

/**
 * @brief ����url���������е�վ��������
 * @param url �����url
 * @param[out] site վ����( be sure it is enough,larger than UL_MAX_SITE_LEN)
 * @return NULLʧ�ܣ�����Ϊָ��site��ָ��
 */
char *easou_get_site(const char *url, char *site);

/**
 * @brief �淶��·������ʽ,��'\\', './', '/.', '../', '/..', '//'����ʽ���й淶��
 * @param[in,out] path ��ת����·��
 */
void easou_normalize_path(char *path);

/**
 * @brief get directory of path
 */
void remove_path_file_name(char *path);

/**
 * @brief ��url�л�ȡ�˿���Ϣ
 * @param url �����url
 * @param[out] pport �˿�
 * @return 1�ɹ���0ʧ��
 */
int easou_get_port(const char* url, int* pport);

/**
 * @brief ��url�л�ȡ��̬�Ĳ��֣�?����;֮ǰ�Ĳ��֣�
 * @param url �����url
 * @param[out] staticurl ��̬���ֻ�����
 */
void easou_get_static_part(const char *url, char *staticurl);

//int easou_parse_url(char *input,char *site,char *port,char *path);
//char *easou_get_path(char *url, char *path);
//char *easou_get_site(char *url, char *site);
//int easou_get_port(char* url,int* pport);
//void easou_get_static_part(char *url, char *staticurl);

/**
 * @brief �ж�url�Ƿ��Ƕ�̬url
 * @returnu 0���Ƕ�̬url����0���Ƕ�̬url
 */
int easou_isdyn(const char* str);

/**
 * @brief �ж�url�Ƿ�Ϸ������������ĳ������»��޸�url���룬����ʱ��Ҫע�⡣
 * @param url ����url
 * @returnu 1�Ϸ���0���Ϸ�
 */
int easou_check_url(char* url);

/**
 * @brief ��վ�����л�ȡ���ɲ���, ����"www.easou.com"�����"easou"
 * @param site վ����
 * @param[out] trunk ������ɲ��ֵĻ�����
 * @param size ��������С
 * @return 1�ɹ���-1δ֪����-2վ��û�����ɲ��֣�-3վ�㲻����'.'
 */
int easou_fetch_trunk(const char* site, char *trunk, int size);

/**
 * @brief ���վ�����Ƿ���IP��ַ
 * @param sitename վ����
 * @return 0���ǣ���0��
 */
int easou_is_dotip(const char *sitename);

/**
 * @brief ��վ�����л�ȡ���ɲ��֣�������ͬ@ref easou_fetch_trunk()
 * @param site վ����
 * @param[out] domain ������ɲ��ֵĻ�����
 * @param size ��������С
 * @return NULLʧ�ܣ�����Ϊָ��site���ɲ��ֵ�ָ��
 */
const char* easou_fetch_maindomain(const char* site, char *domain, int size);

/**
 * @brief ���url�Ƿ�淶��
 * @param url ����url
 * @return 1�ǣ�0����
 */
int easou_isnormalized_url(const char *url);

/**
 * @brief ��urlת��Ϊͳһ����ʽ\n
 * ִ��@ref easou_normalize_site, @ref easou_normalize_port, @ref easou_single_path, @ref easou_normalize_path
 * @param url ��ת����url
 * @param[out] buf ת�����url������
 * @return 1�ɹ���0��Чurl
 * @note you can use easou_normalize_url(buf, buf) to save an extra buffer.
 */
int easou_normalize_url(const char* url, char* buf);

/**
 * @brief ��վ�������й淶������дתΪСд��
 * @param site վ����
 * @return 1�ɹ���0ʧ��
 */
int easou_normalize_site(char *site);

/**
 * ���˿��ַ������й淶�������˿ڷ�Χ�Ϸ��ԣ���ȥ��80�˿ڵ��ַ�����
 * @param port ָ��Ķ˿ڵ�ָ��
 * @return 1�ɹ���0ʧ��
 */
int easou_normalize_port(char *port);

/**
 *  ����url���������еĸ�������,֧�ּӳ���url����߿�֧�ֵ�1024��path��ɵ�800��site��ɵ�128
 *
 *  @param[in]  input          �����url
 *  @param[in]  site           site�ֶεĴ洢bufָ��
 *  @param[in]  site_size      site�������Ĵ�С���ɸ��ݴ��ֶ����ú����site����,Ĭ��Ϊ128,ʹ��ʱ���ʵ���С.
 *  @param[in]  port           port�ֶεĴ洢bufָ��
 *  @param[in]  port_size      port�ֶεĴ�С
 *  @param[in]  path           path�ֶεĴ洢bufָ��
 *  @param[in]  max_path_size  path�ֶεĴ�С,�ɸ��ݴ��ֶ����ú����path����,Ĭ��Ϊ800,ʹ��ʱ���ʵ���С.
 *  @param[out] site           siteֵ
 *  @param[out] port           portֵ
 *  @param[out] path           path·��
 *  @return �����������
 *  - 1   ��ʾ����
 *  - 0  ��Чurl��ʽ
 *  - @note Ϊ��֤����ȫ,�����buf��С��Ĭ�����ֵ
 */
int easou_parse_url_ex(const char *input, char *site, size_t site_size, char *port, size_t port_size, char *path, size_t max_path_size);

/**
 *  ����url���������е�·������,֧�ּӳ���url����߿�֧�ֵ�1024��path��ɵ�800��site��ɵ�128
 *
 *  @param[in]  url          �����url
 *  @param[in]  path         site�ֶεĴ洢bufָ��
 *  @param[in]  path_size    path�ֶεĴ�С,�ɸ��ݴ��ֶ����ú����path����,Ĭ��Ϊ800,ʹ��ʱ���ʵ���С.
 *  @param[out] path         path·��
 *  @return �����������
 *  - ��NULL   ָ��·����ָ��
 *  - NULL     ��ʾʧ��
 *  - @note Ϊ��֤����ȫ,�����path_size��С��Ĭ�����ֵ
 */
char *easou_get_path_ex(const char *url, char *path, size_t path_size);

/**
 *  ����url���������е�վ��������,֧�ּӳ���url����߿�֧�ֵ�1024��path��ɵ�800��site��ɵ�128
 *
 *  @param[in]  url            �����url
 *  @param[in]  site           site�ֶεĴ洢bufָ��
 *  @param[in]  site_size      site�������Ĵ�С���ɸ��ݴ��ֶ����ú����site����,Ĭ��Ϊ128,ʹ��ʱ���ʵ���С.
 *  @param[out] site           siteֵ
 *  @return �����������
 *  - ��NULL   ָ��site��ָ��
 *  - NULL     ��ʾʧ��
 *  - @note Ϊ��֤����ȫ,�����site_size��С��Ĭ�����ֵ
 */
char *easou_get_site_ex(const char *url, char *site, size_t site_size);

/**
 *  ��url�л�ȡ�˿���Ϣ,֧�ּӳ���url����߿�֧�ֵ�1024��path��ɵ�800��site��ɵ�128
 *
 *  @param[in]  input          �����url
 *  @param[in]  pport          port�ֶεĴ洢bufָ��
 *  @param[out] pport          portֵ
 *  @return �����������
 *  - 1   ��ʾ�ɹ�
 *  - 0   ��ʾʧ��
 */
int easou_get_port_ex(const char* url, int* pport);

/**
 *  ��urlת��Ϊͳһ����ʽ\n,֧�ּӳ���url����߿�֧�ֵ�1024��path��ɵ�800��site��ɵ�128
 *  ִ��@ref easou_normalize_site, @ref easou_normalize_port, @ref easou_single_path, @ref easou_normalize_path
 *
 *  @param[in]  url           ��ת����url
 *  @param[in]  buf           ת�����url������
 *  @param[in]  buf_size      buf�Ĵ�С
 *  @param[out] buf           ת�����url
 *  @return �����������
 *  - 1   �ɹ�
 *  - 0   ��Чurl
 *  - @note Ϊ��֤����ȫ,�����site_size��С��Ĭ�����ֵ
 */
int easou_normalize_url_ex(const char* url, char* buf, size_t buf_size);

/**
 *  ��url�л�ȡ��̬�Ĳ��֣�?����;֮ǰ�Ĳ��֣�,֧�ּӳ���url����߿�֧�ֵ�1024��path��ɵ�800��site��ɵ�128
 *
 *  @param[in]  url                 �����url
 *  @param[in]  staticurl           ��̬���ֻ�����
 *  @param[in]  staticurl_size      buf�Ĵ�С
 *  @param[out] staticurl           ��̬����
 *  @return ��
 */
void easou_get_static_part_ex(const char *url, char *staticurl, size_t staticurl_size);

/**
 *  ���url�Ƿ�淶��,֧�ּӳ���url����߿�֧�ֵ�1024��path��ɵ�800��site��ɵ�128
 *
 *  @param[in]  url                 ����url
 *  @param[out] ��
 *  @return �����жϽ��
 *  - 1   ��
 *  - 0   ����
 */
int easou_isnormalized_url_ex(const char *url);

/**
 *  �淶��·������ʽ\n,֧�ּӳ���url����߿�֧�ֵ�1024��path��ɵ�800��site��ɵ�128
 * ��'\\', './', '/.', '../', '/..', '//'����ʽ���й淶��
 *
 *  @param[in]  path           ��ת����·��
 *  @param[out] path           ת�����·��
 *  @return ��
 */
void easou_normalize_path_ex(char *path);

/**
 *  ��һ��URL·��,֧�ּӳ���url����߿�֧�ֵ�1024��path��ɵ�800��site��ɵ�128
 *
 *  @param[in]  path         path·��
 *  @param[out] path         ��һ������·��
 *  @return ���ع�һ�����
 *  - 1   ����
 *  - 0   ��Чurl��ʽ·��
 */
int easou_single_path_ex(char *path);

/**
 *  �ж�url�Ƿ�Ϸ�,֧�ּӳ���url����߿�֧�ֵ�1024��path��ɵ�800��site��ɵ�128
 *
 *  @param[in]  url           ��ת����url
 *  @param[out] ��
 *  @return �����������
 *  - 1   �Ϸ�
 *  - 0   ���Ϸ�
 */
int easou_check_url_ex(char *url);

/**
 *  ����url���������еĸ�������,֧�ּӳ���url����߿�֧�ֵ�2048��path��ɵ�1600��site��ɵ�256
 *
 *  @param[in]  input          �����url
 *  @param[in]  site           site�ֶεĴ洢bufָ��
 *  @param[in]  site_size      site�������Ĵ�С���ɸ��ݴ��ֶ����ú����site����,Ĭ��Ϊ256,ʹ��ʱ���ʵ���С.
 *  @param[in]  port           port�ֶεĴ洢bufָ��
 *  @param[in]  port_size      port�ֶεĴ�С
 *  @param[in]  path           path�ֶεĴ洢bufָ��
 *  @param[in]  max_path_size  path�ֶεĴ�С,�ɸ��ݴ��ֶ����ú����path����,Ĭ��Ϊ1600,ʹ��ʱ���ʵ���С.
 *  @param[out] site           siteֵ
 *  @param[out] port           portֵ
 *  @param[out] path           path·��
 *  @return �����������
 *  - 1   ��ʾ����
 *  - 0  ��Чurl��ʽ
 *  - @note Ϊ��֤����ȫ,�����buf��С��Ĭ�����ֵ
 */
int easou_parse_url_ex2(const char *input, char *site, size_t site_size, char *port, size_t port_size, char *path, size_t max_path_size);
/**
 *  ����url���������е�·������,֧�ּӳ���url����߿�֧�ֵ�2048��path��ɵ�1600��site��ɵ�256
 *
 *  @param[in]  url          �����url
 *  @param[in]  path         site�ֶεĴ洢bufָ��
 *  @param[in]  path_size    path�ֶεĴ�С,�ɸ��ݴ��ֶ����ú����path����,Ĭ��Ϊ1600,ʹ��ʱ���ʵ���С.
 *  @param[out] path         path·��
 *  @return �����������
 *  - ��NULL   ָ��·����ָ��
 *  - NULL     ��ʾʧ��
 *  - @note Ϊ��֤����ȫ,�����path_size��С��Ĭ�����ֵ
 */
char *easou_get_path_ex2(const char *url, char *path, size_t path_size);

/**
 *  ����url���������е�վ��������,֧�ּӳ���url����߿�֧�ֵ�2048��path��ɵ�1600��site��ɵ�256
 *
 *  @param[in]  url            �����url
 *  @param[in]  site           site�ֶεĴ洢bufָ��
 *  @param[in]  site_size      site�������Ĵ�С���ɸ��ݴ��ֶ����ú����site����,Ĭ��Ϊ256,ʹ��ʱ���ʵ���С.
 *  @param[out] site           siteֵ
 *  @return �����������
 *  - ��NULL   ָ��site��ָ��
 *  - NULL     ��ʾʧ��
 *  - @note Ϊ��֤����ȫ,�����site_size��С��Ĭ�����ֵ
 */
char *easou_get_site_ex2(const char *url, char *site, size_t site_size);

/**
 *  ��url�л�ȡ�˿���Ϣ,֧�ּӳ���url����߿�֧�ֵ�2048��path��ɵ�1600��site��ɵ�256
 *
 *  @param[in]  input          �����url
 *  @param[in]  pport          port�ֶεĴ洢bufָ��
 *  @param[out] pport          portֵ
 *  @return �����������
 *  - 1   ��ʾ�ɹ�
 *  - 0   ��ʾʧ��
 */
int easou_get_port_ex2(const char* url, int* pport);

/**
 *  ��urlת��Ϊͳһ����ʽ\n,֧�ּӳ���url����߿�֧�ֵ�2048��path��ɵ�1600��site��ɵ�256
 *  ִ��@ref easou_normalize_site, @ref easou_normalize_port, @ref easou_single_path, @ref easou_normalize_path
 *
 *  @param[in]  url           ��ת����url
 *  @param[in]  buf           ת�����url������
 *  @param[in]  buf_size      buf�Ĵ�С
 *  @param[out] buf           ת�����url
 *  @return �����������
 *  - 1   �ɹ�
 *  - 0   ��Чurl
 *  - @note Ϊ��֤����ȫ,�����site_size��С��Ĭ�����ֵ
 */
int easou_normalize_url_ex2(const char* url, char* buf, size_t buf_size);

/**
 *  ��url�л�ȡ��̬�Ĳ��֣�?����;֮ǰ�Ĳ��֣�,֧�ּӳ���url����߿�֧�ֵ�2048��path��ɵ�1600��site��ɵ�256
 *
 *  @param[in]  url                 �����url
 *  @param[in]  staticurl           ��̬���ֻ�����
 *  @param[in]  staticurl_size      buf�Ĵ�С
 *  @param[out] staticurl           ��̬����
 *  @return ��
 */
void easou_get_static_part_ex2(const char *url, char *staticurl, size_t staticurl_size);

/**
 *  ���url�Ƿ�淶��,֧�ּӳ���url����߿�֧�ֵ�2048��path��ɵ�1600��site��ɵ�256
 *
 *  @param[in]  url                 ����url
 *  @param[out] ��
 *  @return �����жϽ��
 *  - 1   ��
 *  - 0   ����
 */
int easou_isnormalized_url_ex2(const char *url);

/**
 *  �淶��·������ʽ\n,֧�ּӳ���url����߿�֧�ֵ�2048��path��ɵ�1600��site��ɵ�256
 * ��'\\', './', '/.', '../', '/..', '//'����ʽ���й淶��
 *
 *  @param[in]  path           ��ת����·��
 *  @param[out] path           ת�����·��
 *  @return ��
 */
void easou_normalize_path_ex2(char *path);

/**
 *  ��һ��URL·��,֧�ּӳ���url����߿�֧�ֵ�2048��path��ɵ�1600��site��ɵ�256
 *
 *  @param[in]  path         path·��
 *  @param[out] path         ��һ������·��
 *  @return ���ع�һ�����
 *  - 1   ����
 *  - 0   ��Чurl��ʽ·��
 */
int easou_single_path_ex2(char *path);

/**
 *  �ж�url�Ƿ�Ϸ�,֧�ּӳ���url����߿�֧�ֵ�2048��path��ɵ�1600��site��ɵ�256
 *
 *  @param[in]  url           ��ת����url
 *  @param[out] ��
 *  @return �����������
 *  - 1   �Ϸ�
 *  - 0   ���Ϸ�
 */
int easou_check_url_ex2(char *url);

/**
 * @brief ���url���
 */
int get_url_depth(const char * url);

/**
 * @brief ҳ���ڵ����URLƴ��һ������URL
 */
int easou_combine_url(char *result_url, const char *base_url, const char *relative_url);
void easou_combine_url_inner(char *url, char *domain, char *port, char *path);

/**
 * @brief �ǲ���������ҳ��url
 **/
int is_like_top_url(const char *url);

#define MAX_DIR_NUM 20
#define MAX_PARAM_NUM 10

typedef struct _url_t
{
	const char *site;
	int site_len;

	const char *port;
	int port_len;

	const char *path;
	int path_len;

	int dir_num;
	const char *dir[MAX_DIR_NUM];
	int dir_len[MAX_DIR_NUM];

	int param_num;
	const char *name[MAX_PARAM_NUM];
	int name_len[MAX_PARAM_NUM];
	const char *value[MAX_PARAM_NUM];
	int value_len[MAX_PARAM_NUM];
} url_t;

#define MAX_ELE_LEN 200
#define MAX_DELIM_LEN 256
#define MAX_PATTERN_NUM 32

#define DIGIT 1
#define ALPHA 2
#define CHN_1 3
#define CHN_2 4
#define DELIM 5
#define EASOU_URL_OTHER 6

#define ELE_DIR 0
#define ELE_FILE 1
#define ELE_NAME 2
#define ELE_VALUE 3
#define MAX_SHIFT_NUM 1
#define MAX_SEPS_NUM 200

typedef struct _p_element_t
{
	int type;
	char raw[MAX_ELE_LEN];
	char regular[MAX_ELE_LEN];
	int pattern_len;
} element;

typedef struct _p_url_t
{
	char site[MAX_SITE_LEN];
	int site_len;
	char port[MAX_PORT_LEN];
	int port_len;
	element dir[MAX_DIR_NUM];
	int dir_num;
	bool has_file;
	element file;
	int param_num;
	element name[MAX_PARAM_NUM];
	element value[MAX_PARAM_NUM];
} p_url;

const char *parse_url_inner(url_t *url, const char *url_buffer, int length);

int easou_single_path_inner(char *path);

/**
 * @brief �ж��Ƿ���ҳ
 */
bool is_home_page(const char* url, const int urlLen);

/**
 * @brief �ж�url�Ƿ���Ŀ¼
 * @return int
 * @retval 1, Ŀ¼
 * @retval 0, ��Ŀ¼
 */
int is_dir_url(const char *url);

/**
 * @brief url���Ƿ���ڷ�ascii �ַ�
 */
bool has_not_ascii_char(const unsigned char* url);

/**
 * @brief ��urlת���������ַ���
 * @return 0, �ɹ�; -1, ʧ��
 */
int urlstr_to_regexstr(const char *url, int url_len, char *buf, int buf_len);

#endif /* EASOU_URL_H_ */

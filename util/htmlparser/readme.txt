Version:parser1.3.2.7.1

�汾���ԣ�
css server interface chnage. 
1. add free interface.
2. add global lock to make sure it will be init only once in one process.

�仯��
include/easou_vhtml_tree.h
lib/libvhtmlparser.a

=======================================================================

Version:parser1.3.2.7

�汾���ԣ�
1.���汾parser1.3.2.6��汾parser1.3.1.9 merge��
  ��Ҫ����parser1.3.2.6�Ļ����������˳�ȡ�������ӵ�ַ��������������-IFRAME�Լ�ͼƬ���ӵ���ȡ;
  ���⣬��xpath�ӿ�����С�޸�-��easou_xpath.cpp���õ�1000��Ϊ5000

=======================================================================
Version: parser1.3.2.6

�汾���ԣ�
1. ����css�ļ���ȡͳ����Ϣ
M       easou_vhtml_tree.cpp
M       easou_vhtml_basic.h

=======================================================================
Version: parser1.3.2.5

�汾���ԣ�
1. tmplt_apply_exreg_rule��¶Ϊ�ӿڣ���һЩ�����޸�
2. tmplt_extract_result_t����ex_reg��Ա
���Ͻӿڸı���Ҫ�Ƿ�����ʹ��node listʱ��Ҳ��Ӧ��ģ��������ų�����

�޸���	extractor/easou_extractor_template.h
	extractor/easou_extractor_template.cpp

=======================================================================
Version: parser1.3.2.4

�汾���ԣ�
1. �����ȡcase�޸ģ�ȥ��realtitle��ȡβ����( { �ַ�
�޸��� extractor/easou_extractor_com.cpp
2. ģ���ȡcase�޸ģ��޸ĸ���ҳ�����ݳ�ȡ����������
3. coredump�޸ģ��޸ļ���������»���ֵ�assert fail���⣨ͬparser1.3.1.7�е������޸ģ�
�޸��� extractor/easou_extractor_template.cpp
4. ע�͵�һЩ��־���
�޸��� cssparser/easou_css_parser_inner.cpp

=======================================================================
Version: parser1.3.2.3

�汾���ԣ�
1. �޸�parser1.3.2.2��ģ���ȡż����core������
�޸���	extractor/easou_extractor_template.cpp

2. ��԰���ҳ�Ľ�realtitle��ȡ�����磺���Ʋ��ɣ� realtitle��ȡΪ ���Ʋ��
�޸���	extractor/easou_extractor_title.h
	extractor/easou_extractor_title.cpp

=======================================================================
Version: parser1.3.2.2

�汾���ԣ�
1. ����ģ���ȡ����һҳ�ӿڣ�������һЩ����Ľ���

Ч�ʲ��ԣ�
1. ģ���ʼ��ʱ�� <1s����������²���ֵ500ms���ң������mysql���غܸߣ�����ʱ����á�
2. �Ƿ�����ģ�� < 1ms
3. ģ���ȡ < 1ms

allnum:2100 alltime:11627972us avg:5537.13us
tmplt_try:2100 tmplt_no_hit:1999 tmplt_rt_fail:11 tmplt_ac_fail:4 tmplt_hit:86 tmlt_pic_num:253
all:2100
               area_partition: avg:411.32    us
              html_tree_parse: avg:799.64    us
   html_vtree_parse_with_tree: avg:792.21    us
               mark_area_tree: avg:2280.69   us
                        parse: avg:4300.30   us
                template:entry avg:44.35     us
              template:extract avg:14.46     us

====================ͬparser1.3.2.1�Ƚϣ��޸ĵ���======================
htmlparser
	easou_html_extractor.h|cpp ����int html_tree_extract_link(html_node_list_t* list, char* baseUrl, link_t* link, int& num);����
extractor
	�޸���easou_extractor_template.h|cpp
	
����δ�޸�
utils
cssparser
vhtmlparser
ahtmlparser
markparser
interface
pagetype
bbsparser
linktype

===================��ע=======================
������pageclassify1.5.0.1�����ϰ汾
������wapResClassify2.1.0.1�����ϰ汾

cc_library(name = 'prefix_tree',
               srcs = ['prefix_tree.cc',
                           ],
               deps = [
                      '//base:base',
                      '//file:simple_line_reader',
                          ]
           )

cc_library(name = 'trie',
          srcs = ['trie.cc',
                  'trie_node.cc',
                  ],
         )

cc_binary(name = 'sample',
               srcs = ['sample.cc',
                           ],
               deps = [
                      '//base:base',
                      ':prefix_tree',
                          ]
           )

cc_test(name = 'prefix_tree_unittest',
               srcs = ['prefix_tree_unittest.cc',
                           ],
               deps = [
                      '//base:base',
                      ':prefix_tree',
                          ]
           )

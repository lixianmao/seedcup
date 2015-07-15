#ifndef PARSER_HTML_H_
#define PARSER_HTML_H_

#include <string>

namespace html {

class parser_html {
private:
    class Node {
        private:
            enum TYPE {ELEMETN = 1,ATTRIBUTE,TEXT,DOCMENT};
            std::string m_nodeName;
            TYPE m_nodeType;
          //  cssAttribute m_cssAttribute;
        public:
            std::string m_nodeValue;
    };
    struct dom_node { //HTML DOM 节点
        dom_node *parent;
        dom_node *first_child, *last_child;
        dom_node *prev_sibling, *next_sibling;
        Node data;
    };
    void parser(std::string filename);
public:
    parser_html(std::string filename);
    ~parser_html();

};

}
#endif

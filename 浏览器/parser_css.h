#ifndef PARSER_CSS_H_
#define PARSER_CSS_H_

#include <map>
#include <string>
#include <list>
#include <vector>
#include <stack>
#include "CommonClass.h"

namespace css{

class parser_css {
public:
    parser_css(std::string filename);
    ~parser_css();
    std::vector<CSSRule>& getCSSRule() {return rule;}
private:
    struct CSSPreRule {
        std::string perSelector;
        struct property_node *perProperty;
        CSSPreRule(std::string str,property_node *p):perSelector(str),perProperty(p) {}
    };
    void BuildStyleSheet(std::string filename);
    bool Joint(parser_css &x);
    void Trim(std::string &s);
    void BuildCSSRuleTree();
    void DealWithString(std::string &s);
    void DealWithCSSFault(std::string &s);
    std::string::size_type IsAttribute(std::string &s);
    CSSParserSelector* DealWithSelector(std::string &token);
    //typedef std::map<std::string,struct property_node*> styleSheetMap;
    //styleSheetMap styleSheet; //这里改为list或vector
    std::vector<struct CSSPreRule> styleSheet;
    std::vector<CSSRule> rule;
};

}

#endif

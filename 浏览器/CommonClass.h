#ifndef COMMONCLASS_H_
#define COMMONCLASS_H_


struct property_node {
     std::string m_name; //属性名字
     std::string m_value; //属性值
     struct property_node *next; //下一个属性
     property_node():m_name(""),m_value(""),next(NULL) {}
};

struct CSSParserSelector {
     enum TYPE {ID,CLASS,ELEMENT};
     enum RELATION {NONE,DESCENDANT,CHILD,AND};
     TYPE m_Type;
     std::string m_Name;
     RELATION m_Relation;
     struct CSSParserSelector *ptr;
     CSSParserSelector():m_Relation(NONE),ptr(NULL) {}
};

struct CSSRule {
     CSSParserSelector *selector;
     property_node *property;
     CSSRule() {}
     CSSRule(CSSParserSelector *sel,property_node *prop):selector(sel),property(prop) {}
};

#endif

#include "parser_css.h"
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <sstream>

namespace css {
parser_css::parser_css(std::string filename)
{
     BuildStyleSheet(filename);
     BuildCSSRuleTree();
}

parser_css::~parser_css()
{

}

bool parser_css::Joint(parser_css &x)
{
     return true;
}

void parser_css::Trim(std::string &s)
{
     if(s.empty()) {
          return;
     }
     std::string temp(s);
     int i = 0;
     while(isspace(s[i])) i++;
     s = s.substr(i);
     i = s.length()-1;
     while(isspace(s[i])) i--;
     s = s.substr(0,i+1);
     //s.erase(0,s.find_first_not_of(" "));
     //s.erase(s.find_last_not_of(" ")+1);
     //s.erase(0,s.find_first_not_of('\t'));
     //s.erase(s.find_last_not_of('\t')+1);
}

void parser_css::DealWithString(std::string &s)
{
     Trim(s);
     std::string str(s);
     std::string temp1;
     std::string temp2;
     std::string::size_type index;
     for(size_t i=0; i<str.length(); i++) {
          if(str[i] == '>' || str[i] == ',') {
               temp1 = temp1 + " " + str[i] + " ";
          } else if(str[i] == '#' || str[i] == '.') {
               if(i > 0 && str[i-1] != ' ') {
                    temp1 = temp1 + " " + "&" + " " + str[i];  //处理#id.class的情况
               } else {
                  temp1 += str[i];
               }

          } else {
               temp1 += str[i];
          }
     }

     for(size_t i=0; i<temp1.length(); i++) {
          if(temp1[i] != ' ') {
               temp2 += temp1[i];
          } else {
               if(temp2.back() == ' ') {
                    continue;
               } else {
                    temp2 += temp1[i];
               }
          }
     }
     s = temp2;
}


CSSParserSelector* parser_css::DealWithSelector(std::string &token)
{
     CSSParserSelector *p = new CSSParserSelector();
     if(token[0] == '#') {
          p->m_Type = CSSParserSelector::ID;
          p->m_Name = token.substr(1);
     } else if(token[0] == '.') {
          p->m_Type = CSSParserSelector::CLASS;
          p->m_Name = token.substr(1);
     } else {
          p->m_Type = CSSParserSelector::ELEMENT;
          p->m_Name = token;
     }
     return p;
}

std::string::size_type parser_css::IsAttribute(std::string &s)
{
    std::string::size_type index;
    if((index = s.find("display")) != std::string::npos) {
        return index;
    } else if((index = s.find("position")) != std::string::npos) {
        return index;
    } else if((index = s.find("width")) != std::string::npos){
        return index;
    } else if((index = s.find("height")) != std::string::npos) {
        return index;
    } else if((index = s.find("padding")) != std::string::npos) {
        return index;
    } else if((index = s.find("border")) != std::string::npos) {
        return index;
    } else if((index = s.find("margin")) != std::string::npos) {
        return index;
    } else if((index = s.find("left")) != std::string::npos) {
        return index;
    } else if((index = s.find("right")) != std::string::npos) {
        return index;
    } else if((index = s.find("top")) != std::string::npos) {
        return index;
    } else if((index = s.find("bottom")) != std::string::npos) {
        return index;
    } else if((index = s.find("color")) != std::string::npos) {
        return index;
    } else if((index = s.find("line-height")) != std::string::npos) {
        return index;
    } else if((index = s.find("font-size")) != std::string::npos) {
        return index;
    } else if((index = s.find("font-style")) != std::string::npos) {
        return index;
    } else if((index = s.find("font-weight")) != std::string::npos) {
        return index;
    } else if((index = s.find("text-align")) != std::string::npos) {
        return index;
    } else if((index = s.find("line-break")) != std::string::npos) {
        return index;
    } else if((index = s.find("display")) != std::string::npos) {
        return index;
    } else {
        return index;
    }

}


void parser_css::DealWithCSSFault(std::string &s)
{
    std::string str(s);
    std::string temp_before;
    std::string temp_cur;
    std::string::size_type index;
    std::istringstream stream(str);
    s = "";
    while(stream>>temp_cur) {
        if((index = IsAttribute(temp_cur)) != std::string::npos) {
            if(index == 0) { //处理第一个元素和非合体元素
                if(temp_before == "" || temp_before == ";") {
                    s = s + temp_cur + " ";
                } else {
                    s = s + " ; " + temp_cur + " ";
                }
            } else {
                temp_before = temp_cur.substr(0,index);
                temp_cur = temp_cur.substr(index);
                s = s + " " + temp_before + " ; " + temp_cur + " ";
                temp_before = temp_cur;
            }
        } else if(temp_cur == "}") {
            if(temp_before != ";") {
                s = s + " ; " + temp_cur;
            }
        } else{
            temp_before = temp_cur;
            s = s + temp_cur + " ";
        }
    }
}

inline void parser_css::BuildStyleSheet(std::string filename)
{
     std::ifstream input(filename.c_str());
     std::string s;
     std::string temp;
     char ch;
     while(!input.eof()) {
          s = "";
          std::map<std::string,std::string> property;
          while((ch=input.get()) != EOF) {
               if(ch == '\n' || ch == '\r') {
                    continue;
               } else if(ch == '}') {
                    s = s + ' ' + ch;
                    break;
               } else if(ch == ':' || ch == ';') {
                    s = s + ' ' + ch + ' ';
               } else {
                    s += ch;
               }
          }

          std::string::size_type index = s.find('{');
          std::string select(s,0,index);
          if(select == "")
               break;
          DealWithString(select);
          // std::cout<<select;//<<std::endl;
          //styleSheet[select] = NULL;
          s = s.substr(index+1);
          DealWithCSSFault(s);
          //std::cout<<std::endl<<s<<std::endl;
          while((index = s.find(';')) != std::string::npos) {
               std::string str;
               std::string::size_type index1 = s.find(':');
               str.assign(s,0,index1);
               std::string temp(s,index1+1,index-index1-1);
               Trim(str);
               Trim(temp);
               property[str] = temp;
               //std::cout<<str<<111<<temp<<std::endl;
               s = s.substr(index+1);
          }

          property_node *q;
          property_node *start = NULL;
          for(std::map<std::string,std::string>::const_iterator iter = property.begin();
                    iter != property.end(); ++iter) {
               q = new property_node();
               q->m_name = iter->first;
               q->m_value = iter->second;
               q->next = start;
               start = q;
               //std::cout<<8<<iter->first<<8<<" "<<8<<iter->second<<8<<std::endl;
          }
          //styleSheet[select] = start;
          styleSheet.push_back(CSSPreRule(select,start));
     }
}

inline void parser_css::BuildCSSRuleTree()
{
     std::stack<CSSParserSelector*> stk;
     std::vector<CSSParserSelector*> parserSelector;
     for(std::vector<CSSPreRule>::iterator iter = styleSheet.begin();
               iter != styleSheet.end(); ++iter) {
          std::string selector = (*iter).perSelector;
          std::string token;
          /*std::cout<<1<<selector<<1<<std::endl;
          property_node *p = iter->perProperty;
          while(p) {
                std::cout<<p->m_name<<p->m_value<<std::endl;
                p = p->next;
          }*/
          std::istringstream stream(selector);
loop:
          if(stream>>token) {
               stk.push(DealWithSelector(token));
          }
          while(stream>>token) {
               if(token == ",") {
                    parserSelector.push_back(stk.top());
                    stk.pop();
                    goto loop;
               } else if(token == ">" && stream>>token) {
                    CSSParserSelector *p = DealWithSelector(token);
                    CSSParserSelector *q = stk.top();
                    stk.pop();
                    p->m_Relation = CSSParserSelector::CHILD;
                    p->ptr = q;
                    stk.push(p);
               } else if(token == "&" &&stream>>token) { //处理#id.class的情况
                    CSSParserSelector *p = DealWithSelector(token);
                    CSSParserSelector *q = stk.top();
                    stk.pop();
                    p->m_Relation = CSSParserSelector::AND;
                    p->ptr = q;
                    stk.push(p);
               } else { //blank
                    CSSParserSelector *p = DealWithSelector(token);
                    CSSParserSelector *q = stk.top();
                    stk.pop();
                    p->m_Relation = CSSParserSelector::DESCENDANT;
                    p->ptr = q;
                    stk.push(p);
               }
          }
          parserSelector.push_back(stk.top());
          stk.pop();
          for(std::vector<CSSParserSelector*>::iterator iter1 = parserSelector.begin();
                    iter1 != parserSelector.end(); ++iter1) {
               //std::cout<<1<<(*iter1)->m_Type<<1<<(*iter1)->m_Name<<1<<std::endl;
               rule.push_back(CSSRule(*iter1,(*iter).perProperty));
          }
          parserSelector.clear(); //shifouyouwenti
     }
     /*for(std::vector<CSSRule>::iterator iter2 = rule.begin(); iter2 != rule.end(); ++iter2) {
          std::cout<<iter2->selector->m_Name<<std::endl;
          CSSParserSelector *p = iter2->selector;
          while(p) {
               std::cout<<p->m_Name<<std::endl;
               p = p->ptr;
          }
     }*/
}

}


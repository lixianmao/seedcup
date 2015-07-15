#include "RenderTree.h"
#include "CommonClass.h"
#include <algorithm>

namespace flyingFish {

RenderTree::RenderTree(std::string filename)
{
     doc = reader.prepareDocument(filename);
     node = doc->firstNode;
     ChangeToRenderTree(node);
}

void RenderTree::CalculatePropertyToPx(RenderNode *node)
{
     if(node) {  //要计算positoon属性为absolute还是其他的来计算某些如width这些值。
          std::string str = node->getTypeOfRenderNode();
          if(str == "html" || str == "head" || str == "link") {
               return;
          } else {
          }
     }
}

void RenderTree::ChangeToRenderTree(RenderNode *node) //////这里死循环
{
     //CalculatePropertyToPx(node);
     if(node) {
          std::string str = node->getTypeOfRenderNode();
          if(str == "link") { //wrong or not
               css::parser_css *p = new css::parser_css(node->getNodeHref());
               parser.push_back(p);
          } else if(str == "body") {
               node->getCSSProperty().display = "block";
               node->getCSSProperty().margin = "8px 8px 8px 8px";
               //node->cssProperty.fontSize = "16px";
               //node->cssProperty.lineHeight = "1.2";
               ChangeCSSProperty(node);
          } else if(str == "div") {
               node->getCSSProperty().display = "block";
               ChangeCSSProperty(node);
          } else if(str == "p") {
               node->getCSSProperty().display = "block";
               node->getCSSProperty().margin = "lem 0 1em 0";
               ChangeCSSProperty(node);
          } else if(str == "h") {
               node->getCSSProperty().display = "block";
               node->getCSSProperty().fontSize = "32px";
               node->getCSSProperty().fontWeight = "bold";
               node->getCSSProperty().margin = "0.67em 0 0.67em 0";
               ChangeCSSProperty(node);
          } else if(str == "span") {
               ChangeCSSProperty(node);
          } else if(str == "strong") {
               node->getCSSProperty().fontWeight = "bold";
               ChangeCSSProperty(node);
          } else if(str == "em") {
               node->getCSSProperty().fontStyle = "italic";
               ChangeCSSProperty(node);
          } else if(str == "text") { //text 这里还需要处理
               ChangeCSSProperty(node);
          }
          std::vector<RenderNode*> children = *(node->getChildNodes());
          std::vector<RenderNode*>::iterator i;
          for(i = children.begin(); i != children.end(); i++) {
               ChangeToRenderTree(*i);
          }
     }
}

void RenderTree::SetPropertyValue(RenderNode *node,property_node *propertyNode)
{
     if(propertyNode->m_name == "display") {
          node->getCSSProperty().display = propertyNode->m_value;
     } else if(propertyNode->m_name == "position") {
          node->getCSSProperty().position = propertyNode->m_value;;
     } else if(propertyNode->m_name == "width") {
          node->getCSSProperty().width = propertyNode->m_value;;
     } else if(propertyNode->m_name == "height") {
          node->getCSSProperty().height = propertyNode->m_value;;
     } else if(propertyNode->m_name == "padding") {
          node->getCSSProperty().padding = propertyNode->m_value;;
     } else if(propertyNode->m_name == "border") {
          node->getCSSProperty().border = propertyNode->m_value;;
     } else if(propertyNode->m_name == "margin") {
          node->getCSSProperty().margin = propertyNode->m_value;;
     } else if(propertyNode->m_name == "left") {
          node->getCSSProperty().left = propertyNode->m_value;;
     } else if(propertyNode->m_name == "right") {
          node->getCSSProperty().right = propertyNode->m_value;;
     } else if(propertyNode->m_name == "top") {
          node->getCSSProperty().top = propertyNode->m_value;;
     } else if(propertyNode->m_name == "bottom") {
          node->getCSSProperty().bottom = propertyNode->m_value;;
     } else if(propertyNode->m_name == "color") {
          node->getCSSProperty().color = propertyNode->m_value;;
     } else if(propertyNode->m_name == "line-height") {
          node->getCSSProperty().lineHeight = propertyNode->m_value;;
     } else if(propertyNode->m_name == "font-size") {
          node->getCSSProperty().fontSize = propertyNode->m_value;;
     } else if(propertyNode->m_name == "font-style") {
          node->getCSSProperty().fontStyle = propertyNode->m_value;;
     } else if(propertyNode->m_name == "font-weight") {
          node->getCSSProperty().fontWeight = propertyNode->m_value;;
     } else if(propertyNode->m_name == "text-align") {
          node->getCSSProperty().textAlign = propertyNode->m_value;;
     } else if(propertyNode->m_name == "line-break") {
          node->getCSSProperty().lineBreak = propertyNode->m_value;;
     }
}


bool RenderTree::IsSelectorMatchRenderNode(CSSParserSelector *selector,RenderNode *renderNode)
{
     std::string name = selector->m_Name;
     if(renderNode->getTypeOfRenderNode() == "html") {
          return false;
     }
     if(selector->m_Type==CSSParserSelector::ELEMENT&&name==renderNode->getTypeOfRenderNode()) {
          return true;
     }
     if(selector->m_Type==CSSParserSelector::ID&&name==renderNode->getNodeId()) {
          return true;
     }
     if(selector->m_Type==CSSParserSelector::CLASS) {
          std::vector<std::string> classNodes = renderNode->getNodeClass();
          if(classNodes.end() != find(classNodes.begin(),classNodes.end(),name)) {
               return true;
          } else {
               return false;
          }
     }
     return false;
}

bool RenderTree::IsSelectorsMatchRenderTree(CSSParserSelector *selector,RenderNode *renderNode)
{
     if(IsSelectorMatchRenderNode(selector,renderNode)) {
          if(selector->m_Relation == CSSParserSelector::NONE) {
               return true;
          } else if(selector->m_Relation == CSSParserSelector::AND) {
               return IsSelectorsMatchRenderTree(selector->ptr,renderNode);
          } else if(selector->m_Relation == CSSParserSelector::CHILD) {
               return IsSelectorsMatchRenderTree(selector->ptr,renderNode->parentNode);
          } else {
               return IsMatchDescendantRelation(selector->ptr,renderNode->parentNode);
          }
     } else {
          return false;
     }
}

bool RenderTree::IsMatchDescendantRelation(CSSParserSelector *selector,RenderNode *renderNode)
{
     while(renderNode->getTypeOfRenderNode() != "html"
               && (!IsSelectorMatchRenderNode(selector,renderNode))) {
          renderNode = renderNode->parentNode;
     }
     if(renderNode->getTypeOfRenderNode() != "html") {
          return IsSelectorsMatchRenderTree(selector,renderNode);
     } else {
          return false;
     }
}


void RenderTree::ChangeCSSProperty(RenderNode *node)
{
     std::vector<CSSRule> matchRules;
     for(std::vector<css::parser_css*>::iterator iter1 = parser.begin();
               iter1 != parser.end(); ++iter1) {
          std::vector<CSSRule> cssRule = (*iter1)->getCSSRule();
          for(std::vector<CSSRule>::iterator iter2 = cssRule.begin();
                    iter2 != cssRule.end(); ++iter2) {
               CSSParserSelector *cssParSelector = iter2->selector;
               if(IsSelectorsMatchRenderTree(cssParSelector,node)) {
                    matchRules.push_back(*iter2);
               }
          }
     }
     //继承其父亲的属性,
     if(node->parentNode->getTypeOfRenderNode() != "html") {
          node->getCSSProperty().color = node->parentNode->getCSSProperty().color;
          node->getCSSProperty().fontSize = node->parentNode->getCSSProperty().fontSize;
          node->getCSSProperty().lineHeight = node->parentNode->getCSSProperty().lineHeight;
          node->getCSSProperty().textAlign = node->parentNode->getCSSProperty().textAlign;
          node->getCSSProperty().fontStyle = node->parentNode->getCSSProperty().fontStyle;
          node->getCSSProperty().fontWeight = node->parentNode->getCSSProperty().fontWeight;
          node->getCSSProperty().lineBreak = node->parentNode->getCSSProperty().lineBreak;
     }
     //按优先级排序
     InsertSort(matchRules,matchRules.size());
     std::cout<<matchRules.size()<<std::endl;
     for(std::vector<CSSRule>::iterator iter3 = matchRules.begin();
               iter3 != matchRules.end(); ++iter3) {
          property_node *property = iter3->property;
          while(property) {
               SetPropertyValue(node,property);
               property = property->next;
          }
     }
}


bool RenderTree::IsLessEqual(CSSRule &m,CSSRule &n)
{
     CSSParserSelector *selector1 = m.selector;
     CSSParserSelector *selector2 = n.selector;
     int priority1 = 0;
     int priority2 = 0;
     while(selector1) {
          if(selector1->m_Type == CSSParserSelector::ELEMENT) {
               priority1 += 1;
          } else if(selector1->m_Type == CSSParserSelector::CLASS) {
               priority1 += 10;
          } else {
               priority1 += 100;
          }
          selector1 = selector1->ptr;
     }

     while(selector2) {
          if(selector2->m_Type == CSSParserSelector::ELEMENT) {
               priority2 += 1;
          } else if(selector2->m_Type == CSSParserSelector::CLASS) {
               priority2 += 10;
          } else {
               priority2 += 100;
          }
          selector2 = selector2->ptr;
     }
     return (priority1 <= priority2);

}

void RenderTree::InsertSort(std::vector<CSSRule> &a,size_t n)
{
     size_t i,k;
     CSSRule x;
     size_t j = 0;
     for(i=1; i<n; ++i) {
          x = a[i];
          j = 0;
          while(j<i && IsLessEqual(a[j],a[i]))
               ++j;
          for(k=i; k>j; --k) {
               a[k] = a[k-1];
          }
          a[j] = x;
     }
}

}

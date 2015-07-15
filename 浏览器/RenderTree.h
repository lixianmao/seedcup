#ifndef RENDERTREE_H_
#define RENDERTREE_H_

#include <string>
#include "parser_css.h"
#include "Document.h"
#include "HTMLReader.h"
#include "RenderNode.h"

namespace flyingFish{


class RenderTree {
public:
    RenderTree(std::string filename);
    RenderNode* getRenderNode() {return node;}
private:
    bool IsLessEqual(CSSRule &m,CSSRule &n);
    void InsertSort(std::vector<CSSRule> &a,size_t n);
    bool IsMatchDescendantRelation(CSSParserSelector*,RenderNode *);
    bool IsSelectorMatchRenderNode(CSSParserSelector *selector,RenderNode *renderNode);
    bool IsSelectorsMatchRenderTree(CSSParserSelector *selector,RenderNode *renderNode);
    void ChangeCSSProperty(RenderNode *node);
    void ChangeToRenderTree(RenderNode *node);
    void CalculatePropertyToPx(RenderNode *node);
    void SetPropertyValue(RenderNode *node,property_node *propertyNode);
    HTMLReader reader;
    Document *doc;
    RenderNode *node;
    std::vector<css::parser_css*> parser;
};


}
#endif

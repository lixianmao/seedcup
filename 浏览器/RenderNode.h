#ifndef RENDERNODE_H
#define RENDERNODE_H

#include <iostream>
#include <string>
#include <vector>
#include "parser_css.h"

class RenderNode {
private:
     struct CSSProperty {
          std::string offsetLeft;
          std::string offsetTop;
          std::string display;
          std::string position;
          std::string width;
          std::string height;
          std::string padding;
          std::string border;
          std::string margin;
          std::string left;
          std::string right;
          std::string top;
          std::string bottom;
          std::string color;
          std::string lineHeight;
          std::string fontSize;
          std::string fontStyle;
          std::string fontWeight;
          std::string textAlign;
          std::string lineBreak;
          CSSProperty() {
               offsetLeft = "0";
               offsetTop = "0";
               display = "inline";
               position = "static";
               width = "auto";
               height = "auto";
               padding = "0 0 0 0";
               border = "1px 1px 1px 1px";
               margin = "0 0 0 0";
               left = "auto";
               right = "auto";
               top = "auto";
               bottom = "auto";
               color = "#000";//以下的都可以继承
               lineHeight = "1.2";
               fontSize = "16px";
               fontStyle = "normal";
               fontWeight = "normal";
               textAlign = "left";
               lineBreak = "normal";
          }
     };
     std::string typeOfNode;
     std::vector<RenderNode*> *childNodes;
     std::string nodeId;
     std::vector<std::string> nodeClass;
     std::string nodeHref;
     std::string nodeText;
     bool isOpen;
    CSSProperty cssProperty;
public:
    RenderNode *parentNode;
     RenderNode();
     virtual ~RenderNode();

     void setTypeOfRenderNode(std::string typeToSet);
     std::string getTypeOfRenderNode();

     void setParentNode(RenderNode *nodeToSet);
     RenderNode* getParentNode();

     void addChildNode(RenderNode *nodeToAdd);
     std::vector<RenderNode*> *getChildNodes();

     void setNodeId(std::string idToSet);
     std::string getNodeId();

     void setNodeClass(std::vector<std::string> classToSet);
     std::vector<std::string> getNodeClass();

     void setNodeHref(std::string hrefToSet);
     std::string getNodeHref();

     void setIsOpen(bool valueToSet);
     bool getIsOpen();

     void setNodeText(std::string textToSet);
     std::string getNodeText();
     CSSProperty& getCSSProperty() {return cssProperty;}
};


#endif // RENDERNODE_H


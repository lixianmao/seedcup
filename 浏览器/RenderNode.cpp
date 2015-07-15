#include <vector>
#include "RenderNode.h"

RenderNode::RenderNode()
{
    childNodes = new std::vector<RenderNode*>;
}

RenderNode::~RenderNode()
{
    //
}

void RenderNode::setTypeOfRenderNode(std::string typeToSet)
{
    typeOfNode = typeToSet;
}

std::string RenderNode::getTypeOfRenderNode()
{
    return typeOfNode;
}

void RenderNode::setParentNode(RenderNode *nodeToSet)
{
    parentNode = nodeToSet;
}

RenderNode* RenderNode::getParentNode()
{
    return parentNode;
}

void RenderNode::addChildNode(RenderNode *nodeToAdd)
{
    childNodes->push_back(nodeToAdd);
}

std::vector<RenderNode*>* RenderNode::getChildNodes()
{
    return childNodes;
}

void RenderNode::setNodeId(std::string idToSet)
{
    nodeId = idToSet;
}
std::string RenderNode::getNodeId()
{
    return nodeId;
}

void RenderNode::setNodeClass(std::vector<std::string> classToSet)
{
    nodeClass = classToSet;
}
std::vector<std::string> RenderNode::getNodeClass()
{
    return nodeClass;
}

void RenderNode::setNodeHref(std::string hrefToSet)
{
    nodeHref = hrefToSet;
}
std::string RenderNode::getNodeHref()
{
    return nodeHref;
}

void RenderNode::setIsOpen(bool valueToSet)
{
    isOpen = valueToSet;
}
bool RenderNode::getIsOpen()
{
    return isOpen;
}

void RenderNode::setNodeText(std::string textToSet)
{
    nodeText = textToSet;
}

std::string RenderNode::getNodeText()
{
    return nodeText;
}



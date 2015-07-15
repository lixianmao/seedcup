#include "Document.h"

Document::Document()
{
    firstNode = new RenderNode;
    firstNode = NULL;
}

Document::~Document()
{
    clearTree();
}

RenderNode* Document::getFirstNode()
{
    return firstNode;
}

void Document::constructTree(RenderNode *parentNode, RenderNode *childNode)
{
    if(parentNode == NULL)
    {
        parentNode = firstNode;
    }
    if(firstNode == NULL)
    {
        firstNode = childNode;
    }
    else
    {
        childNode->setParentNode(parentNode);
        parentNode->addChildNode(childNode);
    }
}

void Document::clearTree()
{
    delete firstNode;
    firstNode = new RenderNode;
    firstNode = NULL;
}


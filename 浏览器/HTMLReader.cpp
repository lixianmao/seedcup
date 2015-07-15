#include "HTMLReader.h"
#include <fstream>
#include <cctype>
#include <iostream>

HTMLReader::HTMLReader()
{
    currentNode = new RenderNode;
    currentNode = NULL;
    currentParentNode = new RenderNode;
}

HTMLReader::~HTMLReader()
{
    delete document;
}

Document* HTMLReader::parseDocumentText(std::string documentText)
{
    document = new Document;
    parseState currentState = initialMode;
    std::string::iterator iter;

    for(iter = documentText.begin(); iter != documentText.end(); iter++)
    {
        switch(currentState)
        {
            case initialMode:
            {
                if(*iter == '<')
                {
                    currentState = tagOpen;
                }
                break;
            }
            case tagOpen:
            {
                if(*iter == '/')
                {
                    currentState = endTagName;
                }
                else if(isalpha(*iter))
                {
                    iter--;
                    currentState = tagName;
                }
                break;
            }
            case tagName:
            {
                std::string tag = getTagName(iter, currentState);
                //TODO
                if(currentNode != NULL && currentNode->getIsOpen())
                {
                    currentParentNode = currentNode;
                }
                currentNode = createNode(tag);

                document->constructTree(currentParentNode, currentNode);
                if(!currentNode->getIsOpen()) // 类似于<link />
                {
                    currentNode = currentParentNode;
                    currentParentNode = currentNode->getParentNode();   //add diy
                }
                break;
            }
            case endTagName:
            {
                if(*iter == '>')
                {
                    currentState = endTagOpen;
                }
                else
                {
                    std::string tagEnd;
                    while(currentState == endTagName)
                    {
                        if(*iter == '>')
                        {
                            currentState = endTagOpen;
                        }
                        else
                        {
                            tagEnd.push_back(*iter);
                            iter++;
                        }
                    }
                    if(tagEnd == "html")
                    {
                        return document;
                    }
                    //下面这段代码还需要研究 move to find the closed node
                    while(!parentNodeClosed(currentNode, tagEnd));
                    currentParentNode = currentNode->getParentNode();
                    //currentState = text;
                }
                break;
            }
            case endTagOpen:
            {
                if(*iter == '<')
                {
                    currentState = tagOpen;
                }
                else if(*iter != ' ')
                {
                    currentState = text;
                    iter--;
                }
                break;
            }
            case text:
            {
                std::string textToSet = getTextString(iter, currentState);
                if(currentNode != NULL && currentNode->getIsOpen())
                {
                    currentParentNode = currentNode;
                }
                currentNode = createNode("text");
                currentNode->setNodeText(textToSet);
                document->constructTree(currentParentNode, currentNode);

                currentNode = currentParentNode;
                currentParentNode = currentNode->getParentNode();      //add diy
                break;
            }
            case tagAttr:
            {
                std::string attrString;
                while(currentState == tagAttr)
                {
                    attrString.push_back(*iter);
                    iter++;
                    if(*iter == '/')
                    {
                        currentState = endTagName;
                    }
                    else if(*iter == '>')
                    {
                        currentState = endTagOpen;
                    }
                }
                if(currentNode->getTypeOfRenderNode() == "link")
                {
                    std::string hrefToSet = extractNodeHref(attrString);
                    currentNode->setNodeHref(hrefToSet);
                }
                else
                {
                    currentNode->setNodeId(extractNodeId(attrString));
                    currentNode->setNodeClass(extractNodeClass(attrString));
                }
                break;
            }
            default:
                break;
        }
    }
    return document;
}

std::string HTMLReader::getTagName(std::string::iterator &iter, parseState &currentState)
{
    std::string tag;
    while(currentState == tagName)
    {
        tag.push_back(*iter);
        iter++;

        if(*iter == '>')
        {
            currentState = endTagOpen;
        }
        else if(*iter == ' ')
        {
            currentState = tagAttr;
        }
    }
    return tag;
}

/* 获取当前结点中的文本内容，此时iter指向文本第一个字符 */
std::string HTMLReader::getTextString(std::string::iterator &iter, parseState &currentState)
{
    std::string textString;
    while(currentState ==  text)
    {
        textString.push_back(*iter);
        iter++;

        if(*iter == '<')
        {
            currentState = tagOpen;
        }
    }
    return textString;
}

Document* HTMLReader::prepareDocument(std::string HTMLFilePath)
{
    try
    {
        std::ifstream HTMLDocument(HTMLFilePath.c_str());
        std::string documentText;
        std::string tempString;
        bool hasHTML = true;

        while(!HTMLDocument.eof())
        {
            std::getline(HTMLDocument, tempString);
            if(documentText.empty())
            {
                if(tempString != "html")
                {
                    hasHTML = false;
                    documentText = "<html>";
                }
                documentText += tempString;
            }
            else
            {
                documentText += tempString;
            }
        }
        if(!hasHTML)
        {
            documentText += "</html>";
        }

        HTMLDocument.close();
        return parseDocumentText(documentText);
    }
    catch(std::string error)
    {
        std::cout<<error<<std::endl;
        return NULL;
    }
}

RenderNode* HTMLReader::createNode(std::string nodeName)
{
    RenderNode *node = new RenderNode;
    node->setTypeOfRenderNode(nodeName);
    if(nodeName == "html")
    {
        node->setParentNode(NULL);
        node->setIsOpen(true);
    }
    else if(nodeName == "head" || nodeName == "body")
    {
        currentParentNode = document->firstNode;
        node->setIsOpen(true);
    }
    else if(nodeName == "link" && nodeName == "text")
    {
        node->setIsOpen(false);
    }
    else
    {
        node->setIsOpen(true);
    }
    return node;
}

bool HTMLReader::parentNodeClosed(RenderNode *node, std::string typeOfNode)
{
    if (node->getTypeOfRenderNode() == typeOfNode)
    {
        node->setIsOpen(false);
        currentNode = currentNode->getParentNode();

        return true;
    }
    else
    {
        currentNode = currentNode->getParentNode();
        return false;
    }
}

std::string HTMLReader::extractNodeId(std::string attrString)
{
    std::string::iterator iter = attrString.begin();
    std::string id;
    int flag = 0;
    for(; iter < attrString.end(); iter++)
    {
        if(flag == 0 && *iter == 'i')
            flag = 1;
        else if(flag == 1 && *iter == 'd')
            flag = 2;
        else if(flag == 2 && *iter == '=')
            flag = 3;
        else if(flag == 3 && *iter == '"')
            flag = 4;
        else if(flag == 4)
        {
            if(*iter == '"')
                break;
            else
                id.push_back(*iter);
        }
        else
        {
            flag = 0;
            id.clear();
        }

    }
    return id;
}

std::string HTMLReader::extractNodeHref(std::string attrString)
{
    std::string::iterator iter = attrString.begin();
    std::string href;
    int flag = 0;
    for(; iter < attrString.end(); iter++)
    {
        if(flag == 0 && *iter == 'h')
            flag = 1;
        else if(flag == 1 && *iter == 'r')
            flag = 2;
        else if(flag == 2 && *iter == 'e')
            flag = 3;
        else if(flag == 3 && *iter == 'f')
            flag = 4;
        else if(flag == 4 && *iter == '=')
            flag = 5;
        else if(flag == 5 && *iter == '"')
            flag = 6;
        else if(flag == 6)
        {
            if(*iter == '"')
                break;
            else
                href.push_back(*iter);
        }
        else
        {
            flag = 0;
            href.clear();
        }
    }
    return href;
}

std::vector<std::string> HTMLReader::extractNodeClass(std::string attrString)
{
    std::string::iterator iter;
    std::vector<std::string> classes;
    std::string tempString;
    int flag = 0;
    for(iter = attrString.begin(); iter != attrString.end(); iter++)
    {
        if(flag == 0 && *iter == 'c')
            flag = 1;
        else if(flag == 1 && *iter == 'l')
            flag = 2;
        else if(flag == 2 && *iter == 'a')
            flag = 3;
        else if(flag == 3 && *iter == 's')
            flag = 4;
        else if(flag == 4 && *iter == 's')
            flag = 5;
        else if(flag == 5 && *iter == '=')
            flag = 6;
        else if(flag == 6 && *iter == '"')
            flag = 7;
        else if(flag == 7)
        {
            if(*iter == '"')
            {
                classes.push_back(tempString);
                break;
            }
            else if(*iter == ' ')
            {
                classes.push_back(tempString);
                tempString.clear();
            }
            else
                tempString.push_back(*iter);
        }
        else
            flag = 0;
    }
    return classes;
}


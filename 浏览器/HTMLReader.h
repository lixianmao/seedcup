
#ifndef HTMLREADER_H
#define HTMLREADER_H

#include <string>
#include "Document.h"
#include "RenderNode.h"

class HTMLReader
{
    public:
        HTMLReader();
        virtual ~HTMLReader();

        enum parseState {
            initialMode,
            text,
            tagOpen,
            endTagOpen,
            tagName,
            endTagName,
            tagAttr
        };

        Document *prepareDocument(std::string HTMLFilePath);
        std::string getTagName(std::string::iterator &iter, parseState &currentState);
        std::string getTextString(std::string::iterator &iter, parseState &currentState);
        RenderNode* createNode(std::string nodeName);
        std::string extractNodeId(std::string attrString);
        std::string extractNodeHref(std::string attrString);
        std::vector<std::string> extractNodeClass(std::string attrString);
    protected:
    private:
        Document *document;
        Document *parseDocumentText(std::string documentText);
        bool parentNodeClosed(RenderNode *node, std::string typeOfNode);
        RenderNode *currentNode;
        RenderNode *currentParentNode;
};


#endif // HTMLREADER_H

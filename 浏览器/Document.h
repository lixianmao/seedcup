#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <string>
#include "RenderNode.h"

class Document {
public:
     Document();
     virtual ~Document();

     void constructTree(RenderNode *parentNode, RenderNode *childNode);
     void clearTree();
     RenderNode *getFirstNode();

     RenderNode *firstNode;
protected:
private:

};
#endif // DOCUMENT_H


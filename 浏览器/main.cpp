#include <iostream>
#include <string>
#include <map>
#include "parser_css.h"
#include "HTMLReader.h"
#include "Document.h"
#include <iostream>
#include "RenderTree.h"

using namespace std;

void visit(RenderNode *node)
{
    if(node)
    {
        std::string str = node->getTypeOfRenderNode();
        if(str != "html" && str != "head" && str != "link"){
        cout<<node->getTypeOfRenderNode()+' ';
        cout<<"href:"+node->getNodeHref()+' ';
        cout<<"id:"+node->getNodeId() + ' ';
        cout<<"class:";
        vector<string> classes = node->getNodeClass();
        vector<string>::iterator iter;
        for(iter = classes.begin(); iter != classes.end(); iter++)
        {
            cout<<*iter + ' ';
        }
        cout<<" text:"+node->getNodeText()<<endl;
        cout<<node->getCSSProperty().offsetLeft<<" "<<node->getCSSProperty().offsetTop<<" "
        <<node->getCSSProperty().display <<" "<<node->getCSSProperty().position << " "
        <<node->getCSSProperty().width <<" "<<node->getCSSProperty().height <<std::endl;
        cout<<node->getCSSProperty().padding
        <<" "<<node->getCSSProperty().border <<" "<<node->getCSSProperty().margin <<" "
        << node->getCSSProperty().left
         <<" "<<node->getCSSProperty().right <<" "
         << node->getCSSProperty().top<<std::endl;
          cout<<node->getCSSProperty().bottom
         <<" "<<node->getCSSProperty().color<<" " <<node->getCSSProperty().lineHeight <<" "
         <<node->getCSSProperty().fontSize
         <<" " <<node->getCSSProperty().fontStyle <<" "<<node->getCSSProperty().fontWeight
         <<" "<<node->getCSSProperty().textAlign<<" " <<node->getCSSProperty().lineBreak<<endl;
        }
        vector<RenderNode*> children = *(node->getChildNodes());
        vector<RenderNode*>::iterator i;
        for(i = children.begin(); i != children.end(); i++)
        {
            visit(*i);
        }
    }
}


int main(void)
{
    //try{
    //css::parser_css pser("test1.txt");
   // } catch(...) {
    //HTMLReader reader;
    //Document *doc;
    //doc = reader.prepareDocument("test.html");

    //RenderNode *node = doc->firstNode;
    //visit(node);
   // }
 //   cout << "Hello world!" << endl;
    flyingFish::RenderTree renderTree("test.html");

    visit(renderTree.getRenderNode());
    return 0;
}

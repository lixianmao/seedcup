#ifndef REGEXPTONFA_H_
#define REGEXPTONFA_H_

#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <stack>

class RegexpToNfa {
private:
     struct State { //NFA状态
        std::string c; //每条边上代表匹配功能的字符串
        State *out; //指向下一个状态的指针，
        State *out1;//当本状态为split时，out1表示分支
        char ch; //后向引用时，ch为数字字符，否则为空格字符
        int listIndex; //状态编号
        State():c(""),out(NULL),out1(NULL),ch(' '),listIndex(-1) {}
        State(std::string _c,State* p1,State* p2,char ct=' ',int listIndex=-1)
        :c(_c),out(p1),out1(p2),ch(ct) {}
    };

    struct Ptrlist { //指向State指针的指针链表
        Ptrlist *next;
        State **outp;
        Ptrlist(State **ptr):next(NULL),outp(ptr) {}
    };

    struct Frag {
        State *start;	//开始节点
        Ptrlist* out;	//out是一组指向未链接到任何东西的状态指针的列表的指针
        Frag():start(NULL),out(NULL) {}
        Frag(State *s,Ptrlist *list):start(s),out(list) {}
    };

    struct List { //状态链表
        State **s;
        int n;//List中状态的个数
    };

    struct Pos { //后向引用所处位置
        int start;
        int end;
        Pos(int b=-1,int e=-1):start(b),end(e) {}
    };

    struct Paren {
        int natom; //token个数
        int nalt; // | 的个数
    };

    const char *str;  //待匹配的字符串
    std::string regexp; //构造NFA的正则表达式
    int listid;
    List currentList;		//当前状态链表
    List nextList;		//下一个状态链表
    State *matchState;  //匹配状态
    std::vector<std::string> edge; //从regexp中分离出的token字符串
    Pos pos[10]; //各个后向引用的开始与结束位置
    int reference[10];
    std::map<char,std::string> result_reference; //每个后向引用对应的必须匹配的字符串
    bool no_addstate;
    int iteratorTimes; //防止死循环的迭代次数
    int isEmpty; //判断某个后向引用是否为空字符串，

    void getToken(std::string re);
    void dealBrace(std::string re);
    void cleanBrace(void);
    void dealWithString(std::string re);
    std::vector<std::string> regexpToPostfix(void);
    Ptrlist *linkPtrlist(Ptrlist *l1, Ptrlist *l2);
    void danglingPointLinkToState(Ptrlist *l, State *s);
    State* postfixToNfa(std::vector<std::string> postfix);
    void FindBracket(const char* pStart, const char*& pEnd);
    std::string GetBracketContent(const char* pStart, const char* pEnd);
    int MatchBracket(char c, std::string str);
    void addStateToList(List *l, State *s);
    List* initializeList(State *s, List *l);
    bool isEqual(std::string edgeStr,char c,List *next_list,State *s);
    void nextState(List *cur_list, List *next_list, const char* str, char end);
    int isMatch(List *l);
    int matchString(State *start, int left, int right);

    bool isWBoundary(const char* pStr);
    bool isLBegin(const char* pStr);
    bool isLEnd(const char* pStr);

public:
    RegexpToNfa(const char *s,std::string regular):str(s),regexp(regular) {
        listid = 0;
        currentList.s = (State**)malloc(1000*sizeof(State));
        nextList.s = (State**)malloc(1000*sizeof(State));
        matchState = new State(std::string("Match"),NULL,NULL);
        for(int i=0;i<10;i++) {
            reference[i] = 0;
        }
        no_addstate = false;
        isEmpty = 0;
        iteratorTimes = 0;
    }
    std::string getResult();
    ~RegexpToNfa() {
        free(currentList.s);
        free(nextList.s);
    }
};
#endif



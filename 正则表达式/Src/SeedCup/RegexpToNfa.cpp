#include "RegexpToNfa.h"
#include <iostream>
using namespace std;
using std::vector;
using std::string;
using std::stack;
using std::runtime_error;

/***
*   将输入的字符串按token分开为字符串
*/
void RegexpToNfa::getToken(string re) {
    string::iterator iter_temp;
    string alpha = "bdswBDSW";
    int i=0;
    for(string::iterator iter = re.begin();iter != re.end();++iter) {
        switch(*iter) {
		case '{':   //将包括{与}之间的字符串看成一个token
			iter_temp = iter;
			while(*++iter_temp != '}');
			edge.push_back(string(iter,iter_temp+1));
			iter = iter_temp;
			break;
		case '[':   //将包括[与]之间的字符串看成一个token
			iter_temp = iter;
			while(*++iter_temp != ']');
			edge.push_back(string(iter,iter_temp+1));
			iter = iter_temp;
			break;
		case '\\':{  //处理后向引用以及元字符
			if((alpha.find(*(iter+1)) == string::npos) &&(!isdigit(*(iter+1)))) {
				edge.push_back(string(iter,iter+1)); //非元字符直接放入edge中
			} else {
				edge.push_back(string(iter,iter+2));//是元字符或者后向引用中的数字的处理
                ++iter;
				if(isdigit(*iter)) {
				    result_reference[*iter] = "";
					reference[i++] = *iter-'0'; //如果是后向引用的数字，将其放入reference中
				}
			}
			break;
        }
		default:
			edge.push_back(string(iter,iter+1));
			break;
        }
    }

    //调试用
    //for(vector<string>::iterator iter=edge.begin();iter!=edge.end();++iter)
     //   cout<<*iter<<endl;
}

/****
* 将字符串中的大括号里的重复次数
* 换成代表重复的*，+，？等等
*/
void RegexpToNfa::dealBrace(string re) {
    char a[10];
    char b[10];
    string temp;
    string::size_type pos1 = re.find('}');
    string::size_type behind_data = re.find(',');
    int min;
    int max;
    if(behind_data == string::npos) {  //处理形如{n}的情形
        string::size_type s_t=1;
        for(;s_t!=pos1;++s_t) {
            a[s_t-1] = re[s_t];
        }
        a[s_t-1] = '\0';
        temp = edge.back();
        for(int i=0;i<atoi(a)-1;++i) {
            edge.push_back(temp); //将n个token来取代大括号
        }

    } else if(pos1 == behind_data + 1) {  //处理形如{n，}的情形
        string::size_type s_t=1;
        for(;s_t!=behind_data;++s_t) {
            a[s_t-1] = re[s_t];
        }
        a[s_t-1] = '\0';
        temp = edge.back();
        for(int i=0; i<atoi(a)-1; ++i) { //将n-1个token取代大括号，并加上一个“+”
            edge.push_back(temp);
        }
        edge.push_back(string("+"));
    } else {   //处理形如{n,m}的情形
        string::size_type s_t=1;
        for(;s_t!=behind_data;++s_t) {
            a[s_t-1] = re[s_t];
        }
        a[s_t-1] = '\0';
        min = atoi(a);
        string::size_type s_=behind_data+1;
        for(;s_!=pos1;++s_) {
            b[s_-behind_data-1] = re[s_];
        }
        b[s_-behind_data-1] = '\0';
        max = atoi(b);
        temp = edge.back();
        for(int i=0; i<min-1; ++i) { //将n个token取代大括号
            edge.push_back(temp);
        }
        for(int i=0; i<max-min;++i) {//将m-n个token及“？”加入edge中
            edge.push_back(temp);
            edge.push_back(string("?"));
        }
    }

}

/***
* 调用函数dealBrace来去掉大括号
*/
void RegexpToNfa::cleanBrace(void) {
    vector<string> temp;
    temp = edge;
    edge.clear();
    for(vector<string>::iterator iter = temp.begin();iter != temp.end(); ++iter) {
        if((*iter)[0]=='{') {  //遇到大括号就处理
            dealBrace(*iter);
        } else {
            edge.push_back(*iter);
        }
    }
}

/***
* 调用getToken函数和cleanBrace函数
*/
void RegexpToNfa::dealWithString(string re) {
    getToken(re);
    cleanBrace();
}

/***
* 将去掉大括号的正则表达式变为后缀形式
* 并放在vector<string>中，为变为NFA做准备
*/
vector<string> RegexpToNfa::regexpToPostfix(void) {
    Paren paren;
    stack<struct Paren>parenStk;
    vector<string> postExpr; //存放后缀表达式的结果
    int lefBracketNum=0;
    int j=0;
    int nalt=0, natom=0; //natom代表token个数，nalt代表|个数
    const string errorRegExp = "wrong regular expression";//错误正则表达式输出内容
	for(vector<string>::iterator iter=edge.begin();iter!=edge.end();++iter)
    {
        if(*iter == "(") //处理后向引用问题
        {
            lefBracketNum++; //计算第几个左括号
            j = 0;
            while(reference[j]) { //在reference中找到不为0的数字，其代表着第几个引用。
                if(lefBracketNum==reference[j++]) {
                    pos[lefBracketNum].start = postExpr.size();  //存储括号内元素的起始位置
                    break;
                }
            }
            if(natom>1)
            { //有2个以上token，在其后加上“..”代表两个token的串接
                natom--;//串接在一起后变为1个token
				postExpr.push_back("..");
            }
            paren.natom = natom;
            paren.nalt = nalt;
            parenStk.push(paren); //将每个“（”中的token入栈，看成一个token
            nalt = 0;
            natom = 0;
        }
		else if(*iter == ")")
        {
            j = 0;
            while(reference[j]) {
                if(lefBracketNum==reference[j++]) {
                    pos[lefBracketNum].end = postExpr.size()-1; //存储括号内元素的结束位置
                    break;
                }
            }

            if(natom==0 || parenStk.empty())
                throw runtime_error(errorRegExp+":wrong to match bracket.");
            while(--natom>0)
            { //两个括号间的串接，在其后加“..”
                postExpr.push_back("..");
            }
            while(nalt-->0)
            { //处理|
				postExpr.push_back("|");
            }
            paren=parenStk.top();//弹出parent
            parenStk.pop();
            natom = paren.natom;
            nalt = paren.nalt;
            natom++;
        }
		else if(*iter == "*")
        {
            if(natom==0)
                throw runtime_error(errorRegExp+":wrong to match asterisk.");
			postExpr.push_back(*iter);
        }
		else if(*iter == "+")
        {
            if(natom==0)
                throw runtime_error(errorRegExp+":wrong to match plus sign.");
			postExpr.push_back(*iter);
        }
		else if(*iter == "?")
        {
            if(natom==0)
                throw runtime_error(errorRegExp+":wrong to match question mark.");
            postExpr.push_back(*iter);
        }
		else if(*iter == "|")
        {
            if(natom==0) throw runtime_error(errorRegExp+":wrong to match split.");
            while(--natom>0)
            {
				postExpr.push_back("..");
            }
            nalt++;
        }
        else
        {
            if(natom>1)
            {
                natom--;
                postExpr.push_back("..");
            }
            natom++;
			postExpr.push_back(*iter);
        }
    }
    if(!parenStk.empty())
        throw runtime_error(errorRegExp+":wrong to match bracket.");
    while(--natom>0)
    {
        postExpr.push_back("..");
    }
    while(nalt-->0)
    {
		postExpr.push_back("|");
    }
	//调试用
	//for(vector<string>::const_iterator iter = postExpr.begin();iter!=postExpr.end();++iter)
    //   std::cout<<*iter<<std::endl;
    return postExpr;
}

/***
* 将l1和l2连接起来，并返回结果
*/
RegexpToNfa::Ptrlist* RegexpToNfa::linkPtrlist(Ptrlist *l1,Ptrlist *l2) {
    if(l1 == NULL) {
        return l2;
    }
    Ptrlist *l = l1;
    Ptrlist *p = l;
    for(; p->next!=NULL; p=p->next);
    p->next = l2;
    return l;
}

/***
* 将l中的悬空指针连到状态s上，
* 即为生成NFA时，状态之间的连接
*/
void RegexpToNfa::danglingPointLinkToState(Ptrlist *l, State *s) {
    Ptrlist *p = l;
    for(; p != NULL; p=p->next){
        *(p->outp) = s;
    }
}

/***
* 将生成的后缀表达式转换为NFA
*/
RegexpToNfa::State* RegexpToNfa::postfixToNfa(vector<string> postfix) {
    stack<Frag> stack;
    Frag e1,e2,e;
    State *s;
    int position[20];
    int j=0;
    int k=0;
    int m=0;
    int t=0;
    int flag = 0;
    int index[10];
    for(int i=0;i<10;i++) {
        index[i] = 0;
    }
    //调试用
    //for(vector<string>::iterator iter=postfix.begin();iter!=postfix.end();++iter)
	//   cout<<*iter<<endl;
	for(int i=0;i<20;i++) {
        position[i] = 0;
	}
	for(int i=0;i<10;i++) {
        if(pos[i].start != -1) {
            position[j++] = pos[i].start;
            position[j++] = pos[i].end;
            index[k++] = i;
        }
	}
	for(vector<string>::iterator iter=postfix.begin();iter!=postfix.end();++iter)
    {
        int dist=iter-postfix.begin();
        char ch;
        //处理后向引用中各个引用下标范围
        if(m<j&&dist >= position[m]&&dist <= position[m+1]) {
            flag = 1;
        } else if(m<j&&dist>position[m+1]) { //处理两个引用连着的特殊情况
            if(m<j&&position[m+1]==position[m+2]-1) {
                flag=1;
            }
            t++;
            m+=2;
        }
        ch = index[t]+'0';
        if(*iter =="..") {//取出两个操作数
            e2 = stack.top();
            stack.pop();
            e1 = stack.top();
            stack.pop();
            danglingPointLinkToState(e1.out, e2.start);//“..”将两个状态连接起来并入栈
            stack.push(Frag(e1.start, e2.out));
		}
		else if(*iter == "|"){//取出两个操作数
            e2 = stack.top();
            stack.pop();
            e1 = stack.top();
            stack.pop();
            if(flag) { //如果*iter在引用范围内，State的ch参数为引用数字字符
                s = new State(string("Split"),e1.start,e2.start,ch);
                flag = 0;
            } else {
                s = new State(string("Split"),e1.start, e2.start,' ');
            }
            stack.push(Frag(s, linkPtrlist(e1.out, e2.out)));
		} else if(*iter == "?"){
            e = stack.top();
            stack.pop();
            if(flag) {
                s = new State(string("Split"),e.start,NULL,ch);
                flag = 0;
            } else {
                s = new State(string("Split"), e.start, NULL,' ');
            }
            stack.push(Frag(s, linkPtrlist(e.out, new Ptrlist(&s->out1))));
		} else if(*iter == "*") {//取出一个操作数
            e = stack.top();
            stack.pop();
            if(flag) {
                s = new State(string("Split"),e.start,NULL,ch);
                flag = 0;
            } else {
                s = new State(string("Split"), e.start, NULL,' ');
            }
            danglingPointLinkToState(e.out, s);//往回连接起来，形成循环
            stack.push(Frag(s, new Ptrlist(&s->out1)));
		} else if(*iter == "+") {
            e = stack.top();
            stack.pop();
            if(flag) {
                s = new State(string("Split"),e.start,NULL,ch);
                flag = 0;
            } else {
                s = new State(string("Split"), e.start, NULL,' ');
            }
            danglingPointLinkToState(e.out, s);
            stack.push(Frag(e.start, new Ptrlist(&s->out1)));
		} else {
			if(flag) {
                s = new State(*iter,NULL,NULL,ch);
                flag = 0;
            } else {
                s = new State(*iter,NULL,NULL,' ');
            }
            stack.push(Frag(s, new Ptrlist(&s->out)));
		}
    }
    e = stack.top();
    stack.pop();
    danglingPointLinkToState(e.out, matchState);//最后与成功连接的状态串接起来
    return e.start;
}

/***
*  获取中括号[]的位置
*  通过指针pStart, pEnd返回左括号[和右括号]的位置信息
*/
void RegexpToNfa::FindBracket(const char* pStart, const char*& pEnd)
{
    const char* ps = pStart;
    if(*ps == '[')
    {
        ps++;
        while(*ps && *ps != ']')
        {
            ps++;
        }
        if(*ps == ']') pEnd = ps;
    }
}
/***
* 根据中括号里第一个字符位置指针pStart和最后一个字符地位置指针pEnd
* 返回其中所有可能取值的字符串的集合，作为一个字符串
*/
string RegexpToNfa::GetBracketContent(const char* pStart, const char* pEnd)
{
    const char* p = pStart;
    bool isNegate = (*p == '^');
    string str;
    vector<short> collection(128, 0);
    short linkMod = 0;    //连接线'-'

    str.reserve(2 * (pEnd - pStart));
    if(isNegate) ++p;
    while(p <= pEnd)
    {
        if(*p == '-' && (p > pStart && p < pEnd))
        {
            //TODO: 需要做抛出异常处理,比如[9-0]
            if(*(p - 1) > *(p+1));
            if(linkMod == 0)         //处理[1-3-5-9]之类的情况
            {
                linkMod = 1;
                for(short i = *(p - 1); i <= *(p + 1); i++)
                {
                    collection[i] = 1;
                }
            }
            else if(linkMod == 1 && *(p - 2) == '-')
            {
                linkMod = 0;
                collection[*p] = 1;
            }
        }
        else
        {
            collection[*p] = 1;
        }
        ++p;
    }

    if(!isNegate)
    {
        for(short j = 0; j < 128; j++)
        {
            if(!collection[j]) continue;
            str.push_back(j);
        }
    }
    else
    {
        for(short j = 0; j < 128; j++)
        {
            if(collection[j]) continue;
            str.push_back(j);
        }
    }
    return str;
}
/***
* 判断字符c是否是中括号取值集合里面的一个
*/
int RegexpToNfa::MatchBracket(char c, string str)
{
    const char* p = str.c_str();
    for(unsigned i = 0; i < str.length(); i++)
    {
        if(*(p+i) == c)
            return 1;
    }
    return 0;
}


/***
* 匹配时，每匹配到一个状态就将
* 其添加到clist或者nlist中
*/
void RegexpToNfa::addStateToList(List *l, State *s) {
    //如果所添加的状态为NULL或者与前面状态重复，直接返回
    if(s == NULL || s->listIndex == listid)
        return;
    s->listIndex = listid;
    if(s->c == string("Split"))
    {//如果是Split，跳过此状态，直接到其后的两个分支状态中
        addStateToList(l, s->out);
        addStateToList(l, s->out1);
        return;
    }
    //将此状态加入Ptrlist链表中
    l->s[l->n++] = s;
}

/***
* clist的初始化
*/
RegexpToNfa::List* RegexpToNfa::initializeList(State *s, List *l) {
    listid++;
    l->n = 0;
    addStateToList(l, s);
    return l;
}

bool isWBoundry(const char* pStr)
{
    if(!*(pStr - 1) && isalnum(*pStr))  //字符串开头匹配单词开头
        return true;
    else if(isalnum(*(pStr-1)) && !isalnum(*pStr))  //字符串中间匹配单词结尾
        return true;
    else if(!isalnum(*(pStr-1)) && isalnum(*pStr))  //字符串中间匹配单词开头
        return true;
    else if(isalnum(*(pStr - 1)) && !*pStr)         //字符串结尾匹配单词结尾
        return true;
    else
        return false;
}

//行开头
bool RegexpToNfa::isLBegin(const char* pStr)
{
    if(!*(pStr - 1) || *(pStr - 1) == '\n' || (*(pStr - 1) == 'b' && *(pStr - 2) == '\\' && *(pStr - 3)))
        return true;
    else
        return false;
}

//行结尾
bool RegexpToNfa::isLEnd(const char* pStr)
{
    if(!*(pStr + 1) || *(pStr + 1) == '\n')
        return true;
    else
        return false;
}


/***
* 判断字符c与NFA的状态是否匹配
*/
bool RegexpToNfa::isEqual(string edgeStr,char c,List *next_list,State *s)
{
    string str_temp(1,c);
    if(edgeStr ==str_temp )   //两个字符token相同
    {
        return true;
    }
    else if((edgeStr == ".")&&(c != '\n'))     //正则表达式的字符token为"."且不为'\n'
    {
        return true;
    }
    else if(edgeStr[0] =='\\')     //处理元字符情况
    {
        if(edgeStr == "\\w")
        {
            return isalnum(c);
        }
        else if(edgeStr =="\\s")
        {
            return isspace(c);
        }
        else if(edgeStr == "\\d")
        {
            return isdigit(c);
        }
        else if(edgeStr == "\\W")
        {
            return (!isalnum(c));
        }
        else if(edgeStr == "\\S")
        {
            return (!isspace(c));
        }
        else if(edgeStr == "\\D")
        {
            return (!isdigit(c));
        }
        else if(isdigit(edgeStr[1]))    //处理后向引用的情况
        {
            no_addstate = false;
            if(result_reference[edgeStr[1]].size()>1)
            {
                if(result_reference[edgeStr[1]][0]==c)   //将字符c与前面的结果集依次比较
                {
                    no_addstate = true;
                    addStateToList(next_list,s); //再次处理此状态，知道结果集中为空或者匹配错误
                    result_reference[edgeStr[1]]=result_reference[edgeStr[1]].substr(1);
                    return true;
                }
                else
                {
                    no_addstate = false;
                    return false;
                }
            }
            else if(result_reference[edgeStr[1]].size()==1)
            {
                if(result_reference[edgeStr[1]][0]==c)
                {
                    no_addstate = false;//最后一个字符匹配，返回true，进行下一个状态匹配
                    return true;
                }
                return false;
            }
            else
            {
                isEmpty = 1;//处理存放后向引用的向量为空时的情况
                no_addstate = false;
                return true;
            }
        }
        else
        {
            return false;
        }
    }
    else if(edgeStr[0] == '[')    //处理中括号中的选择项
    {
        const char* pStart = edgeStr.c_str();
        const char* pEnd;
        FindBracket(pStart, pEnd);
        string retStr = GetBracketContent(pStart+1, pEnd-1);
        return MatchBracket(c, retStr);
    }
    else
    {
        return false;
    }
    return false;
}
/***
* 对一个字符c进行匹配状态操作
*/
void RegexpToNfa::nextState(List *cur_list, List *next_list, const char* testStr, char end)
{
    int i;
    State *state;
    listid++;
    next_list->n = 0;

    //对clist中的每一个状态与字符c进行匹配测试
    for(i=0; i<cur_list->n; i++)//宽度优先遍历
    {
        state = cur_list->s[i];
        if((*testStr == end) && (state->out != NULL)&&((state->out->c == "\\b" && isWBoundry(testStr+1))  //"\\b"在表达式结尾
            ||(state->out->c == "\\B" && !isWBoundry(testStr+1))))
        {
            if(isEqual(state->c, *testStr, next_list, state))
            {
                state = state->out;
                if(state->out != NULL && state->out->c == "$")
                    addStateToList(next_list, state->out->out);
                else
                    addStateToList(next_list, state->out);
            }

        }
        else if((state->c == "\\b" && isWBoundry(testStr))   //"\\b"不在表达式结尾
                           || (state->c == "\\B" && !isWBoundry(testStr)))
        {
            state = state->out;
            if(state->c == "^" && isLBegin(testStr))
                state = state->out;
            if(isEqual(state->c, *testStr, next_list, state))
            {
                addStateToList(next_list, state->out);
            }
        }
        else if(state->c == "^" && isLBegin(testStr))
        {
            state = state->out;
            if(state->c == "\\b")
                state = state->out;
            if(isEqual(state->c, *testStr, next_list, state))
            {
                addStateToList(next_list, state->out);
            }
        }
        else if((*testStr == end)  &&(state->out != NULL)&& state->out->c == "$" && isLEnd(testStr))
        {
            if(isEqual(state->c, *testStr, next_list, state))
            {
                state = state->out;
                if(state->out != NULL && state->out->c == "\\b")
                    addStateToList(next_list, state->out->out);
                else
                    addStateToList(next_list, state->out);
            }

        }
        else if(isEqual(state->c, *testStr, next_list, state))
        {

            if(state->ch != ' ')   //后向引用时，如果state->ch为数字字符
            {
                //表示后面有后向引用会用到匹配的字符，加入结果集result_reference中
                result_reference[state->ch] += (*testStr);
            }
            if(!no_addstate)
                addStateToList(next_list, state->out);//下一个状态
        }
    }
}

/***
* 判断字符串是否成功匹配
*/
int RegexpToNfa::isMatch(List *l) {
    int i;
    for(i=0; i<l->n; i++)
        if(l->s[i] == matchState)
            return 1;
		return 0;
}

/***
* 匹配整个字符串s
*/
int RegexpToNfa::matchString(State *start, int left, int right)
{
    List *cur_list, *next_list, *t;
    cur_list = initializeList(start, &currentList);	//当前状态
    next_list = &nextList;
    if(right == left-1)
        return isMatch(cur_list);
    for(int pos = left; pos <= right; pos++)
    {
        //对每个字符进行匹配操作
        nextState(cur_list, next_list, str + pos, *(str+right));

        if(next_list->n==0)  //没有链表后，提前退出
        {
            return 0;
        }
        //交换本次状态链表与下一个状态链表
        t = cur_list;
        cur_list = next_list;
        next_list = t;

        if(isEmpty)
        {
            if(iteratorTimes++ >= 100) {
               // pos--; //回退一个字符
                //isEmpty = 0;
                break;
            }
            pos--; //回退一个字符
            isEmpty = 0;
        }
    }
    return isMatch(cur_list);
}

/***
* 初始化NFA后，进行匹配，并将
* 匹配结果返回
*/
std::string RegexpToNfa::getResult()
{
    vector<string> postStr;
    dealWithString(regexp);//处理正则表达式
    postStr = regexpToPostfix();
    State *s=postfixToNfa(postStr);
    //for(vector<string>::iterator iter = postStr.begin();iter != postStr.end();++iter)
    //   std::cout<<*iter<<std::endl;

    for(int left = 0; left < (int)strlen(str); left++)
    {
        for(int right = strlen(str)-1; right >= left-1; right--)
        {
            isEmpty = 0;
            no_addstate = false;
            iteratorTimes = 0;
            if(matchString(s, left, right))
            {
                string resultStr = str;
                return resultStr.substr(left, right - left + 1);
            }
            result_reference.clear();
        }
    }
    return "";
}



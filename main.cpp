#include <stdio.h>
#include <stack>

using namespace std;

bool isFinished;

struct BTree
{
    int *k;
    struct BTree **p;
    int n; //data가 들어있는 갯수
    bool isLeafNode;
};

//차수가 m인 빈 B-tree 초기화.
BTree* getNode(int m)
{
    BTree *newBT = new BTree;
    newBT->k = new int[m-1];
    newBT->p = new BTree *[m];
    newBT->n = 0;
    newBT->isLeafNode = true;

    for(int i=0;i<m;i++)
    {
        newBT->p[i] = NULL;
    }

    return newBT;
}

void inorder(BTree *bt, int m)
{
    int i;
    if(bt == NULL) return;
    for(i=0; i < bt->n; i++)
    {
        inorder(bt->p[i], m);
        printf("%d ", bt->k[i]);
    }
    inorder(bt->p[i], m);
}

stack<BTree *> path;
stack<int> savedidx;
stack<BTree *> splitedNode;

int split(BTree *&bt, int m, int insertIdx, int newKey)
{
    int mid, idx;
    BTree *preLevelNode, *sameLevelNode, *tmp;

    sameLevelNode = getNode(m);
    sameLevelNode->isLeafNode = true;

    //tmp에 값을 복사
    tmp = getNode(m+1);
    if(bt->isLeafNode) //leafNode일때 split방법(pointer를 고려하지 않는 방식
    {
        //임시 노드에 값 복사.
        for(int i=0; i<insertIdx;i++)
        {
            tmp->k[i] = bt->k[i];
        }
        tmp->k[insertIdx] = newKey; //idx위치에 newKey삽입.
        for(int i=insertIdx+1;i<m+1;i++) //
        {
            tmp->k[i] = bt->k[i-1];
        }
    }
    else //내부모드일떄 pointer를 복사중 <내부노드가 아니라는 말은 overflow전파가 일어났다는 말.
    {
        for(int i=0; i<insertIdx;i++)
        {
            tmp->k[i] = bt->k[i];
            tmp->p[i] = bt->p[i];
        }
        //idx위치에 newKey삽입.
        tmp->k[insertIdx] = newKey;
        tmp->p[insertIdx] = bt->p[insertIdx];

        //하위 노드 연결.
        tmp->k[insertIdx+1] = bt->k[insertIdx+1];
        tmp->p[insertIdx+1] = splitedNode.top();
        splitedNode.pop();

        //나머지 노드 복사.
        for(int i=insertIdx+2;i<m;i++)
        {
            tmp->k[i] = bt->k[i-1];
            tmp->p[i] = bt->p[i-1];
        }
    }

    //split
    mid = m/2;
    for(int i=0;i<mid;i++)
    {
        bt->k[i] = tmp->k[i];
        bt->p[i] = tmp->p[i];
    }
    for(int i=mid; i<m; i++)
    {
        sameLevelNode->k[i-mid] = tmp->k[i+1];
        sameLevelNode->p[i-mid] = tmp->p[i+1];
        sameLevelNode->n++;

        bt->k[i] = 0;
        bt->p[i] = NULL;
    }
    bt->n = mid-1;
    //연결해야할 노드를 저장.
    splitedNode.push(sameLevelNode);

    if(path.empty()) //위로 올려줄 상위 노드 없음. tree level ++해주는 작업.
    {
        preLevelNode = getNode(m);
        preLevelNode->isLeafNode = false;
        preLevelNode->k[0] = tmp->k[mid];
        preLevelNode->p[0] = bt;
        preLevelNode->p[1] = splitedNode.top();
        splitedNode.pop();
        preLevelNode->n = 1;
        isFinished = true;
    }
    return tmp->k[mid];
}

//한 노드 안에서 sKey가 들어갈 위치를 찾는 함수.
int searchNode(BTree *bt, int m, int sKey)
{
   int idx=0;
   for(int i=0; i<bt->n;i++)
   {
       if(bt->k[i] < sKey) idx++;
   }
   return idx;
}

void insertBT(BTree *&bt, int m, int newKey)
{
    //B-tree가 null인 경우. 새로 만들어줌.
    if(bt == NULL)
    {
        bt = getNode(m);
        bt->k[0] = newKey;
        bt->n = 1;
        isFinished = true;
        return;
    }

    //삽입 위치 검색.
    struct BTree *x = bt;
    int i;
    while(x != NULL)
    {
        i = searchNode(x, m, newKey);//newKey가 들어갈 idx의 위치를 찾음.
        if(x->k[i] == newKey) return;//중복 발견. 함수 종료.
        //삽입 할 키를 발견하지 못했음.-> 스택에 현재 위치를 넣고 아래로 내려감.
        path.push(x);
        savedidx.push(i);
        x = x->p[i];
    }

    //stack의 맨 윗부분에는 LeafNode가 저장됨.
    //newKey 삽입.
    isFinished = false;

    //패스에서 삽입할 노드의 주소와 인덱스를 받아옴.
    //이 경우 leaf node의 정보를 가져오는 작업임.
    x = path.top();
    path.pop();
    i = savedidx.top();
    savedidx.pop();

    while(!isFinished)
    {
        if(x->n < m-1) //overflow가 일어나지 않는 경우.
        {
            //들어갈 곳 찾기
            int tmpi = searchNode(x,m,newKey);
            int tmpK;
            BTree *tmpP, *newP;
            x->n++;
            if(!splitedNode.empty())
            {
                newP = splitedNode.top();
                splitedNode.pop();
            }
            else newP = NULL;
            for(int tt = tmpi; tt<x->n; tt++) //기존값을 밀고 들어감.
            {
                tmpK = x->k[tt];
                tmpP = x->p[tt];
                x->k[tt] = newKey;
                x->p[tt] = newP;
                newKey = tmpK;
                newP = tmpP;
            }
            isFinished = true;
        }
        else //overflow가 일어난 경우.
        {
            newKey = split(bt, m, i, newKey);
        }
    }
}


int main()
{
    /*
    //m=3
    BTree *t = NULL;
    insertBT(t, 3, 30); inorder(t, 3); printf("\n");
    insertBT(t, 3, 20); inorder(t, 3); printf("\n");
    insertBT(t, 3, 62); inorder(t, 3); printf("\n");
    insertBT(t, 3, 110); inorder(t, 3); printf("\n");
    insertBT(t, 3, 140); inorder(t, 3); printf("\n");
    insertBT(t, 3, 15); inorder(t, 3); printf("\n");
    insertBT(t, 3, 65); inorder(t, 3); printf("\n");
    insertBT(t, 3, 136); inorder(t, 3); printf("\n");
    insertBT(t, 3, 150); inorder(t, 3); printf("\n");
    insertBT(t, 3, 120); inorder(t, 3); printf("\n");

    insertBT(t, 3, 40); inorder(t, 3); printf("\n");
    insertBT(t, 3, 132); inorder(t, 3); printf("\n");
    insertBT(t, 3, 19); inorder(t, 3); printf("\n");
    insertBT(t, 3, 128); inorder(t, 3); printf("\n");
    insertBT(t, 3, 138); inorder(t, 3); printf("\n");
    insertBT(t, 3, 100); inorder(t, 3); printf("\n");
    insertBT(t, 3, 16); inorder(t, 3); printf("\n");
    insertBT(t, 3, 145); inorder(t, 3); printf("\n");
    insertBT(t, 3, 70); inorder(t, 3); printf("\n");
    insertBT(t, 3, 42); inorder(t, 3); printf("\n");

    insertBT(t, 3, 69); inorder(t, 3); printf("\n");
    insertBT(t, 3, 43); inorder(t, 3); printf("\n");
    insertBT(t, 3, 26); inorder(t, 3); printf("\n");
    insertBT(t, 3, 60); inorder(t, 3); printf("\n");
    insertBT(t, 3, 130); inorder(t, 3); printf("\n");
    insertBT(t, 3, 50); inorder(t, 3); printf("\n");
    insertBT(t, 3, 18); inorder(t, 3); printf("\n");
    insertBT(t, 3, 7); inorder(t, 3); printf("\n");
    insertBT(t, 3, 36); inorder(t, 3); printf("\n");
    insertBT(t, 3, 58); inorder(t, 3); printf("\n");

    insertBT(t, 3, 22); inorder(t, 3); printf("\n");
    insertBT(t, 3, 41); inorder(t, 3); printf("\n");
    insertBT(t, 3, 59); inorder(t, 3); printf("\n");
    insertBT(t, 3, 57); inorder(t, 3); printf("\n");
    insertBT(t, 3, 54); inorder(t, 3); printf("\n");
    insertBT(t, 3, 33); inorder(t, 3); printf("\n");
    insertBT(t, 3, 75); inorder(t, 3); printf("\n");
    insertBT(t, 3, 124); inorder(t, 3); printf("\n");
    insertBT(t, 3, 122); inorder(t, 3); printf("\n");
    insertBT(t, 3, 123); inorder(t, 3); printf("\n");
*/

    //m=4
    BTree *t = NULL;
    insertBT(t, 4, 30); inorder(t, 4); printf("\n");
    insertBT(t, 4, 20); inorder(t, 4); printf("\n");
    insertBT(t, 4, 62); inorder(t, 4); printf("\n");
    insertBT(t, 4, 110); inorder(t, 4); printf("\n");
    insertBT(t, 4, 140); inorder(t, 4); printf("\n");
    insertBT(t, 4, 15); inorder(t, 4); printf("\n");
    insertBT(t, 4, 65); inorder(t, 4); printf("\n");
    insertBT(t, 4, 136); inorder(t, 4); printf("\n");
    insertBT(t, 4, 150); inorder(t, 4); printf("\n");
    insertBT(t, 4, 120); inorder(t, 4); printf("\n");

    insertBT(t, 4, 40); inorder(t, 4); printf("\n");
    insertBT(t, 4, 132); inorder(t, 4); printf("\n");
    insertBT(t, 4, 19); inorder(t, 4); printf("\n");
    insertBT(t, 4, 128); inorder(t, 4); printf("\n");
    insertBT(t, 4, 138); inorder(t, 4); printf("\n");
    insertBT(t, 4, 100); inorder(t, 4); printf("\n");
    insertBT(t, 4, 16); inorder(t, 4); printf("\n");
    insertBT(t, 4, 145); inorder(t, 4); printf("\n");
    insertBT(t, 4, 70); inorder(t, 4); printf("\n");
    insertBT(t, 4, 42); inorder(t, 4); printf("\n");

    insertBT(t, 4, 69); inorder(t, 4); printf("\n");
    insertBT(t, 4, 43); inorder(t, 4); printf("\n");
    insertBT(t, 4, 26); inorder(t, 4); printf("\n");
    insertBT(t, 4, 60); inorder(t, 4); printf("\n");
    insertBT(t, 4, 130); inorder(t, 4); printf("\n");
    insertBT(t, 4, 50); inorder(t, 4); printf("\n");
    insertBT(t, 4, 18); inorder(t, 4); printf("\n");
    insertBT(t, 4, 7); inorder(t, 4); printf("\n");
    insertBT(t, 4, 36); inorder(t, 4); printf("\n");
    insertBT(t, 4, 58); inorder(t, 4); printf("\n");

    insertBT(t, 4, 22); inorder(t, 4); printf("\n");
    insertBT(t, 4, 41); inorder(t, 4); printf("\n");
    insertBT(t, 4, 59); inorder(t, 4); printf("\n");
    insertBT(t, 4, 57); inorder(t, 4); printf("\n");
    insertBT(t, 4, 54); inorder(t, 4); printf("\n");
    insertBT(t, 4, 33); inorder(t, 4); printf("\n");
    insertBT(t, 4, 75); inorder(t, 4); printf("\n");
    insertBT(t, 4, 124); inorder(t, 4); printf("\n");
    insertBT(t, 4, 122); inorder(t, 4); printf("\n");
    insertBT(t, 4, 123); inorder(t, 4); printf("\n");

}





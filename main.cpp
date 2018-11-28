#include <stdio.h>
#include <stack>

using namespace std;

bool isFinished;

struct BTree
{
    int *k;
    struct BTree **p;
    int n; //data�� ����ִ� ����
    bool isLeafNode;
};

//������ m�� �� B-tree �ʱ�ȭ.
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

    //tmp�� ���� ����
    tmp = getNode(m+1);
    if(bt->isLeafNode) //leafNode�϶� split���(pointer�� ������� �ʴ� ���
    {
        //�ӽ� ��忡 �� ����.
        for(int i=0; i<insertIdx;i++)
        {
            tmp->k[i] = bt->k[i];
        }
        tmp->k[insertIdx] = newKey; //idx��ġ�� newKey����.
        for(int i=insertIdx+1;i<m+1;i++) //
        {
            tmp->k[i] = bt->k[i-1];
        }
    }
    else //���θ���ϋ� pointer�� ������ <���γ�尡 �ƴ϶�� ���� overflow���İ� �Ͼ�ٴ� ��.
    {
        for(int i=0; i<insertIdx;i++)
        {
            tmp->k[i] = bt->k[i];
            tmp->p[i] = bt->p[i];
        }
        //idx��ġ�� newKey����.
        tmp->k[insertIdx] = newKey;
        tmp->p[insertIdx] = bt->p[insertIdx];

        //���� ��� ����.
        tmp->k[insertIdx+1] = bt->k[insertIdx+1];
        tmp->p[insertIdx+1] = splitedNode.top();
        splitedNode.pop();

        //������ ��� ����.
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
    //�����ؾ��� ��带 ����.
    splitedNode.push(sameLevelNode);

    if(path.empty()) //���� �÷��� ���� ��� ����. tree level ++���ִ� �۾�.
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

//�� ��� �ȿ��� sKey�� �� ��ġ�� ã�� �Լ�.
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
    //B-tree�� null�� ���. ���� �������.
    if(bt == NULL)
    {
        bt = getNode(m);
        bt->k[0] = newKey;
        bt->n = 1;
        isFinished = true;
        return;
    }

    //���� ��ġ �˻�.
    struct BTree *x = bt;
    int i;
    while(x != NULL)
    {
        i = searchNode(x, m, newKey);//newKey�� �� idx�� ��ġ�� ã��.
        if(x->k[i] == newKey) return;//�ߺ� �߰�. �Լ� ����.
        //���� �� Ű�� �߰����� ������.-> ���ÿ� ���� ��ġ�� �ְ� �Ʒ��� ������.
        path.push(x);
        savedidx.push(i);
        x = x->p[i];
    }

    //stack�� �� ���κп��� LeafNode�� �����.
    //newKey ����.
    isFinished = false;

    //�н����� ������ ����� �ּҿ� �ε����� �޾ƿ�.
    //�� ��� leaf node�� ������ �������� �۾���.
    x = path.top();
    path.pop();
    i = savedidx.top();
    savedidx.pop();

    while(!isFinished)
    {
        if(x->n < m-1) //overflow�� �Ͼ�� �ʴ� ���.
        {
            //�� �� ã��
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
            for(int tt = tmpi; tt<x->n; tt++) //�������� �а� ��.
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
        else //overflow�� �Ͼ ���.
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





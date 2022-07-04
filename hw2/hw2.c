#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Node *NodePtr;
typedef struct ListNode *ListNodePtr;
typedef struct Node
{
    int id;
    int child_cnt;
    NodePtr parent;
    ListNodePtr child;
}Node;
typedef struct ListNode
{
    NodePtr data;
    ListNodePtr link;
}ListNode;

int read_data(int *ptr, int cnt)
{
    int *qtr = ptr;
    for (int i = 0; i < cnt; i++)
    {
        scanf("%d", &*qtr);
        qtr++;
    }
    *qtr = '\0';
    return 0;
}

int insert_parent(NodePtr node, int *parent, NodePtr node_data)
{
    if (*parent != -1) //有parent
    {
        NodePtr ptr = node_data;
        while ((ptr->id != *parent) && ptr!=NULL) //尋找parent
        {
            ptr++;
        }
        node->parent = ptr;
    }
    return 0;
}

int insert_child(NodePtr node, int child, NodePtr node_data)
{
    NodePtr ptr = node_data;
    ListNodePtr qtr = malloc(sizeof(ListNode)); //存放child資料
    while ((ptr->id != child) && (ptr != NULL)) //尋找child
    {
        ptr++;
    }
    qtr->data = ptr;
    qtr->link = NULL;
    if (node->child == NULL) //若此節點尚未存取任何child
    {
        node->child = qtr; //直接把指標指向qtr
    }
    else
    {
        ListNodePtr ltr = node->child;
        ListNodePtr tmp = ltr;
        while (ltr) //迴圈尋找最後存的child
        {
            tmp = ltr;
            ltr = ltr->link;
        }
        tmp->link = qtr; //把新的child接到後面
    }
    return 0;
}

int create_data(int cnt, NodePtr node_data, int *finial_parent)
{
    NodePtr ptr = node_data;
    int *qtr = finial_parent;
    int i = 0;
    while (i < cnt) //所有節點資料初始化
    {
        (ptr + i)->id = i;
        (ptr + i)->child_cnt = 0;
        (ptr + i)->parent = NULL;
        (ptr + i)->child = NULL;
        i++;
    }

    ptr = node_data;
    i = 0;
    while(i < cnt) //將parent與child的資料放回結構陣列
    {
        qtr = finial_parent;
        insert_parent(ptr+i, qtr+i, node_data); //插入parent資料
        int j = 0;
        while(j < cnt)
        {
            if(*(qtr + j) == (ptr + i)->id)
            {
                (ptr + i)->child_cnt = (ptr + i)->child_cnt + 1;
                insert_child(ptr+i, j, node_data); //插入child資料
            }
            j++;
        }
        i++;
    }
    return 0;
}

int find_root(int *root, int *original_parent)
{
    int i = 0;
    int *ptr = original_parent;
    while(*(ptr + i) != -1) //尋找root
    {
        i++;
    }
    *root = i;
    return 0;
}

int find_times(int cnt, NodePtr node_data)
{
    NodePtr ptr = node_data;
    int max_times = 0;
    int i = 0;
    while (i < cnt) //掃過所有節點
    {
        if ((ptr + i)->child_cnt == 0) //找到沒有child的節點(leaf)
        {
            NodePtr node_parent = (ptr + i)->parent; //往前找他的parent
            int times = 2; //次數計算初值
            while (node_parent->parent) //直到找到的parent是root時結束計算
            {
                NodePtr ntr = node_data;
                int j = 0;
                while ((node_parent->id != (ntr + j)->id) && j < cnt) //往回找節點的parent
                {
                    j++;
                }
                node_parent = (ntr + j)->parent; //把找到的值當作新的parent
                times++;
            }
            if (times > max_times)
            {
                max_times = times;
            }
        }
        i++;
    }
    printf("%d\n", max_times);
    return 0;
}

int print(int cnt, int *original_parent) //輸出
{
    int *ptr = original_parent;
    int i = 0;
    while (i < cnt)
    {
        printf("%d", *(ptr + i));
        if (i < cnt - 1)
        {
            printf(" ");
        }
        else
        {
            printf("\n");
        }
        i++;
    }
    return 0;
}

int run(int cnt, int root, NodePtr node_data, int *original_parent, int *final_parent)
{
    print(cnt, original_parent); //輸出原始資料

    int *target = malloc(sizeof(int) * cnt); //存所有目標節點
    int *changing_node = malloc(sizeof(int) * cnt); //需要一起改變傳輸方向的節點
    int ch_cnt = 0; //需要一起改變傳輸方向的節點計數

    *target = (node_data + root)->id; //初始目標節點為root
    ListNodePtr lptr = (node_data + root)->child; //指標指向root的第一個child

    int *tptr = target;
    int *cptr = changing_node;

    while (lptr) //將root的child的id放進changing_node
    {
        *cptr = lptr->data->id;
        cptr++;
        lptr = lptr->link;
        ch_cnt++;
    }

    while (ch_cnt > 0) //找到目標節點的child(如果沒有任何child存在則停止執行)
    {
        //找到child在original_parent位置的數值，改成目標節點的數值(迴圈)
        int i = 0;
        cptr = changing_node;
        while (i < ch_cnt)
        {
            *(original_parent + *cptr) = *(final_parent + *cptr);
            cptr++;
            i++;
        }

        //輸出
        print(cnt, original_parent);

        //將child設成新的目標節點(迴圈)
        memset(target, 0, cnt);
        tptr = target;
        cptr = changing_node;
        i=0;
        while (i < ch_cnt)
        {
            //*tptr++ = *cptr++;
            *tptr= *cptr;
            //printf("changing_node == %d\n",*cptr);
            tptr++;
            cptr++;
            i++;
        }

        //找到目標節點的child，將child放進changing_node(迴圈)
        memset(changing_node, 0, cnt);
        tptr = target;
        cptr = changing_node;
        i=0;
        int k=0;
        while (i < ch_cnt)
        {
            lptr = (node_data + *tptr)->child;
            while(lptr)
            {
                k++;
                *cptr = lptr->data->id;
                //printf("*cptr = %d\t\t\tk=%d\n",*cptr,k);
                cptr++;
                lptr = lptr->link;
            }
            i++;
            tptr++;
        }
        ch_cnt=k;
        //printf("k === %d\n",k);
        //ch_cnt--;
    }
    free(target);
    free(changing_node);
    return 0;
}

int free_data(int cnt, NodePtr node_data)
{
    NodePtr ptr = node_data;
    int i = 0;
    while (i < cnt)
    {
        if ((ptr + i)->child)
        {
            ListNodePtr qtr = (ptr + i)->child;
            ListNodePtr tmp = NULL;
            while (qtr)
            {
                tmp = qtr;
                qtr = qtr->link;
                free(tmp);
            }
        }
        i++;
    }
    free(node_data);
    return 0;
}

int main()
{
    //宣告
    int node_cnt; //節點數
    int root; //根節點
    int *original_parent; //原始輸入
    int *finial_parent; //最終輸出

    //讀取資料
    scanf("%d", &node_cnt);
    original_parent = malloc(sizeof(int) * (node_cnt + 1));
    finial_parent = malloc(sizeof(int) * (node_cnt + 1));
    read_data(original_parent, node_cnt);
    read_data(finial_parent, node_cnt);

    //建立結構陣列
    NodePtr node_data = malloc(sizeof(Node) * (node_cnt + 1));

    //建立資料到結構陣列
    create_data(node_cnt, node_data, finial_parent);

    //找到根節點
    find_root(&root, original_parent);

    //找到需要輸出幾次，輸出
    find_times(node_cnt, node_data);

    //開始輸出
    run(node_cnt, root, node_data, original_parent, finial_parent);

    //結束程式釋放記憶體空間
    free(original_parent);
    free(finial_parent);
    free_data(node_cnt, node_data);

    return 0;
}
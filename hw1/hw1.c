#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

struct node
{
    int id;
    int deg;//有幾個鄰居
    double x;
    double y;
    double va;
};

struct node nodes[1024];//存點用的結構陣列

int dist(struct node *pt,struct node *nb)//判斷是否為鄰居（距離小於1）
{
    double d_x,d_y;
    d_x=pt->x - nb->x;
    d_y=pt->y - nb->y;
    if((d_x*d_x + d_y*d_y) >= 1)
    {
        return -1;
    }
    return 1;
}

int main()
{
    //store elements
    int nums=0,rounds=0;
    scanf("%d %d",&nums,&rounds);
    for(int i=0;i<nums;i++)
    {
        scanf("%d %lf %lf %lf",&nodes[i].id,&nodes[i].x,&nodes[i].y,&nodes[i].va);
    }

    //searching neighbors
    for(int i=0;i<nums;i++)
    {
        int nb_cnt=0;//鄰居數
        for(int j=0;j<nums;j++)
        {
            if(i==j)
            {
                continue;
            }
            if(dist(&nodes[i],&nodes[j])==1)
            {
                nb_cnt++;
            }
        }
        nodes[i].deg=nb_cnt;//將鄰居數存回deg裡
    }

    //create two matrices, one for va, one for tva & nb_tva
    double nd_va[nums];//各點的值
    double t_va[nums][nums];//信任度
    for(int i=0;i<nums;i++)
    {
        nd_va[i]=nodes[i].va;
    }

    //caculating tva
    for(int i=0;i<nums;i++)
    {
        double sum_nb_va=0.0;//存取所有信任度的總和
        for(int j=0;j<nums;j++)
        {
            if(i==j)
            {
                continue;
            }
            if(dist(&nodes[i],&nodes[j])==1)
            {
                if(nodes[i].deg>nodes[j].deg)
                {
                    t_va[i][j]=(double)1/(nodes[i].deg*2);
                }
                else
                {
                    t_va[i][j]=(double)1/(nodes[j].deg*2);
                }
                sum_nb_va=sum_nb_va+t_va[i][j];
            }
            else
            {
                t_va[i][j]=0;
            }
        }
        t_va[i][i]=1-sum_nb_va;//將總和用1扣除即為對自己的信任度
    }

    //run "rounds" times operation & print out each value (結果取到小數點後第二位)
    printf("%d %d\n",nums,rounds);
    for(int row=0;row<nums;row++)
    {
        printf("%.2lf",nd_va[row]);
        if((row+1)!=nums)
        {
            printf(" ");
        }
    }
    printf("\n");
    double new_va[nums];//暫存新的va
    for(int i=0;i<rounds-1;i++)//跑rounds-1次
    {
        for(int row=0;row<nums;row++)//矩陣乘法
        {
            new_va[row]=0;
            for(int col=0;col<nums;col++)//公式
            {
                new_va[row]=new_va[row]+t_va[row][col]*nd_va[col];
            }
            printf("%.2lf",new_va[row]);
            if((row+1)!=nums)
            {
                printf(" ");
            }
        }
        for(int i=0;i<nums;i++)//將新的value覆蓋掉舊的
        {
            nd_va[i]=new_va[i];
        }
        printf("\n");
    }

    return 0;
}
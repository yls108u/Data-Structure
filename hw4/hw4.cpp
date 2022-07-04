#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<string>

void init(bool **bits, int m, int r, int **a, int **b, int p) //依照要求做資料初始化
{
    (*bits) = (bool*)calloc(m, sizeof(bool));
    (*a) = (int*)calloc(r, sizeof(int));
    (*b) = (int*)calloc(r, sizeof(int));
    srand(1);
    for(int i = 0; i < r; i++){
        *(*a + i) = rand() % (p - 1) + 1;
    }
    srand(2);
    for(int i = 0; i < r; i++){
        *(*b + i) = rand() % (p - 1) + 1;
    }
    return ;
}

int myhash(char *str, int count, int m, int r, int p, int *a, int *b) //回傳hash後key值
{
    std::hash<std::string> hasher;
    return (a[count] * hasher(str) + b[count]) % p % m;
}

void insert(bool *bits, int m, int r, int p, char *str, int *a, int *b)
{
    for(int i = 0; i < r; i++){
        int key = myhash(str, i, m, r, p, a, b); //呼叫hash
        *(bits + key) = true;
    }
    return ;
}

bool query(bool *bits, int m, int p, int r, char *str, int *a, int *b)
{
    for(int i = 0; i < r; i++){
        int key = myhash(str, i, m, r, p, a, b); //呼叫hash
        if(*(bits + key) != true) return false; //bool array裡面不是1，回傳錯誤
    }
    return true;
}

int main()
{
    int prime;
    int hash_function;
    int bit_cnt, word_cnt, test_cnt;
    int *a = NULL;
    int *b = NULL;
    bool *bits = NULL;

    scanf("%d %d %d %d %d", &bit_cnt, &hash_function, &word_cnt, &test_cnt, &prime);

    //初始化bool array, a, b
    init(&bits, bit_cnt, hash_function, &a, &b, prime);
    char *word = (char *)malloc(sizeof(char) * 1024);
    char *test = (char *)malloc(sizeof(char) * 1024);

    //把hash的數值放進bool array裡
    for(int i = 0; i < word_cnt; i++){
        scanf("%s", word);
        insert(bits, bit_cnt, hash_function, prime, word, a, b);
    }

    //檢查test資料有沒有在bool array裡
    for(int i = 0; i < test_cnt; i++){
        scanf("%s", test);
        if(query(bits, bit_cnt, prime, hash_function, test, a, b)){
            printf("%s: true\n", test);
        }
        else
            printf("%s: false\n", test);
    }

    return 0;
}
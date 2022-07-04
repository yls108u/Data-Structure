#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>

typedef struct Switch *Switchptr;
typedef struct Switch{
    int id;
    int capacity;
}Switch;

typedef struct Link *Linkptr;
typedef struct Link{
    int id1;
    int id2;
    int weight;
}Link;

typedef struct Rule *Ruleptr;
typedef struct Rule{
    int id;
    int priority;
    char *src;
    char *dst;
    char *act;
}Rule;

typedef struct R_table *R_tableptr;
typedef struct R_table{
    int src_upper;
    int src_lower;
    int dst_upper;
    int dst_lower;
    Ruleptr rule;
}R_table;

int bits; //bit數
int rules; //rule數
int max_id; //記錄目前最大的id
int ori_max_id;
int max_priority; //記錄目前最大的優先度
int error = 0;

void create_dst_table(int start, int cnt, int **cost, int *dst) //將起點switch與各個switch權重從cost_table中取出放進dst_table
{
    for(int i = 0; i < cnt; i++){
        *(dst + i) = *( *(cost + i) + start);
    }
    return ;
}

void create_renew_table(int start, int cnt,int *dst, int *renew) //先記錄起點switch為哪些switch的上家，起點的上家預設-1
{
    for(int i = 0; i < cnt; i++){
        *(renew + i) = -1;
        if( *(dst + i) > 0){
            *(renew + i) = start;
        }
    }
    return ;
}

void reset(int cnt, int *ptr) //清空int陣列成-1
{
    for(int i = 0; i <= cnt; i++){
        *(ptr + i) = -1;
    }
    return ;
}

int update(int point, int cnt, int *done_switches) //當所選的switch在done_switches裡沒有被記錄時回傳1
{
    for(int i = 0; i < cnt; i++){
        if( *(done_switches + i) == point) return 0;
    }
    return 1;
}

int compare(int point, int last, int cnt, int *renew) //藉由比較renew_table來決定，在更新value與原先value相同時，最短路徑是否需要更新
{
    /*printf("compare renew:");
    for(int i=0;i<cnt;i++){
        printf("%d ",*(renew + i));
    }printf("\n");
    printf("point=%d\tlast=%d\n",point,last);*/

    int *origin = malloc(sizeof(int) * (cnt + 1));
    int *New = malloc(sizeof(int) * (cnt + 1));
    int *temp = malloc(sizeof(int) * (cnt + 1));
    int o_head = 0, n_head = 0, tail = last;

    reset(cnt, origin);
    reset(cnt, New);
    reset(cnt, temp);
    
    for(int i = 0; i < cnt; i++){
        *(temp + i) = last;
        if( *(renew + last) == -1){
            o_head = i + 1;
            break;
        }
        last = *(renew + last);
    }
    for(int i = o_head - 1, j = 0; i >= 0; i--, j++){
        *(origin + j) = *(temp + i);
    }

    reset(cnt, temp);
    for(int i = 0; i < cnt; i++){
        *(temp + i) = tail;
        if(i == 0){
            tail = point;
            continue;
        }
        if( *(renew + tail) == -1){
            n_head = i + 1;
            break;
        }
        tail = *(renew + tail);
    }
    for(int i = n_head - 1, j = 0; i >= 0; i--, j++){
        *(New + j) = *(temp + i);
    }

    /*printf("nhead=%d\tohead=%d\n",n_head,o_head);
    printf("start:\n");
    for(int j = 0; j < cnt; j++){
        printf("new=%d\tori=%d\n",*(New + j),*(origin + j));
    }printf("end\n");*/

    if(n_head < o_head){ //新的路徑switch數較少
        free(origin);
        free(New);
        free(temp);
        return 1;
    }
    else if(n_head > o_head){ //新的路徑switch數較多
        free(origin);
        free(New);
        free(temp);
        return 0;
    }
    else{ //switch數相同,比較id
        for(int i = 0; i < cnt; i++){
            if( *(New + i) < *(origin + i)){
                //printf("access!\n");
                free(origin);
                free(New);
                free(temp);
                return 1;
            }
            else if( *(New + i) > *(origin + i)){
                //printf("fail!\n");
                free(origin);
                free(New);
                free(temp);
                return 0;
            }
        }
    }
    return 0;
}

void shortestpath(int end, int cnt, int **cost, int *dst, int *renew, int *path)
{
    int *done_switches = malloc(sizeof(int) * (cnt + 1));
    reset(cnt, done_switches);
    for(int i = 0; i < cnt; i++){
        int target = 0; //每次更新路徑用的switch
        int min = 1000; //dst的最小值
        for(int j = 0; j < cnt; j++){
            if(( *(dst + j) > 0) && ( *(dst + j) < min) && update(j, cnt, done_switches)){
                target = j;
                min = *(dst + j);
                *(done_switches + i) = target;
            }
        }

        /*printf("dst:\t");
        for(int i=0;i<cnt;i++){
            printf("%d ",*(dst+i));
        }printf("\n");
        printf("renew:\t");
        for(int i=0;i<cnt;i++){
            printf("%d ",*(renew+i));
        }printf("\n");
        printf("target = %d\n",target);*/
    
        int sum = 0; //用target更新過後的路徑cost
        for(int j = 0; j < cnt; j++){
            if( *( *(cost + target) + j) > 0){
                sum = *( *(cost + target) + j) + min;
                if((sum < *(dst + j)) || *(dst + j) == -1){
                    *(dst + j) = sum;
                    *(renew + j) = target;
                }
                else if(sum == *(dst + j)){
                    //printf("sum=%d\tdst=%d\tj=%d\n",sum,*(dst+j),j);
                    if(compare(target, j, cnt, renew)) *(renew + j) = target;
                }
            }
        }
    }

    int *temp = malloc(sizeof(int) * (cnt + 1));
    int head = 0;
    reset(cnt, temp);
    for(int i = 0; i < cnt; i++){
        *(temp + i) = end;
        if( *(renew + end) == -1){
            head = i + 1;
            break;
        }
        end = *(renew + end);
    }
    for(int i = head - 1, j = 0; i >= 0; i--, j++){ //把路徑copy回path裡
        *(path + j) = *(temp + i);
        //printf("%d ",*(path + j));
    }
    printf(" ");

    free(done_switches);
    free(temp);
    return ;
}

void bound(char *ptr, int *upper, int *lower)
{
    int cnt = bits;
    for(int i = 0; i < bits; i++){
        if( *(ptr + i) == '*'){ //printf("%d + 2^%d ",*upper,cnt-1);
            *upper = *upper + (1 << cnt) - 1; //printf("= %d\n",*upper);
            break;
        }
        char *qtr = malloc(sizeof(char));
        *qtr = *(ptr + i); //printf("qtr = %s",qtr);
        int value = atoi(qtr); //printf("%d ",value);
        cnt--;
        *upper = *upper + ( value << (cnt));
        *lower = *lower + ( value << (cnt));
    }
    return ;
}

void create_q_table(Ruleptr Rules, R_tableptr *q_table)
{
    for(int i = 0; i < rules; i++){
        int s_upper = 0, s_lower = 0;
        char *sptr = (Rules + i)->src;
        bound(sptr, &s_upper, &s_lower); //printf("src:%s up:%d low:%d\n",sptr,s_upper,s_lower);
        int d_upper = 0, d_lower = 0;
        char *dptr = (Rules + i)->dst;
        bound(dptr, &d_upper, &d_lower); //printf("dst:%s up:%d low:%d\n",dptr,d_upper,d_lower);
        for(int j = s_lower; j <= s_upper; j++){
            for(int k = d_lower; k <= d_upper; k++){
                if(( *(q_table + j) + k)->rule == NULL){
                    ( *(q_table + j) + k)->src_upper = s_upper;
                    ( *(q_table + j) + k)->src_lower = s_lower;
                    ( *(q_table + j) + k)->dst_upper = d_upper;
                    ( *(q_table + j) + k)->dst_lower = d_lower;
                    ( *(q_table + j) + k)->rule = Rules + i; //printf("id = %d\n",( *(q_table + j) + k)->rule->id);
                }
            }
        }
    }
    return ;
}

void clean(int *Rules)
{
    for(int i = 0; i < rules; i++){
        *(Rules + i) = 0;
    }
    return ;
}

int done(int r_cnt, int *total_rules, int id)
{
    for(int i = 0; i < r_cnt; i++){ //printf("id = %d\n",id);
        if(*(total_rules + i) == id) return 1;
    }
    return 0;
}

int is_internal(int r_upper, int r_lower, int c_upper, int c_lower, R_tableptr rule)
{
    //printf("rupper = %d\trlower = %d \n",r_upper,r_lower);
    //printf("src_upper = %d\tsrc_lower = %d \n",(rule)->src_upper,(rule)->src_lower);
    //printf("cupper = %d\tclower = %d \n",c_upper,c_lower);
    //printf("dst_upper = %d\tdst_lower = %d \n",(*rule)->dst_upper,(*rule)->dst_lower);
    return (r_lower <= rule->src_lower && rule->src_upper <= r_upper && c_lower <= rule->dst_lower && rule->dst_upper <= c_upper);
}

void insert_id(int r_cnt, int *total_rules, int id)
{
    *(total_rules + r_cnt) = id; //printf("insert access id = %d\n",*(total_rules+r_cnt));
    return ;
}

void sort_internal_rules(int r_cnt, int *cur_in_rules)
{
    /*printf("UNsorted internal rules:\n");
    for(int i=0;i<rules;i++){
        printf("%d ",*(cur_in_rules + i));
    }printf("\n");*/

    int len = 1;
    //if(r_cnt == 0) return ;

    if( *(cur_in_rules + r_cnt - 1) != 0) len = r_cnt + 1;
    else{
        for(int i = 0; i < r_cnt; i++){
            if( *(cur_in_rules + i) == 0){
                *(cur_in_rules + i) = *(cur_in_rules + r_cnt);
                *(cur_in_rules + r_cnt) = 0;
                len = i + 1;
                break;
            }
        }
    }
    //printf("len=%d\n",len);
    for(int i = 0; i < len; i++){
        for(int j = 0; j < len - i - 1; j++){
            if( *(cur_in_rules + j) > *(cur_in_rules + j + 1)){
                int temp = *(cur_in_rules + j + 1);
                *(cur_in_rules + j + 1) = *(cur_in_rules + j);
                *(cur_in_rules + j) = temp;
            }
        }
    }

    /*printf("sorted internal rules:\n");
    for(int i=0;i<rules;i++){
        printf("%d ",*(cur_in_rules + i));
    }printf("\n");*/
    return ;
}

void write_q(int row, int col, int r_range, int c_range, char *q_src, char *q_dst) //待檢查！！！！！！！！！
{
    int ignore_range_r = 0, ignore_range_c = 0;
    if(r_range){
        while(r_range){
            r_range = r_range / 2;
            ignore_range_r++;
        }
        ignore_range_r--;
    }
    if(c_range){
        while(c_range){
            c_range = c_range / 2;
            ignore_range_c++;
        }
        ignore_range_c--;
    }
    memset(q_src, 0, bits);
    memset(q_dst, 0, bits);
    for(int i = bits; i > ignore_range_r; i--){
        int r_pow = 1 << (i - 1);
        if(row / r_pow){
            strcat(q_src, "1");
            row = row - r_pow;
        }
        else strcat(q_src, "0");
    }
    if(ignore_range_r) strcat(q_src, "*");
    for(int i = bits; i > ignore_range_c; i--){
        int c_pow = 1 << (i - 1);
        if(col / c_pow){
            strcat(q_dst, "1");
            col = col - c_pow;
        }
        else strcat(q_dst, "0");
    }
    if(ignore_range_c) strcat(q_dst, "*");
    return ;
}

void qcpy(int *r_cnt, int *p_r_cnt, int *area, int *p_area, int *cur, int *pre)
{
    *p_r_cnt = *r_cnt;
    *p_area = *area;
    for(int i = 0; i < rules; i++){
        *(pre + i) = *(cur + i);
    }
    return ;
}

void rcpy(int *qptr, int *tptr)
{
    for(int i = 0; i < rules; i++){
        *(qptr + i) = *(tptr + i);
    }
    return ;
}

int judge_q(int in_cnt, int p_in_cnt, int area, int p_area, int *cur, int *pre)
{
    /*for(int j=0;j<rules;j++){
        printf("%d<->%d\t",*(pre+j),*(cur+j));
    }printf("\n");*/

    //printf("1fault\n");
    //printf("incnt=%d\tpincnt=%d\n",in_cnt,p_in_cnt);
    if(in_cnt > p_in_cnt) return 1;
    else if(in_cnt < p_in_cnt) return 0;

    //printf("2fault\n");
    //printf("parea=%d\tarea=%d\n",p_area,area);
    if(p_area > area) return 1;
    else if(p_area < area) return 0;

    //printf("3fault\n");
    /*for(int i = 0; i < rules; i++){
        if(*(pre + i) > *(cur + i)) return 1;
        if(*(pre + i) < *(cur + i)) return 0;
    }*/
    int pre_q, cur_q, backward;
    for(pre_q = 0; *(pre + pre_q) < ori_max_id && *(pre + pre_q) != 0; pre_q++) ;
    for(cur_q = 0; *(cur + cur_q) < ori_max_id && *(cur + cur_q) != 0; cur_q++) ;
    //printf("preq=%d curq=%d\n",pre_q,cur_q);
    for(backward = in_cnt; backward - pre_q > 0 && backward - cur_q > 0; backward--){
        if(*(cur + backward - 1) > *(pre + backward - 1)) return 1;
        else if(*(cur + backward - 1) < *(pre + backward - 1)) return 0;
    }
    if(cur_q < pre_q) return 1;
    else if(cur_q > pre_q) return 0;
    for(int i = 0; i < backward; i++){
        if(*(cur + i) < *(pre + i)) return 1;
        else if(*(cur + i) > *(pre + i)) return 0;
    }
    //printf("你不應該跑到這的！！\n");
    return 0;
}

void create_q(Ruleptr q, char *q_src, char *q_dst)
{
    char *action = malloc(sizeof(char) * 5);
    strcpy(action,"Fwd");
    q->id = max_id++;
    q->priority = max_priority++;
    q->src = q_src;
    q->dst = q_dst;
    q->act = action;
    return ;
}

void sort_q_rules(int *q_rules, Ruleptr Rules)
{
    /*printf("\nRules info:\n");
    for(int i =0;i<rules;i++){
        printf("id=%d\tpri=%d\n",(Rules+i)->id,(Rules+i)->priority);
    }printf("\n");*/
    /*printf("\nunsort q_rule:\n");
    for(int i =0;i<rules;i++){
        printf("%d ",*(q_rules + i));
    }printf("\n");*/
    int r_cnt = 0;
    for(r_cnt = 0; *(q_rules + r_cnt) != 0; r_cnt++) ;
    for(int i = 0; i < r_cnt; i++){
        for(int j = 0; j < r_cnt - i - 1; j++){
            int cur_id = *(q_rules + j);
            int next_id = *(q_rules + j + 1);
            if((Rules + cur_id - 1)->priority < (Rules + next_id - 1)->priority){
                int temp = *(q_rules + j + 1);
                *(q_rules + j + 1) = *(q_rules + j);
                *(q_rules + j) = temp;
            }
        }
    }
    /*printf("q_rule:\n");
    for(int i =0;i<rules;i++){
        printf("%d ",*(q_rules + i));
    }printf("\n");*/
    return ;
}

void search_q(int size, Ruleptr Rules, int *q_rules, R_tableptr **q_table, Ruleptr q)
{   //printf("0.0\n");
    float utility = 0.0;
    int table_size = 1 << bits;
    int r_bound, c_bound;
    int internal = 0, overlapping = 0, r_cnt = 0;
    int p_in_cnt = 0; //上一個q的internal rule數
    int area = 0, p_area = 0; //目前q的area, 上一個q的area
    char *q_src = malloc(sizeof(char) * bits);
    char *q_dst = malloc(sizeof(char) * bits);
    int *total_rules = malloc(sizeof(int) * rules); //目前q的所有rules id
    int *cur_in_rules = malloc(sizeof(int) * rules); //目前q的internal rules id
    int *pre_in_rules = malloc(sizeof(int) * rules); //上一個q的internal rules id
    clean(pre_in_rules);
    for(int r_pow = 0; r_pow <= bits; r_pow++){ //固定q的面積
        for(int c_pow = 0; c_pow <= bits; c_pow++){
            for(int k = r_bound = (1 << r_pow); k <= table_size; k = k + (1 << r_pow)){ //在table範圍內搜尋q
                for(int l = c_bound = (1 << c_pow); l <= table_size; l = l + (1 << c_pow)){
                    internal = 0, overlapping = 0, r_cnt = 0, area = 0;
                    clean(total_rules);
                    clean(cur_in_rules);
                    for(int m = k - 1, o = 0; o < r_bound; m--, o++){ //在q範圍內搜尋rule
                        for(int n = l - 1, p = 0; p < c_bound; n--, p++){
                            if(done(r_cnt, total_rules, (*q_table)[m][n].rule->id)) continue; //已經找過的id就跳過
                            if(is_internal(k - 1, k - r_bound, l - 1, l - c_bound, &(*q_table)[m][n])){
                                internal++;
                                insert_id(r_cnt, cur_in_rules, (*q_table)[m][n].rule->id);
                                sort_internal_rules(r_cnt, cur_in_rules);
                            }
                            else overlapping++;
                            insert_id(r_cnt, total_rules, (*q_table)[m][n].rule->id); //將新的rule存進total rules
                            r_cnt++;
                            if(r_cnt > size) break; //存的rule大過於switch size 跳出
                        }
                        if(r_cnt > size) break; //存的rule大過於switch size 跳出
                    }
                    if((r_cnt > size) || (internal - 1 < 0)) continue; //所得q不符合要求,繼續找
                    else if(((float)(internal - 1) / (float)(overlapping + 1)) > utility){
                        utility = (float)(internal - 1) / (float)(overlapping + 1);
                        area = r_bound * c_bound;
                        write_q(k - 1, l - 1, r_bound, c_bound, q_src, q_dst); //紀錄q的範圍(位置:(列,行),面積:列寬*行寬,src,dst)
                        qcpy(&internal, &p_in_cnt, &area, &p_area, cur_in_rules, pre_in_rules);
                        rcpy(q_rules, total_rules);
                    }
                    else if(((float)(internal - 1) / (float)(overlapping + 1)) == utility && utility != 0){ //utility相同時，依比較結果決定是否更改成新的q
                        area = r_bound * c_bound;
                        if(judge_q(internal, p_in_cnt, area, p_area, cur_in_rules, pre_in_rules)){
                            write_q(k - 1, l - 1, r_bound, c_bound, q_src, q_dst);
                            qcpy(&internal, &p_in_cnt, &area, &p_area, cur_in_rules, pre_in_rules);
                            rcpy(q_rules, total_rules);
                        }
                    }
                }
            }
        }
    }
    //printf("UUU=%.2f\n",utility);
    /*printf("rcnt = %d\n",r_cnt);
    printf("q_rule unsort:\n");
    for(int i =0;i<rules;i++){
        printf("%d ",*(q_rules + i));
    }printf("\n");*/
    if(utility == 0) error = 1;
    create_q(q, q_src, q_dst);
    sort_q_rules(q_rules, Rules); //把優先度高的rule放到前面
    rules++; //q增加表示rule總數增加(包含被覆蓋的rule)
    free(total_rules);
    free(cur_in_rules);
    free(pre_in_rules);
}

void print(int switch_id, Ruleptr q, int *q_rules, Ruleptr Rules)
{
    int r_cnt;
    for(r_cnt = 0; ( *(q_rules + r_cnt) != 0) && (r_cnt < rules); r_cnt++) ;
    printf("%d %d\n", switch_id, r_cnt);

    /*printf("\ncovered & print rules:\n");
    for(r_cnt = 0; ( *(q_rules + r_cnt) != 0) && (r_cnt < rules); r_cnt++){
        printf("%d ",*(q_rules+r_cnt));
    }printf("\n");*/

    for(int i = 0; i < r_cnt; i++){
        int rule_id = *(q_rules + i) - 1;
        char *output = calloc(0, sizeof(char) *20);
        char *Rsrc = (Rules + rule_id)->src;
        char *Rdst = (Rules + rule_id)->dst;
        //printf("\nRule id = %d priority = %d src = %s dst = %s act = %s\n",(Rules + rule_id)->id,(Rules + rule_id)->priority,Rsrc,Rdst,(Rules + rule_id)->act);
        char *qsrc = q->src;
        char *qdst = q->dst; 
        //printf("q id = %d priority = %d src = %s dst = %s act = %s\n",q->id,q->priority,q->src,q->dst,q->act);
        for(int j = 0; j < bits; j++){
            if(*(Rsrc + j) == *(qsrc + j) && *(Rsrc + j) != '*' && *(qsrc + j) != '*') printf("%c", *(Rsrc + j));
            else if(*(Rsrc + j) == '*' && *(qsrc + j) == '*'){
                printf("*");
                break;
            }
            else if(*(Rsrc + j) == '*'){
                printf("%s",(qsrc + j));
            }
            else if(*(qsrc + j) == '*'){
                printf("%s",(Rsrc + j));
            }
        }
        printf(" ");
        for(int j = 0; j < bits; j++){
            if(*(Rdst + j) == *(qdst + j) && *(Rdst + j) != '*' && *(qdst + j) != '*') printf("%c", *(Rdst + j));
            else if(*(Rdst + j) == '*' && *(qdst + j) == '*'){
                printf("*");
                break;
            }
            else if(*(Rdst + j) == '*'){
                printf("%s",(qdst + j));
            }
            else if(*(qdst + j) == '*'){
                printf("%s",(Rdst + j));
            }
        }
        printf(" %s\n",(Rules + rule_id)->act);
    }
    return ;
}

void cover_rule(Ruleptr *q, R_tableptr **q_table)
{
    int s_upper = 0, s_lower = 0;
    char *qsrc = (*q)->src;
    bound(qsrc, &s_upper, &s_lower); //printf("qsrc=%s supper=%d slower=%d\n",qsrc,s_upper,s_lower);
    int d_upper = 0, d_lower = 0;
    char *qdst = (*q)->dst;
    bound(qdst, &d_upper, &d_lower); //printf("qdst=%s dupper=%d dlower=%d\n",qdst,d_upper,d_lower);
    for(int i = s_lower; i <= s_upper; i++){
        for(int j = d_lower; j <= d_upper; j++){
            ( *( *(q_table + 0) + i) + j)->src_upper = s_upper;
            ( *( *(q_table + 0) + i) + j)->src_lower = s_lower;
            ( *( *(q_table + 0) + i) + j)->dst_upper = d_upper;
            ( *( *(q_table + 0) + i) + j)->dst_lower = d_lower;
            ( *( *(q_table + 0) + i) + j)->rule = *q;
            //printf("new id = %d\n",( *( *(q_table + 0) + i) + j)->rule->id);
        }
    }
    /*printf("\nNew table:\n");
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            printf("%d\t",( *( *(q_table + 0) + i) + j)->rule->id);
        }printf("\n");
    }*/
    /*printf("\nNew table act:\n");
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            printf("%s\t",( *( *(q_table + 0) + i) + j)->rule->act);
        }printf("\n");
    }*/
    return ;
}

void place_rules(int end, int *path, int s_cnt, Switchptr switches, R_tableptr *q_table, Ruleptr Rules)
{
    int path_cnt;
    int *q_rules = malloc(sizeof(int) * (rules + 1));
    for(path_cnt = 0; *(path + path_cnt) != -1 && path_cnt < s_cnt; path_cnt++) ;
    printf("%d\n", path_cnt);
    for(int i = 0; i < path_cnt; i++){
        //printf("%d ", *(path + i));
        int current_switch = *(path + i);
        int switch_size = (switches + current_switch)->capacity;
        Ruleptr q = Rules + rules;
        clean(q_rules);
        search_q(switch_size, Rules, q_rules, &q_table, q); //找最佳q
        if(error == 1){
            printf("%d 0\n",*(path + i));
            error = 0;
            if( *(path + i) == end) break;
            continue;
        }
        print(*(path + i), q, q_rules, Rules); //輸出
        cover_rule(&q, &q_table); //將新的rule q覆蓋回q_table
        if( *(path + i) == end) break;
    }
    free(q_rules);
    return ;
}

int main()
{
    //read data
    scanf("%d", &bits);
    int start, end; //起點＆終點switch
    scanf("%d %d", &start, &end);

    int switches_cnt; //num of switches
    scanf("%d", &switches_cnt);
    Switchptr switches = (Switchptr)malloc(sizeof(Switch) * (switches_cnt + 1)); //存所有的switch
    for(int i = 0; i < switches_cnt; i++){
        scanf("%d %d", &(switches + i)->id, &(switches + i)->capacity);
    }

    int links_cnt; //num of links
    scanf("%d",&links_cnt);
    Linkptr links = (Linkptr)malloc(sizeof(Link) * (links_cnt + 1)); //存switch之間的權重
    for(int i = 0; i < links_cnt; i++){
        scanf("%d %d %d", &(links + i)->id1, &(links + i)->id2, &(links + i)->weight);
    }

    scanf("%d", &rules);
    max_id = 1; //記錄所有的id，從1開始往上加
    ori_max_id = rules;
    max_priority = rules + 1; //記錄輸出優先度，數值越大優先度越高
    Ruleptr Rules = (Ruleptr)malloc(sizeof(Rule) * (rules + 100)); //存所有的rule，包括之後產生的q都會放裡面
    for(int i = 0; i < rules; i++){
        char *sip = malloc(sizeof(char) * (bits + 1));
        char *dip = malloc(sizeof(char) * (bits + 1));
        char *action = malloc(sizeof(char) * (bits + 1));
        scanf("%s %s %s", sip, dip, action);
        (Rules + i)->id = max_id++; //id從1開始
        (Rules + i)->priority = rules -i; //數值大的優先度較高
        //printf("id = %d\tpri = %d\n",(Rules + i)->id,(Rules + i)->priority);
        (Rules + i)->src = sip;
        (Rules + i)->dst = dip;
        (Rules + i)->act = action;
    }

    //find path
    int **cost_table = malloc(sizeof(int*) * (switches_cnt + 1)); //存switch路之間的cost
    for(int i = 0; i < switches_cnt; i++){
        *(cost_table + i) = malloc(sizeof(int) * (switches_cnt + 1));
    }
    for(int i = 0; i < switches_cnt; i++){ //全部初始化為-1
        for(int j = 0; j < switches_cnt; j++){
            *( *(cost_table + i) + j) = -1;
            if(i == j) *( *(cost_table + i) + j) = 0;
        }
    }
    for(int i = 0; i < links_cnt; i++){ //把有紀錄權重的switch連結數值放進table
        cost_table[(links + i)->id1][(links + i)->id2] = (links + i)->weight;
        cost_table[(links + i)->id2][(links + i)->id1] = (links + i)->weight;
    }

    int *dst_table = malloc(sizeof(int) * (switches_cnt + 1)); //存起點到各點的cost
    create_dst_table(start, switches_cnt, cost_table, dst_table);

    int *renew_table = malloc(sizeof(int) * (switches_cnt + 1)); //存更新各點cost的switch
    create_renew_table(start, switches_cnt, dst_table, renew_table);

    int *shortest_path = malloc(sizeof(int) * (switches_cnt + 1)); //最短路徑
    for(int i = 0; i < switches_cnt; i++){
        *(shortest_path + i) = -1;
    }
    shortestpath(end, switches_cnt, cost_table, dst_table, renew_table, shortest_path);

    //place rules into switches and print out
    int size = 1 << bits; //存rule的q_table的大小
    R_tableptr *q_table = malloc(sizeof(R_tableptr) * (size + 1)); //存rule的table
    for(int i = 0; i < size; i++){
        *(q_table + i) = malloc(sizeof(R_table) * (size + 1));
    }
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            ( *(q_table + i) + j)->rule = NULL;
        }
    }
    create_q_table(Rules, q_table);
    /*printf("Origin q_table:\n");
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            printf("%d\t",( *(q_table + i) + j)->rule->id);
        }printf("\n");
    }*/
    place_rules(end, shortest_path, switches_cnt, switches, q_table, Rules);

    return 0;
}
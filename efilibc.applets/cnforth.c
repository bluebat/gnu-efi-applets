//   Copyright (c) 2017  Chai Fei <cforth@cfxyz.com>
//
//   cnforth -- not forth
//
//   GNU-EFI porting by Wei-Lun Chao <bluebat@member.fsf.org>, 2017

#include <efi.h>
#include <efilib.h>
#include <efilibc/efilibc.h>
#include <efilibc/stdio.h>
#include <efilibc/string.h>
#include <efilibc/stdlib.h>
#include <efilibc/ctype.h>
#include "cnforth.h"

int check_blank(char c)
{
    return (c==' ' || c=='\t' || c=='\n');
}


char *parse_word()
{
    char *now;
    while (check_blank(*text_p)) //跳过字符串头部的空白字符后返回指针
        text_p++;
    
    now = text_p;
    
    while ( !check_blank(*text_p)  && (*text_p)!='\0') //跳过字符串中第一个词
        text_p++;
    
    if(*text_p == '\0') 
        return now;
    *text_p = '\0';   //将字符串中第一个词后的空格变为'\0'
    text_p++;
    
    return now;
}


Word *create(char *name, fn_p  fp)
{
    Word *w=(Word*)malloc(sizeof(Word));
    w->code_p=fp;
    
    w->name=(char*)malloc(strlen(name) + 1);
    strcpy(w->name,name);
    
    w->wplist=NULL;   

    w->flag = HIDE_WORD;

    return w;
}


void does(Word *w, Word **list, int n)
{
    if(n != 0) {
        w->wplist = (Word**)malloc(n);
        memcpy(w->wplist,list, n);
    } else {
        w->wplist = list;
    }
    w->flag = REVEAL_WORD;
}


Word *def_core(char *name, fn_p  fp)
{
    Word *w=create(name, fp);
    w->flag = REVEAL_WORD;

    return w;
}


void colon_code()
{
    RP++;
    *RP=(CELL)IP;
    IP=(*IP)->wplist-1;
    PRINT("[DEBUG]进入子例程\n")
}


void cons_code()
{
    ds_push((CELL)((*IP)->wplist));
}


void var_code()
{
    ds_push((CELL)*IP);
}


Dict *dict_init()
{
    Dict *dict=(Dict*)malloc(sizeof(Dict));
    dict->size = 0;
    dict->head = NULL;
    return dict;
}


int dict_ins_next(Dict *dict, Word *word)
{
    word->link = dict->head;
    dict->head = word;
    dict->size++;
    return 0;
}


Word *dict_search_name(Dict *dict, char *name)
{
    Word *w = dict->head;
    //为了支持递归忽略隐藏词
    while ((w != NULL && strcmp(w->name,name))
        || (w != NULL && !strcmp(w->name,name) && w->flag == HIDE_WORD))
    {  
        w=w->link;
    }
    
    return w;
}


void dict_destroy_word(Word *word)
{
    free(word->name);
    if(word->code_p == colon_code) free(word->wplist);
    free(word);
}


int dict_rem_after(Dict *dict, char *name)
{
    Word *w = dict_search_name(dict, name);
    Word *del_w;
    
    if(w == NULL)
    {
        printf("%s :\n\tCan't find!\n", name);
    }
    else
    {
        if(w->wplist == NULL)
        {
            printf("\tCore Word can't be deleted\n");
            return 0;
        }
        do
        {
            del_w = dict->head;
            dict->head = dict->head->link;
            dict_destroy_word(del_w);
            dict->size--;
        } while(del_w != w);
        
        return 1;
    }
    
    return 0;
}


void explain()
{
    Word  **IP_end = IP;
    IP=IP_head;
    
    while(IP != IP_end)
    {
        PRINT("[DEBUG]解释执行> %s\n", (*IP)->name)
        
        (*IP)->code_p();
        ++IP;
    }
    IP_head = IP;
}


int is_num(char *s)
{
    if(*s == '-')
        s++;

    while (*s != 0)
    {
        if (!isdigit((CELL)*s)) 
            return 0;
        s++;
    }
    return 1;
}


int find(Dict *dict, char *name)
{
    Word *word_p;
    word_p = dict_search_name(dict, name);
    
    if(!strcmp(":", name) || !strcmp("]", name)) state = COMPILE;
    
    if(state == COMPILE)
    {
        if(word_p==NULL)    //词典链表搜索不到名字后，去判断是不是数字
        {
            if (!is_num(name))    
            {
                return 0;    //如果不是数字，返回0
            }
            else 
            {               //如果是数字
                PRINT("[DEBUG]成功找到数字%s\n",name)
                ip_push(dict_search_name(dict, "(lit)"), IP_head);   //将push核心词指针存入IP数组        
                ip_push((Word*)(CELL)(atoi(name)), IP_head);    //将CELL型数强制转换为Word指针类型

                return 1;
            }            
        }
        else if(word_p->flag == IMMD_WORD)  //立即词
        {
            PRINT("[DEBUG]执行立即词 %s\n", name)
            if(word_p->wplist != NULL)
            {
                in_interpret();
                ip_push(word_p, IP_head);
                explain();
                out_interpret();
            }
            else
            {
                word_p->code_p();
            }
        }
        else
        {
            PRINT("[DEBUG]成功编译%s词\n",name)
            ip_push(word_p, IP_head);
        }
    }
    else if(state == EXPLAIN)
    {
        if(word_p==NULL)    //词典链表搜索不到名字后，去判断是不是数字
        {
            if (!is_num(name))    
            {
                return 0;    //如果不是数字，返回0
            }
            else 
            {               //如果是数字
                PRINT("[DEBUG]数据栈压入 %s\n",name)
                ds_push((CELL)(atoi(name)));

                return 1;
            }            
        }
        else
        {
            PRINT("[DEBUG]成功找到%s词\n",name)
            ip_push(word_p, IP_head);
            explain();
        }
    }

    return 1;
}


void empty_stack()
{
    DP=DS-1;
    RP=RS-1;
}


void stack_error(int n)
{
    switch(n)
    {
        case 1: printf("Stack underflow\n"); break;
        case 2: printf("Stack overflow\n"); break;
    }
    exit(0);
}


void ip_push(Word *w, Word** list)
{
    if(IP >= list+BUFF_LEN){stack_error(2);}
    *IP=w;
    IP++;
}


void ds_push(CELL n)
{
    if(DP >= DS+STACK_LEN-1){stack_error(2);}
    DP++;
    *DP=n;
}


void rs_push(CELL n)
{
    if(RP >= RS+STACK_LEN-1){stack_error(2);}
    RP++;
    *RP=n;
}


CELL ds_pop()
{
    if(DP <= DS-1){stack_error(1);}
    DP--;
    return *(DP+1); 
}


CELL rs_pop()
{
    if(RP <= RS-1){stack_error(1);}
    RP--;
    return *(RP+1); 
}


CELL ds_top()
{
    if(DP <= DS-1){stack_error(1);}
    return *DP;
}


CELL rs_top()
{
    if(RP <= RS-1){stack_error(1);}
    return *RP;
}


void lit()
{
    IP++;
    ds_push((CELL)*IP);
    PRINT("[DEBUG]数%ld压入数据栈\n", (CELL)*IP)
}


void popds()
{
    printf("%ld\n", ds_pop());
}


void bye()
{
    exit(1);
}


void ret()
{
    IP=(Word**)(rs_pop());
    PRINT("[DEBUG]从子例程返回\n")
}


void depth()
{
    ds_push((CELL)(DP-DS+1));
}


void add()
{
    ds_push(ds_pop() + ds_pop());
}


void sub()
{
    CELL d = ds_pop();
    ds_push(ds_pop() - d);
}


void mul()
{
    ds_push(ds_pop() * ds_pop());
}


void divv()
{
    CELL d = ds_pop();
    ds_push(ds_pop() / d);
}


void drop()
{
    ds_pop();
}


void showds()
{
    printf("<%ld> ", (CELL)(DP-DS+1));
    CELL *i=DS;
    for (;i<=DP ;i++ )
    {
        printf("%ld ",*i);
    }
    printf("\n");
}


void pick()
{
    CELL k = ds_pop();
    if(DP-k+1 <= DS-1){stack_error(1);}
    ds_push(*(DP-k+1));
}


void roll()
{
    CELL k = ds_pop();
    if(DP-k+1 <= DS-1){stack_error(1);}
    CELL dk = *(DP-k+1);
    for(; k>1; k--) {
        *(DP-k+1) = *(DP-k+2);
    }
    ds_pop();
    ds_push(dk);
}


void invar()
{
    Word *p = (Word *)(ds_pop());
    p->wplist = (Word **)ds_pop();
}


void outvar() 
{
    Word *p = (Word *)(ds_pop());
    ds_push((CELL)(p->wplist));
}


void equal()
{
    if(ds_pop() == ds_pop())
    {
        ds_push(-1);
    }
    else
    {
        ds_push(0);
    }
}


void noequal()
{
    if(ds_pop() != ds_pop())
    {
        ds_push(-1);
    }
    else
    {
        ds_push(0);
    }
}


void morethan()
{
    CELL d = ds_pop();
    if(ds_pop() > d)
    {
        ds_push(-1);
    }
    else
    {
        ds_push(0);
    }
}


void lessthan()
{
    CELL d = ds_pop();
    if(ds_pop() < d)
    {
        ds_push(-1);
    }
    else
    {
        ds_push(0);
    }
}


void if_branch()
{
    if(ds_pop() == 0)
    {
        IP = IP + (CELL)(*(IP+1));
    }
    else
    {
        IP++;
    }
}


void branch()
{
    IP = IP + (CELL)(*(IP+1));
}


void doo()
{
    CELL index = ds_pop();
    CELL limit = ds_pop();
    if(limit <= index)
    {
        IP = IP + (CELL)(*(IP+1)); 
    }
    else
    {
        IP++;
        index++;
        rs_push(index);
        rs_push(limit);
    }
}


void loopp() 
{
    IP = IP - (CELL)(*(IP+1)); 
    ds_push(rs_pop());
    ds_push(rs_pop());
}


void tor()
{
    rs_push(ds_pop());
}


void rto()
{
    ds_push(rs_pop());
}


void rat()
{
    ds_push(rs_top());
}


void emit()
{
    putchar((char)(ds_pop()));
}


void words()
{
    Word *w = forth_dict->head;
    while (w != NULL)
    {  
        printf("%s ", w->name);
        w=w->link;
    }
    printf("\n");
}


void immediate()
{
    forth_dict->head->flag = IMMD_WORD;
}


void pushds_cfa()
{
    current_text = parse_word();
    ds_push((CELL)dict_search_name(forth_dict, current_text));
}


void compile_wplist()
{
    IP++;
    Word **tmp = IP;
    Word *word_p = *IP;
    Word **IP_over = (Word **)rs_pop();
    IP = (Word **)rs_pop();
    if(word_p->wplist != NULL)
    {
        Word **p = word_p->wplist;
        Word *end = dict_search_name(forth_dict, "ret");
        for (; *p != end; p++)
        {
            ip_push(*p, forth_dict->wplist_tmp);
        }
    }
    else
    {
        PRINT("[DEBUG]编译核心词 %s\n", word_p->name)
        ip_push((Word *)word_p, forth_dict->wplist_tmp);
    }
    rs_push((CELL)IP);
    rs_push((CELL)IP_over);
    IP = tmp;
}


void compile_s()
{
    Word **tmp = IP;
    Word **IP_over = (Word **)rs_pop();
    IP = (Word **)rs_pop();
    CELL num = ds_pop();
    PRINT("[DEBUG]编译栈顶数 %ld\n", num)
    ip_push((Word *)num, forth_dict->wplist_tmp);
    rs_push((CELL)IP);
    rs_push((CELL)IP_over);
    IP = tmp;
}


void in_interpret()
{
    state = EXPLAIN;
    IP_head = IP_list;
    rs_push((CELL)IP);
    IP=IP_head;
}


void out_interpret()
{
    IP_head = forth_dict->wplist_tmp;
    IP = (Word **)rs_pop();
    state = COMPILE;
}


void myself()
{
    ip_push(forth_dict->head, IP_head);
}


void defcolon()
{
    IP_head = forth_dict->wplist_tmp;
    IP=IP_head;
    current_text = parse_word();
    dict_ins_next(forth_dict, create(current_text, colon_code));
}


void endcolon()
{
    ip_push(dict_search_name(forth_dict, "ret"), IP_head);
    int n = (CELL)IP - (CELL)IP_head;
    does(forth_dict->head, IP_head, n);
    
    //DEBUG模式下打印出IP指针列表
    if(DEBUG_MODE) {
        printf("[DEBUG]IP指针列表> ");
        Word **p=IP_head;
        for (;p<IP ;p++ )
        {
            printf("%ld ",(CELL)(*p));
        }
        printf("\n");
        showds();
    }
    
    IP_head = IP_list;
    IP=IP_head;
    state = EXPLAIN;
}


void _if()
{
    ip_push(dict_search_name(forth_dict, "?branch"), IP_head);
    rs_push((CELL)IP);
    ip_push((Word *)0, IP_head);
}


void _else()
{
    ip_push(dict_search_name(forth_dict, "branch"), IP_head);
    Word** else_p = IP;
    Word** if_p = (Word**)(rs_pop());
    rs_push((CELL)else_p);
    *if_p = (Word*)(IP - if_p + 1);
    ip_push((Word *)0, IP_head);
}


void _then()
{
    Word** branch_p = (Word**)(rs_pop());
    *branch_p = (Word*)(IP - branch_p); 
}


void _do()
{
    ip_push(dict_search_name(forth_dict, "(do)"), IP_head);
    rs_push((CELL)IP);
    ip_push((Word *)0, IP_head);
    
}


void _loop()
{
    ip_push(dict_search_name(forth_dict, "(loop)"), IP_head); 
    Word** do_p = (Word**)(rs_pop());
    *do_p = (Word*)(IP - do_p + 1); 
    ip_push((Word*)(IP - do_p + 1), IP_head); 
}


void see()
{
    current_text = parse_word();
    Word *word_p = dict_search_name(forth_dict, current_text);
    
    if(word_p == NULL)
    {
        printf("%s :\n\tCan't find!\n", current_text);
    }
    else
    {   //反编译wplist，得出扩展词的字符串定义
        printf("%s :\n\t", current_text);
        if(word_p->code_p == colon_code)
        {
            Word **p = word_p->wplist;
            Word *end = dict_search_name(forth_dict, "ret");
            Word *dict_p = forth_dict->head;
            for(; *p != end; p++)
            {
                while (dict_p != NULL && dict_p != *p)
                {  
                    dict_p=dict_p->link;
                }

                if(dict_p != NULL)
                    printf("%s ", (*p)->name);
                else
                    printf("%ld ", (CELL)(*p));
                dict_p = forth_dict->head;
            }
            printf(";");
            if(word_p->flag == IMMD_WORD)
                printf(" immediate\n");
            else
                printf("\n");
        }
        else
        {
            printf("%s\n", word_p->name);
        }
    }
}


void forget()
{
    current_text = parse_word();
    dict_rem_after(forth_dict, current_text); //删除当前扩展词以及词典中该词之后定义的所有扩展词
}


void var()
{
    current_text = parse_word();
    dict_ins_next(forth_dict, create(current_text, var_code));
    does(forth_dict->head, (Word **)0, 0);
}


void cons()
{
    current_text = parse_word();
    dict_ins_next(forth_dict, create(current_text, cons_code));
    does(forth_dict->head, (Word **)ds_pop(), 0);
    
}


void load()
{
    current_text = parse_word();
    load_file(current_text);
}


void interpret()
{
    state = EXPLAIN;
    text_p = forth_text;
    IP_head = IP_list;
    IP=IP_head;
       
    while (*(current_text = parse_word()) != '\0')
    {
        if(!strcmp(".\"",current_text))  //如果是." str " 则立即编译其中的字符串str
        {
            PRINT("[DEBUG]编译字符串\n")

            char tempstr[BUFF_LEN]; 
            while(*text_p != '\"')
            {
                sprintf(tempstr, "%ld", (CELL)(*text_p));
                find(forth_dict, tempstr);
                find(forth_dict, "emit");
                text_p++;
            }
            text_p++;
        }
        else if(!strcmp("(",current_text))  //注释模式
        {

            while(*text_p != ')')
            {
                text_p++;
            }
            text_p++;
        }
        else if(!find(forth_dict, current_text))
        {
            printf("[%s]?\n",current_text);
            empty_stack();
            IP=IP_head;
            return;
        }
    }
}


//从外部文件读取Forth代码
int load_file(char *file_path)
{
    FILE *fp; //文件指针
    char c;
    int i = 0;
    int colon_flag = FALSE;

    if((fp = fopen(file_path, "r")) == NULL)
    {
        printf("Can't open %s\n", file_path);
        return 0;
    }
    
    do
    {
        c = getc(fp);
        if((c != '\n' && c != (char)EOF) 
            || (c == '\n' && colon_flag == TRUE))
        {
            if(c == ':') colon_flag = TRUE;
            else if(c == ';') colon_flag = FALSE;
            forth_text[i] = c;
            i++;
        }
        else if((c == '\n' && colon_flag == FALSE)
            || c == (char)EOF)
        {
            forth_text[i] = '\0';
            interpret();
            i = 0;
        }           
    } while(c != (char)EOF);
    fclose(fp);

    return 1;
}

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  UINTN argc;
  CHAR16 **Argv;
  
  InitializeLib(ImageHandle, SystemTable);
  efilibc_init(ImageHandle);
  argc = GetShellArgcArgv(ImageHandle, &Argv);
  
  
    empty_stack();
    IP_head = IP_list;
    IP = IP_head;
    forth_dict= dict_init();
    
    //初始化词典
    dict_ins_next(forth_dict, def_core("(lit)",lit));
    dict_ins_next(forth_dict, def_core(".",popds));
    dict_ins_next(forth_dict, def_core("bye",bye));
    dict_ins_next(forth_dict, def_core("ret",ret));
    dict_ins_next(forth_dict, def_core("depth",depth));
    dict_ins_next(forth_dict, def_core("+",add));
    dict_ins_next(forth_dict, def_core("-",sub));
    dict_ins_next(forth_dict, def_core("*",mul));
    dict_ins_next(forth_dict, def_core("/",divv));
    dict_ins_next(forth_dict, def_core("drop",drop));
    dict_ins_next(forth_dict, def_core(".s",showds));
    dict_ins_next(forth_dict, def_core("pick",pick));
    dict_ins_next(forth_dict, def_core("roll",roll));
    dict_ins_next(forth_dict, def_core("!", invar));
    dict_ins_next(forth_dict, def_core("@", outvar));
    dict_ins_next(forth_dict, def_core("=",equal));
    dict_ins_next(forth_dict, def_core("<>",noequal));
    dict_ins_next(forth_dict, def_core(">",morethan));
    dict_ins_next(forth_dict, def_core("<",lessthan));
    dict_ins_next(forth_dict, def_core("?branch",if_branch));
    dict_ins_next(forth_dict, def_core("branch",branch));
    dict_ins_next(forth_dict, def_core("(do)",doo));
    dict_ins_next(forth_dict, def_core("(loop)",loopp));
    dict_ins_next(forth_dict, def_core(">r",tor));
    dict_ins_next(forth_dict, def_core("r>",rto));
    dict_ins_next(forth_dict, def_core("r@",rat));
    dict_ins_next(forth_dict, def_core("emit", emit));
    dict_ins_next(forth_dict, def_core("words",words));
    dict_ins_next(forth_dict, def_core("immediate",immediate));
    dict_ins_next(forth_dict, def_core("compile", compile_wplist)); 
    dict_ins_next(forth_dict, def_core(",", compile_s));
    dict_ins_next(forth_dict, def_core("'", pushds_cfa));
    
    dict_ins_next(forth_dict, def_core("[",in_interpret)); immediate();
    dict_ins_next(forth_dict, def_core("]",out_interpret)); immediate();
    dict_ins_next(forth_dict, def_core("myself", myself)); immediate();
    dict_ins_next(forth_dict, def_core(":",defcolon)); immediate();
    dict_ins_next(forth_dict, def_core(";",endcolon)); immediate();
    dict_ins_next(forth_dict, def_core("if",_if)); immediate();
    dict_ins_next(forth_dict, def_core("else",_else)); immediate();
    dict_ins_next(forth_dict, def_core("then",_then)); immediate();
    dict_ins_next(forth_dict, def_core("do",_do)); immediate();
    dict_ins_next(forth_dict, def_core("loop",_loop)); immediate();
    dict_ins_next(forth_dict, def_core("see",see)); immediate();
    dict_ins_next(forth_dict, def_core("forget",forget)); immediate();
    dict_ins_next(forth_dict, def_core("variable",var)); immediate();
    dict_ins_next(forth_dict, def_core("constant",cons)); immediate();
    dict_ins_next(forth_dict, def_core("load",load)); immediate();
    
    char argv[3][80];
    unsigned int i, j;
    for (i=0;i<argc;i++) {
      for (j=0;j<StrLen(Argv[i]);j++) {
        argv[i][j] = (char)Argv[i][j];
      }
      argv[i][j] = '\0';
    }

    for(; argc > 1; argc--)
    {
        load_file(argv[argc-1]);
    }

    while (strcmp(forth_text, "bye") != 0)
    {
        printf(">>> ");
        gets(forth_text);
        interpret();
    }

    
    return 0;
}

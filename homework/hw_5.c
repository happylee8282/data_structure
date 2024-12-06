#include <stdio.h>
#include <stdlib.h>

typedef int element;

//구조체
typedef struct DListNode 
{
    element data;
    struct DListNode* llink;
    struct DListNode* rlink;
} DListNode;

//초기화
void init(DListNode* phead) 
{
    phead->llink = phead;
    phead->rlink = phead;
}

// 이중 연결 리스트의 노드를 출력
void print_dlist(DListNode* phead) {
    DListNode* p;
    for (p = phead->rlink; p != phead; p = p->rlink) 
    {
        printf("<%d> ", p->data);
    }
    printf("\n");
}

// 오른쪽에 삽입한다.
void dinsert(DListNode* before, element data) {
    DListNode* newnode = (DListNode*)malloc(sizeof(DListNode));
    newnode->data = data;
    newnode->llink = before;
    newnode->rlink = before->rlink;
    before->rlink->llink = newnode;
    before->rlink = newnode;
}

// 노드 removed를 삭제한다.
void ddelete(DListNode* head, DListNode* removed) 
{
    if (removed == head) return;
    removed->llink->rlink = removed->rlink;
    removed->rlink->llink = removed->llink;
    free(removed);
}

// 특정 위치에 데이터를 삽입하는 함수
void dinsert_at_position(DListNode* head, int position, element data) 
{
    DListNode* p = head;
    for (int i = 0; i < position && p->rlink != head; i++) 
    {
        p = p->rlink;
    }
    dinsert(p, data);
}


// 이중 연결 리스트를 사용한 삽입 정렬 함수
void insertion_sort(DListNode* head) 
{
    if (head->rlink == head || head->rlink->rlink == head) return;

    DListNode* sorted = head->rlink->rlink;
    while (sorted != head) 
    {
        DListNode* get = sorted;
        DListNode* current = sorted->llink;

        sorted = sorted->rlink;

        while (current != head && current->data > get->data) 
        {
            current = current->llink;
        }

        get->llink->rlink = get->rlink;
        get->rlink->llink = get->llink;

        get->rlink = current->rlink;
        get->llink = current;
        current->rlink->llink = get;
        current->rlink = get;

        print_dlist(head); 
    }
}

int main(void) 
{
    DListNode* head = (DListNode*)malloc(sizeof(DListNode));

    init(head);

    dinsert(head, 1);
    dinsert(head, 3);
    dinsert(head, 9);
    dinsert(head, 7);
    dinsert(head, 2);
    dinsert(head, 3);
    print_dlist(head);

    // 삽입 정렬 수행
    insertion_sort(head);

    // 정렬된 리스트 출력
    //print_dlist(head);

    free(head);
    return 0;
}

/*
단순 연결 리스트에서 2개의 위치를 입력받아 위치를 변경하는 swap 기능을 구현
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int element;

typedef int element;
typedef struct ListNode { // 노드 타입을 구조체로 정의한다.
	element data;
	struct ListNode* link;
} ListNode;

ListNode* insert_first(ListNode* head, int value)
{
	ListNode *p = (ListNode*)malloc(sizeof(ListNode));
	p->data = value;
	p->link = head;
	head = p;
	return head;
}

ListNode* insert(ListNode* head, ListNode* pre, element value)
{
	ListNode* p = (ListNode*)malloc(sizeof(ListNode));
	p->data = value;
	p->link = pre->link;
	pre->link = p;
	return head;
}


ListNode* delete_first(ListNode* head)
{
	ListNode* remove;
	if (head == NULL) return NULL;
	remove = head;
	head = remove->link;
	free(remove);
	return head;
}

ListNode* delete(ListNode* head,ListNode* pre)
{
	ListNode* remove;
	remove = pre->link;
	pre->link = remove->link;
	free(remove);
	return head;
}

ListNode* extract(ListNode* head, ListNode* pre)
{
	ListNode* extracted;
	extracted = pre->link;
	pre->link = extracted->link;
	return extracted;
}

void print_list(ListNode*head)
{
	for (ListNode* p = head; p != NULL; p = p->link)
		printf("%d->", p->data);
	printf("NULL\n");
	
}

ListNode* Swap(ListNode* head, ListNode* pre1, ListNode* pre2)
{
	if (pre1->link != pre2)
	{
		ListNode* node2 = extract(head, pre2);
		ListNode* node1 = extract(head, pre1);



		insert(head, pre1, node2->data);
		insert(head, pre2, node1->data);
	}
	else
	{
		ListNode* node2 = extract(head, pre2);

		insert(head, pre1, node2->data);
	}
		
}

int main(void)
{
	ListNode* head = NULL;
	ListNode* pre1 = NULL;
	ListNode* pre2 = NULL;
	ListNode* node;
	int count = 1;

	int input1, input2;

	for (int i = 0; i < 5; i++) {
		head = insert_first(head, i);
	}
	print_list(head);

	printf("swap1 : ");
	scanf_s("%d", &input1);
	printf("swap2 : ");
	scanf_s("%d", &input2);

	node = head;
	while (node) 
	{
		if (count + 1 == input1)
		{
			pre1 = node;
		}
		else if (count + 1 == input2)
		{
			pre2 = node;
		}

		node = node->link;
		count++;
	}


	Swap(head, pre1, pre2);

	print_list(head);

	return 0;
}


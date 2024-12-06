/*이진탐색트리의 데이터를 heap을 이용한 우선순위큐로 변환하시오.*/
#include <stdio.h>
#include <stdlib.h>

#define MAX_ELEMENT 200

// 트리 노드 구조체 정의
typedef struct TreeNode {
    int key;                // 노드의 값
    struct TreeNode* left;  // 왼쪽 자식 노드를 가리키는 포인터
    struct TreeNode* right; // 오른쪽 자식 노드를 가리키는 포인터
} TreeNode;

// 새로운 노드를 생성하는 함수
TreeNode* new_node(int item) {
    TreeNode* temp = (TreeNode*)malloc(sizeof(TreeNode));
    temp->key = item;
    temp->left = temp->right = NULL;
    return temp;
}

typedef struct {
    int key;
} element;


typedef struct {
    element heap[MAX_ELEMENT];
    int heap_size;
} HeapType;




// 트리에 노드를 삽입하는 함수
TreeNode* insert_node(TreeNode* node, int key) {
    // 트리가 공백이면 새로운 노드를 반환한다. 
    if (node == NULL) return new_node(key);
    // 그렇지 않으면 순환적으로 트리를 내려간다. 
    if (key < node->key)
        node->left = insert_node(node->left, key);
    else if (key > node->key)
        node->right = insert_node(node->right, key);
    // 변경된 루트 포인터를 반환한다. 
    return node;
}

// RVL사용
void inorder(TreeNode* root, element* array, int* count) {
    if (root != NULL) {
        inorder(root->right, array, count);
        array[(*count)].key = root->key;
        ++(*count);
        inorder(root->left, array, count);
    }
}

// 생성 함수
HeapType* create()
{
    return (HeapType*)malloc(sizeof(HeapType));
}
// 초기화 함수
void init(HeapType* h)
{
    h->heap_size = 0;
}

// 현재 요소의 개수가 heap_size인 히프 h에 item을 삽입한다.
// 삽입 함수
void insert_max_heap(HeapType* h, element item)
{
    int i;
    i = ++(h->heap_size);
    // 트리를 거슬러 올라가면서 부모 노드와 비교하는 과정
    while ((i != 1) && (item.key > h->heap[i / 2].key)) {
        h->heap[i] = h->heap[i / 2];
        i /= 2;
    }
    h->heap[i] = item; // 새로운 노드를 삽입
}

// 삭제 함수
element delete_max_heap(HeapType* h)
{
	int parent, child;
	element item, temp;
	item = h->heap[1];
	temp = h->heap[(h->heap_size)--];
	parent = 1;
	child = 2;
	while (child <= h->heap_size) {
		// 현재 노드의 자식노드 중 더 작은 자식노드를 찾는다.
		if ((child < h->heap_size) &&
			(h->heap[child].key) < h->heap[child + 1].key)
			child++;
		if (temp.key >= h->heap[child].key) break;
		// 한 단계 아래로 이동
		h->heap[parent] = h->heap[child];
		parent = child;
		child *= 2;
	}
	h->heap[parent] = temp;
	return item;
}


int main(void) {
    TreeNode* root = NULL;
    HeapType* main_heap;
    element array[MAX_ELEMENT];
    int count = 0;

    root = insert_node(root, 30);
    root = insert_node(root, 20);
    root = insert_node(root, 10);
    root = insert_node(root, 40);
    root = insert_node(root, 50);
    root = insert_node(root, 60);

    inorder(root, array, &count); //count와 index값 받음

    main_heap = create(); // 히프 생성
    init(main_heap); // 초기화

    for (int i = 0; i < count; i++)
    {
        insert_max_heap(main_heap, array[i]);
    }

    for (int i = 0; i < count; i++)
    {
        element val = delete_max_heap(main_heap);
        printf("< %d > ", val);
    }

    return 0;
}

/*()와 {}가 사용된 중위표기법 수식을 후위표기법으로 변환하는 프로그램을 작성하시오.*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_SIZE 100

typedef int element;
typedef struct {
	element data[MAX_STACK_SIZE];
	int top;
} StackType;

// 스택 초기화 함수
void init_stack(StackType* s)
{
	s->top = -1;
}

// 공백 상태 검출 함수
int is_empty(StackType* s)
{
	return (s->top == -1);
}
// 포화 상태 검출 함수
int is_full(StackType* s)
{
	return (s->top == (MAX_STACK_SIZE - 1));
}

// 삽입함수
void push(StackType* s, element item)
{
	if (is_full(s)) {
		fprintf(stderr, "스택 포화 에러\n");
		return;
	}
	else s->data[++(s->top)] = item;
}
// 삭제함수
element pop(StackType* s)
{
	if (is_empty(s)) {
		fprintf(stderr, "스택 공백 에러\n");
		exit(1);
	}
	else return s->data[(s->top)--];
}
// 피크함수
element peek(StackType* s)
{
	if (is_empty(s)) {
		fprintf(stderr, "스택 공백 에러\n");
		exit(1);
	}
	else return s->data[s->top];
}
// ===== 스택 코드의 끝 =====

// 가중치 값 넣기
int prec(char op)
{
	switch (op) {
	case '{': case '}': return 0;
	case '(': case ')': return 1;
	case '+': case '-': return 2;
	case '*': case '/': return 3;
	}
	return -1;
}

//중위 표기수식 -> 후위 표기 수식
void infix_to_postfix(char exp[])
{
	int i = 0;
	char ch, top_op;
	int len = strlen(exp);
	StackType s;

	init_stack(&s); // 스택 초기화
	for (i = 0; i < len; i++)
	{
		ch = exp[i];

		switch (ch)
		{
		case '+': case '-': case '*': case '/': // 연산자
			// 스택에 있는 연산자의 우선순위가 더 크거나 같으면 출력
			while (!is_empty(&s) && (prec(ch) <= prec(peek(&s))))
				printf("%c", pop(&s));
			push(&s, ch);
			break;

		case '(': // 왼쪽 괄호
			push(&s, ch);
			break;

		case ')': // 오른쪽 괄호
			top_op = pop(&s);
			while (top_op != '(')
			{
				printf("%c", top_op);
				top_op = pop(&s);
			}
			break;
			// 왼쪽 괄호를 만날때까지 출력

		case '{': // 왼쪽 괄호

			push(&s, ch);
			break;

		case '}': // 왼쪽 괄호
			top_op = pop(&s);
			while (top_op != '{')
			{
				printf("%c", top_op);
				top_op = pop(&s);
			}
			break;


		default: // 피연산자
			printf("%c", ch);
			break;
		}
	}
	while (!is_empty(&s)) // 스택에 저장된 연산자들 출력
		printf("%c", pop(&s));
}

int main(void)
{
	char* s = "{(2+3)*4}+9";
	printf("중위표시수식 %s \n", s);
	printf("후위표시수식 ");
	infix_to_postfix(s);
	printf("\n");
	return 0;
}

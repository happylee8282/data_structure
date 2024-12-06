/*
노래방 예약 시스템을 큐를 이용하여 코드로 작성하는 과제입니다.
1. 현재 10분 남아있는 상황

2. 예약(1), 노래부르기(2),우선예약(3)

3. 예약 및 우선 예약은 번호와 시간을 입력 받도록 구현

4. 우선 예약은 앞쪽에 예약

5. 노래부르기를 선택하면 제일 앞에 있는 노래의 시간이 차감

6. 남은 시간이 0이하가 되면 프로그램 종료
*/

#include <stdio.h>
#include <stdlib.h>
#define MAX_QUEUE_SIZE 50
typedef int element1;

typedef struct songtype { // 큐 타입
	int num;
	int time;
} element;

typedef struct { // 큐 타입
	element data[MAX_QUEUE_SIZE];
	int front, rear;
} DequeType;

// 오류 함수
void error(char* message)
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}

// 초기화
void init_deque(DequeType* q)
{
	q->front = q->rear = 0;
}

int is_empty(DequeType* q)
{
	return (q->front == q->rear);
}

// 포화 상태 검출 함수
int is_full(DequeType* q)
{
	return ((q->rear + 1) % MAX_QUEUE_SIZE == q->front);
}


void add_rear(DequeType* q, element item)
{
	if (is_full(q))
		error("큐가 포화상태입니다");
	q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
	q->data[q->rear] = item;
}

void add_front(DequeType* q, element val)
{
	if (is_full(q))
		error("큐가 포화상태입니다");
	q->data[q->front] = val;
	q->front = (q->front - 1 + MAX_QUEUE_SIZE) % MAX_QUEUE_SIZE;
}

element delete_front(DequeType* q)
{
	if (is_empty(q))
		error("큐가 공백상태입니다");
	q->front = (q->front + 1) % MAX_QUEUE_SIZE;
	return q->data[q->front];
}

element delete_rear(DequeType* q)
{
	int prev = q->rear;
	if (is_empty(q))
		error("큐가 공백상태입니다");
	q->rear = (q->rear - 1 + MAX_QUEUE_SIZE) % MAX_QUEUE_SIZE;
	return q->data[prev];
}

element get_front(DequeType* q)
{
	if (is_empty(q))
		error("큐가 공백상태입니다");
	return q->data[(q->front + 1) % MAX_QUEUE_SIZE];
}

element get_rear(DequeType* q)
{
	if (is_empty(q))
		error("큐가 공백상태입니다");
	return q->data[q->rear];
}


void queue_print(DequeType* q)
{
	printf("DEQUE(front=%d rear=%d) = ", q->front, q->rear);
	if (!is_empty(q)) {
		int i = q->front;
		do {
			i = (i + 1) % (MAX_QUEUE_SIZE);
			printf("%d(%d) | ", q->data[i].num, q->data[i].time);
			if (i == q->rear)
				break;
		} while (i != q->front);
	}
	printf("\n");
}

int main(void)
{
	DequeType queue;
	element song;
	int num;
	int time;
	int stare_time = 10;
	int choose;
	init_deque(&queue);
	printf("--데이터 추가 단계--\n");
	while (!is_full(&queue))
	{
		if (stare_time <= 0)
		{
			printf("노래방 시간이 만료되었습니다.");
			return;
		}
		else
		{
			printf("<남은시간 : %d분> 예약(1) 노래부르기(2) 우선예약(3) : ", stare_time);
			scanf_s("%d", &choose);
			if (choose == 1)
			{
				printf("노래번호 : ");
				scanf_s("%d", &num);
				printf("노래시간 : ");
				scanf_s("%d", &time);

				song.num = num;
				song.time = time;

				add_rear(&queue, song);
				queue_print(&queue);
			}
			else if (choose == 2)
			{
				while (!is_empty(&queue))
				{
					element s = delete_front(&queue);
					stare_time -= s.time;
					printf("=== %d번 노래 부름\n", s.num);
					queue_print(&queue);
					break;
				}
			}
			else if (choose == 3)
			{
				printf("노래번호 : ");
				scanf_s("%d", &num);
				printf("노래시간 : ");
				scanf_s("%d", &time);

				song.num = num;
				song.time = time;

				add_front(&queue, song);
				queue_print(&queue);
			}

		}
	}
	return 0;
}

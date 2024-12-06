#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include "patient.h"

#define NUM_SURGERY_ROOMS 3 // 수술실 개수를 정의 체크
#define MAX_NAME_LENGTH 100 // 환자 이름 최대 길이 정의
#define MAX_PATIENTS 100
int num_patients = 0;       //입원한 환자수
int current_day = 1; //현재 날짜



typedef struct { //체크
    char name[MAX_NAME_LENGTH];          // 환자의 이름
    int age;                // 나이
    int riskLevel;          // 응급도 (1부터 10 사이의 정수)
    int days_admitted;  // 입원일
    int days_discharge; // 퇴원 예정일
} Patient;


//연결리스트의 노드를 나타내는 구조체
typedef struct ListNode {
    Patient patient;
    struct ListNode* next;
} ListNode;

typedef struct {
    Patient patients[MAX_PATIENTS];
    int size;               // 대기 중인 환자 수
    ListNode* surgeryQueue; // 수술 대기열의 시작 노드
    ListNode* treatmentQueue; // 진료 대기열의 시작 노드
} PatientQueue;

//-------------------------------------------------------------------수술 구조체
typedef struct SurgeryRoom {
    int roomNumber;
    struct SurgeryListNode* schedule;
} SurgeryRoom;

typedef struct SurgeryListNode { //체크
    int startTime; // 수술 시작 시간 (HHMM 형식)
    int endTime;   // 수술 종료 시간 (HHMM 형식)
    Patient pa; // 환자 이름
    struct SurgeryListNode* llink;
    struct SurgeryListNode* rlink;
} SurgeryListNode;
//-----------------------------------------------------------------------
typedef struct Node {
    char question[256];
    struct Node* yes;
    struct Node* no;
} Node;

SurgeryRoom surgeryRooms[NUM_SURGERY_ROOMS]; // 수술실 배열 선언


void change_surgery(int* currentTime, SurgeryRoom surgeryRooms[]);
void updateSchedules(SurgeryRoom surgeryRooms[], int currentTime, int next);
void reserveSurgery(SurgeryRoom surgeryRooms[], int* nextRoom, int currentTime, Patient pa);
int findEarliestEndTimeRoom(SurgeryRoom surgeryRooms[], int* earliestEndTime);
void display(SurgeryListNode* head);
void ddelete(SurgeryListNode** head, int startTime, int endTime, const char* name);
void insert(SurgeryListNode** head, int startTime, int endTime, const char* name);
SurgeryListNode* surgerycreateNode(int startTime, int endTime, const char* name);

int currentTime = 1000; // HHMM 형식으로 초기 시간 설정
int s = 0;                      //5명씩 진료 받기 위한 변수
int date = 0101;                       //날짜를 나타내는 변수

Patient patients[MAX_PATIENTS];     //입원 환자 배열 //중요 퀵정렬

int patientCount = 0;                         //입원 환자 수
/*********************************************************************************************/


Node* createNode(char* question) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->question, question);
    newNode->yes = NULL;
    newNode->no = NULL;
    return newNode;
}

char* askQuestion(Node* node) {
    char answer;
    while (node->yes != NULL || node->no != NULL) {
        printf("%s", node->question);
        scanf(" %c", &answer);

        if (answer == 'y' || answer == '1') {
            node = node->yes;
        }
        else if (answer == 'n' || answer == '2') {
            node = node->no;
        }
        else {
            printf("y 또는 n 으로 응답해주세요 .\n");
        }
    }
    return(node->question);
}

Node* createAkinator() {
    Node* root = createNode("1.외과 2.내과 : ");

    //외과
    root->yes = createNode("허리가 아프십니까? (y/n) : ");
    root->yes->yes = createNode("신경외과");

    root->yes->no = createNode("가슴이 아프십니까? (y/n) : ");
    root->yes->no->yes = createNode("흉부외과");
    root->yes->no->no = createNode("정형외과");

    //내과
    root->no = createNode("배가 아프십니까? (y/n) : ");
    root->no->yes = createNode("소화기내과");

    root->no->no = createNode("호흡기 관련 증상이 있습니까? (기침, 호흡곤란 등) (y/n) : ");
    root->no->no->yes = createNode("호흡기내과");

    root->no->no->no = createNode("일반내과");

    return root;
}

/*********************************************************************************************/

// 초기화 함수: 환자 큐를 초기화하고 시간을 현재 시간으로 설정합니다.
void initQueue(PatientQueue* pq) {
    pq->size = 0;
    pq->surgeryQueue = NULL;      // 수술 대기열 초기화
    pq->treatmentQueue = NULL;    // 진료 대기열 초기화
}

// 두 환자 정보를 교환하는 함수
void swap(Patient* a, Patient* b) {
    Patient temp = *a;
    *a = *b;
    *b = temp;
}

// 힙 정렬의 업 힙 함수: 새로 추가된 환자를 힙의 올바른 위치로 올려 보냅니다.
void heapifyUp(PatientQueue* pq, int index) {
    while (index > 0) {
        int parentIndex = (index - 1) / 2;
        if (pq->patients[index].riskLevel <= pq->patients[parentIndex].riskLevel) {
            break;  // 힙 속성을 만족할 때까지 반복
        }
        swap(&pq->patients[index], &pq->patients[parentIndex]);
        index = parentIndex;
    }
}

// 힙 정렬의 다운 힙 함수: 힙의 루트에서 가장 큰 값을 찾아 아래로 내려 보냅니다.
void heapifyDown(PatientQueue* pq, int index) {
    int leftChild, rightChild, largest, size = pq->size;

    while (index < size) {
        leftChild = 2 * index + 1;
        rightChild = 2 * index + 2;
        largest = index;

        if (leftChild < size && pq->patients[leftChild].riskLevel > pq->patients[largest].riskLevel) {
            largest = leftChild;
        }

        if (rightChild < size && pq->patients[rightChild].riskLevel > pq->patients[largest].riskLevel) {
            largest = rightChild;
        }

        if (largest == index) {
            break;  // 힙 속성을 만족할 때까지 반복
        }

        swap(&pq->patients[index], &pq->patients[largest]);
        index = largest;
    }
}

// 환자를 대기열에 추가하는 함수: 힙의 특성을 유지하며 환자를 삽입합니다.
//heapifyUp()함수를 통해 우선순위 큐에서 최대힙 특성을 유지하여 환자를 삽입함.
void insertPatient(PatientQueue* pq, Patient p) {
    if (pq->size == MAX_PATIENTS) {
        printf("환자가 꽉 찼습니다!\n");
        return;
    }

    pq->patients[pq->size] = p;
    heapifyUp(pq, pq->size);  // 힙을 유지하기 위해 올바른 위치로 조정
    pq->size++;
}

// 우선 순위가 가장 높은 환자를 제거하고 반환하는 함수
//heapifyDown()함수를 통해 우선순위 큐에서 루트에 있는 환자 제거 후 반환
//연결리스트를 통해 반환된 환자를 수술 리스트, 진료 리스트에 넣음
Patient removeHighestPriority(PatientQueue* pq)
{
    if (pq->size == 0) {
        printf("환자가 없습니다!\n");
        Patient empty = { "", 0, 0, 0 };
        return empty;
    }
    Patient highestPriorityPatient = pq->patients[0];
    pq->patients[0] = pq->patients[pq->size - 1];
    pq->size--;
    heapifyDown(pq, 0);  // 힙을 유지하기 위해 올바른 위치로 조정

    int surgery;
    while (1) {
        printf("다음 환자 : %s, 위험도: %d\n", highestPriorityPatient.name, highestPriorityPatient.riskLevel);
        printf("수술을 받아야 하나요? (1: 예 / 0: 아니오): ");

        scanf("%d", &surgery);

        if (surgery == 1) {
            surgery_patient(highestPriorityPatient);
            break;
        }
        else if (surgery == 0) {
            Node* akinator_root = createAkinator();

            char* department;
            department = askQuestion(akinator_root);

            if (highestPriorityPatient.riskLevel > 5)
            {
                int stayDays = rand() % 3 + 1; // 1에서 3일 사이의 랜덤 입원 기간
                highestPriorityPatient.days_admitted = current_day;                          //환자의 입원일
                highestPriorityPatient.days_discharge = highestPriorityPatient.days_admitted + stayDays;
                printf("%s환자분 %s로 진료 후  %d일 동안 입원하였습니다.\n", highestPriorityPatient.name, department, stayDays);
                strcpy(patients[num_patients].name, highestPriorityPatient.name);
                patients[num_patients].days_admitted = highestPriorityPatient.days_admitted;
                patients[num_patients].days_discharge = highestPriorityPatient.days_discharge;
                num_patients += 1;
                if (s != 4) {
                    printf("다음 환자 받겠습니다.\n");
                    printf("--------------------다음환자--------------------------\n");
                    s += 1;
                }
                else {
                    printf("5명 진료가 끝났습니다.\n");
                }

                break;
            }
            else {
                printf("%s환자분 %s로 진료 후 퇴원하였습니다.\n", highestPriorityPatient.name, department);
                break;
            }

        }
        else {
            printf("잘못 입력했습니다. 숫자 1 또는 0을 입력해주세요.\n");
            while (getchar() != '\n'); // 입력 버퍼 비우기
        }
    }
    return highestPriorityPatient;
}
// 랜덤 환자 정보를 생성하는 함수
void generateRandomPatient(PatientQueue* pq) {
    const char* names[] = {
        "유지민", "김수환", "박민수", "최지우", "정수연",
        "이도현", "강호동", "유재석", "하하", "송지효",
        "홍길동", "김민정", "이순신", "김유신", "강감찬",
        "이방원", "장보고", "안중근", "김기인", "윤봉길",
        "김정우", "김구", "박명수", "이민용", "손시우",
        "이민기", "정지훈", "김건부", "서민정", "정일우"
    };
    int numNames = sizeof(names) / sizeof(names[0]);

    for (int i = 0; i < 5; i++) {
        Patient p;
        strcpy(p.name, names[rand() % numNames]);
        p.age = rand() % 100 + 1;  // 1세부터 100세까지
        p.riskLevel = rand() % 10 + 1;  // 1부터 10까지

        insertPatient(pq, p);
        printf(" 환자 %s 응급도 %d로 대기명단에 추가되었습니다.\n\n", p.name, p.riskLevel);
    }
}

// 현재 대기 중인 환자 목록을 출력하는 함수
void printQueue(PatientQueue* pq) {
    if (pq->size == 0) {
        printf("대기 중인 환자가 없습니다.\n\n");
        return;
    }
    printf("현재 대기 중인 환자 목록:\n");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < pq->size; i++) {
        Patient p = pq->patients[i];
        printf("이름: %s\n나이: %d\n응급도: %d\n",
            p.name, p.age, p.riskLevel);
        printf("------------------------------------------------------------\n");
    }
    printf("\n");
}

// 현재 수술 대기열을 출력하는 함수
void printSurgeryQueue(PatientQueue* pq) {
    printf("현재 수술 대기 중인 환자 목록:\n");
    ListNode* current = pq->surgeryQueue;
    while (current != NULL) {
        printf("이름: %s\n나이: %d\n응급도: %d\n",
            current->patient.name, current->patient.age, current->patient.riskLevel);
        printf("------------------------------------------------------------\n");
        current = current->next;
    }
    printf("\n");
}

// 현재 진료 대기열을 출력하는 함수
void printTreatmentQueue(PatientQueue* pq) {
    printf("현재 진료 대기 중인 환자 목록:\n");
    ListNode* current = pq->treatmentQueue;
    while (current != NULL) {
        printf("이름: %s\n나이: %d\n응급도: %d\n",
            current->patient.name, current->patient.age, current->patient.riskLevel);
        printf("------------------------------------------------------------\n");
        current = current->next;
    }
    printf("\n");
}

// 시간 변경 함수
void change_surgery(int* currentTime, SurgeryRoom surgeryRooms[]) {
    int previousTime = *currentTime;

    printf("현재 시간을 입력하세요 (HHMM 형식, 예: 1230): ");
    scanf("%d", currentTime); // 새로운 현재 시간 입력
    int newHour = *currentTime / 100;
    int newMinute = *currentTime % 100;

    if (newHour < 0 || newHour > 23 || newMinute < 0 || newMinute > 59) { // 시간 형식 유효성 검사
        printf("잘못된 시간입니다. 다시 입력하세요.\n");
        change_surgery(currentTime, surgeryRooms);
    }
    else {
        int previousTotalMinutes = (previousTime / 100) * 60 + (previousTime % 100);
        int newTotalMinutes = newHour * 60 + newMinute;

        // 하루가 지났는지 확인
        if (newTotalMinutes < previousTotalMinutes) {
            current_day += 1;
            updateSchedules(surgeryRooms, *currentTime, 1); // 유효한 시간 입력 시 스케줄 업데이트
        }

        updateSchedules(surgeryRooms, *currentTime, 0); // 유효한 시간 입력 시 스케줄 업데이트
    }
}


//==========================================================================================
int surgery_patient(Patient pa) {
    int choice;
    int nextRoom = 0;
    int i = 0;
    if (i == 0) {
        reserveSurgery(surgeryRooms, &nextRoom, currentTime, pa); // 수술실 예약
        i = 1;
    }
    else {
        printf("이미 수술실 예약이 되었습니다.\n");
    }
    printf("---------------------------------------------------\n");
    do {
        printf("\n메뉴를 선택하세요:\n");
        printf("1. 현재 수술 스케줄 보기\n");
        printf("2. 다음 환자\n");
        printf("선택: ");

        scanf("%d", &choice); // 메뉴 선택 입력

        switch (choice) {

        case 1:
            printf("\n현재 수술 스케줄:\n");
            for (int i = 0; i < NUM_SURGERY_ROOMS; ++i) {
                printf("수술실 %d:\n", surgeryRooms[i].roomNumber);
                display(surgeryRooms[i].schedule); // 각 수술실의 스케줄 출력
                printf("\n");
            }
            break;
        case 2:
            if (s != 4) {
                printf("다음 환자 받겠습니다.\n");
                printf("--------------------다음환자--------------------------\n");
                s += 1;
            }
            else {
                printf("5명 진료가 끝났습니다.\n");
            }
            break;
        default:
            printf("잘못된 선택입니다. 다시 선택해주세요.\n");
        }
    } while (choice != 2); // 종료 선택 시까지 반복

    return 0;
}

void updateSchedules(SurgeryRoom surgeryRooms[], int currentTime, int next) { //체크
    srand(time(NULL));
    for (int i = 0; i < NUM_SURGERY_ROOMS; ++i) {
        SurgeryListNode* current = surgeryRooms[i].schedule; // 현재 수술실의 스케줄 가져오기
        while (current != NULL) {
            if (current->endTime <= currentTime || next == 1) { // 수술이 끝난 경우
                int stayDays = rand() % 3 + 1; // 1에서 3일 사이의 랜덤 입원 기간
                current->pa.days_admitted = current_day;                          //환자의 입원일
                if (next == 1) current->pa.days_admitted -= 1;
                current->pa.days_discharge = current->pa.days_admitted + stayDays;


                strcpy(patients[num_patients].name, current->pa.name); //입원가는방법
                patients[num_patients].days_admitted = current->pa.days_admitted;
                patients[num_patients].days_discharge = current->pa.days_discharge;
                num_patients += 1;


                printf("방 %d의 수술이 종료되었습니다: 시작 시간: %02d:%02d, 종료 시간: %02d:%02d, 환자 이름: %s\n", surgeryRooms[i].roomNumber, current->startTime / 100, current->startTime % 100, current->endTime / 100, current->endTime % 100, current->pa.name);
                printf("%s는 %d일 동안 입원합니다.\n", current->pa.name, stayDays);

                ddelete(&(surgeryRooms[i].schedule), current->startTime, current->endTime, current->pa.name); // 종료된 수술을 스케줄에서 삭제
                current = surgeryRooms[i].schedule; // 현재 포인터를 리스트의 헤드로 갱신
            }
            else {
                current = current->rlink;
            }
        }
    }
}

SurgeryListNode* surgerycreateNode(int startTime, int endTime, const char* name) {
    SurgeryListNode* newNode = (SurgeryListNode*)malloc(sizeof(SurgeryListNode));
    newNode->startTime = startTime;
    newNode->endTime = endTime;
    strcpy(newNode->pa.name, name); // 환자 이름 복사
    newNode->llink = NULL;
    newNode->rlink = NULL;
    return newNode;
}

void insert(SurgeryListNode** head, int startTime, int endTime, const char* name) {
    SurgeryListNode* newNode = surgerycreateNode(startTime, endTime, name);
    if (*head == NULL) {
        *head = newNode;
    }
    else {
        SurgeryListNode* current = *head;
        while (current->rlink != NULL) { // 리스트 끝까지 탐색
            current = current->rlink;
        }
        current->rlink = newNode;
        newNode->llink = current;
    }
}

void ddelete(SurgeryListNode** head, int startTime, int endTime, const char* name) {
    if (*head == NULL) {
        return;
    }

    SurgeryListNode* current = *head;
    while (current != NULL) {
        if (current->startTime == startTime && current->endTime == endTime && strcmp(current->pa.name, name) == 0) { // 삭제할 노드 찾기
            if (current->llink != NULL) {
                current->llink->rlink = current->rlink; // 이전 노드의 링크 업데이트
            }
            else {
                *head = current->rlink; // 첫 번째 노드를 삭제할 경우 헤드 업데이트
            }
            if (current->rlink != NULL) {
                current->rlink->llink = current->llink; // 다음 노드의 링크 업데이트
            }
            free(current); // 삭제할 노드의 메모리 해제
            return;
        }
        current = current->rlink; // 다음 노드로 이동
    }
}

void display(SurgeryListNode* head) {
    printf("수술 스케줄:\n");
    while (head != NULL) {
        printf("시작 시간: %02d:%02d, 종료 시간: %02d:%02d, 환자 이름: %s -> ", head->startTime / 100, head->startTime % 100, head->endTime / 100, head->endTime % 100, head->pa.name);
        head = head->rlink;
    }
    printf("없음\n");
}

int findEarliestEndTimeRoom(SurgeryRoom surgeryRooms[], int* earliestEndTime) { //체크
    *earliestEndTime = 2400;
    int roomNumber = -1;

    for (int i = 0; i < NUM_SURGERY_ROOMS; ++i) {
        SurgeryListNode* current = surgeryRooms[i].schedule;
        int lastEndTime = 0;

        while (current != NULL) {
            lastEndTime = current->endTime; // 수술실 마지막 수술 종료 시간 추적
            current = current->rlink; // 다음 노드로 이동
        }

        if (lastEndTime < *earliestEndTime) {
            *earliestEndTime = lastEndTime; // 가장 빠른 종료 시간 갱신
            roomNumber = i; // 가장 빠른 종료 시간을 가진 수술실 번호 갱신
        }
    }

    return roomNumber;
}

void reserveSurgery(SurgeryRoom surgeryRooms[], int* nextRoom, int currentTime, Patient pa) { //체크
    char patientName[MAX_NAME_LENGTH];
    printf("----------------------수술 예약----------------------\n");
    printf("현재, %02d일 %02d:%02d입니다.\n", current_day, currentTime / 100, currentTime % 100);
    int startTime = currentTime;
    int endTime = (startTime + 100) % 2400; // 수술이 1시간 걸린다고 가정
    if ((startTime + 100) > 2400)pa.days_discharge++;
    strcpy(patientName, pa.name);
    printf("예약할 환자의 이름을 입력하세요: %s\n", patientName);

    int roomNumber = *nextRoom;
    int earliestEndTime;

    // 비어있는 방 찾기
    for (int i = 0; i < NUM_SURGERY_ROOMS; ++i) {
        if (surgeryRooms[roomNumber].schedule == NULL) {
            insert(&(surgeryRooms[roomNumber].schedule), startTime, endTime, patientName); // 새 수술을 방의 스케줄에 삽입
            printf("수술이 방 %d에 %02d:%02d부터 %02d:%02d까지 예약되었습니다.\n", surgeryRooms[roomNumber].roomNumber, startTime / 100, startTime % 100, endTime / 100, endTime % 100);
            *nextRoom = (*nextRoom + 1) % NUM_SURGERY_ROOMS; // 다음 사용 가능한 방 갱신

            return;
        }
        roomNumber = (roomNumber + 1) % NUM_SURGERY_ROOMS; // 다음 방으로 이동
    }

    //모든 방이 가득 찬 경우, 가장 빠른 종료 시간을 가진 방 찾기
    roomNumber = findEarliestEndTimeRoom(surgeryRooms, &earliestEndTime);
    startTime = earliestEndTime;
    endTime = (startTime + 100) % 2400;

    insert(&(surgeryRooms[roomNumber].schedule), startTime, endTime, patientName); // 새 수술을 찾은 방의 스케줄에 삽입
    printf("모든 수술방이 예약되어 있습니다. 가장 빨리 끝나는 방 %d에 %02d:%02d부터 %02d:%02d까지 예약되었습니다.\n", surgeryRooms[roomNumber].roomNumber, startTime / 100, startTime % 100, endTime / 100, endTime % 100);
}


//==========================================================================================

// 입원일과 퇴원일을 기준으로 정렬하기 위한 비교 함수
int comparePatients(const Patient* a, const Patient* b) {
    if (a->days_admitted != b->days_admitted) {
        return a->days_admitted - b->days_admitted;
    }
    else {
        return a->days_discharge - b->days_discharge;
    }
}


// 퀵 정렬의 partition 함수
int partition(Patient list[], int left, int right)
{
    Patient pivot, temp;
    int low, high;
    low = left;
    high = right + 1;
    pivot = list[left];
    do {
        do
            low++;
        while (low <= right && list[low].days_discharge < pivot.days_discharge);
        do
            high--;
        while (high >= left && list[high].days_discharge > pivot.days_discharge);
        if (low < high) swap(&list[low], &list[high]);
    } while (low < high);
    swap(&list[left], &list[high]);
    return high;
}

void quick_sort(Patient list[], int left, int right)
{
    if (left < right) {
        int q = partition(list, left, right);
        quick_sort(list, left, q - 1);
        quick_sort(list, q + 1, right);
    }
}


// 환자 정보를 출력하는 함수
void print_patient_data() {
    printf("\n환자 정보:\n");
    printf("--------------------------------------------------------------\n");
    printf("이름\t\t입원일\t\t퇴원 예정일\n");
    printf("--------------------------------------------------------------\n");
    for (int i = 0; i < num_patients; i++) {
        printf("%s\t\t%d일\t\t%d일\n", patients[i].name, patients[i].days_admitted, patients[i].days_discharge);
    }
}


//==========================================================================================


int main() {
    srand(time(NULL));
    PatientQueue pq;
    initQueue(&pq);


    int choice;
    for (int i = 0; i < NUM_SURGERY_ROOMS; ++i) {
        surgeryRooms[i].roomNumber = i + 1; // 수술실 번호 설정
        surgeryRooms[i].schedule = NULL;    // 스케줄 초기화
    }
    while (1) {
        printf("\n-------응급실 시스템-------\n");
        printf(" %d일   %02d : %02d\n", current_day, currentTime / 100, currentTime % 100);
        printf("1. 랜덤 환자 접수 (5명)\n");
        //1번 넣고 진료 넣고-> 2번 넣고 -> 하루지난 다음에 -> 랜덤 -> 진료하고-> 2일차때 시간단위 수술실, 입원일
        printf("2. 환자 진료(5명씩 진료)\n");
        printf("3. 수술 스케줄 보기\n");
        printf("4. 현재 시간 변경\n");  //중요 -> 코드적인 설명(change--)
        printf("5. 현재 진료 대기 중인 환자 목록 출력\n");
        printf("6. 입원한 환자 현황\n");
        printf("7. 종료\n");

        printf("선택 해주세요: ");
        if (scanf("%d", &choice) != 1) {
            printf("잘못된 입력입니다. 숫자를 입력해주세요.\n\n");
            while (getchar() != '\n'); // 입력 버퍼 비우기
            continue;
        }
        getchar(); // 버퍼에 남아 있는 개행 문자 제거

        switch (choice) {
        case 1:
            generateRandomPatient(&pq);
            break;
        case 2: {
            for (int i = 0; i < 5; i++) {
                removeHighestPriority(&pq);
            }
            break;
        }
        case 3:
            printf("\n현재 수술 스케줄:\n");
            for (int i = 0; i < NUM_SURGERY_ROOMS; ++i) {
                printf("수술실 %d:\n", surgeryRooms[i].roomNumber);
                display(surgeryRooms[i].schedule); // 각 수술실의 스케줄 출력
                printf("\n");
            }
            break;
        case 4:
            change_surgery(&currentTime, surgeryRooms); // 현재 시간 변경
            break;
        case 5:
            printQueue(&pq);
            break;
        case 6:
            quick_sort(patients, 0, num_patients - 1);
            print_patient_data();
            break;
        case 7:
            printf("안녕히가세요.\n");
            return 0;
            printf("잘못된 선택입니다. 다시 선택해주세요.\n\n");
        }
    }

    return 0;
}

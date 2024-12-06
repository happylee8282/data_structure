#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include "patient.h"

#define NUM_SURGERY_ROOMS 3 // ������ ������ ���� üũ
#define MAX_NAME_LENGTH 100 // ȯ�� �̸� �ִ� ���� ����
#define MAX_PATIENTS 100
int num_patients = 0;       //�Կ��� ȯ�ڼ�
int current_day = 1; //���� ��¥



typedef struct { //üũ
    char name[MAX_NAME_LENGTH];          // ȯ���� �̸�
    int age;                // ����
    int riskLevel;          // ���޵� (1���� 10 ������ ����)
    int days_admitted;  // �Կ���
    int days_discharge; // ��� ������
} Patient;


//���Ḯ��Ʈ�� ��带 ��Ÿ���� ����ü
typedef struct ListNode {
    Patient patient;
    struct ListNode* next;
} ListNode;

typedef struct {
    Patient patients[MAX_PATIENTS];
    int size;               // ��� ���� ȯ�� ��
    ListNode* surgeryQueue; // ���� ��⿭�� ���� ���
    ListNode* treatmentQueue; // ���� ��⿭�� ���� ���
} PatientQueue;

//-------------------------------------------------------------------���� ����ü
typedef struct SurgeryRoom {
    int roomNumber;
    struct SurgeryListNode* schedule;
} SurgeryRoom;

typedef struct SurgeryListNode { //üũ
    int startTime; // ���� ���� �ð� (HHMM ����)
    int endTime;   // ���� ���� �ð� (HHMM ����)
    Patient pa; // ȯ�� �̸�
    struct SurgeryListNode* llink;
    struct SurgeryListNode* rlink;
} SurgeryListNode;
//-----------------------------------------------------------------------
typedef struct Node {
    char question[256];
    struct Node* yes;
    struct Node* no;
} Node;

SurgeryRoom surgeryRooms[NUM_SURGERY_ROOMS]; // ������ �迭 ����


void change_surgery(int* currentTime, SurgeryRoom surgeryRooms[]);
void updateSchedules(SurgeryRoom surgeryRooms[], int currentTime, int next);
void reserveSurgery(SurgeryRoom surgeryRooms[], int* nextRoom, int currentTime, Patient pa);
int findEarliestEndTimeRoom(SurgeryRoom surgeryRooms[], int* earliestEndTime);
void display(SurgeryListNode* head);
void ddelete(SurgeryListNode** head, int startTime, int endTime, const char* name);
void insert(SurgeryListNode** head, int startTime, int endTime, const char* name);
SurgeryListNode* surgerycreateNode(int startTime, int endTime, const char* name);

int currentTime = 1000; // HHMM �������� �ʱ� �ð� ����
int s = 0;                      //5�� ���� �ޱ� ���� ����
int date = 0101;                       //��¥�� ��Ÿ���� ����

Patient patients[MAX_PATIENTS];     //�Կ� ȯ�� �迭 //�߿� ������

int patientCount = 0;                         //�Կ� ȯ�� ��
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
            printf("y �Ǵ� n ���� �������ּ��� .\n");
        }
    }
    return(node->question);
}

Node* createAkinator() {
    Node* root = createNode("1.�ܰ� 2.���� : ");

    //�ܰ�
    root->yes = createNode("�㸮�� �����ʴϱ�? (y/n) : ");
    root->yes->yes = createNode("�Ű�ܰ�");

    root->yes->no = createNode("������ �����ʴϱ�? (y/n) : ");
    root->yes->no->yes = createNode("��οܰ�");
    root->yes->no->no = createNode("�����ܰ�");

    //����
    root->no = createNode("�谡 �����ʴϱ�? (y/n) : ");
    root->no->yes = createNode("��ȭ�⳻��");

    root->no->no = createNode("ȣ��� ���� ������ �ֽ��ϱ�? (��ħ, ȣ���� ��) (y/n) : ");
    root->no->no->yes = createNode("ȣ��⳻��");

    root->no->no->no = createNode("�Ϲݳ���");

    return root;
}

/*********************************************************************************************/

// �ʱ�ȭ �Լ�: ȯ�� ť�� �ʱ�ȭ�ϰ� �ð��� ���� �ð����� �����մϴ�.
void initQueue(PatientQueue* pq) {
    pq->size = 0;
    pq->surgeryQueue = NULL;      // ���� ��⿭ �ʱ�ȭ
    pq->treatmentQueue = NULL;    // ���� ��⿭ �ʱ�ȭ
}

// �� ȯ�� ������ ��ȯ�ϴ� �Լ�
void swap(Patient* a, Patient* b) {
    Patient temp = *a;
    *a = *b;
    *b = temp;
}

// �� ������ �� �� �Լ�: ���� �߰��� ȯ�ڸ� ���� �ùٸ� ��ġ�� �÷� �����ϴ�.
void heapifyUp(PatientQueue* pq, int index) {
    while (index > 0) {
        int parentIndex = (index - 1) / 2;
        if (pq->patients[index].riskLevel <= pq->patients[parentIndex].riskLevel) {
            break;  // �� �Ӽ��� ������ ������ �ݺ�
        }
        swap(&pq->patients[index], &pq->patients[parentIndex]);
        index = parentIndex;
    }
}

// �� ������ �ٿ� �� �Լ�: ���� ��Ʈ���� ���� ū ���� ã�� �Ʒ��� ���� �����ϴ�.
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
            break;  // �� �Ӽ��� ������ ������ �ݺ�
        }

        swap(&pq->patients[index], &pq->patients[largest]);
        index = largest;
    }
}

// ȯ�ڸ� ��⿭�� �߰��ϴ� �Լ�: ���� Ư���� �����ϸ� ȯ�ڸ� �����մϴ�.
//heapifyUp()�Լ��� ���� �켱���� ť���� �ִ��� Ư���� �����Ͽ� ȯ�ڸ� ������.
void insertPatient(PatientQueue* pq, Patient p) {
    if (pq->size == MAX_PATIENTS) {
        printf("ȯ�ڰ� �� á���ϴ�!\n");
        return;
    }

    pq->patients[pq->size] = p;
    heapifyUp(pq, pq->size);  // ���� �����ϱ� ���� �ùٸ� ��ġ�� ����
    pq->size++;
}

// �켱 ������ ���� ���� ȯ�ڸ� �����ϰ� ��ȯ�ϴ� �Լ�
//heapifyDown()�Լ��� ���� �켱���� ť���� ��Ʈ�� �ִ� ȯ�� ���� �� ��ȯ
//���Ḯ��Ʈ�� ���� ��ȯ�� ȯ�ڸ� ���� ����Ʈ, ���� ����Ʈ�� ����
Patient removeHighestPriority(PatientQueue* pq)
{
    if (pq->size == 0) {
        printf("ȯ�ڰ� �����ϴ�!\n");
        Patient empty = { "", 0, 0, 0 };
        return empty;
    }
    Patient highestPriorityPatient = pq->patients[0];
    pq->patients[0] = pq->patients[pq->size - 1];
    pq->size--;
    heapifyDown(pq, 0);  // ���� �����ϱ� ���� �ùٸ� ��ġ�� ����

    int surgery;
    while (1) {
        printf("���� ȯ�� : %s, ���赵: %d\n", highestPriorityPatient.name, highestPriorityPatient.riskLevel);
        printf("������ �޾ƾ� �ϳ���? (1: �� / 0: �ƴϿ�): ");

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
                int stayDays = rand() % 3 + 1; // 1���� 3�� ������ ���� �Կ� �Ⱓ
                highestPriorityPatient.days_admitted = current_day;                          //ȯ���� �Կ���
                highestPriorityPatient.days_discharge = highestPriorityPatient.days_admitted + stayDays;
                printf("%sȯ�ں� %s�� ���� ��  %d�� ���� �Կ��Ͽ����ϴ�.\n", highestPriorityPatient.name, department, stayDays);
                strcpy(patients[num_patients].name, highestPriorityPatient.name);
                patients[num_patients].days_admitted = highestPriorityPatient.days_admitted;
                patients[num_patients].days_discharge = highestPriorityPatient.days_discharge;
                num_patients += 1;
                if (s != 4) {
                    printf("���� ȯ�� �ްڽ��ϴ�.\n");
                    printf("--------------------����ȯ��--------------------------\n");
                    s += 1;
                }
                else {
                    printf("5�� ���ᰡ �������ϴ�.\n");
                }

                break;
            }
            else {
                printf("%sȯ�ں� %s�� ���� �� ����Ͽ����ϴ�.\n", highestPriorityPatient.name, department);
                break;
            }

        }
        else {
            printf("�߸� �Է��߽��ϴ�. ���� 1 �Ǵ� 0�� �Է����ּ���.\n");
            while (getchar() != '\n'); // �Է� ���� ����
        }
    }
    return highestPriorityPatient;
}
// ���� ȯ�� ������ �����ϴ� �Լ�
void generateRandomPatient(PatientQueue* pq) {
    const char* names[] = {
        "������", "���ȯ", "�ڹμ�", "������", "������",
        "�̵���", "��ȣ��", "���缮", "����", "����ȿ",
        "ȫ�浿", "�����", "�̼���", "������", "������",
        "�̹��", "�庸��", "���߱�", "�����", "������",
        "������", "�豸", "�ڸ��", "�̹ο�", "�սÿ�",
        "�̹α�", "������", "��Ǻ�", "������", "���Ͽ�"
    };
    int numNames = sizeof(names) / sizeof(names[0]);

    for (int i = 0; i < 5; i++) {
        Patient p;
        strcpy(p.name, names[rand() % numNames]);
        p.age = rand() % 100 + 1;  // 1������ 100������
        p.riskLevel = rand() % 10 + 1;  // 1���� 10����

        insertPatient(pq, p);
        printf(" ȯ�� %s ���޵� %d�� ����ܿ� �߰��Ǿ����ϴ�.\n\n", p.name, p.riskLevel);
    }
}

// ���� ��� ���� ȯ�� ����� ����ϴ� �Լ�
void printQueue(PatientQueue* pq) {
    if (pq->size == 0) {
        printf("��� ���� ȯ�ڰ� �����ϴ�.\n\n");
        return;
    }
    printf("���� ��� ���� ȯ�� ���:\n");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < pq->size; i++) {
        Patient p = pq->patients[i];
        printf("�̸�: %s\n����: %d\n���޵�: %d\n",
            p.name, p.age, p.riskLevel);
        printf("------------------------------------------------------------\n");
    }
    printf("\n");
}

// ���� ���� ��⿭�� ����ϴ� �Լ�
void printSurgeryQueue(PatientQueue* pq) {
    printf("���� ���� ��� ���� ȯ�� ���:\n");
    ListNode* current = pq->surgeryQueue;
    while (current != NULL) {
        printf("�̸�: %s\n����: %d\n���޵�: %d\n",
            current->patient.name, current->patient.age, current->patient.riskLevel);
        printf("------------------------------------------------------------\n");
        current = current->next;
    }
    printf("\n");
}

// ���� ���� ��⿭�� ����ϴ� �Լ�
void printTreatmentQueue(PatientQueue* pq) {
    printf("���� ���� ��� ���� ȯ�� ���:\n");
    ListNode* current = pq->treatmentQueue;
    while (current != NULL) {
        printf("�̸�: %s\n����: %d\n���޵�: %d\n",
            current->patient.name, current->patient.age, current->patient.riskLevel);
        printf("------------------------------------------------------------\n");
        current = current->next;
    }
    printf("\n");
}

// �ð� ���� �Լ�
void change_surgery(int* currentTime, SurgeryRoom surgeryRooms[]) {
    int previousTime = *currentTime;

    printf("���� �ð��� �Է��ϼ��� (HHMM ����, ��: 1230): ");
    scanf("%d", currentTime); // ���ο� ���� �ð� �Է�
    int newHour = *currentTime / 100;
    int newMinute = *currentTime % 100;

    if (newHour < 0 || newHour > 23 || newMinute < 0 || newMinute > 59) { // �ð� ���� ��ȿ�� �˻�
        printf("�߸��� �ð��Դϴ�. �ٽ� �Է��ϼ���.\n");
        change_surgery(currentTime, surgeryRooms);
    }
    else {
        int previousTotalMinutes = (previousTime / 100) * 60 + (previousTime % 100);
        int newTotalMinutes = newHour * 60 + newMinute;

        // �Ϸ簡 �������� Ȯ��
        if (newTotalMinutes < previousTotalMinutes) {
            current_day += 1;
            updateSchedules(surgeryRooms, *currentTime, 1); // ��ȿ�� �ð� �Է� �� ������ ������Ʈ
        }

        updateSchedules(surgeryRooms, *currentTime, 0); // ��ȿ�� �ð� �Է� �� ������ ������Ʈ
    }
}


//==========================================================================================
int surgery_patient(Patient pa) {
    int choice;
    int nextRoom = 0;
    int i = 0;
    if (i == 0) {
        reserveSurgery(surgeryRooms, &nextRoom, currentTime, pa); // ������ ����
        i = 1;
    }
    else {
        printf("�̹� ������ ������ �Ǿ����ϴ�.\n");
    }
    printf("---------------------------------------------------\n");
    do {
        printf("\n�޴��� �����ϼ���:\n");
        printf("1. ���� ���� ������ ����\n");
        printf("2. ���� ȯ��\n");
        printf("����: ");

        scanf("%d", &choice); // �޴� ���� �Է�

        switch (choice) {

        case 1:
            printf("\n���� ���� ������:\n");
            for (int i = 0; i < NUM_SURGERY_ROOMS; ++i) {
                printf("������ %d:\n", surgeryRooms[i].roomNumber);
                display(surgeryRooms[i].schedule); // �� �������� ������ ���
                printf("\n");
            }
            break;
        case 2:
            if (s != 4) {
                printf("���� ȯ�� �ްڽ��ϴ�.\n");
                printf("--------------------����ȯ��--------------------------\n");
                s += 1;
            }
            else {
                printf("5�� ���ᰡ �������ϴ�.\n");
            }
            break;
        default:
            printf("�߸��� �����Դϴ�. �ٽ� �������ּ���.\n");
        }
    } while (choice != 2); // ���� ���� �ñ��� �ݺ�

    return 0;
}

void updateSchedules(SurgeryRoom surgeryRooms[], int currentTime, int next) { //üũ
    srand(time(NULL));
    for (int i = 0; i < NUM_SURGERY_ROOMS; ++i) {
        SurgeryListNode* current = surgeryRooms[i].schedule; // ���� �������� ������ ��������
        while (current != NULL) {
            if (current->endTime <= currentTime || next == 1) { // ������ ���� ���
                int stayDays = rand() % 3 + 1; // 1���� 3�� ������ ���� �Կ� �Ⱓ
                current->pa.days_admitted = current_day;                          //ȯ���� �Կ���
                if (next == 1) current->pa.days_admitted -= 1;
                current->pa.days_discharge = current->pa.days_admitted + stayDays;


                strcpy(patients[num_patients].name, current->pa.name); //�Կ����¹��
                patients[num_patients].days_admitted = current->pa.days_admitted;
                patients[num_patients].days_discharge = current->pa.days_discharge;
                num_patients += 1;


                printf("�� %d�� ������ ����Ǿ����ϴ�: ���� �ð�: %02d:%02d, ���� �ð�: %02d:%02d, ȯ�� �̸�: %s\n", surgeryRooms[i].roomNumber, current->startTime / 100, current->startTime % 100, current->endTime / 100, current->endTime % 100, current->pa.name);
                printf("%s�� %d�� ���� �Կ��մϴ�.\n", current->pa.name, stayDays);

                ddelete(&(surgeryRooms[i].schedule), current->startTime, current->endTime, current->pa.name); // ����� ������ �����ٿ��� ����
                current = surgeryRooms[i].schedule; // ���� �����͸� ����Ʈ�� ���� ����
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
    strcpy(newNode->pa.name, name); // ȯ�� �̸� ����
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
        while (current->rlink != NULL) { // ����Ʈ ������ Ž��
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
        if (current->startTime == startTime && current->endTime == endTime && strcmp(current->pa.name, name) == 0) { // ������ ��� ã��
            if (current->llink != NULL) {
                current->llink->rlink = current->rlink; // ���� ����� ��ũ ������Ʈ
            }
            else {
                *head = current->rlink; // ù ��° ��带 ������ ��� ��� ������Ʈ
            }
            if (current->rlink != NULL) {
                current->rlink->llink = current->llink; // ���� ����� ��ũ ������Ʈ
            }
            free(current); // ������ ����� �޸� ����
            return;
        }
        current = current->rlink; // ���� ���� �̵�
    }
}

void display(SurgeryListNode* head) {
    printf("���� ������:\n");
    while (head != NULL) {
        printf("���� �ð�: %02d:%02d, ���� �ð�: %02d:%02d, ȯ�� �̸�: %s -> ", head->startTime / 100, head->startTime % 100, head->endTime / 100, head->endTime % 100, head->pa.name);
        head = head->rlink;
    }
    printf("����\n");
}

int findEarliestEndTimeRoom(SurgeryRoom surgeryRooms[], int* earliestEndTime) { //üũ
    *earliestEndTime = 2400;
    int roomNumber = -1;

    for (int i = 0; i < NUM_SURGERY_ROOMS; ++i) {
        SurgeryListNode* current = surgeryRooms[i].schedule;
        int lastEndTime = 0;

        while (current != NULL) {
            lastEndTime = current->endTime; // ������ ������ ���� ���� �ð� ����
            current = current->rlink; // ���� ���� �̵�
        }

        if (lastEndTime < *earliestEndTime) {
            *earliestEndTime = lastEndTime; // ���� ���� ���� �ð� ����
            roomNumber = i; // ���� ���� ���� �ð��� ���� ������ ��ȣ ����
        }
    }

    return roomNumber;
}

void reserveSurgery(SurgeryRoom surgeryRooms[], int* nextRoom, int currentTime, Patient pa) { //üũ
    char patientName[MAX_NAME_LENGTH];
    printf("----------------------���� ����----------------------\n");
    printf("����, %02d�� %02d:%02d�Դϴ�.\n", current_day, currentTime / 100, currentTime % 100);
    int startTime = currentTime;
    int endTime = (startTime + 100) % 2400; // ������ 1�ð� �ɸ��ٰ� ����
    if ((startTime + 100) > 2400)pa.days_discharge++;
    strcpy(patientName, pa.name);
    printf("������ ȯ���� �̸��� �Է��ϼ���: %s\n", patientName);

    int roomNumber = *nextRoom;
    int earliestEndTime;

    // ����ִ� �� ã��
    for (int i = 0; i < NUM_SURGERY_ROOMS; ++i) {
        if (surgeryRooms[roomNumber].schedule == NULL) {
            insert(&(surgeryRooms[roomNumber].schedule), startTime, endTime, patientName); // �� ������ ���� �����ٿ� ����
            printf("������ �� %d�� %02d:%02d���� %02d:%02d���� ����Ǿ����ϴ�.\n", surgeryRooms[roomNumber].roomNumber, startTime / 100, startTime % 100, endTime / 100, endTime % 100);
            *nextRoom = (*nextRoom + 1) % NUM_SURGERY_ROOMS; // ���� ��� ������ �� ����

            return;
        }
        roomNumber = (roomNumber + 1) % NUM_SURGERY_ROOMS; // ���� ������ �̵�
    }

    //��� ���� ���� �� ���, ���� ���� ���� �ð��� ���� �� ã��
    roomNumber = findEarliestEndTimeRoom(surgeryRooms, &earliestEndTime);
    startTime = earliestEndTime;
    endTime = (startTime + 100) % 2400;

    insert(&(surgeryRooms[roomNumber].schedule), startTime, endTime, patientName); // �� ������ ã�� ���� �����ٿ� ����
    printf("��� �������� ����Ǿ� �ֽ��ϴ�. ���� ���� ������ �� %d�� %02d:%02d���� %02d:%02d���� ����Ǿ����ϴ�.\n", surgeryRooms[roomNumber].roomNumber, startTime / 100, startTime % 100, endTime / 100, endTime % 100);
}


//==========================================================================================

// �Կ��ϰ� ������� �������� �����ϱ� ���� �� �Լ�
int comparePatients(const Patient* a, const Patient* b) {
    if (a->days_admitted != b->days_admitted) {
        return a->days_admitted - b->days_admitted;
    }
    else {
        return a->days_discharge - b->days_discharge;
    }
}


// �� ������ partition �Լ�
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


// ȯ�� ������ ����ϴ� �Լ�
void print_patient_data() {
    printf("\nȯ�� ����:\n");
    printf("--------------------------------------------------------------\n");
    printf("�̸�\t\t�Կ���\t\t��� ������\n");
    printf("--------------------------------------------------------------\n");
    for (int i = 0; i < num_patients; i++) {
        printf("%s\t\t%d��\t\t%d��\n", patients[i].name, patients[i].days_admitted, patients[i].days_discharge);
    }
}


//==========================================================================================


int main() {
    srand(time(NULL));
    PatientQueue pq;
    initQueue(&pq);


    int choice;
    for (int i = 0; i < NUM_SURGERY_ROOMS; ++i) {
        surgeryRooms[i].roomNumber = i + 1; // ������ ��ȣ ����
        surgeryRooms[i].schedule = NULL;    // ������ �ʱ�ȭ
    }
    while (1) {
        printf("\n-------���޽� �ý���-------\n");
        printf(" %d��   %02d : %02d\n", current_day, currentTime / 100, currentTime % 100);
        printf("1. ���� ȯ�� ���� (5��)\n");
        //1�� �ְ� ���� �ְ�-> 2�� �ְ� -> �Ϸ����� ������ -> ���� -> �����ϰ�-> 2������ �ð����� ������, �Կ���
        printf("2. ȯ�� ����(5�� ����)\n");
        printf("3. ���� ������ ����\n");
        printf("4. ���� �ð� ����\n");  //�߿� -> �ڵ����� ����(change--)
        printf("5. ���� ���� ��� ���� ȯ�� ��� ���\n");
        printf("6. �Կ��� ȯ�� ��Ȳ\n");
        printf("7. ����\n");

        printf("���� ���ּ���: ");
        if (scanf("%d", &choice) != 1) {
            printf("�߸��� �Է��Դϴ�. ���ڸ� �Է����ּ���.\n\n");
            while (getchar() != '\n'); // �Է� ���� ����
            continue;
        }
        getchar(); // ���ۿ� ���� �ִ� ���� ���� ����

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
            printf("\n���� ���� ������:\n");
            for (int i = 0; i < NUM_SURGERY_ROOMS; ++i) {
                printf("������ %d:\n", surgeryRooms[i].roomNumber);
                display(surgeryRooms[i].schedule); // �� �������� ������ ���
                printf("\n");
            }
            break;
        case 4:
            change_surgery(&currentTime, surgeryRooms); // ���� �ð� ����
            break;
        case 5:
            printQueue(&pq);
            break;
        case 6:
            quick_sort(patients, 0, num_patients - 1);
            print_patient_data();
            break;
        case 7:
            printf("�ȳ���������.\n");
            return 0;
            printf("�߸��� �����Դϴ�. �ٽ� �������ּ���.\n\n");
        }
    }

    return 0;
}

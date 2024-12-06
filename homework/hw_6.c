#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SWAP(x, y, t) ( (t)=(x), (x)=(y), (y)=(t) )

// 선택정렬
void selection_sort(int list[], int n)
{
    int i, j, least, temp;
    for (i = 0; i < n - 1; i++) {
        least = i;
        for (j = i + 1; j < n; j++) // 최솟값 탐색
            if (list[j] < list[least]) least = j;
        SWAP(list[i], list[least], temp);
    }
}

// 삽입정렬
void insertion_sort(int list[], int n)
{
    int i, j, key;
    for (i = 1; i < n; i++)
    {
        key = list[i];
        for (j = i - 1; j >= 0 && list[j] > key; j--)
            list[j + 1] = list[j];
        list[j + 1] = key;
    }
}

// 버블정렬
void bubble_sort(int list[], int n)
{
    int i, j, temp;
    for (i = n - 1; i > 0; i--)
    {
        for (j = 0; j < i; j++)
        {
            if (list[j] > list[j + 1])
                SWAP((list[j]), (list[j + 1]), temp);
        }
    }
}

// 셀 정렬
void inc_insertion_sort(int list[], int first, int last, int gap)
{
    int i, j, key;
    for (i = first + gap; i <= last; i = i + gap)
    {
        key = list[i];
        for (j = i - gap; j >= first && key < list[j]; j = j - gap)
            list[j + gap] = list[j];
        list[j + gap] = key;
    }
}

void shell_sort(int list[], int n)
{
    int i, gap;
    for (gap = n / 2; gap > 0; gap = gap / 2)
    {
        if ((gap % 2) == 0) gap++;
        for (i = 0; i < gap; i++)
            inc_insertion_sort(list, i, n - 1, gap);
    }
}

// 합병정렬
int sorted[50000]; // 추가 공간이 필요
void merge(int list[], int left, int mid, int right)
{
    int i, j, k, l;
    i = left; j = mid + 1; k = left;
    // 분할 정렬된 list의 합병
    while (i <= mid && j <= right)
    {
        if (list[i] <= list[j]) sorted[k++] = list[i++];
        else sorted[k++] = list[j++];
    }
    if (i > mid) // 남아 있는 레코드의 일괄 복사
        for (l = j; l <= right; l++)
            sorted[k++] = list[l];
    else // 남아 있는 레코드의 일괄 복사
        for (l = i; l <= mid; l++)
            sorted[k++] = list[l];
    // 배열 sorted[]의 리스트를 배열 list[]로 복사
    for (l = left; l <= right; l++)
        list[l] = sorted[l];
}

void merge_sort(int list[], int left, int right)
{
    int mid;
    if (left < right)
    {
        mid = (left + right) / 2; // 리스트의 균등분할
        merge_sort(list, left, mid); // 부분리스트 정렬
        merge_sort(list, mid + 1, right);//부분리스트 정렬
        merge(list, left, mid, right); // 합병
    }
}

// 퀵정렬
void quick_sort(int list[], int left, int right)
{
    if (left < right)
    {
        int q = partition(list, left, right);
        quick_sort(list, left, q - 1);
        quick_sort(list, q + 1, right);
    }
}

int partition(int list[], int left, int right)
{
    int pivot, temp;
    int low, high;

    low = left;
    high = right + 1;
    pivot = list[left];
    do {
        do
            low++;
        while (low <= right && list[low] < pivot);
        do
            high--;
        while (high >= left && list[high] > pivot);
        if (low < high) SWAP(list[low], list[high], temp);
    } while (low < high);
    SWAP(list[left], list[high], temp);
    return high;
}

int main(void)
{
    int i;
    int n = 50000;
    int list[50000];
    int selection_sort_val[50000];
    int insertion_sort_val[50000];
    int bubble_sort_val[50000];
    int shell_sort_val[50000];
    int merge_sort_val[50000];
    int quick_sort_val[50000];

    srand(time(NULL));
    for (i = 0; i < n; i++) // 난수 생성 및 출력
    {
        list[i] = rand() % 50000; // 난수 발생 범위 0~49999
        selection_sort_val[i] = list[i];
        insertion_sort_val[i] = list[i];
        bubble_sort_val[i] = list[i];
        shell_sort_val[i] = list[i];
        merge_sort_val[i] = list[i];
        quick_sort_val[i] = list[i];
    }

    clock_t start, end;
    printf("랜덤인 경우\n");

    start = clock();
    selection_sort(selection_sort_val, n); // 선택정렬 호출
    end = clock();
    printf("selection_sort time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    insertion_sort(insertion_sort_val, n); // 삽입정렬 호출
    end = clock();
    printf("insertion_sort time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    bubble_sort(bubble_sort_val, n); // 버블정렬 호출
    end = clock();
    printf("bubble_sort time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    shell_sort(shell_sort_val, n); // 셸 정렬 호출
    end = clock();
    printf("shell_sort time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    merge_sort(merge_sort_val, 0, n - 1); // 합병정렬 호출
    end = clock();
    printf("merge_sort time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    quick_sort(quick_sort_val, 0, n - 1); // 퀵정렬 호출
    end = clock();
    printf("quick_sort time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);


    printf("나열된 경우\n");
    for (i = 0; i < n; i++) // 난수 생성 및 출력
    {
        list[i] = i;
        selection_sort_val[i] = list[i];
        insertion_sort_val[i] = list[i];
        bubble_sort_val[i] = list[i];
        shell_sort_val[i] = list[i];
        merge_sort_val[i] = list[i];
        quick_sort_val[i] = list[i];
    }


    start = clock();
    selection_sort(selection_sort_val, n); // 선택정렬 호출
    end = clock();
    printf("selection_sort time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    insertion_sort(insertion_sort_val, n); // 삽입정렬 호출
    end = clock();
    printf("insertion_sort time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    bubble_sort(bubble_sort_val, n); // 버블정렬 호출
    end = clock();
    printf("bubble_sort time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    shell_sort(shell_sort_val, n); // 셸 정렬 호출
    end = clock();
    printf("shell_sort time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    merge_sort(merge_sort_val, 0, n - 1); // 합병정렬 호출
    end = clock();
    printf("merge_sort time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);


    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// ============ ПРЕОБРАЗОВАНИЕ ЧИСЕЛ ============

// Для 32 бит
typedef union {
    float f;
    unsigned int u;
} un32;

// Для 64 бит
typedef union {
    double d;
    unsigned long long u;
} un64;

// Для 128 бит
typedef union {
    long double ld;
    unsigned __int128 u;
} un128;

// Получение машинного представления 32 бит
unsigned int get32(long double x) {
    un32 a;
    a.f = (float)x;
    return a.u;
}

// Получение машинного представления 64 бит
unsigned long long get64(long double x) {
    un64 a;
    a.d = (double)x;
    return a.u;
}

// Получение машинного представления 128 бит
unsigned __int128 get128(long double x) {
    un128 a;
    a.ld = x;
    return a.u;
}

// Обратное преобразование из 32 бит
long double back32(unsigned int x) {
    un32 a;
    a.u = x;
    return (long double)a.f;
}

// Обратное преобразование из 64 бит
long double back64(unsigned long long x) {
    un64 a;
    a.u = x;
    return (long double)a.d;
}

// Обратное преобразование из 128 бит
long double back128(unsigned __int128 x) {
    un128 a;
    a.u = x;
    return a.ld;
}

// Для печати 128-битного числа
void print128(unsigned __int128 x, char *buf) {
    unsigned long long hi = (unsigned long long)(x >> 64);
    unsigned long long lo = (unsigned long long)x;
    sprintf(buf, "0x%016llX%016llX", hi, lo);
}

// ============ ГЕНЕРАЦИЯ ЧИСЕЛ ============

// Случайное число от a до b с P знаками после запятой
long double rand_num(long double a, long double b, int p) {
    long double r = a + (long double)rand() / RAND_MAX * (b - a);
    
    // Округление до P знаков
    long double mult = powl(10, p);
    r = roundl(r * mult) / mult;
    
    return r;
}

// ============ ЗАПИСЬ В ФАЙЛЫ ============

// Запись таблицы 1 (только числа)
void write_table1(const char *fname, long double *nums, int k, int v) {
    FILE *f = fopen(fname, "w");
    if (!f) return;
    
    fprintf(f, "ВАРИАНТ %d\n", v);
    fprintf(f, "================================\n\n");
    fprintf(f, "N\tВещественное число\n");
    fprintf(f, "-\t------------------\n");
    
    for (int i = 0; i < k; i++) {
        // Для печати long double используем %Lf
        fprintf(f, "%d\t%.10Lf\n", i + 1, nums[i]);
    }
    
    fclose(f);
}

// Запись таблицы 2 (число, машинное представление, ошибка)
void write_table2(const char *fname, long double *nums, int k, int v, int bits) {
    FILE *f = fopen(fname, "w");
    if (!f) return;
    
    fprintf(f, "ВАРИАНТ %d (%d бит)\n", v, bits);
    fprintf(f, "================================\n\n");
    fprintf(f, "N\tВещественное число\tМашинное представление\tОшибка\n");
    fprintf(f, "-\t------------------\t--------------------\t------\n");
    
    for (int i = 0; i < k; i++) {
        long double err = 0.0;
        char mach[100] = "";
        
        if (bits == 32) {
            unsigned int m = get32(nums[i]);
            long double back = back32(m);
            err = fabsl(nums[i] - back);
            sprintf(mach, "0x%08X", m);
        }
        else if (bits == 64) {
            unsigned long long m = get64(nums[i]);
            long double back = back64(m);
            err = fabsl(nums[i] - back);
            sprintf(mach, "0x%016llX", m);
        }
        else if (bits == 128) {
            unsigned __int128 m = get128(nums[i]);
            long double back = back128(m);
            err = fabsl(nums[i] - back);
            print128(m, mach);
        }
        
        fprintf(f, "%d\t%.10Lf\t%s\t%Le\n", i + 1, nums[i], mach, err);
    }
    
    fclose(f);
}

// ============ ГЛАВНАЯ ФУНКЦИЯ ============

int main() {
    // Для случайных чисел
    srand(time(NULL));
    
    // Входные данные
    int N = 20;         // кол-во студентов
    int K = 5;          // кол-во заданий
    int bits = 128;     // разрядность (32, 64, 128)
    long double a = -10.0L;  // начало диапазона
    long double b = 10.0L;   // конец диапазона
    int P = 3;          // знаков после запятой
    
    printf("Генерация заданий...\n");
    printf("Студентов: %d\n", N);
    printf("Заданий на студента: %d\n", K);
    printf("Разрядность: %d бит\n", bits);
    printf("Диапазон: [%Lf, %Lf]\n", a, b);
    printf("Знаков после запятой: %d\n", P);
    printf("\n");
    
    // Создаем папки
    system("mkdir -p zadanie");
    system("mkdir -p proverka");
    
    // Для каждого студента
    for (int student = 1; student <= N; student++) {
        // Генерируем K случайных чисел
        long double nums[100];
        
        for (int i = 0; i < K; i++) {
            nums[i] = rand_num(a, b, P);
        }
        
        // Имена файлов
        char fname1[100];
        char fname2[100];
        
        sprintf(fname1, "zadanie/var%d.txt", student);
        sprintf(fname2, "proverka/var%d.txt", student);
        
        // Записываем таблицы
        write_table1(fname1, nums, K, student);
        write_table2(fname2, nums, K, student, bits);
        
        printf("Вариант %d готов\n", student);
    }
    
    printf("\nГотово!\n");
    printf("Папка 'zadanie' - задания для студентов\n");
    printf("Папка 'proverka' - ответы для проверки\n");
    
    return 0;
}

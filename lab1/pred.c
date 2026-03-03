#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// === ПРЕОБРАЗОВАНИЕ ЧИСЕЛ ===

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
unsigned int get32(double x) {
    un32 a;
    a.f = (float)x;
    return a.u;
}

// Получение машинного представления 64 бит
unsigned long long get64(double x) {
    un64 a;
    a.d = x;
    return a.u;
}

// Получение машинного представления 128 бит
unsigned __int128 get128(double x) {
    un128 a;
    a.ld = (long double)x;
    return a.u;
}

// Обратное преобразование из 32 бит
double back32(unsigned int x) {
    un32 a;
    a.u = x;
    return a.f;
}

// Обратное преобразование из 64 бит
double back64(unsigned long long x) {
    un64 a;
    a.u = x;
    return a.d;
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
double rand_num(double a, double b, int p) {
    double r = a + (double)rand() / RAND_MAX * (b - a);
    
    // Округление до P знаков
    double mult = pow(10, p);
    r = round(r * mult) / mult;
    
    return r;
}

// ============ ЗАПИСЬ В ФАЙЛЫ ============

// Запись таблицы 1 (только числа)
void write_table1(const char *fname, double *nums, int k, int v) {
    FILE *f = fopen(fname, "w");
    if (!f) return;
    
    fprintf(f, "ВАРИАНТ %d\n", v);
    fprintf(f, "================================\n\n");
    fprintf(f, "N\tВещественное число\n");
    fprintf(f, "-\t------------------\n");
    
    for (int i = 0; i < k; i++) {
        fprintf(f, "%d\t%.10f\n", i + 1, nums[i]);
    }
    
    fclose(f);
}

// Запись таблицы 2 (число, машинное представление, ошибка)
void write_table2(const char *fname, double *nums, int k, int v, int bits) {
    FILE *f = fopen(fname, "w");
    if (!f) return;
    
    fprintf(f, "ВАРИАНТ %d (%d бит)\n", v, bits);
    fprintf(f, "================================\n\n");
    fprintf(f, "N\tВещественное число\tМашинное представление\tОшибка\n");
    fprintf(f, "-\t------------------\t--------------------\t------\n");
    
    for (int i = 0; i < k; i++) {
        double err = 0.0;
        char mach[100] = "";
        
        if (bits == 32) {
            unsigned int m = get32(nums[i]);
            double back = back32(m);
            err = fabs(nums[i] - back);
            sprintf(mach, "0x%08X", m);
        }
        else if (bits == 64) {
            unsigned long long m = get64(nums[i]);
            double back = back64(m);
            err = fabs(nums[i] - back);
            sprintf(mach, "0x%016llX", m);
        }
        else if (bits == 128) {
            unsigned __int128 m = get128(nums[i]);
            long double back = back128(m);
            err = fabs(nums[i] - (double)back);
            print128(m, mach);
        }
        
        fprintf(f, "%d\t%.10f\t%s\t%e\n", i + 1, nums[i], mach, err);
    }
    
    fclose(f);
}

int main() {
    // Для случайных чисел
    srand(time(NULL));
    
    // Входные данные
    int N = 20;        // кол-во студентов
    int K = 5;         // кол-во заданий
    int bits = 128;    // разрядность (32, 64, 128)
    double a = -1000.0;  // начало диапазона
    double b = 1000.0;   // конец диапазона
    int P = 9;         // знаков после запятой
    
    printf("Генерация заданий...\n");
    printf("Студентов: %d\n", N);
    printf("Заданий на студента: %d\n", K);
    printf("Разрядность: %d бит\n", bits);
    printf("Диапазон: [%f, %f]\n", a, b);
    printf("Знаков после запятой: %d\n", P);
    printf("\n");
    
    // Создаем папки
    system("mkdir -p zadanie");
    system("mkdir -p proverka");
    
    // Для каждого студента
    for (int student = 1; student <= N; student++) {
        // Генерируем K случайных чисел
        double nums[100];
        
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

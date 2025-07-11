#include <stdio.h>

// Сортировка вставками: весь «тяжёлый» цикл — в inline-asm
void insertion_sort(int* a, int n) {
	register int* base asm("rdi") = a;  // Закрепляем указатель на массив в rdi
	register int len asm("rsi") = n;    // Закрепляем длину массива в rsi

	asm volatile(
		// Инициализация счетчика i = 1 (начинаем со второго элемента)
		"mov    $1, %%ecx\n"           // ecx = i = 1
		
		// Проверка условия выхода (если массив пустой или из одного элемента)
		"cmp    %%ecx, %%esi\n"         // Сравниваем i (ecx) с n (esi)
		"jle    9f\n"                   // Если i >= n, переход к метке 9 (конец)

		// Начало внешнего цикла (проход по массиву)
		"1:\n"                         // Метка начала внешнего цикла
		// Загрузка текущего элемента (key = a[i])
		"mov    (%%rdi, %%rcx, 4), %%eax\n"  // eax = a[i] (key)
		
		// Инициализация j = i-1
		"lea    -1(%%rcx), %%edx\n"     // edx = j = i - 1

		// Начало внутреннего цикла (поиск места для вставки)
		"2:\n"                         // Метка начала внутреннего цикла
		// Проверка границы массива (j >= 0)
		"cmp    $0, %%edx\n"           // Сравниваем j с 0
		"jl     4f\n"                  // Если j < 0, переход к вставке (метка 4)
		
		// Загрузка a[j] для сравнения
		"mov    (%%rdi, %%rdx, 4), %%r8d\n"  // r8d = a[j]
		
		// Сравнение a[j] и key
		"cmp    %%r8d, %%eax\n"        // Сравниваем key (eax) и a[j] (r8d)
		"jge    4f\n"                   // Если key >= a[j], место найдено - к вставке
		
		// Сдвиг элемента вправо (a[j+1] = a[j])
		"mov    %%r8d, 4(%%rdi, %%rdx, 4)\n"  // a[j+1] = a[j] (сдвиг)
		
		// Уменьшение j (j--)
		"dec    %%edx\n"               // j = j - 1
		"jmp    2b\n"                   // Повтор внутреннего цикла

		// Место для вставки key
		"4:\n"                         // Метка вставки
		// Коррекция индекса (j+1, так как мы могли выйти по j < 0)
		"inc    %%edx\n"               // j = j + 1
		// Вставка key на найденное место
		"mov    %%eax, (%%rdi, %%rdx, 4)\n"  // a[j] = key
		
		// Переход к следующему элементу (i++)
		"inc    %%ecx\n"                // i = i + 1
		// Проверка условия продолжения внешнего цикла
		"cmp    %%ecx, %%esi\n"         // Сравниваем i с n
		"jg     1b\n"                   // Если i < n, повторяем внешний цикл

		// Конец сортировки
		"9:\n"                         // Метка выхода
		: // Нет выходных операндов
		: // Входные операнды переданы через регистры
		: "rax", "rcx", "rdx", "r8", "cc", "memory"
	);
}

int main(){
	int arr[] = {30, 6, 1000, 5, 8, 100};
	int n = sizeof(arr) / sizeof(arr[0]);

	insertion_sort(arr, n);

	for(int i = 0; i < n; ++i){
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;
}
#ifndef LAB1_H_INCLUDED
#define LAB1_H_INCLUDED

#include <stdio.h>
#include <pthread.h>
#include <pthread.h>


// ************************* Константы ************************************

// Продолжительность жизни (измеряется в количествах сделанных переходов)
const int kLifeTime = 10;    

// Продолжительность голодания (измеряется в количествах сделанных переходов)
const int kStarvationTime = 5;  

// Размеры карты 
const unsigned int kMapSizeX = 4;
const unsigned int kMapSizeY = 4;

// ************************************************************************


// **************************** Описание объектов *************************

// Виды животных
typedef enum {ANIMAL_1, ANIMAL_2, ANIMAL_3, NONE} TypeAnimal;

// Передвижение
typedef enum {RIGHT, LEFT, UP, DOWN} Direction;

// Координаты
typedef struct {
    int x;
    int y;
} Coordinate;

// Атрибуты животного
typedef struct {
    Coordinate coord;       // Координаты
    TypeAnimal type;        // Тип животного
    int life_time;          // Время жизни
    int startvation_time;   // Время голодания
} AnimalAttributes;

// ************************************************************************


// *************************** Глобальные перменные ***********************

// База данных о животных
AnimalAttributes db_animals[16];

// Поле по которым перемещаются животные
int map[4][4];

// Мьютекс
pthread_mutex_t mutex;

// Указатель на файл
FILE *log_file;

// ************************************************************************


// *************************** Прототипы функций **************************

// Создание потоков
void CreateThread(int row, int column, TypeAnimal type);

// Генерация псевдослучайных чисел на определнном промежутке
int GetRandRangeInt(int min, int max);

// Функция работающая в отдельном потоке
void* Animal(void* atr);

// Вывод карты в консоль
void* PrintMap(void* arg);

// ************************************************************************


#endif /* LAB1_H_INCLUDED */
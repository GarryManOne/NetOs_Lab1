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
const unsigned int kMapSizeX = 8;
const unsigned int kMapSizeY = 8;

// ************************************************************************


// **************************** Описание объектов *************************

// Виды животных
typedef enum {ANIMAL_1, ANIMAL_2, ANIMAL_3} TypeAnimal;

// Передвижение
typedef enum {RIGHT, LEFT, UP, DOWN} Direction;

// Координаты
typedef struct {
    int x;
    int y;
} Coordinate;

// Атрибуты животного
typedef struct {
    Coordinate coord;
    TypeAnimal type;
    int life_time;
    int startvation_time;
    pthread_t thread_id;
} AnimalAttributes;

// Атрибуты карты
typedef struct {
    pthread_t thread_id;
    TypeAnimal type;
} MapAttributes;

// ************************************************************************


// *************************** Глобальные перменные ***********************

// Поле по которым перемещаются животные
MapAttributes** map;

// Мьютекс
pthread_mutex_t** mutexes;

// Указатель на файл
FILE *fp;

// ************************************************************************


// *************************** Прототипы функций **************************

void* Animal(void* atr);

// Удаление массива мьютексов
void DeleteArrayMutexes(pthread_mutex_t** array_mutexes, unsigned int row, unsigned int column);

// Инициализация мьютексов
void InitArrayMutexes(pthread_mutex_t** array_mutexes, unsigned int row, unsigned int column);

// Создание массива мьютексов
pthread_mutex_t** CreateArrayMutexes(unsigned int row, unsigned int column);

// Создание карты
MapAttributes** CreateMap(unsigned int row, unsigned int column);
// Открытие файла для записи в потоках
void OpenFile(char* fileName);

// Вывод карты в консоль
void PrintMap(MapAttributes*** map, unsigned int row, unsigned int column);

// Ожидание потоков
void CreateJoins(pthread_t* thread, unsigned int count_threads);

// ************************************************************************


#endif /* LAB1_H_INCLUDED */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <malloc.h>
#include "lab1.h"

// Генерация псевдослучайных чисел на определнном промежутке
int get_rand_range_int(int min, int max){
    return rand() % (max - min) + min;
}

// Функция работающая в отдельном потоке
void* Animal(void* atr){

    // Начальное состояние
    AnimalAttributes* attributes = (AnimalAttributes*) atr;

    while (1){
        // Смотрим продолжительность жизни 
        if (attributes->life_time == 0 || attributes->startvation_time == 0)
        {
            pthread_exit(NULL);
        }

        // Запоминаем координаты
        Coordinate coor_old = attributes->coord;

        // Случайное движение
        if (attributes->return_back == 1)
        {
            switch ((Direction)(get_rand_range_int(0, 3))){
                case RIGHT:
                    if ((attributes->coord.x + 1) < kMapSizeX){
                        attributes->coord.x += 1;
                    }
                    else continue;
                    break;
                case LEFT: 
                    if ((attributes->coord.x - 1) > 0){
                        attributes->coord.x -= 1;
                    }
                    else continue;
                    break;
                case UP: 
                    if ((attributes->coord.y - 1) > 0){
                    attributes->coord.y -= 1;
                    }
                    else continue;
                    break;
                case DOWN: 
                    if ((attributes->coord.y + 1) < kMapSizeY){
                        attributes->coord.y += 1;
                    }
                    else continue;
                    break;
                default:
                    break;
            }
        }
        else{
            attributes->return_back = 0;
        }

        fprintf(fp, "%u=%d [%d][%d]->[%d][%d]\n", pthread_self(), (char *)attributes->type, attributes->coord.x, attributes->coord.y, coor_old.x, coor_old.y);

        // Может мы уже умерли
        pthread_mutex_lock(&mutexes[coor_old.x][coor_old.y]);
        if (map[coor_old.x][coor_old.y].thread_id != attributes->thread_id)
        {
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&mutexes[coor_old.x][coor_old.y]);

         // Действие при переходе
        int coordX = attributes->coord.x;
        int coordY = attributes->coord.y;
        
        pthread_mutex_lock(&mutexes[coor_old.x][coor_old.y]);
        // Проверка какое животное находится в этой ячейке
        if (map[coordX][coordY].thread_id == 0){  
            
            // Спаривание :)
            if (attributes->type == map[coordX][coordY].type){
                pthread_t *thread_animal = (pthread_t*)malloc(sizeof(pthread_t));
                CreateThreads(thread_animal, 1, attributes->type);
                fprintf(fp, "%u -> + %u\n", pthread_self(), *thread_animal);
            }
            // Ест
            else if ((attributes->type + 1 ) % 3 == map[coordX][coordY].type){
                map[coordX][coordY].thread_id = attributes->thread_id;
                map[coordX][coordY].type = attributes->type;
                attributes->life_time -= 1;
                attributes->startvation_time -= 1;
                fprintf(fp, "%u -> x %u\n", pthread_self(), map[coordX][coordY].thread_id);
            }
            // Его едят
            else{
                fprintf(fp, "%u -> - %u\n", map[coor_old.x][coor_old.y].thread_id, map[coordX][coordY].thread_id);
                map[coor_old.x][coor_old.y].thread_id = 0;
                pthread_exit(NULL);
            }
            pthread_mutex_unlock(&mutexes[coor_old.x][coor_old.y]);
        }
        else
        {
            // Обнуление прошлой ячейки
            map[coor_old.x][coor_old.y].thread_id = 0;

            // Переход
            map[coordX][coordY].thread_id = attributes->thread_id;
            map[coordX][coordY].type = attributes->type;
            attributes->life_time -= 1;
            attributes->startvation_time -= 1;

        }
        pthread_mutex_unlock(&mutexes[coor_old.x][coor_old.y]);
    }
    
    return NULL;
}

// Создание потоков
void CreateThreads(pthread_t* threads, unsigned int count_threads, TypeAnimal type){

    for (int i = 0; i < count_threads; i++){
        AnimalAttributes *animal_attributes = (AnimalAttributes*)malloc(sizeof(AnimalAttributes));
        animal_attributes->type = type;                             // тип животного
        animal_attributes->life_time = kLifeTime;                   // время жизни
        animal_attributes->startvation_time = kStarvationTime;      // время голодания
        animal_attributes->thread_id = threads[i];                  // id потока
        animal_attributes->return_back = 0;                         // флаг перемещения назад

        int flag = 0;
        do
        {
            animal_attributes->coord.x = get_rand_range_int(0, kMapSizeX);
            animal_attributes->coord.y = get_rand_range_int(0, kMapSizeY);
            pthread_mutex_lock(&mutexes[animal_attributes->coord.x][animal_attributes->coord.y]);
            if (map[animal_attributes->coord.x][animal_attributes->coord.y].thread_id == 0){
                map[animal_attributes->coord.x][animal_attributes->coord.y].type = type;
                map[animal_attributes->coord.x][animal_attributes->coord.y].thread_id = threads[i]; 
            }
            else{
                flag = 1;
            }
            pthread_mutex_unlock(&mutexes[animal_attributes->coord.x][animal_attributes->coord.y]);
        } while ( flag == 0);

        pthread_create(&threads[i], NULL, &Animal, animal_attributes);

        // Вывод отладочной информации
        printf("[%d][%d] = %d\n", animal_attributes->coord.x, animal_attributes->coord.y, type);
        printf("type=%d l_t=%d s_t=%d [%d] [%d]\n", animal_attributes->type, animal_attributes->life_time, animal_attributes->startvation_time, animal_attributes->coord.x, animal_attributes->coord.y);
        // printf("%u %d\n", animal_coordinate->thread_id = tr[i], animal_coordinate->type);

    }
}

// Ожидание потоков
void CreateJoins(pthread_t* threads, unsigned int count_threads){
    for (int i = 0; i < count_threads; i++){
        pthread_join(threads[i], NULL);
    }
}

// Вывод карты в консоль
void PrintMap(MapAttributes*** map, unsigned int row, unsigned int column){
    for (int i = 0; i < row; i++){
            for (int j = 0; j < column; j++){
                if (map[i][j] != NULL){
                    switch (map[i][j]->type)
                    {
                        case ANIMAL_1:
                            printf("[0]");
                            break;
                        case ANIMAL_2:
                            printf("[1]");
                            break;
                        case ANIMAL_3:
                            printf("[2]");
                            break;           
                        default:
                            break;
                    }
                }
                else{
                    printf("[*]");
                }
            }
            printf("\n");
    }
}

// Открытие файла для записи в потоках
void OpenFile(char* fileName){
    if ((fp = fopen(fileName, "w")) == NULL){
        printf("Не удалось открыть файл");
        getchar();
    }
}

// Создание карты
MapAttributes** CreateMap(unsigned int row, unsigned int column){
    MapAttributes** map_attrubutes = (MapAttributes**)malloc(row * sizeof(MapAttributes*));

    for (int i = 0; i < row; i++){
        map_attrubutes[i] = (MapAttributes*)malloc(column * sizeof(MapAttributes));
    }

    return map_attrubutes;
}

// Создание массива мьютексов
pthread_mutex_t** CreateArrayMutexes(unsigned int row, unsigned int column){
    pthread_mutex_t** mutexes = (pthread_mutex_t**)malloc(row * sizeof(pthread_mutex_t*));

    for (int i = 0; i < row; i++){
        mutexes[i] = (pthread_mutex_t*)malloc(column * sizeof(pthread_mutex_t));
    }

    return mutexes;
}

// Инициализация мьютексов
void InitArrayMutexes(pthread_mutex_t** array_mutexes, unsigned int row, unsigned int column){
    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++){
            pthread_mutex_init(&array_mutexes[i][j], NULL);
        }
    }
}

// Удаление массива мьютексов
void DeleteArrayMutexes(pthread_mutex_t** array_mutexes, unsigned int row, unsigned int column){
for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++){
            pthread_mutex_destroy(&array_mutexes[i][j]);
        }
    }
}

// Главная функция
int main(int argc, char *argv[]){
    
    srand(time(NULL));
    setbuf(stdout, 0);

    // Проверка на количество параметров
    if (argc != 4){
        printf("Неверное количество параметров!\n");
        return 1;
    }

    // Вывод информационных данных в консоль
    int animal1Count = atoi(argv[1]);
    printf("TypeAnimal 1 type count: %d\n", animal1Count);

    int animal2Count = atoi(argv[2]);
    printf("TypeAnimal 2 type count: %d\n", animal2Count);

    int animal3Count = atoi(argv[3]);
    printf("TypeAnimal 3 type count: %d\n",animal3Count);

    // Проверка на максимальное количество потоков которые возможно создать
    int allAnimalsCount = animal1Count + animal2Count + animal3Count;
    if (allAnimalsCount > kMapSizeX * kMapSizeY){
        printf("Превышенно количество животных! Максимум: %d\n", kMapSizeX * kMapSizeY);
        return 1;
    }

    // Создание карты
    map = CreateMap(kMapSizeX, kMapSizeY);

    // Обнуление карты 
    for (int i = 0; i < kMapSizeX; i++){
        for (int j = 0; j < kMapSizeY; j++){
            map[i][j].thread_id = 0;
        }
    }

    // Создание и инициализация мьютексов
    mutexes = CreateArrayMutexes(kMapSizeX, kMapSizeY);
    InitArrayMutexes(mutexes, kMapSizeX, kMapSizeY);

    pthread_t animal_1[animal1Count];
    pthread_t animal_2[animal2Count];
    pthread_t animal_3[animal3Count];

    // Создание потоков
    CreateThreads(animal_1, animal1Count, ANIMAL_1);
    CreateThreads(animal_2, animal2Count, ANIMAL_2);
    CreateThreads(animal_3, animal3Count, ANIMAL_3);

    // Создание джойнов
    CreateJoins(animal_1, animal1Count);
    CreateJoins(animal_2, animal2Count);
    CreateJoins(animal_3, animal3Count);

    // Освобождение памяти
    DeleteArrayMutexes(mutexes, kMapSizeX, kMapSizeY);

    for (int i = 0; i < kMapSizeX; i++){
        free(map[i]);
    }
    free(map);

    return 0;
}
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
void* animal(void* atr){

    // Начальное состояние
    AnimalAttributes* attributes = (AnimalAttributes*) atr;

    while (1){
        // Смотрим продолжительность жизни 
        if (attributes->kLifeTime == 0 || attributes->kStarvationTime == 0)
        {
            pthread_exit(NULL);
        }

        // Запоминаем координаты
        Coordinate coor_old = attributes->coord;

        // Случайное движение 
        switch ((Direction)(get_rand_range_int(0, 3))){
            case RIGHT:
                if ((attributes->coord.x + 1) < kSizeX){
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
                if ((attributes->coord.y + 1) < kSizeY){
                    attributes->coord.y += 1;
                }
                else continue;
                break;
            default:
                break;
        }

        fprintf(fp, "%u=%d [%d][%d]->[%d][%d]\n", pthread_self(), (char *)attributes->type, attributes->coord.x, attributes->coord.y, coor_old.x, coor_old.y);

        // Действие при переходе
        int coordX = attributes->coord.x;
        int coordY = attributes->coord.y;

        pthread_mutex_lock(&lock_field); // блокировка мьютекса

        if (actionField[coor_old.x][coor_old.y]->dead == 1)
        {
            pthread_exit(NULL);
        }

        // Проверка какое животное находится в этой ячейке
        if (actionField[coordX][coordY] != NULL){  
            
            // Спаривание :)
            if (attributes->type == actionField[coordX][coordY]->type){
                pthread_t *thread_animal = malloc(sizeof(pthread_t));
                // printf("%u породил %u\n", pthread_self(), *thread_animal);
                fprintf(fp, "%u -> + %u\n", pthread_self(), *thread_animal);
                create_threads(thread_animal, 1, attributes->type);
                pthread_mutex_unlock(&lock_field); // разблокировка мьютекса
                continue;
            }
            // Ест
            else if ((attributes->type + 1 ) % 3 == actionField[coordX][coordY]->type){
                
                // printf("%u съел %u\n", pthread_self(), actionField[coordX][coordY]->thread_id);
                fprintf(fp, "%u -> x %u\n", pthread_self(), actionField[coordX][coordY]->thread_id);
                actionField[coordX][coordY]->dead = 1;

                AnimalCoordinate *an_coord = malloc(sizeof(AnimalCoordinate));
                an_coord->thread_id = actionField[coor_old.x][coor_old.y]->thread_id;
                an_coord->type = attributes->type;

                actionField[coor_old.x][coor_old.y] = NULL;

                actionField[coordX][coordY] = an_coord;
            }
            // Его едят
            else{
                // printf("%u умер от %u\n", actionField[coor_old.x][coor_old.y]->thread_id, actionField[coordX][coordY]->thread_id);
                fprintf(fp, "%u -> - %u\n", actionField[coor_old.x][coor_old.y]->thread_id, actionField[coordX][coordY]->thread_id);
                actionField[coor_old.x][coor_old.y] = NULL;
                pthread_exit(NULL);
            }
        }
        else
        {
            AnimalCoordinate *an_coord = malloc(sizeof(AnimalCoordinate));

            an_coord->thread_id = actionField[coor_old.x][coor_old.y]->thread_id;
            an_coord->type = attributes->type;

            actionField[coor_old.x][coor_old.y] = NULL;
            actionField[coordX][coordY] = an_coord;
        }
        pthread_mutex_unlock(&lock_field);
    }
    
    return NULL;
}

// Создание потоков
void create_threads(pthread_t tr[],int count, TypeAnimal type){

    for (int i = 0; i < count; i++){
        AnimalAttributes *animal_attributes = malloc(sizeof(AnimalAttributes));
        animal_attributes->type = type;                             // тип животного
        animal_attributes->life_time = kLifeTime;                   // время жизни
        animal_attributes->startvation_time = kStarvationTime;      // время голодания

        MapAttributes *map_attributes = malloc(sizeof(MapAttributes));
        map_attributes->thread_id = tr[i];                          // id потока
        map_attributes->type = type;                                // тип животного
        map_attributes->dead = 0;                                   // состояние

        // pthread_mutex_unlock(&lock_field); // блокирвка мьютекса

        // Выбор случайного положения
        do
        {
            animal_attributes->coord.x = get_rand_range_int(0, kMapSizeX);
            animal_attributes->coord.y = get_rand_range_int(0, kMapSizeY);

        } while (actionField[animal_attributes->coord.x][animal_attributes->coord.y] != NULL);

        actionField[animal_atr->coord.x][animal_atr->coord.y] = animal_coordinate;

        pthread_mutex_unlock(&lock_field); // разблокировка мьютекса

        pthread_create(&tr[i], NULL, &animal, animal_atr);

        // Вывод отладочной информации
        printf("[%d][%d] = %d\n", animal_atr->coord.x, animal_atr->coord.y, type);
        printf("type=%d l_t=%d s_t=%d [%d] [%d]\n", animal_atr->type, animal_atr->kLifeTime, animal_atr->kStarvationTime, animal_atr->coord.x, animal_atr->coord.y);
        printf("%u %d\n", animal_coordinate->thread_id = tr[i], animal_coordinate->type);

    }
}

void create_join(pthread_t tr[],int count){
    for (int i = 0; i < count; i++){
        pthread_join(tr[i], NULL);
    }
}

// Открытие файла для записи в потоках
void open_file(char* fileName){
    
    if ((fp = fopen(fileName, "w")) == NULL)
    {
        printf("Не удалось открыть файл");
        getchar();
    }

}

// Создание массива мьютексов
pthread_mutex_t** create_array_mutexes(unsigned int row, unsigned int column){
    pthread_mutex_t** mutexes = (pthread_mutex_t**)malloc(row * sizeof(pthread_mutex_t*));

    for (int i = 0; i < column; i++)
    {
        mutexes[i] = (pthread_mutex_t*)malloc(column * sizeof(pthread_mutex_t));
    }
     
    return mutexes;
}

// Инициализация мьютексов
void init_array_mutexes(pthread_mutex_t** array_mutexes, unsigned int row, unsigned int column){
    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++){
            pthread_mutex_init(&array_mutexes[i][j], NULL);
        }
    }
}

// Удаление массива мьютексов
void delete_array_mutexes(pthread_mutex_t** array_mutexes, unsigned int row, unsigned int column){
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

    // Обнуление карты 
    for (int i = 0; i < kMapSizeX; i++){
        for (int j = 0; j < kMapSizeY; j++){
            actionField[i][j] = NULL;
        }
    }

    // Создание и инициализация мьютексов
    mutexes = create_array_mutexes(kMapSizeX, kMapSizeY);
    init_array_mutexes(mutexes, kMapSizeX, kMapSizeY);

    // // Инициализация мьютекса
    // pthread_mutex_init(&lock_field, NULL);

    // Создание потоков
    pthread_t animal_1[animal1Count];
    pthread_t animal_2[animal2Count];
    pthread_t animal_3[animal3Count];

    create_threads(animal_1, animal1Count, ANIMAL_1);
    create_threads(animal_2, animal2Count, ANIMAL_2);
    create_threads(animal_3, animal3Count, ANIMAL_3);

    // pthread_create(&print, NULL, &print_field, NULL);
    // pthread_cond_broadcast(&stack_cond);

    // create_join(animal_1,animal1Count);
    // create_join(animal_2,animal2Count);
    // create_join(animal_3,animal3Count);

    // Вывод отладочной информации
    for (int i=0; i < kSizeX; i++){
        for (int j=0; j < kSizeY; j++)
        {
            if (actionField[i][j] != NULL)
            {
                switch (actionField[i][j]->type)
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
    
    for (int i = 0; i < animal1Count; i++)
    {
        pthread_join(animal_1[i], NULL);
    }

    for (int i = 0; i < animal2Count; i++)
    {
        pthread_join(animal_2[i], NULL);
    }

    for (int i = 0; i < animal1Count; i++)
    {
        pthread_join(animal_2[i], NULL);
    }

    pthread_mutex_destroy(&lock_field);

    return 0;
}




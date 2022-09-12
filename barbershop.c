/*
Implemented barbershop problem with three semaphores and maximal number of 10 threads(customers) and one thread for the barber,
When there is no customers, the barber sleeps, when there is more than maximal number of customers, that is 4 (three in the waiting room and one in barber`s stool)
the customer thread invokes void function balks(). Ideally the array for customer threads should be dinamically alocated on heap, but I worked with an assumption
that only ten people can enter the shop at a time and only for can get their new haircut :)
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// predefined number of max customers
#define MAX_NUMBER_OF_CUSTOMERS 4
#define MAX_NUMB_OF_THREADS 10
// number of customers at the barbershop currently
int numberOfCustomers = 0;
// counter for decrementing
// int counter=MAX_NUMBER_OF_CUSTOMERS;
// number of seats in the waiting Room
int numberOfSeats = 4;
// needed semaphores
sem_t customersSemaphore;
sem_t barberSemaphore;
sem_t barbersStool;

// threads needed

pthread_t barber;
pthread_t customers[MAX_NUMB_OF_THREADS];

// function prototypes
void balk();
void getHairCut();
void cutHair();
void *barberFunc(void *arg);
void *customersFunc(void *arg);

void balk()
{
    printf("\nThe barber is busy, customer leaves the shop!\n");
    numberOfCustomers--;
}

void cutHair()
{
    int timeNeededForAtrim = rand() % 6 + 1;

    sem_wait(&barbersStool);
    numberOfCustomers--;
    sem_post(&barbersStool);

    printf("\nThe customer is getting a trim which lasts %d minutes(s)\n", timeNeededForAtrim);
    sleep(timeNeededForAtrim); // function sleep from stdio.h, simulates the action of trimming a haircut
}

void getHairCut()
{
    printf("The customer got his new Haircut, and is leaving the shop :D\n\n");
}

void *barberFunc(void *arg)
{
    while (numberOfCustomers > 0)
    {
        sem_wait(&barberSemaphore);
        sem_post(&customersSemaphore);
        cutHair();
    }
    pthread_exit(NULL);
}

void *customersFunc(void *args)
{
    if (numberOfCustomers > numberOfSeats)
    {
        balk();
    }
    else
    {
        sem_wait(&barbersStool); // made like a semaphore but it works almost like mutex
        if (numberOfCustomers == 0)
        {
          printf("No more customers at the shop the barber can go to sleep!\n");
            exit(0);
        }

        sem_post(&barbersStool);
        printf("The customer  takes a seat at the waiting room, currently %d place(s) are taken in the waiting room", numberOfCustomers);

        sem_post(&barberSemaphore);
        sem_wait(&customersSemaphore);
        getHairCut();
    }
    pthread_exit(NULL);
}

int main()
{
    int exedingNumbOfCustomers = 0;
    sem_init(&customersSemaphore, 0, 0);
    sem_init(&barberSemaphore, 0, 0);
    sem_init(&barbersStool, 0, 1);

    printf("Enter the number of customers in the shop:\n");
    scanf("%d", &numberOfCustomers);
    if (numberOfCustomers > MAX_NUMBER_OF_CUSTOMERS)
        exedingNumbOfCustomers = numberOfCustomers - MAX_NUMBER_OF_CUSTOMERS;

    if (pthread_create(&barber, NULL, &barberFunc, NULL) != 0)
        perror("Barber thread not created successfully!\n");

    if (numberOfCustomers <= MAX_NUMBER_OF_CUSTOMERS)
    {

        for (int i = 0; i < MAX_NUMBER_OF_CUSTOMERS; i++)
        {
            if (pthread_create(&customers[i], NULL, &customersFunc, NULL) != 0)
                perror("Customer threads not created successfully !\n");

            sleep(3);
        }
    }
    else
    {
        exedingNumbOfCustomers += MAX_NUMBER_OF_CUSTOMERS;

        for (int i = 0; i < exedingNumbOfCustomers; i++)
        {
            if (pthread_create(&customers[i], NULL, &customersFunc, NULL) != 0)
                perror("Customer threads not created successfully !\n");

            sleep(3);
        }
    }

    if (pthread_join(barber, NULL) != 0)
        perror("Barber thread not joined successfuly!\n");

    if (numberOfCustomers <= MAX_NUMBER_OF_CUSTOMERS)
    {
        for (int i = 0; i < MAX_NUMBER_OF_CUSTOMERS; i++)
        {
            if (pthread_join(customers[i], NULL) != 0)
                perror("Customer threads not joined successfully!\n");
        }
    }
    else
    {
        exedingNumbOfCustomers += MAX_NUMBER_OF_CUSTOMERS;
        for (int i = 0; i < exedingNumbOfCustomers; i++)
        {
            if (pthread_join(customers[i], NULL) != 0)
                perror("Customer threads not joined successfully!\n");
        }
    }

    sem_destroy(&customersSemaphore);
    sem_destroy(&barberSemaphore);
    sem_destroy(&barbersStool);

    printf("\n\n End of the programme :D \n\n");
    return 0;
}

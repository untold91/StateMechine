/**************************************************************************************************************************/
/* C SOURCE FILE                                                                                                          */
/**************************************************************************************************************************/
/* Project    : State Machine Template
 * File Name  : stateMachine.c
 * Author     : Dhanush.K (dhanushsandy98@gmail.com)
 * Date       : 10/11/2021
 * 
 * Revised on : 11/11/2021
 * Version    : V0.0.3
 * 
 * Brief      : Sample Template for Single State Machine Approach. Event will receive from user or a internal process, 
 *              based on the events State Machine will respond and change to the next state.
 * 
 * Reference  : -> https://en.wikipedia.org/wiki/Finite-state_machine
 * 
 */
/**************************************************************************************************************************/
/* Licence    : MIT
 * Copyright (c) 2021 Dhanush.K
 */
/**************************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h> // sleep lib
#include <pthread.h>

/**************************************************************************************************************************/
/* Macro Declarations                                                                                                     */
/**************************************************************************************************************************/

#define ZERO          0
#define ONE           1
#define TWO           2

#define OK            0

#define MAX_QEVENT   16
#define NUL         255

/**************************************************************************************************************************/
/* Typedef, Enums, Structures and Union Declarations                                                                      */
/**************************************************************************************************************************/

/**
 * @brief App State Enum Container.
 * 
 */
typedef enum _STATE_APP_
{
    STATE_APP_STARTUP,
    STATE_APP_IDLE,
    STATE_APP_LEARN_MODE,
    STATE_APP_PROCESS_MODE,
} STATE_APP;

/**
 * @brief App Event enum Container.
 * 
 */
typedef enum EVENT_APP_
{
    EVENT_EMPTY,
    EVENT_APP_LEARN_MODE,
    EVENT_APP_PROCESS_MODE,
    EVENT_APP_FINISH,
    EVENT_EXIT,
} EVENT_APP;

/**************************************************************************************************************************/
/* Global Variable Declarations                                                                                           */
/**************************************************************************************************************************/

/**
 * @brief Event Queue Base Array, Holds the input events. Size of the array based on 'MAX_QEVENT' Macro.
 * Default = 16.
 * 
 */
static uint8_t appEventQBase[MAX_QEVENT] = {NUL};

/**
 * @brief Current State Holder.
 * 
 */
static STATE_APP currentState = STATE_APP_STARTUP;

/**
 * @brief Mutual exclusion for our threads
 * 
 */
pthread_mutex_t mutexsum = PTHREAD_MUTEX_INITIALIZER;

/**************************************************************************************************************************/
/* Function Declarations                                                                                                  */
/**************************************************************************************************************************/

void AppStateManager(EVENT_APP event);
static void ChangeState(STATE_APP newState);

bool xEventNotify(uint8_t event);
bool xEventQueueReceive(uint8_t *event);

void *userProcess();
void *appEventHandler();

/**************************************************************************************************************************/
/* Main Program                                                                                                           */
/**************************************************************************************************************************/

int main(void)
{
    /**
     * @brief Program Exit Status Flag.
     * 
     */
    uint8_t done = false;

    /* Create threads
     * Thread 1: Takes the Input from User.
     * Thread 2: Process the New Events.
     */

    // Set up threads
    pthread_t threads[TWO];
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Pointer for exit status flag variable.
    void *ptr = &done;

    // Run the sender and receiver threads
    pthread_create(&threads[ZERO], &attr, userProcess,     ptr );
    pthread_create(&threads[ONE ], &attr, appEventHandler, NULL);

    // Wait until done is TRUE then exit program.
    while (!done);

    return OK;
}

/**************************************************************************************************************************/
/* Function Definations                                                                                                   */
/**************************************************************************************************************************/

/**
 * @brief App State Manger
 * 
 * @param event 
 */
void AppStateManager(EVENT_APP event)
{
    printf("AppStateManager St: %d, Ev: %d\r\n", currentState, event);

    switch (currentState)
    {
    case STATE_APP_STARTUP:
        printf("APP Startup\n");
        ChangeState(STATE_APP_IDLE);
        printf("Change Start to Idle\n");
        break;

    case STATE_APP_IDLE:
        if (EVENT_APP_LEARN_MODE == event)
        {
            printf("Change Start to Learn Mode\n");
            sleep(5);
            ChangeState(STATE_APP_LEARN_MODE);
        }
        break;

    case STATE_APP_LEARN_MODE:
        if (EVENT_APP_PROCESS_MODE == event)
        {
            printf("Change Start to Process Mode\n");
            xEventNotify(EVENT_APP_FINISH);
            sleep(5);
            ChangeState(STATE_APP_PROCESS_MODE);
        }
        break;

    case STATE_APP_PROCESS_MODE:
        if (EVENT_APP_FINISH == event)
        {
            printf("Finished\n");
            ChangeState(STATE_APP_IDLE);
            printf("Change Start to Idle\n");
        }
        break;

    default:
        // Do nothing.
        break;
    }
}

/**
 * @brief Change the Process from one state to another.
 * 
 * @param newState 
 */
static void ChangeState(STATE_APP newState)
{
    printf("State changed from %d to %d\r\n", currentState, newState);

    currentState = newState;
}

/**
 * @brief Notify the Events to events Queue.
 * 
 * @param event 
 * @return true 
 * @return false 
 */
bool xEventNotify(uint8_t event)
{
    for (size_t i = ZERO; i < MAX_QEVENT; i++)
    {
        if (appEventQBase[i] == NUL)
        {
            appEventQBase[i] = event;
            if (i + ONE <= MAX_QEVENT)
                appEventQBase[i + ONE] = NUL;
            break;
        }
        else
            continue;
    }
    return true;
}

/**
 * @brief Insert the Event to Events Queue.
 * 
 * @param event 
 * @return true 
 * @return false 
 */
bool xEventQueueReceive(uint8_t *event)
{
    if (appEventQBase[ZERO] != NUL)
    {
        *event = appEventQBase[ZERO];
        for (size_t i = ONE; i < MAX_QEVENT; i++)
        {
            if (appEventQBase[i] != NUL)
            {
                appEventQBase[i - ONE] = appEventQBase[i];
            }
            else
            {
                appEventQBase[i - ONE] = NUL;
                break;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Event Handler, Which will invoke the State machine when there is a new event in the queue.
 * 
 * @return void* 
 */
void *appEventHandler()
{
    uint8_t event;

    AppStateManager(EVENT_EMPTY);   // Startup the Application.

    while (true)
    {
        // Push the events to the sate manager with it is avalibale in QBase
        if (xEventQueueReceive((uint8_t *)(&event)) == true)
        {
            printf("Event: %d\r\n", event);
            AppStateManager((EVENT_APP)event);
        }
    }
}

/**
 * @brief User Events, Receive new events from user and notify the Event Queue Base.
 * 
 * @return void* 
 */
void *userProcess(void *argv)
{
    uint8_t *done = (uint8_t *)argv;
    while (true)
    {
        uint8_t event;

        // Take the Event from User Process
        scanf("%hhd", &event);

        // Check the Event Exit or not
        if (event == EVENT_EXIT)
        {
            perror("Exiting the Application");
            *done = true;
            pthread_mutex_destroy(&mutexsum);
            pthread_exit(NULL);
        }
        else if(*done == false)
        {
            xEventNotify(event);
            pthread_mutex_unlock(&mutexsum);
        }
    }
}

/**************************************************************************************************************************/
/* End of File                                                                                                            */
/**************************************************************************************************************************/
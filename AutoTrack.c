#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "tracker4.h"

#define MAX_LIMIT 20

int main(void) {
    Vehicle currentVehicle;
    welcome();
    
    while (1) {
        mainMenu();
        int selection = getMenuOption();
        switch (selection) {
            case 1: 
                printf("Select\n\n");
                listVehicle();
                selection = getMenuOption();
                currentVehicle = selectVehicle(selection);
                printf("You have selected the %i %s %s\n\n", currentVehicle.year, currentVehicle.make, currentVehicle.model);
                vehicleOptions();
                selection = getMenuOption();
                switch(selection) {
                    case 1:
                        printf("Viewing Vehicle information.\n\n");
                        vehicleInformation(&currentVehicle);
                        break;
                    case 2:
                        printf("View Upcoming Maintenance.\n\n");
                        break;
                    case 3:
                        printf("Updating Mileage\n\n");
                        updateMileage(&currentVehicle);
                        break;
                    case 4:
                        printf("View Maintenance Schedule\n\n");
                        viewMaintSched(&currentVehicle);
                        break;
                    case 5:
                        printf("View Mainenance Logs\n\n");
                        break;
                    case 6:
                        printf("Adding a task\n\n");
                        addTask(&currentVehicle);
                        break;
                    case 7:
                        printf("Returning to main menu.\n\n");
                        break;
                    default:
                        printf("Invalid selection\n\n");
                        break;
                }
                break;
            case 2:
                buildVehicle();
                break;
            case 3:
                printf("Delete\n");
                listVehicle();
                selection = getMenuOption();
                deleteVehicle(selection);
                break;
             case 4:
                printf("Goodbye!\n\n");
                exit(0);
        }
    }

    return 0;
}

void welcome() {
    printf("Welcome to AutoTrack, vehicle maintenance tracking software\n");
    printf("Written by Chris Broman, 2022\n\n");
}

void mainMenu() {
    printf("***************************  Main Menu  *************************************\n");
    printf("Please select from the following list of options: \n\n");
    printf("1. Select Vehicle                       2. Create New Vehicle Profile\n");
    printf("3. Delete Vehicle                       4. Exit Program\n\n>> ");
}

int getMenuOption() {
    int select;
    scanf("%i", &select);
    printf("\n");
    return select;
}

void buildVehicle() {
    for (int i = 0; i < 999; i++) {
        char name[11];
        char index[3];
        sprintf(index, "%d", i);
        strcpy(name, "vehicle");
        strcat(name, index);
        if (access(name, F_OK) == 0) {
            continue;
        } else {
            FILE *outfile = fopen(name, "w");
            if (outfile == NULL) {
                fprintf(stderr, "Error opening file\n");
                exit(1);
            }
            
            Vehicle vehicle;
            strcpy(vehicle.ident, name);
            printf("Vehicle Year: ");
            int year;
            scanf("%i", &year);
            vehicle.year = year;

            printf("Make: ");
            char make[20];
            scanf("%s", make);
            strcpy(vehicle.make, make);

            printf("Model: ");
            char model[30];
            scanf("%s", model);
            strcpy(vehicle.model, model);

            printf("Mileage: ");
            int mileage;
            scanf("%i", &mileage);
            vehicle.mileage = mileage;

            for (int i = 0; i < 99; i++) {
                vehicle.task[i].exists = false;
                strcpy(vehicle.task[i].job, "");
                vehicle.task[i].interval = 0;
                vehicle.task[i].lastDone = 0;
            }

            fwrite(&vehicle, sizeof(vehicle), 1, outfile);
            fclose(outfile);
            break;
        }
    }
} 

void listVehicle(){
    for (int i = 0; i < 999; i++) {
        char name[20];
        char index[30];
        strcpy(name, "vehicle");
        sprintf(index, "%d", i);
        strcat(name, index);
        Vehicle vehicle;
        if (access(name, F_OK) == 0) {
            FILE* infile = fopen(name, "r");
            if (infile == NULL) {
                fprintf(stderr, "\nError loading vehicle.\n");
                return;
            }
            while(fread(&vehicle, sizeof(vehicle), 1, infile));
            fclose(infile);
            printf("%i. %i %s %s\n", i, vehicle.year, vehicle.make, vehicle.model);
        } 
    }
    printf("\n");
}

void deleteVehicle(int selection) {
    char name[20];
    char index[3];
    strcpy(name, "vehicle");
    sprintf(index, "%d", selection);
    strcat(name, index);
    remove(name);
}

Vehicle selectVehicle(int selection) {
    Vehicle selectedVehicle;
    char name[20];
    char index[3];
    strcpy(name, "vehicle");
    sprintf(index, "%d", selection);
    strcat(name, index);

    FILE* infile = fopen(name, "r");
    if (infile == NULL) {
        fprintf(stderr, "Error Loading Vehicle.\n");
        exit(1);
    }
    while(fread(&selectedVehicle, sizeof(selectedVehicle), 1, infile));
    fclose(infile);
    return selectedVehicle;
}

void vehicleOptions() {
    printf("Please select from the following list of options:\n\n");
    printf("1. View Vehicle Information                 2. View Upcoming Maintenance\n");
    printf("3. Update Mileage                           4. View Maintenance Schedule\n");
    printf("5. View Maintenance Log                     6. Add Maintenance Task\n\n>> ");
}

void updateMileage(Vehicle* vehicle) {
    printf("Please enter the current odometer reading: ");
    int mileage;
    scanf("%i", &mileage);
    vehicle->mileage = mileage;

    FILE* outfile = fopen(vehicle->ident, "w");
    if (outfile == NULL) {
        fprintf(stderr, "Error\n");
        exit(1);
    }
    fwrite(vehicle, sizeof(Vehicle), 1, outfile);
    fclose(outfile);

    printf("Current odometer reading for %i %s %s is %i km\n\n", vehicle->year, vehicle->make, vehicle->model, vehicle->mileage);
    
}

void vehicleInformation(Vehicle* vehicle) {
    printf("Information for %i %s %s\n\n", vehicle->year, vehicle->make, vehicle->model);
    printf("Make:                               %s\n", vehicle->make);
    printf("Model:                              %s\n", vehicle->model);
    printf("Year                                %i\n", vehicle->year);
    printf("Odometer Reading:                   %i\n", vehicle->mileage);
    printf("VIN number:                         ####\n");
    printf("\nNext maintenance item due:        ##########\n");
    printf("\n");
}

void addTask(Vehicle *vehicle) {
    char job[30];
    int interval;
    int lastDone;
    int n = 0;
    Maintenance newTask;

    for (int i = 0; i < 99; i++) {
        if (vehicle->task[i].exists == true) {
            n++;
        } 
    }
   
    printf("Task Name: ");
    getchar();
    fgets(job, 30, stdin);
    strcpy(newTask.job, job);
    printf("Interval: ");
    scanf("%i", &interval);
    newTask.interval = interval;
    printf("Last Completed: ");
    scanf("%i", &lastDone);
    newTask.lastDone = lastDone;

    vehicle->task[n] = newTask;

    FILE* outfile = fopen(vehicle->ident, "w");
    if (outfile == NULL) {
        fprintf(stderr, "Error\n");
    }
    fwrite(vehicle, sizeof(vehicle), 1, outfile);
    fclose(outfile);
}

void viewMaintSched(Vehicle* vehicle) {
    printf("%i %s %s\n\n", vehicle->year, vehicle->make, vehicle->model);
}
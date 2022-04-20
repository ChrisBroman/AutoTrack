#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include "AutoTrack.h"

#define BUFFER_SIZE 64

int main(void) {
    Vehicle garage[99] = {0};
    int select_vehicle;
    int numVehicles;
    initializeGarage(garage);
    welcome();
    while(1) {
        mainMenu();
        numVehicles = getNumVehicles() - 1;
        int selection = getMenuOption(4);
        switch (selection) {
            case 1:
                listVehicle();
                select_vehicle = getMenuOption(numVehicles);
                currentOption(select_vehicle);
                vehicleOptions();
                int select = getMenuOption(9);
                switch (select) {
                    case 1:
                        vehicleInformation(select_vehicle);
                        break;
                    case 2:
                        upcomingMaint(select_vehicle);
                        break;
                    case 3:
                        updateMileage(select_vehicle);
                        break;
                    case 4:
                        viewSchedule(select_vehicle);
                        break;
                    case 5:
                        maintenanceLog(select_vehicle);
                        break;
                    case 6:
                        addTask(select_vehicle);
                        break;
                    case 7:
                        completeMaintenance(select_vehicle);
                        break;
                    case 8:
                        removeTask(select_vehicle);
                        break;
                    default:
                        break;
                }                 
                break;
            case 2:
                buildVehicle();
                break;
            case 3:
                printf("Select a vehicle to delete: \n\n");
                listVehicle();
                select_vehicle = getMenuOption(numVehicles);
                deleteVehicle(select_vehicle);
                break;
            case 4:
                printf("\nGoodbye!\n");
                exit(0);
        }
    }
    return 0;
}

void initializeGarage(Vehicle* vehicle) {
    if (access("garage.dat", F_OK) != 0) {
        FILE* outfile = fopen("garage.dat", "w");
        if (outfile == NULL) {
            printf("Initialize garage error\n");
            exit(1);
        }
        fwrite(vehicle, sizeof(vehicle), 1, outfile);
        fclose(outfile);
    }
}

bool parse_int(char *string, int *integer) {
    int i = 0;
    while (isspace(string[i])) i++;
    int length = strlen(string);
    if (i == length) return false;
    char integer_buffer[BUFFER_SIZE];
    int integer_chars = 0;

    if (string[i] == '-') {
        integer_buffer[integer_chars] = '-';
        integer_chars++;
        i++;
        if (!isdigit(string[i])) return false;
    }

    while(i < length && !isspace(string[i])) {
        if (!isdigit(string[i])) return false;
        integer_buffer[integer_chars] = string[i];
        integer_chars++;
        i++;
    }
    
    integer_buffer[integer_chars] = '\0';
    while (isspace(string[i])) i++;
    if (string[i] != '\0') return false;
    *integer = atoi(integer_buffer);
    return true;
}

int getNumVehicles() {
    int number = 0;
    Vehicle loadedVehicles[99];

    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) exit(1);
    while(fread(loadedVehicles, sizeof(loadedVehicles), 1, infile));
    fclose(infile);

    for (int i = 0; i < 99; i++) {
        if (loadedVehicles[i].exists == true) {
            number++;
        }
    }
    return number;
}

void welcome() {
    system("clear");
    printf("Welcome to AutoTrack, vehicle maintenance tracking software\n");
    printf("Written by Chris Broman, 2022\n\n");
}

void mainMenu() {
    printf("***************************  Main Menu  *************************************\n");
    printf("Please select from the following list of options: \n\n");
    printf("1. Select Vehicle                       2. Create New Vehicle Profile\n");
    printf("3. Delete Vehicle                       4. Exit Program\n\n");
}

int getMenuOption(int max) {
    int integer = 0;
    bool parsed_correct = true;
    do {
        printf("\n>> ");
        char buffer[BUFFER_SIZE];
        fgets(buffer, BUFFER_SIZE, stdin);
        parsed_correct = parse_int(buffer, &integer);
        printf("\n\nPlease select from the available options.\n\n");

         
    } while (!parsed_correct || integer > max);
    return integer;   
}

void currentOption(int select) {
    Vehicle loadedVehicle[99];
    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) {
        fprintf(stderr, "Error opening file.\n");
        exit(1);
    }
    while(fread(loadedVehicle, sizeof(loadedVehicle), 1, infile));
    fclose(infile);


    printf("Please select an option for %i %s %s: \n\n", loadedVehicle[select].year, loadedVehicle[select].make, loadedVehicle[select].model);

}

void listVehicle() {
    Vehicle vehicle[99];
    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) exit(1);    
    while(fread(vehicle, sizeof(vehicle), 1, infile));
    fclose(infile);
    printf("\n");
    for (int i = 0; i < 99; i++) {
        if (vehicle[i].exists == true) {
            printf("%i. %i %s %s\n", i, vehicle[i].year, vehicle[i].make, vehicle[i].model);
        }      
    }
    printf("\n");

}

void buildVehicle() {
    system("clear");
    Vehicle newVehicle = {0};
    Vehicle loadedVehicle[99];
    
    printf("Vehicle Year: ");
    int year;
    scanf("%i", &year);
    newVehicle.year = year;

    printf("Make: ");
    char make[20];
    scanf("%s", make);
    strcpy(newVehicle.make, make);

    printf("Model: ");
    char model[30];
    scanf("%s", model);
    strcpy(newVehicle.model, model);

    printf("Mileage (km): ");
    int mileage;
    scanf("%i", &mileage);
    newVehicle.mileage = mileage;

    newVehicle.exists = true;
    newVehicle.totalCost = 0.00;
    newVehicle.schedule->nextDue = newVehicle.schedule->lastDone = newVehicle.schedule->interval;
    newVehicle.schedule->dueIn = newVehicle.schedule->nextDue - newVehicle.mileage;

    for (int i = 0; i < 999; i++) {
        newVehicle.log[i].exists = false;
        newVehicle.log[i].cost = 0.00;
        newVehicle.log[i].mileage = 0;
        strcpy(newVehicle.log[i].date, "");
        strcpy(newVehicle.log[i].task, "");
    }


    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) {
        printf("Infile error\n");
        exit(1);
    }
    while(fread(loadedVehicle, sizeof(loadedVehicle), 1, infile));
    fclose(infile);

    for (int i = 0; i < 99; i++) {
        if (loadedVehicle[i].exists == true) {
            continue;
        } else {
            loadedVehicle[i] = newVehicle;
            break;
        }
    }

    FILE* outfile = fopen("garage.dat", "w");
    if (outfile == NULL) {
        printf("Outfile Error\n");
        exit(1);
    }
    fwrite(loadedVehicle, sizeof(loadedVehicle), 1, outfile);
    fclose(outfile);
}

Vehicle selectVehicle(int selection) {
    system("clear");
    Vehicle loadedVehicle[99];
    Vehicle selectedVehicle = {0};

    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) {
        printf("Infile error\n");
        exit(1);
    }
    while(fread(loadedVehicle, sizeof(loadedVehicle), 1, infile));
    fclose(infile);

    selectedVehicle = loadedVehicle[selection];
    return selectedVehicle;
}

void vehicleOptions() {
    
    printf("1. View Vehicle Information                 2. View Upcoming Maintenance\n");
    printf("3. Update Mileage                           4. View Maintenance Schedule\n");
    printf("5. View Maintenance Log                     6. Add Maintenance Task\n");
    printf("7. Complete Maintenance Task                8. Remove Maintenance Task\n");
    printf("9. Main Menu\n\n");
}

void vehicleInformation(int select) {
    system("clear");
    Vehicle loadedVehicle[99];
    Vehicle vehicle;

    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) exit(1);
    while(fread(loadedVehicle, sizeof(loadedVehicle), 1, infile));
    fclose(infile);

    vehicle = loadedVehicle[select];

    int num = 0;
    for (int i = 0; i < 99; i++) {
        if (vehicle.schedule[i].exists == true) {
            num++;
        }
    }

    int array[num];
   
    for (int i = 0; i < num; i ++) {
        array[i] = vehicle.schedule[i].dueIn;
    }
    
    qsort(array, num, sizeof(int), compare);

    char nextTask[30];
    int nextTaskIn;
    int nextTaskAt;

    for (int i = 0; i < num; i++) {
        if (vehicle.schedule[i].dueIn == array[0]) {
            strcpy(nextTask, vehicle.schedule[i].task);
            nextTaskIn = vehicle.schedule[i].dueIn;
            nextTaskAt = vehicle.schedule[i].nextDue;
        }
    }
        
    printf("***************** Vehicle Infomation **********************\n\n");
    printf("%i %s %s\n", vehicle.year, vehicle.make, vehicle.model);
    printf("Mileage: %i km\n", vehicle.mileage);
    printf("\nNext Maintenance Task Due: %s\n", nextTask);
    printf("Due in: %i km\n", nextTaskIn);
    printf("Due at: %i km\n", nextTaskAt);
    printf("\nTotal Cost of Maintenance: $%2.2f\n", vehicle.totalCost);

    printf("\n\n\n\n");
}

void updateMileage(int select) {
    system("clear");
    Vehicle updateVehicle = {0};
    Vehicle loadedVehicle[99];

    FILE *infile = fopen("garage.dat", "r");
    if (infile == NULL) exit(1);
    while(fread(loadedVehicle, sizeof(loadedVehicle), 1, infile));
    fclose(infile);

    updateVehicle = loadedVehicle[select];

    printf("Please enter the current odometer reading: ");
    int mileage;
    scanf("%i", &mileage);
    updateVehicle.mileage = mileage;
    loadedVehicle[select] = updateVehicle;

    FILE* outfile = fopen("garage.dat", "w");
    if (outfile == NULL) exit(1);
    fwrite(loadedVehicle, sizeof(loadedVehicle), 1, outfile);
    fclose(outfile);
}

void deleteVehicle(int select) {
    system("clear");
    Vehicle loadedVehicle[99];
    Vehicle vehicle = {0};

    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) exit(1);
    while(fread(loadedVehicle, sizeof(loadedVehicle), 1, infile));
    fclose(infile);

    loadedVehicle[select] = vehicle;

    FILE* outfile = fopen("garage.dat", "w");
    if (outfile == NULL) exit(1);
    fwrite(loadedVehicle, sizeof(loadedVehicle), 1, outfile);
    fclose(outfile);
    }

void addTask(int select) {
    system("clear");
    Vehicle loadedVehicle[99];
    Vehicle vehicle;

    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) exit(1);
    while(fread(loadedVehicle, sizeof(loadedVehicle), 1, infile));
    fclose(infile);

    vehicle = loadedVehicle[select];

    printf("Add task to Mainenance Schedule\n\n");

    printf("Task Name: ");
    char job[20];
    getchar();
    fgets(job, 20, stdin);
    
    printf("Interval (km): ");
    int interval;
    scanf("%i", &interval);

    printf("Last Completed (km): ");
    int lastDone;
    scanf("%i", &lastDone);

    for (int i = 0; i < 99; i++) {
        if (vehicle.schedule[i].exists == true) {
            continue;
        } else {
            strcpy(vehicle.schedule[i].task, job);
            vehicle.schedule[i].interval = interval;
            vehicle.schedule[i].lastDone = lastDone;
            vehicle.schedule[i].nextDue = vehicle.schedule[i].lastDone + vehicle.schedule[i].interval;
            vehicle.schedule[i].dueIn = vehicle.schedule[i].nextDue - vehicle.mileage; 
            vehicle.schedule[i].exists = true;
            break;
        }
    }

    loadedVehicle[select] = vehicle;

    FILE* outfile = fopen("garage.dat", "w");
    if (outfile == NULL) exit(1);
    fwrite(loadedVehicle, sizeof(loadedVehicle), 1, outfile);
    fclose(outfile);
}

void viewSchedule(int select) {
    system("clear");
    Vehicle loadedVehicle[99];
    Vehicle vehicle;

    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) exit(1);
    while(fread(loadedVehicle, sizeof(loadedVehicle), 1, infile));
    fclose(infile);

    vehicle = loadedVehicle[select];
    printf("Maintenance Schedule for %i %s %s\n\n", vehicle.year, vehicle.make, vehicle.model);
    printf("Task Name\t\tInterval\tLast Completed\n");
    printf("------------------------------------------------------\n");
    for (int i = 0; i < 99; i++) {
        if (vehicle.schedule[i].exists == true) {
            vehicle.schedule[i].task[strcspn(vehicle.schedule[i].task, "\n")] = 0;
            printf("%-23s %-16i%i\n", vehicle.schedule[i].task, vehicle.schedule[i].interval, vehicle.schedule[i].lastDone);
        } else {
            break;
        }
    }
    printf("\n\n");
}

int compare(const void *x_void, const void *y_void) {
    int x = *(int *)x_void;
    int y = *(int *)y_void;
    return x - y;
}
    

void upcomingMaint(int select) {
    system("clear");
    Vehicle loadedVehicle[99];
    Vehicle vehicle;

    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) exit(1);
    while(fread(loadedVehicle, sizeof(loadedVehicle), 1, infile));
    fclose(infile);

    vehicle = loadedVehicle[select];
    
    int num = 0;
    for (int i = 0; i < 99; i++) {
        if (vehicle.schedule[i].exists == true) {
            num++;
        }
    }

    int array[num];
   
    for (int i = 0; i < num; i ++) {
        array[i] = vehicle.schedule[i].dueIn;
    }
    
    qsort(array, num, sizeof(int), compare);

    printf("\nUpcoming Maintenance for %i %s %s\n", vehicle.year, vehicle.make, vehicle.model);
    printf("\nTask                  Next Due            Mileage Remaining\n");
    printf("-----------------------------------------------------------\n");
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            if (vehicle.schedule[j].dueIn == array[i]) {
                vehicle.schedule[j].nextDue = vehicle.schedule[j].lastDone + vehicle.schedule[j].interval;
                vehicle.schedule[j].dueIn = vehicle.schedule[j].nextDue - vehicle.mileage;
                vehicle.schedule[j].task[strcspn(vehicle.schedule[j].task, "\n")] = 0;
                printf("%-22s%-20i%i\n", vehicle.schedule[j].task, vehicle.schedule[j].nextDue, vehicle.schedule[j].dueIn);
            }
        }
    }
    printf("\n\n");
}

void removeTask(int select) {
    system("clear");
    Vehicle loadedVehicle[99];
    Vehicle vehicle;
    int num = 0;
    int selection;
    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) exit(1);
    while(fread(loadedVehicle, sizeof(loadedVehicle), 1, infile));
    fclose(infile);

    vehicle = loadedVehicle[select];

    printf("Select which task to remove from the schedule: \n\n");
    for (int i = 0; i < 99; i++) {
        if (vehicle.schedule[i].exists == true) {
            vehicle.schedule[i].task[strcspn(vehicle.schedule[i].task, "\n")] = 0;
            printf("%i. %s\n", i, vehicle.schedule[i].task);
            num++;
        }
    }
    printf("\n>> ");
    scanf("%i", &selection);

    strcpy(vehicle.schedule[selection].task, "");
    vehicle.schedule[selection].interval = 0;
    vehicle.schedule[selection].lastDone = 0;
    vehicle.schedule[selection].dueIn = 0;
    vehicle.schedule[selection].nextDue = 0;
    vehicle.schedule[selection].exists = false;

    loadedVehicle[select] = vehicle;

    FILE* outfile = fopen("garage.dat", "w");
    if (outfile == NULL) exit(1);
    fwrite(loadedVehicle, sizeof(loadedVehicle), 1, outfile);
    fclose(outfile); 


    printf("\n\n");
}

void completeMaintenance(int select) {
    system("clear");
    Vehicle loadedVehicle[99];
    Vehicle vehicle;    

    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) exit(1);
    while(fread(loadedVehicle, sizeof(loadedVehicle), 1, infile));
    fclose(infile);

    vehicle = loadedVehicle[select];

    printf("Select which task has been completed: \n\n");

    for (int i = 0; i < 99; i++) {
        if (vehicle.schedule[i].exists == true) {
            vehicle.schedule[i].task[strcspn(vehicle.schedule[i].task, "\n")] = 0;
            printf("%i. %s\n", i, vehicle.schedule[i].task);
        }
    }
    
    char date[12] = {0};                                            //get the date
    time_t rawtime = time(NULL);    
    if (rawtime == -1) {
        
        puts("The time() function failed");
        exit(1);
    }    
    struct tm *ptm = localtime(&rawtime);    
    if (ptm == NULL) {
     
        puts("The localtime() function failed");
        exit(1);
    }
    strftime(date, 12, "%d/%m/%Y", ptm);

    int selection;
    printf("\n>> ");
    scanf("%i", &selection);
    printf("Please enter the current odometer reading: ");
    int mileage;
    scanf("%i", &mileage);
    vehicle.mileage = mileage;
    vehicle.schedule[selection].lastDone = mileage;
    printf("Cost of job: ");
    float cost;
    scanf("%f", &cost);    
    
    strcpy(vehicle.schedule[selection].lastDoneDate, date);

    vehicle.schedule[selection].nextDue = vehicle.schedule[selection].lastDone + vehicle.schedule[selection].interval;
    vehicle.schedule[selection].dueIn = vehicle.schedule[selection].nextDue - vehicle.mileage;    
    
    vehicle.schedule[selection].task[strcspn(vehicle.schedule[selection].task, "\n")] = 0;
    printf("\n\n%s completed on %s\n\n", vehicle.schedule[selection].task, date);
    
    for (int i = 0; i < 200; i++) {
        if (vehicle.log[i].exists == true) {
            continue;
        } else {
            vehicle.log[i].exists = true;
            strcpy(vehicle.log[i].task, vehicle.schedule[selection].task); 
            strcpy(vehicle.log[i].date, date);
            vehicle.log[i].mileage = vehicle.mileage;
            vehicle.log[i].cost = cost;
            vehicle.totalCost = vehicle.totalCost + vehicle.log[i].cost;
            break;
        }
    }   

    loadedVehicle[select] = vehicle;

    FILE* outfile = fopen("garage.dat", "w");
    if (outfile == NULL) exit(1);
    fwrite(loadedVehicle, sizeof(loadedVehicle), 1, outfile);
    fclose(outfile);
}

void maintenanceLog(int select) {
    system("clear");
    Vehicle loadedVehicle[99];
    Vehicle vehicle;

    FILE* infile = fopen("garage.dat", "r");
    if (infile == NULL) exit(1);
    while(fread(loadedVehicle, sizeof(loadedVehicle), 1, infile));
    fclose(infile);

    vehicle = loadedVehicle[select];

    printf("Maintenance Logs for %i %s %s\n\n", vehicle.year, vehicle.make, vehicle.model);
    printf("Task               Mileage            Cost                  Date\n");
    printf("----------------------------------------------------------------\n");

    
    for (int i = 0; i < 200; i++) {
        if (vehicle.log[i].exists == true) {
            vehicle.log[i].task[strcspn(vehicle.log[i].task, "\n")] = 0;
            printf("%-19s%-19i$%-15.2f%s\n", vehicle.log[i].task, vehicle.log[i].mileage, vehicle.log[i].cost, vehicle.log[i].date);
        } else {
            break;
        }
    }
    printf("\n\nTotal Cost: $%.2f\n\n\n\n", vehicle.totalCost);

    loadedVehicle[select] = vehicle;

    FILE* outfile = fopen("garage.dat", "w");
    if (outfile == NULL) exit(1);
    fwrite(loadedVehicle, sizeof(loadedVehicle), 1, outfile);
    fclose(outfile);
}
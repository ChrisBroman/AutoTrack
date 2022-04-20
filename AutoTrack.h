
typedef struct {
    bool exists;
    char date[30];
    char task[64];
    int mileage;
    float cost;
} Log;

typedef struct {
    bool exists;
    char task[64];
    int interval;
    int dueIn;
    int lastDone;
    char lastDoneDate[30];
    int nextDue;  
} Schedule;

typedef struct {
    bool exists;
    int year;
    char make[20];
    char model[30];
    int mileage;
    Schedule schedule[99];
    Log log[200];
    float totalCost;
} Vehicle;

void welcome();
void initializeGarage(Vehicle* currentVehicle);
bool parse_int(char *string, int *integer);
int getNumVehicles();
void mainMenu();
int getMenuOption(int max);
void listVehicle();
void buildVehicle();
Vehicle selectVehicle(int selection);
void vehicleOptions();
void vehicleInformation(int select);
void updateMileage(int select);
void currentOption(int select);
void deleteVehicle(int select);
void addTask(int select);
void viewSchedule(int select);
void upcomingMaint(int select);
int compare(const void *x_void, const void *y_void);
void removeTask(int select);
void completeMaintenance(int select);
void maintenanceLog(int select);
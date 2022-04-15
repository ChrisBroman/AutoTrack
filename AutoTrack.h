typedef struct {
    int identifier;
    char job[30];
    int interval;
    int lastDone;
    int nextDue;
    int dueIn;
    bool exists;
    time_t currentTime;
} Maintenance;

typedef struct {
    char ident[12];
    int year;
    char make[20];
    char model[20];
    int mileage;
    Maintenance task[99];
} Vehicle;



void welcome();
void mainMenu();
int getMenuOption();
void buildVehicle();
void listVehicle();
void deleteVehicle(int selection);
Vehicle selectVehicle(int selection);
void vehicleOptions();
void updateMileage(Vehicle* vehicle);
void vehicleInformation(Vehicle* vehicle);
void addTask(Vehicle *vehicle);
void viewMaintSched(Vehicle* vehicle);
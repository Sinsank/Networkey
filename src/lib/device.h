const char WIRED_DEVICE_FILE_DIR[] = "../db/devices/wired.txt";
const char WIRELESS_DEVICE_FILE_DIR[] = "../db/devices/wireless.txt";
const int HUB_DEVICE_ID = 1;
const int SWITCH_DEVICE_ID = 2;
const int ROUTER_DEVICE_ID = 3;
const int FIREWALL_DEVICE_ID = 4;
const int PC_DEVICE_ID = 5;

struct Device {
    int id;
    char name[1000];
    float price;
    bool wired;
};

int addNewDevice(struct Device device) {
    FILE* file;

    if(device.wired) {
        file = fopen(WIRED_DEVICE_FILE_DIR, "a");
    } else {
        file = fopen(WIRELESS_DEVICE_FILE_DIR, "a");
    }
    char buffer[1000];

    sprintf(buffer, "%d,%s,%.2f\n", device.id, device.name, device.price);
    fputs(buffer, file);

    fclose(file);
    
    return 1;
}

struct Device* readDevices(bool wired) {
    FILE* file;

    if(wired) {
        file = fopen(WIRED_DEVICE_FILE_DIR, "r");
    } else {
        file = fopen(WIRELESS_DEVICE_FILE_DIR, "r");
    }

    int index = 0;
    char buffer[1000];
    struct Device* devices = malloc(sizeof(struct Device) * 100);

    for(int i = 0; fgets(buffer, sizeof(buffer), file); i++) {
        char* token = strtok(buffer, ",");
        struct Device current;

        for(int j = 0; token; j++) {
            if(j == 0) {
                current.id = atoi(token);
            } else if(j == 1) {
                strncpy(current.name, token, sizeof(current.name));
            } else {
                current.price = atof(token);
            }
            token = strtok(NULL, ",");
        }
        devices[index++] = current; 
    }
    struct Device device;

    device.id = -1;
    devices[index] = device;

    fclose(file);
    
    return devices;
}

int updateDevicePrice(struct Device device) {
    FILE* file = fopen(WIRED_DEVICE_FILE_DIR, "r");
    
    if(!device.wired) {
        file = fopen(WIRELESS_DEVICE_FILE_DIR, "r");
    }

    struct Device* devices = readDevices(device.wired);
    char buffer[1000];

    if(device.wired) {
        file = fopen(WIRED_DEVICE_FILE_DIR, "w");
    } else {
        file = fopen(WIRELESS_DEVICE_FILE_DIR, "w");
    }

    for(int i = 0; devices[i].id != -1; i++) {
        struct Device current;
        int id = devices[i].id;
        char* name = devices[i].name;

        if(device.id == id && strcmp(device.name, name) == 0) {
            current = device;
        } else {
            current = devices[i];
        }
        sprintf(buffer, "%d,%s,%.2f\n", current.id, current.name, current.price);
        fputs(buffer, file);
    }
    fclose(file);    

    return 1;
}

int deleteExistingDevice(struct Device device) {
    FILE* file = fopen(WIRED_DEVICE_FILE_DIR, "r");
    
    if(!device.wired) {
        file = fopen(WIRELESS_DEVICE_FILE_DIR, "r");
    }

    struct Device* devices = readDevices(device.wired);
    char buffer[1000];

    if(device.wired) {
        file = fopen(WIRED_DEVICE_FILE_DIR, "w");
    } else {
        file = fopen(WIRELESS_DEVICE_FILE_DIR, "w");
    }

    for(int i = 0; devices[i].id != -1; i++) {
        struct Device current;
        int id = devices[i].id;
        char* name = devices[i].name;

        if(device.id != id || strcmp(name, device.name) != 0) {
            sprintf(buffer, "%d,%s,%.2f\n", devices[i].id, devices[i].name, devices[i].price);
            fputs(buffer, file);
        }
    }
    fclose(file);    

    return 1;
}

struct Device getBestDeviceForPrice(int id, bool wired, float price) {
    struct Device* devices = readDevices(wired);
    int index = 0;
    float bestPriceDifference = INT_MAX;
    struct Device bestDevice;

    for(int i = 0; devices[i].id != -1; i++) {
        if(devices[i].id == id) {
            float devicePrice = devices[i].price;
            float difference = abs(devicePrice - price);

            if(difference < bestPriceDifference) {
                bestPriceDifference = difference;
                bestDevice = devices[i];
            }
        }
    }
    return bestDevice;
}
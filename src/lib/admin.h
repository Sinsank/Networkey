const char ADMIN_FILE_PATH[] = "../db/admin_access.txt";
const char defaultGuestUsername[] = "guest";
const char defaultGuestPassword[] = "1234";

struct Admin {
    char username[1000];
    char password[1000];
};

int checkLoginRequest(char* username, char* password) {
    if(!*username || !*password) {
        return -1;
    }

    char buffer[1000];
    char adminUsername[1000];
    char adminPassword[1000];
    char* token;
    FILE* file = fopen(ADMIN_FILE_PATH, "r");

    fgets(buffer, sizeof(buffer), file);
    token = strtok(buffer, ","); 
    strncpy(adminUsername, token, sizeof(adminUsername));
    token = strtok(NULL, ","); 
    strncpy(adminPassword, token, sizeof(adminPassword));
    
    if(strcmp(adminUsername, username) == 0 && strcmp(adminPassword, password) == 0) {
        return 1;
    } else if(strcmp(defaultGuestUsername, username) == 0 && strcmp(defaultGuestPassword, password) == 0) {
        return 2;
    }
    
    return -1;
}

int changeUsername(char* username) {
    if(!*username) {
        return 0;
    }
    struct Admin admin;
    char buffer[1000];
    char* token;
    FILE* file = fopen(ADMIN_FILE_PATH, "r");
    
    fgets(buffer, sizeof(buffer), file);
    token = strtok(buffer, ",");
    token = strtok(NULL, ",");
    strncpy(admin.password, token, sizeof(admin.password)); 
    strncpy(admin.username, username, sizeof(admin.username));

    sprintf(buffer, "%s,%s", admin.username, admin.password);
    
    fclose(file);

    file = fopen(ADMIN_FILE_PATH, "w");

    fputs(buffer, file);
    fclose(file);

    return 1;
}

int changePassword(char* password) {
    if(!*password) {
        return 0;
    }
    struct Admin admin;
    char buffer[1000];
    char* token;
    FILE* file = fopen(ADMIN_FILE_PATH, "r");
    
    fgets(buffer, sizeof(buffer), file);
    token = strtok(buffer, ",");
    strncpy(admin.username, token, sizeof(admin.username)); 
    strncpy(admin.password, password, sizeof(admin.password)); 

    sprintf(buffer, "%s,%s", admin.username, admin.password);
    fclose(file);

    file = fopen(ADMIN_FILE_PATH, "w");

    fputs(buffer, file);
    fclose(file);

    return 1;
}
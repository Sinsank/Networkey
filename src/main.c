#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>
#include <conio.h>

#include "lib/admin.h" 
#include "lib/device.h"
#include "lib/feedback.h"
#include "lib/helper.h"
#include "lib/ipv4.h"
#include "lib/budget.h"
#include "lib/record.h"

struct UI_Selection {
    char text[1000];
    int key;
};

const char* LOGO =
  "\t\t\t________________________________________________________________________________\n"
  "\t\t\t*                                                                               *\n"
  "\t\t\t*                               N E T W O R K E Y                               *\n"
  "\t\t\t*                                                                               *\n"
  "\t\t\t*                          Network designs made it easy                         *\n"
  "\t\t\t*_______________________________________________________________________________*\n";

const int ADMIN_LOGIN = 1;
const int GUEST_LOGIN = 2;
const int SUCCESSFULL_STATUS = 1;
const struct FileWriteConfig ipTableWriteConfig = {
    .tableWidth = 186,
    .cellWidth = 35,
    .delimeter_h = '-',
    .delimeter_v = '*',
    .format = "%03d.%03d.%03d.%03d/%02d"
};
const char BILL_REPORT_FILE_PATH[] = "reports/bills/";
const char IP_REPORT_FILE_PATH[] = "reports/ips/";

int currentLogin;

void designUIHeading(char* topic, char* format, char* heading) {
    if(topic == NULL || format == NULL) {
        return;
    }
    char underline[100];
    int i;

    for(i = 0; i < strlen(topic); i++) {
        underline[i] = '=';
    }
    underline[i] = '\0';
    sprintf(heading, format, LOGO, topic, underline);
}

void designUIAlert(char* message, char* format, char* alert) {
    if(message == NULL || format == NULL) {
        return;
    }
    char line[100];
    int i;

    for(i = 0; i < strlen(message); i++) {
        line[i] = '-';
    }
    line[i] = '\0';
    sprintf(alert, format, line, message, line);
}

void designUISelectionMenu(struct UI_Selection* uiSelections, int selectionCount, char* menu, int padding) {
    int maxWidthSelectionTextSize = INT_MIN;
    int index;

    for(int i = 0; i < selectionCount; i++) {
        maxWidthSelectionTextSize = fmax(maxWidthSelectionTextSize, strlen(uiSelections[i].text));
    }
    for(index = 0; index < padding; index++) {
        menu[index] = ' ';
    }
    for(int i = 0; i < maxWidthSelectionTextSize + 8; i++) {
        menu[index++] = '-';
    }
    menu[index++] = '\n';

    for(int i = 0; i < selectionCount; i++) {
        struct UI_Selection current = uiSelections[i];
        int j;

        for(j = 0; j < padding; j++) {
            menu[index++] = ' ';
        }
        for(j = 0; j < strlen(current.text); j++) {
            menu[index++] = current.text[j];
        } 
        while(j++ < maxWidthSelectionTextSize) {
            menu[index++] = ' ';
        }
        menu[index++] = ' ';
        menu[index++] = '|';
        menu[index++]  = ' ';

        char key[100];
        sprintf(key, "KEY-%d\n", current.key);

        for(int k = 0; k < strlen(key); k++) {
            menu[index++] = key[k];
        }
    }
    for(int i = 0; i < padding; i++) {
        menu[index++] = ' ';
    }
    for(int i = 0; i < maxWidthSelectionTextSize + 8; i++) {
        menu[index++] = '-';
    }
    menu[index++] = '\0';
}

int adminController() {
    struct UI_Selection* selections = malloc(sizeof(struct UI_Selection) * 6);
    char menu[1000], alert[1000], heading[1000];
    int entry, result;

    struct UI_Selection addDevice = {
        .text = "Add a new device",
        .key = 1
    };
    struct UI_Selection updateDevice = {
        .text = "Update device price",
        .key = 2
    };
    struct UI_Selection deleteDevice = {
        .text = "Delete an existing device",
        .key = 3
    };
    struct UI_Selection analyzeFeedbacks = {
        .text = "Analyze customer feedbacks",
        .key = 4
    };
    struct UI_Selection changeUnameAndPassword = {
        .text = "Change Username & Password",
        .key = 5
    };
    struct UI_Selection logout = {
        .text = "Logout",
        .key = 0
    };

    selections[0] = addDevice;
    selections[1] = updateDevice;
    selections[2] = deleteDevice;
    selections[3] = analyzeFeedbacks;
    selections[4] = changeUnameAndPassword;
    selections[5] = logout;

    system("cls");
    designUIHeading("ADMIN CONSOLE", "%s\n%+71s\n%+71s\n", heading);
    printf("%s", heading);
    designUISelectionMenu(selections, 6, menu, 48);
    printf("\n%s\n", menu);
    printf("\n%+68s", "Entry : ");
    scanf("%d", &entry);

    if(entry == addDevice.key) {
        system("cls");
        designUIHeading("ADD NEW DEVICE", "%s\n%+71s\n%+71s\n", heading);
        printf("%s", heading);
        
        struct Device device;

        printf("\n%+62s", "ID : ");
        scanf("%d", &device.id);
        printf("\n%+64s", "Name : ");
        scanf("%s", device.name);
        printf("\n%+65s", "Price : ");
        scanf("%f", &device.price);
        printf("\n%+80s", "Wired[1]/Wireless[0] : ");
        scanf("%d", &device.wired);

        result = addNewDevice(device);

        if(result == SUCCESSFULL_STATUS) {
            designUIAlert("ACTION SUCCESSFULL", "%+74s\n%+74s\n%+74s\n", alert);
        }
    } else if(entry == updateDevice.key) {
        system("cls");
        designUIHeading("UPDATE DEVICE PRICE", "%s\n%+74s\n%+74s\n", heading);
        printf("%s", heading);

        struct Device device;

        printf("\n%+60s", "ID : ");
        scanf("%d", &device.id);
        printf("\n%+62s", "Name : ");
        scanf("%s", device.name);
        printf("\n%+63s", "Price : ");
        scanf("%f", &device.price);
        printf("\n%+78s", "Wired[1]/Wireless[0] : ");
        scanf("%d", &device.wired);

        result = updateDevicePrice(device);

        if(result == SUCCESSFULL_STATUS) {
            designUIAlert("ACTION SUCCESSFULL", "%+74s\n%+74s\n%+74s\n", alert);
        }

    } else if(entry == deleteDevice.key) {
        system("cls");
        designUIHeading("DELETE DEVICE", "%s\n%+70s\n%+70s\n", heading);
        printf("%s", heading);

        struct Device device;

        printf("\n%+62s", "ID : ");
        scanf("%d", &device.id);
        printf("\n%+64s", "Name : ");
        scanf("%s", device.name);
        printf("\n%+80s", "Wired[1]/Wireless[0] : ");
        scanf("%d", &device.wired);

        result = deleteExistingDevice(device);

        if(result == SUCCESSFULL_STATUS) {
            designUIAlert("ACTION SUCCESSFULL", "%+74s\n%+74s\n%+74s\n", alert);
        }
        
    } else if(entry == analyzeFeedbacks.key) {
        system("cls");
        designUIHeading("ANALYZE CUSTOMER FEEDBACKS", "%s\n%+77s\n%+77s\n", heading);
        printf("%s", heading);

        struct FeedbackReport fbReport = getFeedbackReport();

        printf("\n%+69s : %03d\n", "Positive Feedbacks", fbReport.positive);
        printf("\n%+69s : %03d\n\n", "Negative Feedbacks", fbReport.negative);
        printf("\n%+68s : %0.1f\n", "Happy Probability", fbReport.happyProb);
        printf("\n%+72s : %0.1f\n", "Not Happy Probability", fbReport.nonHappyProb);
        
        designUIAlert("ACTION SUCCESSFULL", "%+73s\n%+73s\n%+73s\n", alert);
        
        printf("\n\n%s", alert);
        getch();
        adminController();
    } else if(entry == changeUnameAndPassword.key) {
        system("cls");
        designUIHeading("CHANGE USERNAME & PASSWORD", "%s\n%+77s\n%+77s\n", heading);
        printf("%s", heading);

        char username[1000], password[1000];

        printf("\n%+66s", "New Username : ");
        scanf("%s", username);
        printf("\n%+66s", "New Password : ");
        scanf("%s", password);
        
        result = changeUsername(username);
        result = changePassword(password);

        if(result == SUCCESSFULL_STATUS) {
            designUIAlert("ACTION SUCCESSFULL", "%+73s\n%+73s\n%+73s\n", alert);
            printf("\n\n%s", alert);
            sleep(3);

            return 0;
        }
    } else if(entry == logout.key) {
        return 0;
    }
    else {
        designUIAlert("INVALID ENTRY", "%+71s\n%+71s\n%+71s\n", alert);
    }
    if(entry != analyzeFeedbacks.key) {
        printf("\n\n%s", alert);
        sleep(3);
        adminController();
    }
    return 0;
}

int guestController() {
    struct UI_Selection* selections = malloc(sizeof(struct UI_Selection) * 3);
    char menu[1000], alert[1000], heading[1000];
    int entry, result;

    struct UI_Selection designNetworkPlan = {
        .text = "Design a Network Plan",
        .key = 1
    };

    struct UI_Selection addCustomerFeedback = {
        .text = "Add a Customer Feedback",
        .key = 2
    };

    struct UI_Selection logout = {
        .text = "Logout",
        .key = 0
    };

    selections[0] = designNetworkPlan;
    selections[1] = addCustomerFeedback;
    selections[2] = logout;

    system("cls");
    designUIHeading("GUEST CONSOLE", "%s\n%+71s\n%+71s\n", heading);
    printf("%s", heading);
    designUISelectionMenu(selections, 3, menu, 49);
    printf("\n%s\n", menu);
    printf("\n%+67s", "Entry : ");
    scanf("%d", &entry);

    if(entry == designNetworkPlan.key) {
        system("cls");
        designUIHeading("DESIGN A NETWORK PLAN", "%s\n%+75s\n%+75s\n", heading);
        printf("%s", heading);

        char planID[100];
        int subnetCount, maxDevices = INT_MIN, deviceCounts[1000], internetConnected, wired, domain;
        float existingBudget;

        printf("\n%+64s", "Plan ID : ");
        scanf("%s", planID);
        printf("\n%+74s", "Number of Subnets : ");
        scanf("%d", &subnetCount);

        for(int i = 0; i < subnetCount; i++) {
            printf("\n%+74s%d : ", "Device Count of LAN-", i + 1);
            scanf("%d", &deviceCounts[i]);

            maxDevices = fmax(maxDevices, deviceCounts[i]);
        }
        printf("\n");
        
        printf("\n%+91s", "IP Domain [ Public(1)/Private(0) ] : ");
        scanf("%d", &domain);
        printf("\n%+92s", "Internet Connected [ Yes(1)/No(0) ] : ");
        scanf("%d", &internetConnected);
        printf("\n%+94s", "Connectivity [ Wired(1)/Wireless(0) ] : ");
        scanf("%d", &wired);

        printf("\n");

        printf("\n%+72s", "Existing Budget : ");
        scanf("%f", &existingBudget);
        
        struct IPv4 ip = chooseAnIP(subnetCount, maxDevices, domain);

        if(ip.availability) {
            char ipTableHeadings[] = "SUBNET,SUBNET_MASK ,NETWORK_ADDRESS ,DIRECT_BROADCAST_ADDRESS,DEVICES ";
            char billFilePath[100], ipFilePath[100];
            int numPCs = 0;

            sprintf(billFilePath, "%sBill-%s.txt", BILL_REPORT_FILE_PATH, planID);
            sprintf(ipFilePath, "%sIP-%s.txt", IP_REPORT_FILE_PATH, planID);

            FILE* ipFile = fopen(ipFilePath, "w");
            insertHeadings(ipFile, ipTableHeadings, ",", ipTableWriteConfig);
            
            struct Subnetwork* subnetworks = generateSubnetworks(ip, subnetCount, deviceCounts);

            for(int i = 0; i < subnetCount; i++) {
                struct Record record = {
                    .networkAddress = subnetworks[i].networkAddress,
                    .directBroadcastAddress = subnetworks[i].directBroadcastAddress,
                    .deviceAddresses = subnetworks[i].hosts,
                    .deviceCount = subnetworks[i].hostCount
                };
                numPCs += record.deviceCount;
                insertRecord(ipFile, record, ipTableWriteConfig, i + 1);
            }
            struct Record record = {.deviceCount = 0};
            insertRecord(ipFile, record, ipTableWriteConfig, -1);

            FILE* billFile = fopen(billFilePath, "w");
            char bill[1000], dateTime[100];
            struct Proposal proposal;

            proposal.requestedAmount = existingBudget;
            proposal.numHubs = subnetCount;
            proposal.numSwitches = subnetCount > 0;
            proposal.numRouters = internetConnected == 1 ? 1 : 0;
            proposal.numFirewalls = internetConnected == 1 ? 1 : 0;
            proposal.numPCs = numPCs;
            proposal.internetConnected = internetConnected;
            proposal.wired = wired;

            struct Budget budget = generateBudgetPlan(&proposal);
            float totalPrice = 0;

            totalPrice = proposal.numHubs * budget.bestHub.price +
                        proposal.numSwitches * budget.bestSwitch.price +
                        proposal.numRouters * budget.bestRouter.price +
                        proposal.numFirewalls * budget.bestFirewall.price +
                        proposal.numPCs * budget.bestPC.price;
            float totalRawPrice = totalPrice;
            addDiscount(&totalPrice);

            if(!proposal.internetConnected) {
                strncpy(budget.bestRouter.name, "NO", sizeof(budget.bestRouter.name));
                strncpy(budget.bestFirewall.name, "NO", sizeof(budget.bestFirewall.name));
            }
            setDateTime(dateTime, 100);
            sprintf(bill, BILL_TEMPLATE, 
                dateTime,
                budget.bestHub.name,
                proposal.numHubs,
                budget.bestHub.price * proposal.numHubs,
                budget.bestSwitch.name,
                proposal.numSwitches,
                budget.bestSwitch.price * proposal.numSwitches,
                budget.bestRouter.name,
                proposal.numRouters,
                budget.bestRouter.price * proposal.numRouters,
                budget.bestFirewall.name,
                proposal.numFirewalls,
                budget.bestFirewall.price * proposal.numFirewalls,
                budget.bestPC.name,
                proposal.numPCs,
                budget.bestPC.price * proposal.numPCs,
                totalRawPrice,
                proposal.requestedAmount,
                totalPrice
            );

            fputs(bill, billFile);
            
            fclose(ipFile);
            fclose(billFile);
            
            printf("\n\n%s\n", bill);
            designUIAlert("CHECKOUT \'reports\' DIRECTORY FOR DETAILED CALCULATIONS", "%+96s\n%+96s\n%+96s\n", alert);
            printf("\n\n%s", alert);
            designUIAlert("ACTION SUCCESSFULL", "%+76s\n%+76s\n%+76s\n", alert);
            printf("\n\n%s", alert);
            getch();
            guestController();
        } else {
            designUIAlert("NO ANY IP ADDRESS AVAILABLE", "%+80s\n%+80s\n%+80s\n", alert);
            printf("\n\n%s", alert);
            sleep(3);
            guestController();
        }
    } else if(entry == addCustomerFeedback.key) {
        system("cls");
        designUIHeading("ADD A CUSTOMER FEEDBACK", "%s\n%+76s\n%+76s\n", heading);
        printf("%s", heading);

        struct Feedback feedback;

        printf("\n%+84s", "Feedback [ Positive(1)/Negative(0) ] : ");
        scanf("%d", &feedback.positive);
        setDateTime(feedback.datetime, 1000);
        
        result = addFeedback(feedback);

        if(result == SUCCESSFULL_STATUS) {
            designUIAlert("THANK YOU FOR YOUR FEEDBACKS", "%+80s\n%+80s\n%+80s\n", alert);
            printf("\n\n%s", alert);
            designUIAlert("ACTION SUCCESSFULL", "%+75s\n%+75s\n%+75s\n", alert);
        }
    } else if(entry == logout.key) {
        return 0;
    } else {
        designUIAlert("INVALID ENTRY", "%+70s\n%+70s\n%+70s\n", alert);
    }
    if(entry != designNetworkPlan.key) {
        printf("\n\n%s", alert);
        sleep(3);
        guestController();
    }
    return 0;
}

int main() {
    char heading[1000];
    char alert[1000];
    int status;

    system("cls");
    
    if(currentLogin == ADMIN_LOGIN) {
        status = adminController();
        
        if(status == 0) {
            currentLogin = -1;

            designUIAlert("LOGOUT AS AN ADMIN", "%+73s\n%+73s\n%+73s\n", alert);
            printf("\n\n%s", alert);
            sleep(3);
            main();
        }
    } else if(currentLogin == GUEST_LOGIN) {
        status = guestController();

        if(status == 0) {
            currentLogin = -1;

            designUIAlert("LOGOUT AS A GUEST", "%+73s\n%+73s\n%+73s\n", alert);
            printf("\n\n%s", alert);
            sleep(3);
            main();
        }
    } else {
        char username[1000], password[1000];

        designUIHeading("USER LOGIN", "%s\n%+70s\n%+70s\n", heading);
        printf("%s", heading);
        printf("\n%+68s", "Username : ");
        scanf("%s", username);
        printf("\n%+68s", "Password : ");
        scanf("%s", password);

        currentLogin = checkLoginRequest(username, password);

        if(currentLogin == ADMIN_LOGIN) {
            designUIAlert("LOGIN AS AN ADMIN", "%+74s\n%+74s\n%+74s\n", alert);
        } else if(currentLogin == GUEST_LOGIN) {
            designUIAlert("LOGIN AS A GUEST", "%+73s\n%+73s\n%+73s\n", alert);
        } else {
            designUIAlert("INVALID LOGIN", "%+72s\n%+72s\n%+72s\n", alert);
        }
        
        printf("\n\n%s", alert);
        sleep(3);
        main();
    }
    
    return 0;
}

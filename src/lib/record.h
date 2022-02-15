const char* BILL_TEMPLATE = 
  "\t\t  DATETIME : %s\n"
  "\t\t  ------------------------------------------------------------------------------------------------\n"
  "\t\t  Best Hub      : %+12s           X          Quantity : %03d           =            %04.2f\n"
  "\t\t  Best Switch   : %+12s           X          Quantity : %03d           =            %04.2f\n"
  "\t\t  Best Router   : %+12s           X          Quantity : %03d           =            %04.2f\n"
  "\t\t  Best Firewall : %+12s           X          Quantity : %03d           =            %04.2f\n"
  "\t\t  Best PC       : %+12s           X          Quantity : %03d           =            %04.2f\n"
  "\t\t                                                                                          --------\n"
  "\t\t  TOTAL         :                                                                         %.2f\n"
  "\t\t                                                                                          ========\n\n"
  "\t\t  AMOUNT EXISTING          : %.2f\n"
  "\t\t  BEST TOTAL + DISCOUNT    : %.2f\n"
  "\t\t  ------------------------------------------------------------------------------------------------\n";


struct Record {
    struct IPv4 networkAddress;
    struct IPv4 directBroadcastAddress;
    struct IPv4* deviceAddresses;
    int deviceCount;
};

struct FileWriteConfig {
    int tableWidth;
    int cellWidth;
    char delimeter_h;
    char delimeter_v;
    char* format;
};

void fillCellWithPadding(char* buffer, char* str, int* i, float padding) {
    for(int j = 0; j < padding; j++) {
        *(buffer + *i) = ' '; 
        *i = *i + 1;
    }
    for(int c = 0; c < strlen(str); c++) {
        *(buffer + *i) = *(str + c);
        *i = *i + 1;
    }
    for(int j = 0; j < padding; j++) {
        *(buffer + *i) = ' '; 
        *i = *i + 1;
    }
}

int insertHeadings(FILE* file, char* headings, char* headingDelimeter, struct FileWriteConfig config) {
    if(file == NULL || !*(headings) || !(*headingDelimeter)) {
        return 0;
    }
    char buffer[1000];
    int i = 0;

    for(int c = 0; c < config.tableWidth; c++) {
        buffer[i++] = config.delimeter_h;
    }
    buffer[i++] = '\n';
    char* heading = strtok(headings, headingDelimeter);
    
    while(heading != NULL) {
        
        buffer[i++] = config.delimeter_v;
        float padding = round((float)(config.cellWidth - strlen(heading)) / 2);
        fillCellWithPadding(buffer, heading, &i, padding);

        heading = strtok(NULL, headingDelimeter);
    }
    buffer[i++] = config.delimeter_v;
    buffer[i] = '\0';

    fprintf(file, "%s\n", buffer);

    return 1;
}

int insertRecord(FILE* file, struct Record record, struct FileWriteConfig config, int lanID) {
    if(file == NULL || record.deviceCount < 0) {
        return 0;
    }
    char buffer[1000], subnetMaskStr[100], networkAddressStr[100], directBroadcastAddressStr[100], hostIPAddressStr[100], subnet[100]; 
    struct IPv4 networkAddress = record.networkAddress;
    struct IPv4 directBroadcastAddress = record.directBroadcastAddress;
    int i = 0;

    for(int c = 0; c < config.tableWidth; c++) {
        buffer[i++] = config.delimeter_h;
    }
    buffer[i++] = '\n';
    
    for(int j = 0; j < record.deviceCount; j++) {
        float padding;
        struct IPv4 hostIPAddress = record.deviceAddresses[j];
        
        sprintf(
            subnet, 
            "LAN: %d", 
            lanID
        );
        padding = round((float)(config.cellWidth - strlen(subnet)) / 2);
        buffer[i++] = config.delimeter_v;

        if(j == 0) {
            fillCellWithPadding(buffer, subnet, &i, padding);
        } else {
            padding = round((float)config.cellWidth / 2);
            fillCellWithPadding(buffer, "", &i, padding);
        }
        
        setSubnetmask(&networkAddress);
        sprintf(
            subnetMaskStr, 
            "%03d.%03d.%03d.%03d ", 
            networkAddress.subnetMask[0], 
            networkAddress.subnetMask[1], 
            networkAddress.subnetMask[2], 
            networkAddress.subnetMask[3]
        );
        padding = round((float)(config.cellWidth - strlen(subnetMaskStr)) / 2);
        buffer[i++] = config.delimeter_v;
        
        if(j == 0) {
            fillCellWithPadding(buffer, subnetMaskStr, &i, padding);
        } else {
            padding = round((float)config.cellWidth / 2);
            fillCellWithPadding(buffer, "", &i, padding);
        }

        sprintf(
            networkAddressStr, 
            config.format, 
            networkAddress.octets[0], 
            networkAddress.octets[1], 
            networkAddress.octets[2], 
            networkAddress.octets[3],
            networkAddress.networkID_Bits
        );
        padding = round((float)(config.cellWidth - strlen(networkAddressStr)) / 2);
        buffer[i++] = config.delimeter_v;
        
        if(j == 0) {
            fillCellWithPadding(buffer, networkAddressStr, &i, padding);
        } else {
            padding = round((float)config.cellWidth / 2);
            fillCellWithPadding(buffer, "", &i, padding);
        }
        
        sprintf(
            directBroadcastAddressStr, 
            config.format, 
            directBroadcastAddress.octets[0], 
            directBroadcastAddress.octets[1], 
            directBroadcastAddress.octets[2], 
            directBroadcastAddress.octets[3],
            directBroadcastAddress.networkID_Bits
        );
        padding = round((float)(config.cellWidth - strlen(directBroadcastAddressStr)) / 2);
        buffer[i++] = config.delimeter_v;

        if(j == 0) {
            fillCellWithPadding(buffer, directBroadcastAddressStr, &i, padding);
        } else {
            padding = round((float)config.cellWidth / 2);
            fillCellWithPadding(buffer, "", &i, padding);
        }

        sprintf(
            hostIPAddressStr, 
            config.format, 
            hostIPAddress.octets[0], 
            hostIPAddress.octets[1], 
            hostIPAddress.octets[2], 
            hostIPAddress.octets[3],
            hostIPAddress.networkID_Bits
        );
        padding = round((float)(config.cellWidth - strlen(hostIPAddressStr)) / 2);
        buffer[i++] = config.delimeter_v;
        fillCellWithPadding(buffer, hostIPAddressStr, &i, padding);
        
        buffer[i++] = config.delimeter_v;
        buffer[i] = '\0';
        fprintf(file, "%s\n", buffer);

        for(int i = 0; *(buffer + i); i++) {
            *(buffer + i) = 0;
        }
        i = 0;
    }
    buffer[i] = '\0';
    fprintf(file, "%s", buffer);
    
    return 1;
} 

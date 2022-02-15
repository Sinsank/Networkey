const int NUM_IP_BITS = 32;
const int OCTET_SIZE = 8;
const int NUM_OCTETS = 4;
const char IP_BULK_PATH[] = "../db/ip_bulk.txt";
const int PUBLIC_IP_FIRST_OCTETS_MAX[] = {127, 191, 223};
const int PRIVATE_IP_FIRST_OCTETS_MAX[] = {10, 172, 192};

struct IPv4 {
    int octets[4];
    int subnetMask[4];
    int networkID_Bits;
    int hostID_Bits;
    char ipClass;
    float price;
    bool public;
    bool availability;
};

struct Subnetwork {
    struct IPv4 networkAddress;
    struct IPv4 directBroadcastAddress;
    struct IPv4 hosts[100];
    int hostCount;
};

int setIPClass(struct IPv4* ip) {
    int firstOctet = ip->octets[0];
    int len = sizeof(PUBLIC_IP_FIRST_OCTETS_MAX) / sizeof(int);

    for(int i = 0; i < len; i++) {
        int max = PUBLIC_IP_FIRST_OCTETS_MAX[i];

        if(!ip->public) {
            max = PRIVATE_IP_FIRST_OCTETS_MAX[i];
        }

        if(firstOctet <= max) {
            ip->ipClass = 'A' + i;
            return 1;
        }
    }
    return 0;
}

int setSubnetmask(struct IPv4* ip) {
    int subnetMaskBin[32] = {0};

    for(int i = 0; i < ip->networkID_Bits; i++) {
        subnetMaskBin[i] = 1;
    }

    int start = 0, index = 0;

    for(int i = 0; i <= NUM_IP_BITS; i += OCTET_SIZE) {
        if(i != 0 && i % OCTET_SIZE == 0) {
            int* octet = copy(subnetMaskBin, start, i, NUM_IP_BITS);
            
            ip->subnetMask[index++] = binToDec(octet, OCTET_SIZE);
            start = i;
        }
    }
    return 1;
}

struct IPv4* getIPs() {
    FILE* file = fopen(IP_BULK_PATH, "r");
    struct IPv4* addressesAvailable = malloc(sizeof(struct IPv4) * 100);
    char buffer[1000];
    int i;

    for(i = 0; fgets(buffer, sizeof(buffer), file); i++) {
        struct IPv4 ip;
        char* token1;
        char* rest1 = buffer;

        for(int j = 0; (token1 = strtok_r(rest1, ",", &rest1)); j++) {
            if(j == 0) {
                char* token2;
                char* rest2 = token1;

                for(int k = 0; (token2 = strtok_r(rest2, ".", &rest2)); k++) {
                    if(k == 3) {
                        char* token3;
                        char* rest3 = token2;

                        for(int l = 0; (token3 = strtok_r(rest3, "/", &rest3)); l++) {
                            if(l == 0) {
                                ip.octets[k] = atoi(token3);
                            } else {
                                ip.networkID_Bits = atoi(token3);
                                ip.hostID_Bits = NUM_IP_BITS - ip.networkID_Bits;
                            }
                        } 
                    } else {
                        ip.octets[k] = atoi(token2);
                    }
                } 
            } else if(j == 1) {
                ip.public = strcmp(token1, "public") == 0;
            } else if(j == 2) {
                ip.price = atof(token1);
            }
        }
        setIPClass(&ip);
        setSubnetmask(&ip);
        ip.availability = true;
        addressesAvailable[i] = ip;
    }
    struct IPv4 ip;

    ip.availability = false;
    addressesAvailable[i] = ip;

    fclose(file);

    return addressesAvailable;
}

struct IPv4 chooseAnIP(int numSubnets, int numIPs, bool public) {
    struct IPv4 ip;
    ip.availability = false;

    if(numSubnets <= 0 || numIPs <= 0) {
        return ip;
    }

    struct IPv4* addresses = getIPs();
    int subnetBitsNeeded = indexOfNearestPowerOfTwo(numSubnets);
    int hostBitsNeeded = indexOfNearestPowerOfTwo(numIPs);
    int i;

    for(i = 0; addresses[i].availability; i++) {
        ip = addresses[i];
        
        if(ip.public == public && ip.hostID_Bits > subnetBitsNeeded) {
            int remainBits = ip.hostID_Bits - subnetBitsNeeded;
            if(remainBits >= hostBitsNeeded) {
                return ip;
            }
        }
    }
    ip = addresses[i];
    ip.availability = false;
    
    return ip;
}

int* toFlattenBinIP(struct IPv4 ip) {
    if(!ip.availability) {
        return NULL;
    }

    static int binIP[32] = {0};
    int index = 0;

    for(int i = 0; i < NUM_OCTETS; i++) {
        int octet = ip.octets[i], octetBin[8] = {0};

        decToBin(octetBin, octet, OCTET_SIZE - 1);

        for(int j = 0; j < OCTET_SIZE; j++) {
            binIP[index++] = octetBin[j];
        }
    }

    return binIP;
}

struct IPv4 getFormattedIP(struct IPv4 ip, int subnetID, int hostID, int subnetID_Bits) {
    struct IPv4 formattedIp;

    if(!ip.availability || subnetID < 0 || hostID < 0) {
        formattedIp.availability = false;
        return formattedIp; 
    }

    int* flattenIp = toFlattenBinIP(ip);

    for(int i = ip.networkID_Bits; i < NUM_IP_BITS; i++) {
        *(flattenIp + i) = 0;
    }

    decToBin(flattenIp, subnetID, ip.networkID_Bits + subnetID_Bits - 1);
    decToBin(flattenIp, hostID, NUM_IP_BITS - 1);

    for(int i = 0, j = 0; i < NUM_IP_BITS; i += OCTET_SIZE, j++) {
        int octet = binToDec(copy(flattenIp, i, i + OCTET_SIZE, NUM_IP_BITS), OCTET_SIZE);
        
        formattedIp.octets[j] = octet;
    }
    formattedIp.networkID_Bits = ip.networkID_Bits + subnetID_Bits;
    formattedIp.hostID_Bits = ip.hostID_Bits - subnetID_Bits;
    formattedIp.ipClass = ip.ipClass;
    formattedIp.price = ip.price;
    formattedIp.public = ip.public;
    formattedIp.availability = ip.availability;

    setSubnetmask(&formattedIp);

    return formattedIp;
}

struct Subnetwork* generateSubnetworks(struct IPv4 ip, int numSubnets, int* deviceCounts) {
    struct Subnetwork* subnetworks = malloc(sizeof(struct Subnetwork) * 100);

    if(numSubnets <= 0 || !ip.availability) {
        return subnetworks;
    }

    int index = 0, numSubnetBits = indexOfNearestPowerOfTwo(numSubnets);

    for(int i = 0; i < numSubnets; i++) {
        struct Subnetwork subnetwork;
        int deviceCount = deviceCounts[i];

        subnetwork.networkAddress = getFormattedIP(ip, i, 0, numSubnetBits);
        subnetwork.directBroadcastAddress = getFormattedIP(ip, i, pow(2, ip.hostID_Bits - numSubnetBits) - 1, numSubnetBits);

        for(int j = 1; j <= deviceCount; j++) {
            struct IPv4 hostIp = getFormattedIP(ip, i, j, numSubnetBits);
            subnetwork.hosts[j - 1] = hostIp;
        }
        subnetwork.hostCount = deviceCount;
        subnetworks[index++] = subnetwork;
    }

    return subnetworks;
}

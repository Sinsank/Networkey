void setDateTime(char* datetime, int size) {
    time_t rawTime = time(NULL);
    struct tm *lt = localtime(&rawTime);
    char* ascDateTime = asctime(lt);

    ascDateTime[strlen(ascDateTime) - 1] = '\0';
    strncpy(datetime, ascDateTime, size);
}

int binToDec(int* bin, int len) {
    int dec = 0;

    for(int i = len - 1, p = 0; i >= 0; i--, p++) {
        dec += (int) pow(2, p) * bin[i];
    }

    return dec;
}

void decToBin(int* bin, int dec, int end) {
    if(dec < 0) {
        return;
    }

    for(int i = end; dec > 0; i--) {
        *(bin + i) = dec % 2;
        dec /= 2;
    }
}

int* copy(int* src, int start, int end, int len) {
    if(start < 0 || start > len || end < 0 || end > len) {
        return NULL;
    }
    static int copied[8];

    for(int s = start; s < end; s++) {
       copied[s - start] = *(src + s);
    }
    
    return copied;
}

int indexOfNearestPowerOfTwo(int val) {
    if(val < 0) {
        return 0;
    }
    return ceil(log(val) / log(2));
}

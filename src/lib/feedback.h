const char FEEDBACK_FILE_PATH[] = "../db/feedbacks.txt";
const char POSITIVE_RATING = '+';
const char NEGATIVE_RATING = '-';

struct FeedbackReport {
    int positive;
    int negative;
    float happyProb;
    float nonHappyProb; 
};

struct Feedback {
    char datetime[1000];
    bool positive;
};

int addFeedback(struct Feedback feedback) {
    FILE* file = fopen(FEEDBACK_FILE_PATH, "a");
    char buffer[1000];
    
    sprintf(buffer, "%s,%c\n", feedback.datetime, feedback.positive ? POSITIVE_RATING : NEGATIVE_RATING);
    
    fputs(buffer, file);
    fclose(file);

    return 1;
}
struct FeedbackReport getFeedbackReport() {
    FILE* file = fopen(FEEDBACK_FILE_PATH, "r");
    struct FeedbackReport fbReport;
    char buffer[1000];
    int positive = 0, negative = 0;

    while(fgets(buffer, sizeof(buffer), file)) {
        char* token = strtok(buffer, ",");

        for(int j = 0; token; j++) {
            if(j == 1) {
                positive = *token == POSITIVE_RATING ? positive + 1 : positive;
                negative = *token == NEGATIVE_RATING ? negative + 1 : negative;
            }
            token = strtok(NULL, ",");
        }
    }
    
    fbReport.positive = positive;
    fbReport.negative = negative;
    fbReport.happyProb = (float) positive / (positive + negative) * 100;
    fbReport.nonHappyProb = (float) negative / (positive + negative) * 100;

    fclose(file);

    return fbReport;
}
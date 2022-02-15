struct Proposal {
    float requestedAmount;
    int numHubs;
    int numSwitches;
    int numRouters;
    int numFirewalls;
    int numPCs;
    bool internetConnected;
    bool wired;
};

struct Budget {
    struct Device bestHub;
    struct Device bestSwitch;
    struct Device bestRouter;
    struct Device bestFirewall;
    struct Device bestPC;
};

struct Discount {
    int boundary;
    float percentage;
};

struct Discount _5percentDiscount = {.boundary = 5000, .percentage = 0.05};
struct Discount _10percentDiscount = {.boundary = 10000, .percentage = 0.10};
struct Discount _25percentDiscount = {.boundary = 50000, .percentage = 0.25};

struct Budget generateBudgetPlan(struct Proposal* proposal) {
    int totalDeviceCount = 
        proposal->numHubs + 
        proposal->numSwitches + 
        proposal->numRouters + 
        proposal->numFirewalls + 
        proposal->numPCs;

    float averageDevicePrice = proposal->requestedAmount / totalDeviceCount;
    struct Budget budget;
    
    if(proposal->numHubs > 0) {
        budget.bestHub = getBestDeviceForPrice(HUB_DEVICE_ID, proposal->wired, averageDevicePrice);
    }
    if(proposal->numPCs > 0) {
        budget.bestPC = getBestDeviceForPrice(PC_DEVICE_ID, proposal->wired, averageDevicePrice);
    }
    if(proposal->numSwitches > 0) {
        budget.bestSwitch = getBestDeviceForPrice(SWITCH_DEVICE_ID, proposal->wired, averageDevicePrice);
    }
    if(proposal->internetConnected) {
        budget.bestRouter = getBestDeviceForPrice(ROUTER_DEVICE_ID, proposal->wired, averageDevicePrice);
        budget.bestFirewall = getBestDeviceForPrice(FIREWALL_DEVICE_ID, proposal->wired, averageDevicePrice);
    }
    
    return budget;
}

void addDiscount(float* totalPrice) {
    if(*(totalPrice) >= _25percentDiscount.boundary) {
        *totalPrice -= (*totalPrice * _25percentDiscount.percentage);
    } else if(*(totalPrice) >= _10percentDiscount.boundary) {
        *(totalPrice) -= (*totalPrice * _10percentDiscount.percentage);
    } else if(*(totalPrice) >= _5percentDiscount.boundary) {
        *(totalPrice) -= (*totalPrice * _5percentDiscount.percentage);
    }
}

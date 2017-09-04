#include "timers.h"

struct Timer {
    int timeout;
    void (*callback)();
};

const int maxtimers = 32;
Timer timers[maxtimers];
int numtimers = 0;
int mintimer = -1;

void UpdateMinTimer() {
    if (numtimers > 0) {
        mintimer = 0;

        for (int i = 1; i < numtimers; i++) {
            if (timers[i].timeout < timers[mintimer].timeout) {
                mintimer = i;
            }
        }
    } else {
        mintimer = -1;
    }
}

void ClearTimers() {
    numtimers = 0;
    mintimer = -1;
}

int AddTimer(int timeout, void (*callback)()) {
    if (numtimers >= maxtimers) {
        return -1;
    }

    // add new timer to end of timers
    timers[numtimers].timeout = timeout;
    timers[numtimers].callback = callback;
    numtimers++;

    UpdateMinTimer();

    return numtimers-1;
}

int RemoveTimer(int i) {
    if (i >= numtimers) {
        return -1;
    }

    // pop last timer into i-th location
    timers[i].timeout = timers[numtimers-1].timeout;
    timers[i].callback = timers[numtimers-1].callback;
    numtimers--;

    UpdateMinTimer();

    return 0;
}

void DecrementTimers(int t) {
    for (int i = 0; i < numtimers; i++) {
        timers[i].timeout -= t;
    }
}

void AdvanceTimers(int t) {
    while (mintimer >= 0 && t >= timers[mintimer].timeout) {
        Timer timer = timers[mintimer];

        t -= timer.timeout;
        DecrementTimers(timer.timeout);
        RemoveTimer(mintimer);

        timer.callback();
    }

    DecrementTimers(t);
}

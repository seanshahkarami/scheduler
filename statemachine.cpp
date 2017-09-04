#include <stdio.h>

// on top of this, can implement a very simple state machine for device with
// a few actions
//
// enable
// disable
// start
// stop
// fail

// another interesting thing is, we can almost entirely drop the
// starting / stopping states. these are just captured using
// one time tasks for the devices

// now, how do we unit test the scheduler?
// what are some basic critera of what we'd want?
//
// 1. no events triggered if not enough time's passed
// 2. exactly the right number of events are triggered
// 3. can incrementally advance time correctly
// 4. if a window of time passes which encompasses multiple events,
//    they are triggerd in order of time

enum Mode {
    DISABLED,
    STOPPED,
    STARTING,
    STARTED,
    STOPPING,
};

const char *modeName(Mode mode) {
    switch (mode) {
        case DISABLED:
            return "disabled";
        case STOPPED:
            return "stopped";
        case STARTING:
            return "starting";
        case STARTED:
            return "started";
        case STOPPING:
            return "stopping";
        default:
            return "invalid";
    }
}

class State {
public:

    void heartbeat();
    void advance(int seconds);
    void advanceTime(int seconds);
    void changeMode(Mode newMode);

    Mode mode;
    int modeSeconds;
    int heartbeatSeconds;
};

void State::changeMode(Mode newMode) {
    printf("DEBUG %s -> %s\n", modeName(mode), modeName(newMode));
    mode = newMode;
    modeSeconds = 0;
    heartbeatSeconds = 0;
}

void State::heartbeat() {
    heartbeatSeconds = 0;
}

void State::advance(int seconds) {
    while (seconds > 0) {
        switch (mode) {
            case DISABLED:
                advanceTime(seconds);
                seconds = 0;
                break;
            case STOPPED:
                // are we ready to start the device?
                if (modeSeconds + seconds >= 60) {
                    changeMode(STARTING);
                    seconds -= 60;
                } else {
                    advanceTime(seconds);
                    seconds = 0;
                }
                break;
            case STARTING:
                // should the device have started yet?
                if (modeSeconds + seconds >= 120) {
                    changeMode(STARTED);
                    seconds -= 120;
                } else {
                    advanceTime(seconds);
                    seconds = 0;
                }
                break;
            case STARTED:
                // has the heartbeat expired?
                if (heartbeatSeconds + seconds >= 300) {
                    changeMode(STOPPING);
                    seconds -= 300;
                } else {
                    advanceTime(seconds);
                    seconds = 0;
                }
                break;
            case STOPPING:
                if (modeSeconds + seconds >= 120) {
                    changeMode(STOPPED);
                    seconds -= 120;
                } else {
                    advanceTime(seconds);
                    seconds = 0;
                }
                break;
        }
    }
}

void State::advanceTime(int seconds) {
    modeSeconds += seconds;
    heartbeatSeconds += seconds;
}

void testDisabled() {
    State nc = {
        .mode = DISABLED,
        .modeSeconds = 0,
        .heartbeatSeconds = 0,
    };

    for (int i = 0; i < 3600; i++) {
        nc.advance(1);

        if (nc.mode != DISABLED) {
            printf("FAIL\n");
            return;
        }
    }

    printf("PASS\n");
}

void testStopped() {
    State nc = {
        .mode = STOPPED,
        .modeSeconds = 0,
    };

    nc.advance(59);
    printf("%s\n", nc.mode == STOPPED ? "PASS" : "FAIL");

    nc.advance(1);
    printf("%s\n", nc.mode == STARTING ? "PASS" : "FAIL");
}

void testStarting() {
    State nc = {
        .mode = STARTING,
        .modeSeconds = 0,
    };

    nc.advance(119);
    printf("%s\n", nc.mode == STARTING ? "PASS" : "FAIL");

    nc.advance(1);
    printf("%s\n", nc.mode == STARTED ? "PASS" : "FAIL");
}

int main() {
    State nc = {.mode = STARTED, .modeSeconds = 0, .heartbeatSeconds = 0};

    nc.advance(100);
    nc.advance(100);
    nc.heartbeat();
    nc.advance(100);
    nc.advance(100);
    nc.heartbeat();
    nc.advance(100);
    nc.advance(100);
    nc.advance(100);
    nc.heartbeat();
    nc.advance(500);
}

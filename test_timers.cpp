#include "timers.h"
#include <stdio.h>

enum Result {
    ONESHOT1,
    ONESHOT2,
    ONESHOT3,
    KICKOFF1,
    KICKOFF2,
    KICKOFF3,
    PERIODIC1,
};

Result results[32];
int numresults = 0;

void ClearResults() {
    numresults = 0;
}

void Oneshot1() {
    results[numresults++] = ONESHOT1;
}

void Oneshot2() {
    results[numresults++] = ONESHOT2;
}

void Oneshot3() {
    results[numresults++] = ONESHOT3;
}

void Kickoff1() {
    results[numresults++] = KICKOFF1;
    AddTimer(25, Oneshot1);
}

void Kickoff2() {
    results[numresults++] = KICKOFF2;
    AddTimer(50, Oneshot2);
}

void Kickoff3() {
    results[numresults++] = KICKOFF3;
    AddTimer(75, Oneshot3);
}

void Periodic1() {
    results[numresults++] = PERIODIC1;
    AddTimer(10, Periodic1);
}

void Setup() {
    ClearResults();
    ClearTimers();
}

void TestTiming() {
    const int T1 = 73;
    const int T2 = 49;
    const int T3 = 37;

    Setup();
    AddTimer(T1, Oneshot1);
    AddTimer(T1+T2, Oneshot2);
    AddTimer(T1+T2+T3, Oneshot3);

    AdvanceTimers(T1-1);
    printf("1.1 %s\n", numresults == 0 ? "PASS" : "FAIL");
    AdvanceTimers(1);
    printf("1.2 %s\n", numresults == 1 ? "PASS" : "FAIL");

    AdvanceTimers(T2-1);
    printf("1.3 %s\n", numresults == 1 ? "PASS" : "FAIL");
    AdvanceTimers(1);
    printf("1.4 %s\n", numresults == 2 ? "PASS" : "FAIL");

    AdvanceTimers(T3-1);
    printf("1.5 %s\n", numresults == 2 ? "PASS" : "FAIL");
    AdvanceTimers(1);
    printf("1.6 %s\n", numresults == 3 ? "PASS" : "FAIL");
}

void TestReodering() {
    Setup();
    AddTimer(100, Oneshot1);
    AddTimer(200, Oneshot2);
    AddTimer(300, Oneshot3);
    AdvanceTimers(500);
    printf("2.1 %s\n", (numresults == 3 && results[0] == ONESHOT1 && results[1] == ONESHOT2 && results[2] == ONESHOT3) ? "PASS" : "FAIL");

    Setup();
    AddTimer(100, Oneshot1);
    AddTimer(300, Oneshot3);
    AddTimer(200, Oneshot2);
    AdvanceTimers(500);
    printf("2.2 %s\n", (numresults == 3 && results[0] == ONESHOT1 && results[1] == ONESHOT2 && results[2] == ONESHOT3) ? "PASS" : "FAIL");

    Setup();
    AddTimer(300, Oneshot3);
    AddTimer(100, Oneshot1);
    AddTimer(200, Oneshot2);
    AdvanceTimers(500);
    printf("2.3 %s\n", (numresults == 3 && results[0] == ONESHOT1 && results[1] == ONESHOT2 && results[2] == ONESHOT3) ? "PASS" : "FAIL");

    Setup();
    AddTimer(300, Oneshot3);
    AddTimer(200, Oneshot2);
    AddTimer(100, Oneshot1);
    AdvanceTimers(500);
    printf("2.4 %s\n", (numresults == 3 && results[0] == ONESHOT1 && results[1] == ONESHOT2 && results[2] == ONESHOT3) ? "PASS" : "FAIL");

    Setup();
    AddTimer(200, Oneshot2);
    AddTimer(300, Oneshot3);
    AddTimer(100, Oneshot1);
    AdvanceTimers(500);
    printf("2.5 %s\n", (numresults == 3 && results[0] == ONESHOT1 && results[1] == ONESHOT2 && results[2] == ONESHOT3) ? "PASS" : "FAIL");

    Setup();
    AddTimer(200, Oneshot2);
    AddTimer(100, Oneshot1);
    AddTimer(300, Oneshot3);
    AdvanceTimers(500);
    printf("2.6 %s\n", (numresults == 3 && results[0] == ONESHOT1 && results[1] == ONESHOT2 && results[2] == ONESHOT3) ? "PASS" : "FAIL");
}

void TestKickoff() {
    Setup();
    AddTimer(100, Kickoff1);
    AddTimer(200, Oneshot2);

    AdvanceTimers(1000);
    printf("3.1 %s\n", (numresults == 3 && results[0] == KICKOFF1 && results[1] == ONESHOT1 && results[2] == ONESHOT2) ? "PASS" : "FAIL");
}

void TestPeriodic() {
    Setup();
    AddTimer(100, Periodic1);

    AdvanceTimers(200);
    printf("4.1 %s\n", numresults == 11 ? "PASS" : "FAIL");
}

int main() {
    TestTiming();
    TestReodering();
    TestKickoff();
    TestPeriodic();
}

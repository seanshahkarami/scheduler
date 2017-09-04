void ClearTimers();
int AddTimer(int timeout, void (*callback)());
int RemoveTimer(int i);
void AdvanceTimers(int t);

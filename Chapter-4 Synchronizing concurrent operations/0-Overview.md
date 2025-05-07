

## This chapter covers 

- Waiting for an event
- Waiting for a one-off event with futures
- waiting with a time limit
- Using the synchronization of operations to simplify code

When we want a thread to wait for another thread to finish a task, we could periodically check "task complete"
but this would consume CPU time and it is unncesary.

C++ standard library provide facilities to synchronize operations:

- condition variables
- futures
And in the concurrency technical specification they are extended with:
- latches
- barriers

One thing to notice is that in HPC and low latency software this techniques are to be avoid in the critical areas,
since they come with their own inneficiencies and overheads.

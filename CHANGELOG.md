# _CHANGELOG_

## V0.0.2.2 (2021-11-11)
> _Unreleased_

Security:

  _None_

Features:

  - created a sate machine and user process.
  - created a 2 threads to run `appEventHandler` and `userProcess` functions.
  - used `sleep` function in the program, inclued `<unistd.h>` lib in case of no timmers.
  - used `QBase` to distribute the events to the state machine.

Fix:

  _None_

Changes:

  - used pointer functions (`ptrf`) and passed the callback functions through the thread process.

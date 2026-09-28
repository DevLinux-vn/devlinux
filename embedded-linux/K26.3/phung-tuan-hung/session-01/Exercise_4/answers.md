rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ # Step 1 — create a regular variable
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ MY_NAME="Quan"
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ echo $MY_NAME        # visible?
Quan
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ # Step 2 — open a child shell and check
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ bash

rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ echo $MY_NAME        # visible? why?

rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ exit
exit
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ # Step 3 — export the variable
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ export MY_NAME="Quan"
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ bash

rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ echo $MY_NAME        # visible? why?
Quan
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ exit
exit
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ # Step 4 — modify variable inside child shell
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ bash

rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ MY_NAME="Alice"
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ echo $MY_NAME        # what do you see inside child shell?
Alice
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ exit
exit
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$ echo $MY_NAME        # back in parent shell — what do you see? why?
Quan
rvc@L4xxxxx-HSSA:~/hungphung/training/devlinux/embedded-linux/K26.3/phung-tuan-hung/session-01/Exercise_4$



Question 1: The difference between a regular variable and an exported variable
+ Regular variable: Catch only in process define it
+ Exported variable: Catch by all process
Question 2: Why changes made inside a child shell do not affect the parent shell?
+ In case homework you only set MY_NAME as regular variable and assign it so it can not impact to other process
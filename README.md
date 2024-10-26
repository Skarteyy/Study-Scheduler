# Study-Scheduler
A simple, effective tool to help students manage their study workload. (Can be used only on a Linux environment)

-You can directly access the app by "scheduler.exe". 

-It saves some basic data of the user to the "UserData" file, and if the contents of the "UserData" are corrupted, you can execute "dataWriter" to fix the problem.

-If you want to make changes to the source code:
  Make sure that gtk3+ is installed, for ubuntu, you can install via terminal by,
  
  sudo apt update  
  sudo apt install libgtk-3-dev 

-Then you can compile it using the "make" command at a terminal. (or copy the code in Makefile to a terminal)
  

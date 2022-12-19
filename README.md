# Process-Management
The purpose of this project is to manage processes and communication between them.

## Project Description
In this project, we find the users who have the lowest personality distance with the user with the given personality characteristics.
In psychology, according to the 5-factor personality model, each person has 5 personality traits.
The 5 stated features are:

1. Openness
2. Conscientiousness
3. Extraversion
4. Agreeableness
5. Neuroticism

In this project, we have a file called [traits.csv](). In each line of this file, 5 numbers are given as integers, which respectively represent the 5 numbers corresponding to the 5 factors mentioned above. We also have a number of csv files (specified with the name ```users-<num>.csv```) and it contains several lines, each line containing 5 numbers that correspond to the 5 factors mentioned above.

First, an initial process is created. This primary process reads the ```traits.csv``` file and then creates a child process for each line of it by successively calling two system calls ```fork()``` and ```exec()``` and gives it the read line information. Then, each created child process creates a new process for each existing ```users-<num>.csv``` file and gives the line information read from ```traits.csv```, which the primary process gave to this process, to the child process.

The newly created process reads the corresponding file and for each line of the read file, calculates the Euclidean distance of that line with the information received from its father and selects the line for which the Euclidean distance calculated is the lowest. Then it gives the content of this line (5 numbers) along with the name of the processed file to its parent process through a [Named Pipe](https://techaccess.in/2021/05/16/named-pipes/).

Each parent process collects the information it receives from its children and selects the data whose Euclidean distance is the minimum among them and gives the file name and 5 numbers related to personality characteristics to the primary process and finally the primary process For each line of the ```traits.csv``` file, one line is written in the output, which is the name of the file and 5 numbers that represent the shortest Euclidean distance with
It has the corresponding line in the ```traits.csv``` file.

Information from parent to child process through [Unnamed Pipe](https://techaccess.in/2021/05/16/un-named-pipes/) and information from The child is transferred to the father through Named Pipe.

## Run Project
Command to run the main program:
```
./main-process <traits-path> <users-path>
```

that ```main-process``` will be the name of your executable file, and instead of ```traits-path``` and ```users-path```, it should be the address of the file ```traits.csv```, and instead of ```users-path```, it should be the path of the users file folder.

Finally, in the ```results.csv``` file, for each line, the name of the file and the 5 personality factors that had the smallest Euclidean distance are written.

For example, if for a line in ```traits.csv```, among all users files, a line from ```users-2.csv``` with numbers 2, 3, 5, 1, 4 has the smallest Euclidean distance, its corresponding line in ```results.csv``` will be as follows:
```
users-2,2,3,5,1,4
```

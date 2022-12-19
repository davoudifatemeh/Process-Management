#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

void read_traits(vector <string> &traits, string file_path);
void _fork(string path, char* user_path, int fd[2]);
void save_result(vector<int> &fifoes, int num_of_process);
void write_pipe(int fd[2], string trait);

int main(int argc, char *argv[])
{
    vector <string> traits;
    vector <int> fifo_fds;
    int num_of_process = 0, fifo_fd;
    string traits_path(argv[1]);
    read_traits(traits, traits_path);
    num_of_process = traits.size();
    for (int i = 0; i < num_of_process; i++)
    {
        string path = "/tmp/fifo";
        string num = to_string(i);
        path += num;
        int fd[2];
        pipe(fd);
        _fork(path, argv[2], fd);
        mkfifo(path.c_str(), 0666);
        fifo_fd = open(path.c_str(), O_RDONLY);
        fifo_fds.push_back(fifo_fd);
        write_pipe(fd, traits[i]);
    }
    for (int i = 0; i < num_of_process; i++)
        wait(NULL);
    save_result(fifo_fds, num_of_process);
    exit(0);
}

void read_traits(vector <string> &traits, string file_path)
{
    fstream f;
    f.open(file_path, ios::in);
    string line;
    while (getline(f, line))
    {
        line[9] = '\0';
        traits.push_back(line);
    }
}

void _fork(string path, char* user_path, int fd[2])
{
    int id = fork();
    if (id == 0)
    {
        char fd0[16], fd1[16];
        sprintf(fd0, "%d", fd[0]);
        sprintf(fd1, "%d", fd[1]);
        char _path[512];
        strcpy(_path, path.c_str());
        char *args[]={fd0, fd1, _path, user_path, NULL};
        execvp("./trait", args);
    }
}

void save_result(vector<int> &fifoes, int num_of_process)
{
    ofstream result;
    result.open("results.csv");
    char buf[256];
    for (int i = 0; i < num_of_process; i++) 
    {
        read(fifoes[i], buf, 256);
        string data(buf);
        result << data;
        if (i != num_of_process - 1) result << "\n";
        close(fifoes[i]);
    }
    result.close();
}

void write_pipe(int fd[2], string trait)
{
    string message = "";
    message += trait;
    char _message[256];
    strcpy(_message, message.c_str());
    close(fd[0]);
    write(fd[1], _message, strlen(_message) + 1);
    close(fd[1]);
}
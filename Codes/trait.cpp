#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define MAX_INT 100000000

using namespace std;

void read_pipe(char *argv[], string &trait, int trait_arr[]);
void _fork(string path, string file_path, int fd[2]);
void write_pipe(int fd[2], string trait);
void read_fifo(vector<int> &fifoes, vector <vector<int>> &res, vector<string> &res_str);
int find_min(int trait_arr[], vector<vector<int>> res);
void write_fifo(string message, int fifo);

int main(int argc, char *argv[])
{
    string fifo_path(argv[2]);
    string user_files_path(argv[3]);
    user_files_path += "/";
    string trait;
    int trait_arr[5] = {0, 0, 0, 0, 0};
    vector <int> fifo_fds;
    int num_of_process = 0, myfifo_fd, fifo_fd;
    myfifo_fd = open(fifo_path.c_str(), O_WRONLY);
    read_pipe(argv, trait, trait_arr);
    struct dirent *de;
    DIR *dr = opendir(user_files_path.c_str());
    while((de = readdir(dr)) != NULL)
    {
        if ((de->d_name)[0] != '.')
        {
            string path = fifo_path;
            string num = to_string(num_of_process);
            path += num;
            string file_name(de->d_name);
            string file_path = user_files_path + file_name;
            num_of_process++;
            int fd[2];
            pipe(fd);
            _fork(path, file_path, fd);
            mkfifo(path.c_str(), 0666);
            fifo_fd = open(path.c_str(), O_RDONLY);
            fifo_fds.push_back(fifo_fd);
            write_pipe(fd, trait);
        }
    }
    closedir(dr);
    for (int i = 0; i < num_of_process; i++)
        wait(NULL);
    vector <vector<int>> res;
    vector<string> res_str;
    read_fifo(fifo_fds, res, res_str);
    int index = find_min(trait_arr, res);
    string message = res_str[index];
    write_fifo(message, myfifo_fd);
    exit(0);
}

void read_pipe(char *argv[], string &trait, int trait_arr[])
{
    char buf[256];
    int fd[2];
    fd[0] = atoi(argv[0]);
    fd[1] = atoi(argv[1]);
    close(fd[1]);
    read(fd[0], buf, 256);
    string data(buf);
    trait = data;
    for(int i = 0; i < data.length(); i += 2)
    {
        char num[2] = {data[i]};
        trait_arr[i/2] = atoi(num);
    }
    close(fd[0]);
}

void _fork(string path, string file_path, int fd[2])
{
    int id = fork();
    if (id == 0)
    {
        char fd0[16], fd1[16];
        sprintf(fd0, "%d", fd[0]);
        sprintf(fd1, "%d", fd[1]);
        char _path[128];
        char _file_path[128];
        strcpy(_path, path.c_str());
        strcpy(_file_path, file_path.c_str());
        char *args[]={fd0, fd1, _path, _file_path, NULL};
        execvp("./user", args);
    }
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

void read_fifo(vector<int> &fifoes, vector <vector<int>> &res, vector<string> &res_str)
{
    int num_of_process = fifoes.size();
    vector <int> temp;
    for(int i = 0; i < num_of_process; i++)
    {
        char buf[256];
        read(fifoes[i], buf, 256);
        string result(buf);
        res_str.push_back(result);
        string numbers = result.substr(result.find(",") + 1);
        temp.clear();
        for(int i = 0; i < numbers.length(); i += 2)
        {
            char num[2] = {numbers[i]};
            temp.push_back(atoi(num));
        }
        res.push_back(temp);
        close(fifoes[i]);
    }
}

int find_min(int trait_arr[], vector<vector<int>> res)
{
    int min = MAX_INT;
    int index = 0;
    for (int i = 0; i < res.size(); i++)
    {
        int number = 0;
        for(int j = 0; j < 5; j++)
        {
            number += (res[i][j] - trait_arr[j])*(res[i][j] - trait_arr[j]);
        }
        if (number < min)
        {
            min = number;
            index = i;
        }
    }
    return index;
}

void write_fifo(string message, int fifo)
{
    char _message[256];
    strcpy(_message, message.c_str());
    write(fifo, _message, strlen(_message) + 1);
    close(fifo);
}
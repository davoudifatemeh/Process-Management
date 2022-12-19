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

#define MAX_INT 100000000

using namespace std;

void read_pipe(char *argv[], int trait[]);
void read_traits(vector <string> &traits_str, vector <vector<int>> &traits, string file_path);
int find_min(vector<vector<int>> traits, int main_trait[]);
void creat_message(string file_path, vector<string> traits_str, int index, string &message_out);
void write_fifo(string message, int fifo);

int main(int argc, char *argv[])
{
    string fifo_path(argv[2]);
    string file_path(argv[3]);
    vector <string> traits_str;
    vector <vector<int>> traits;
    int main_trait[5] = {0, 0, 0, 0, 0};
    int myfifo_fd = open(fifo_path.c_str(), O_WRONLY);
    read_pipe(argv, main_trait);
    read_traits(traits_str, traits, file_path);
    int index = find_min(traits, main_trait);
    string message_out = "";
    creat_message(file_path, traits_str, index, message_out);
    write_fifo(message_out, myfifo_fd);
    exit(0);
}

void read_pipe(char *argv[], int trait[])
{
    char buf[256];
    int fd[2];
    fd[0] = atoi(argv[0]);
    fd[1] = atoi(argv[1]);
    close(fd[1]);
    read(fd[0], buf, 256);
    string data(buf);
    for(int i = 0; i < data.length(); i += 2)
    {
        char num[2] = {data[i]};
        trait[i/2] = atoi(num);
    }
    close(fd[0]);
}

void read_traits(vector <string> &traits_str, vector <vector<int>> &traits, string file_path)
{
    fstream f;
    f.open(file_path, ios::in);
    string line;
    int cnt = 0;
    vector<int> temp;
    while (getline(f, line))
    {
        line[9] = '\0';
        traits_str.push_back(line);
        temp.clear();
        for(int i = 0; i < line.length(); i += 2)
        {
            char num[2] = {line[i]};
            temp.push_back(atoi(num));
        }
        traits.push_back(temp);
    }
    f.close();
}

int find_min(vector<vector<int>> traits, int main_trait[])
{
    int min = MAX_INT;
    int index = 0;
    for (int i = 0; i < traits.size(); i++)
    {
        int number = 0;
        for(int j = 0; j < 5; j++)
        {
            number += (traits[i][j] - main_trait[j])*(traits[i][j] - main_trait[j]);
        }
        if (number < min)
        {
            min = number;
            index = i;
        }
    }
    return index;
}

void creat_message(string file_path, vector<string> traits_str, int index, string &message_out)
{
    message_out = file_path.substr(11, file_path.find(".") - 11) + "," + traits_str[index];
}

void write_fifo(string message, int fifo)
{
    char _message[256];
    strcpy(_message, message.c_str());
    write(fifo, _message, strlen(_message) + 1);
    close(fifo);
}
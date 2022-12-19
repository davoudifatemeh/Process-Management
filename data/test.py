import math
import random

numOfUsers = 5

def generateUsers():
    for k in range(1, numOfUsers):
        with open(f"users/users-{k}.csv", "w") as file:
            for i in range(3):
                string = ""
                for j in range(5):
                    string += str(random.randint(1, 5)) + ","

                string = string[:-1] + ("\n" if i != 2 else "")
                file.write(string)

def createGoldenModel():
    def calEuclideanDistance(li1, li2):
        dist = 0
        for i in range(len(li1)):
            dist += math.pow(li1[i] -li2[i], 2)
        return math.sqrt(dist)


    with open("traits.csv") as file:
        for line in file:
            minDistance = math.inf
            content = 0
            filenum = 0
            traits = list(map(int, line.split(",")))
            for i in range(1, numOfUsers):
                with open(f"users/users-{i}.csv") as usf:
                    for uline in usf:
                        distance = calEuclideanDistance(list(map(int, uline.split(","))), traits)
                        if distance < minDistance:
                            minDistance = distance
                            content = uline if uline[-1] != '\n' else uline[:-1] 
                            filenum = i 

            print(f"{content} filenum={filenum}")

generateUsers()

createGoldenModel()

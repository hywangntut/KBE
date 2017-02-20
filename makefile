CC = g++

CFLAGS = -lm -pthread -Ofast -march=native -Wall -funroll-loops -Wno-unused-result -static -std=c++11

OBJS = CorpusFile.o FileReader.o ICFile.o MethodInfo.o NeuralNetwork.o Parameter.o RCM.o ResourceFile.o VocabTable.o  Word2Vec.o

all: word2vec_joint

%.o : %.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

word2vec_joint : DoTrain.cpp $(OBJS)
	$(CC) DoTrain.cpp $(OBJS) -o word2vec_joint $(CFLAGS)

clean:
	rm -rf word2vec_joint *.o
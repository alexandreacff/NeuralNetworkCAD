#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

#ifndef RAND_MAX
#define RAND_MAX 32767 
#endif

//Função de ativação e sua derivada
double sigmoid(double x){ return 1 / (1 + exp(-x)); }
double dSigmoid(double x) { return x * (1 - x); }

//Inicializando todos os pesos e bias aleatoriamente entre 0 e 1
double init_weight(){ return ((double) rand())/ ((double)RAND_MAX);}

void shuffle(int* array, size_t n){
	if(n > 1){
		size_t i; 
		for(i = 0; i < n - 1; i++){
			size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
			int t = array[j]; 
			array[j] = array[i]; 
			array[i] = t;
		}
	}

}

int main(int argc, char** argv){
	clock_t start_time, end_time;
        double cpu_time_used;

	static const int numInputs = 55; 
	static const int numHiddenNodes = 4;
	static const int numOutputs = 1; 


	//Inicializando a rede neural
	//Definindo as dimensões das hidden layers
	double hiddenLayer[numHiddenNodes]; 
	double outputLayer[numOutputs]; 

	//Atribuindo os bias das hidden layers
	double hiddenLayerBias[numHiddenNodes];
	double outputLayerBias[numOutputs];

	//Definindo os pesos ocultos
	double hiddenWeights[numInputs][numHiddenNodes];
	double outputWeights[numHiddenNodes][numOutputs];

	//Inicializando os inputs e outputs do treinamento 
	static const int numTrainingSets = 4;
	//Definindo os inputs do treinamento 
	double training_inputs[][2] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} }; 

	double training_outputs[][1] = { {0.0f}, {1.0f}, {1.0f}, {0.0f} };  

	for(int i = 0; i < numInputs; i++){
		for(int j = 0; j < numHiddenNodes; j++){
			hiddenWeights[i][j] = init_weight(); 
		}
	}
	for(int i = 0; i < numHiddenNodes; i++){
		hiddenLayerBias[i] = init_weight(); 
		for(int j = 0; j < numOutputs; j++){
			outputWeights[i][j] = init_weight(); 
		}
	}

	for(int i = 0; i < numOutputs; i++){
		outputLayerBias[i] = init_weight(); 
	}
	
	int trainingSetOrder[] = {0,1,2,3};
	//Iterar sobre um número de épocas, a cada época escolhe um par de entradas e sua saída esperada
	const double lr = 15.2f; 
	int epochs = 100000; 
	for(int n = 0; n < epochs; n++){
		shuffle(trainingSetOrder, numTrainingSets);
		for(int x = 0; x < numTrainingSets; x++){
			
			//Inicializando a analise da FeedFoward
			//Calcula a saída da rede dados os pesos atuais tal que sigmoid(hiddenLayerBias + Sum(trainingInput_k * hiddenWeight))
			int i = trainingSetOrder[x]; 

			//Calcula a ativação da camada de ativação
			for(int j = 0; j < numHiddenNodes; j++){
				double activation = hiddenLayerBias[j]; 
				for(int k = 0; k < numInputs; k++){
					activation += training_inputs[i][k] * hiddenWeights[k][j]; 
				}
				hiddenLayer[j] = sigmoid(activation); 
			}

			//Calcula o output da camada de ativação
			for(int j = 0; j < numOutputs; j++){
				double activation = outputLayerBias[j]; 
				for(int k = 0; k < numHiddenNodes; k++){
					activation += hiddenLayer[k] * outputWeights[k][j]; 
				}
				outputLayer[j] = sigmoid(activation);

			}
			
			//Inicializando a análise do Backpropagation
			//Calcula o MSE (Mean Squared Error)
			double deltaOutput[numOutputs];
			for(int j = 0; j < numOutputs; j++){
				double dError = (training_outputs[i][j] - outputLayer[j]); 
				deltaOutput[j] = dError * dSigmoid(outputLayer[j]); 
			}
			
			//Calcula o MSE para os pesos ocultos
			double deltaHidden[numHiddenNodes]; 
			for(int j = 0; j < numHiddenNodes; j++){
				double dError = 0.0f; 
				for(int k = 0; k < numOutputs; k++){
					dError += deltaOutput[k] * outputWeights[j][k]; 
				}
			deltaHidden[j] = dError * dSigmoid(hiddenLayer[j]); 


			}

			//Aplica as mudanças nos pesos dos outputs
			for(int j = 0; j < numOutputs; j++){
				outputLayerBias[j] += deltaOutput[j] * lr;	
				for(int k = 0; k < numHiddenNodes; k++){
					outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j] * lr; 
				}
			}
		
			//Aplica as mudanças nos pesos ocultos
			for(int j = 0; j < numHiddenNodes; j++){
				hiddenLayerBias[j] += deltaHidden[j] * lr; 
				for(int k = 0; k < numInputs; k++){
					hiddenWeights[k][j] += training_inputs[i][k] * deltaHidden[j] * lr; 
				
				}
			}
		}
	}

	//Função de predição 
	double test_input[][2] = { {atof(argv[1]), atof(argv[2])} }; 

	//Calcula a ativação da camada externa oculta
	for(int j = 0; j < numHiddenNodes; j++){
		double activation = hiddenLayerBias[j];
		for(int k = 0; k < numInputs; k++){
			activation += test_input[0][k]*hiddenWeights[k][j]; 
		}
		hiddenLayer[j] = sigmoid(activation);
	}

	//Calcula o output da camada de ativação
	for(int j = 0; j < numOutputs; j++){
		double activation = outputLayerBias[j]; 
		for(int k = 0; k < numHiddenNodes; k++){
			activation += hiddenLayer[k] * outputWeights[k][j]; 
		}
		outputLayer[j] = sigmoid(activation);

	}

	printf("%f", outputLayer[0]);			
        end_time = clock();   // Registro do tempo final

        cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
        printf("Tempo de execucao: %f segundos\n", cpu_time_used);	
}

#include <stdio.h>

__device__ double sigmoid(double x){
	return 1.0f / (1.0f + exp(-x));
}

__device__ double dSigmoid(double x){
	return x * (1.0f - x); 
}

__global__ void forwardFeed(double* inputLayer, double* hiddenWeights, double* hiddenLayer, double* outputLayer, double* outputWeights, double* outputLayerBias, double* hiddenLayerBias, int numHiddenNodes, int numInputs, int numOutputs, int trainingSetIndex) {
    int i = trainingSetIndex;

    for (int j = blockIdx.x * blockDim.x + threadIdx.x; j < numHiddenNodes; j += blockDim.x * gridDim.x) {
        double activation = hiddenLayerBias[j];
        for (int k = 0; k < numInputs; k++) {
            activation += inputLayer[(i * numInputs) + k] * hiddenWeights[(j * numInputs) + k];
        }
        hiddenLayer[j] = sigmoid(activation);
    }

    for (int j = blockIdx.x * blockDim.x + threadIdx.x; j < numOutputs; j += blockDim.x * gridDim.x) {
        double activation = outputLayerBias[j];
        for (int k = 0; k < numHiddenNodes; k++) {
            activation += hiddenLayer[k] * outputWeights[j + (k * numOutputs)];
        }
        outputLayer[j] = sigmoid(activation);
    }
}

__global__ void backpropagate(double* trainingInputs, double* hiddenLayer, double* hiddenWeights, double* outputLayer, double* outputWeights, double* trainingOutputs, double* hiddenLayerBias, double* outputLayerBias, int numHiddenNodes, int numInputs, int numOutputs, int numTrainingSets, double lr) {

    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid < numTrainingSets) {
        int i = tid;

        double deltaOutput[1];

	//Calcula o Mean Squared Error (MSE) dos pesos do output
        for(int j = 0; j < numOutputs; j++){
           double dError = (trainingOutputs[i * numOutputs + j] - outputLayer[j]);
           deltaOutput[j] = dError * dSigmoid(outputLayer[j]);
        }

        double deltaHidden[4];
	//Calcula o MSE para o erro das camadas ocultas 
        for(int j = 0; j < numHiddenNodes; j++){
           double dError = 0.0f; 
           for(int k = 0; k < numOutputs; k++){
               dError += deltaOutput[k] * outputWeights[(j * 1) + k]; 
           }
           deltaHidden[j] = dError * dSigmoid(hiddenLayer[j]); 
        }

	//Aplica as mudanças dos pesos do output 
        for(int j = 0; j < numOutputs; j++){
           outputLayerBias[j] += deltaOutput[j] * lr;
           for(int k = 0; k < numHiddenNodes; k++){
               outputWeights[(k * numOutputs) + j] += hiddenLayer[k] * deltaOutput[j] * lr;
           }
        }

       //Aplicar as mudanças em pesos ocultos
       for(int j = 0; j < numHiddenNodes; j++){
           hiddenLayerBias[j] += deltaHidden[j] * lr; 
           for(int k = 0; k < numInputs; k++){
               hiddenWeights[(k * numOutputs) + j] += trainingInputs[(i * numInputs) + k] * deltaHidden[j] * lr; 
           }
       }
   }
}

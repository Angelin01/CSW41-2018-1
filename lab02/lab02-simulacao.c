#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "cmsis_os.h"
#include "TM4C129.h"
#include "grlib/grlib.h"

#include "cfaf128x128x16.h"

/*
	ESSE É O CÓDIGO-FONTE DE SIMULAÇÃO!
	Contém leves alterações (em vez de mostrar no display, usa printf, etc.)
	Caso queira rodar na placa, deve-se desabilitar o build para
	este arquivo (lab02-simulacao.c), e habilitar para o lab02.c.
	Lembre-se de alterar as configurações de target para utilizar
	a placa e não o simulador.
	Deve-se também trocar o STDOUT para o ITM no Manage Runtime Environment.
*/

/*
	Altere a macro abaixo para gerar output de chaves/mensagens ou diagrama de Gantt.
		false: gera output de chaves/mensagens no console.
		true: gera output do diagrama de Gantt no console.
*/
#define GENERATE_GANTT true

// *** Defina a mensagem desejada na macro MSG_TO_USE! ***

#define MSG_WORD_COUNT 35
#define MSG_TO_USE msg5

// Tempo
const int ticks_factor = 10000;

// Número da próxima thread a executar
int threadNum = 0;

// Sinaliza se é a primeira iteração
bool firstIter = true;

// Sinaliza se a mensagem foi decodificada com sucesso
bool deuboa = false;

// Mensagens codificadas
uint8_t msg1[4*MSG_WORD_COUNT] = {
	0x5a, 0x99, 0x01, 0x00, 0x66, 0x67, 0xfe, 0xff,
	0x76, 0x99, 0x01, 0x00, 0x68, 0x67, 0xfe, 0xff,
	0x7b, 0x99, 0x01, 0x00, 0x61, 0x67, 0xfe, 0xff,
	0x27, 0x99, 0x01, 0x00, 0x26, 0x67, 0xfe, 0xff,
	0x27, 0x99, 0x01, 0x00, 0x4c, 0x67, 0xfe, 0xff,
	0x68, 0x99, 0x01, 0x00, 0x67, 0x67, 0xfe, 0xff,
	0x7b, 0x99, 0x01, 0x00, 0x5a, 0x67, 0xfe, 0xff,
	0x75, 0x99, 0x01, 0x00, 0x5a, 0x67, 0xfe, 0xff,
	0x27, 0x99, 0x01, 0x00, 0x5f, 0x67, 0xfe, 0xff,
	0x6c, 0x99, 0x01, 0x00, 0x5a, 0x67, 0xfe, 0xff,
	0x7b, 0x99, 0x01, 0x00, 0x27, 0x67, 0xfe, 0xff,
	0x27, 0x99, 0x01, 0x00, 0x4b, 0x67, 0xfe, 0xff,
	0x76, 0x99, 0x01, 0x00, 0x5b, 0x67, 0xfe, 0xff,
	0x27, 0x99, 0x01, 0x00, 0x4d, 0x67, 0xfe, 0xff,
	0x6f, 0x99, 0x01, 0x00, 0x68, 0x67, 0xfe, 0xff,
	0x74, 0x99, 0x01, 0x00, 0x5a, 0x67, 0xfe, 0xff,
	0x7a, 0x99, 0x01, 0x00,
	0x8a, 0x65, 0x02, 0x00, 0x8e, 0xbf, 0xfe, 0xff  // Bytes de verificação
};

uint8_t msg2[4*MSG_WORD_COUNT] = {
	0x88, 0x20, 0xae, 0x8b, 0x33, 0xe0, 0x51, 0x74,
	0xa8, 0x20, 0xae, 0x8b, 0x2f, 0xe0, 0x51, 0x74,
	0xbc, 0x20, 0xae, 0x8b, 0xdd, 0xdf, 0x51, 0x74,
	0x85, 0x20, 0xae, 0x8b, 0x2f, 0xe0, 0x51, 0x74,
	0xa8, 0x20, 0xae, 0x8b, 0x1e, 0xe0, 0x51, 0x74,
	0xb7, 0x20, 0xae, 0x8b, 0x25, 0xe0, 0x51, 0x74,
	0x63, 0x20, 0xae, 0x8b, 0x16, 0xe0, 0x51, 0x74,
	0xb2, 0x20, 0xae, 0x8b, 0x32, 0xe0, 0x51, 0x74,
	0x63, 0x20, 0xae, 0x8b, 0x11, 0xe0, 0x51, 0x74,
	0xa4, 0x20, 0xae, 0x8b, 0x28, 0xe0, 0x51, 0x74,
	0xa8, 0x20, 0xae, 0x8b, 0xdd, 0xdf, 0x51, 0x74,
	0x70, 0x20, 0xae, 0x8b, 0xdd, 0xdf, 0x51, 0x74,
	0x97, 0x20, 0xae, 0x8b, 0x25, 0xe0, 0x51, 0x74,
	0xa8, 0x20, 0xae, 0x8b, 0xdd, 0xdf, 0x51, 0x74,
	0x93, 0x20, 0xae, 0x8b, 0x2c, 0xe0, 0x51, 0x74,
	0xaf, 0x20, 0xae, 0x8b, 0x26, 0xe0, 0x51, 0x74,
	0xa6, 0x20, 0xae, 0x8b,
	0x64, 0x30, 0x85, 0xd1, 0xbe, 0xdf, 0x51, 0x74  // Bytes de verificação
};

uint8_t msg3[4*MSG_WORD_COUNT] = {
	0x07, 0x6a, 0x00, 0x00, 0xb5, 0x96, 0xff, 0xff, 
	0x18, 0x6a, 0x00, 0x00, 0x6d, 0x96, 0xff, 0xff, 
	0xf7, 0x69, 0x00, 0x00, 0xbc, 0x96, 0xff, 0xff, 
	0x22, 0x6a, 0x00, 0x00, 0xbf, 0x96, 0xff, 0xff, 
	0x26, 0x6a, 0x00, 0x00, 0x6d, 0x96, 0xff, 0xff, 
	0xe0, 0x69, 0x00, 0x00, 0x6d, 0x96, 0xff, 0xff, 
	0xff, 0x69, 0x00, 0x00, 0xb6, 0x96, 0xff, 0xff, 
	0x1a, 0x6a, 0x00, 0x00, 0xb5, 0x96, 0xff, 0xff, 
	0x27, 0x6a, 0x00, 0x00, 0x6d, 0x96, 0xff, 0xff, 
	0x00, 0x6a, 0x00, 0x00, 0xc6, 0x96, 0xff, 0xff, 
	0xd3, 0x69, 0x00, 0x00, 0x93, 0x96, 0xff, 0xff, 
	0x1c, 0x6a, 0x00, 0x00, 0xbf, 0x96, 0xff, 0xff, 
	0x18, 0x6a, 0x00, 0x00, 0x6d, 0x96, 0xff, 0xff, 
	0xd3, 0x69, 0x00, 0x00, 0x6d, 0x96, 0xff, 0xff, 
	0xd3, 0x69, 0x00, 0x00, 0x6d, 0x96, 0xff, 0xff, 
	0xd3, 0x69, 0x00, 0x00, 0x6d, 0x96, 0xff, 0xff, 
	0xd3, 0x69, 0x00, 0x00,
	0x8c, 0x9e, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00 // Bytes de verificação
};

uint8_t msg4[4*MSG_WORD_COUNT] = {
	0xc5, 0x97, 0xd7, 0x17, 0xf7, 0x68, 0x28, 0xe8,
	0xd6, 0x97, 0xd7, 0x17, 0xaf, 0x68, 0x28, 0xe8, 
	0xb6, 0x97, 0xd7, 0x17, 0xf0, 0x68, 0x28, 0xe8, 
	0xd8, 0x97, 0xd7, 0x17, 0xfb, 0x68, 0x28, 0xe8, 
	0xd6, 0x97, 0xd7, 0x17, 0x02, 0x69, 0x28, 0xe8, 
	0x91, 0x97, 0xd7, 0x17, 0xbc, 0x68, 0x28, 0xe8, 
	0x91, 0x97, 0xd7, 0x17, 0xd7, 0x68, 0x28, 0xe8, 
	0xe0, 0x97, 0xd7, 0x17, 0x03, 0x69, 0x28, 0xe8, 
	0xd6, 0x97, 0xd7, 0x17, 0xfb, 0x68, 0x28, 0xe8, 
	0x91, 0x97, 0xd7, 0x17, 0xd2, 0x68, 0x28, 0xe8, 
	0xd2, 0x97, 0xd7, 0x17, 0xfb, 0x68, 0x28, 0xe8, 
	0xda, 0x97, 0xd7, 0x17, 0xf5, 0x68, 0x28, 0xe8, 
	0xe0, 0x97, 0xd7, 0x17, 0x01, 0x69, 0x28, 0xe8, 
	0xdf, 0x97, 0xd7, 0x17, 0xf8, 0x68, 0x28, 0xe8, 
	0xd2, 0x97, 0xd7, 0x17, 0xaf, 0x68, 0x28, 0xe8, 
	0x91, 0x97, 0xd7, 0x17, 0xaf, 0x68, 0x28, 0xe8, 
	0x91, 0x97, 0xd7, 0x17,
	0x29, 0x63, 0xc3, 0x23, 0x8f, 0x68, 0x28, 0xe8 // Bytes de verificação
};

uint8_t msg5[4*MSG_WORD_COUNT] = {
	0xcd, 0x4f, 0x46, 0xad, 0xe7, 0xb0, 0xb9, 0x52, 
	0x00, 0x50, 0x46, 0xad, 0xd8, 0xb0, 0xb9, 0x52, 
	0xf0, 0x4f, 0x46, 0xad, 0x95, 0xb0, 0xb9, 0x52, 
	0xde, 0x4f, 0x46, 0xad, 0xe5, 0xb0, 0xb9, 0x52, 
	0xfd, 0x4f, 0x46, 0xad, 0xde, 0xb0, 0xb9, 0x52, 
	0xf9, 0x4f, 0x46, 0xad, 0xdc, 0xb0, 0xb9, 0x52, 
	0xfe, 0x4f, 0x46, 0xad, 0xe9, 0xb0, 0xb9, 0x52, 
	0xf0, 0x4f, 0x46, 0xad, 0xda, 0xb0, 0xb9, 0x52, 
	0xf9, 0x4f, 0x46, 0xad, 0x95, 0xb0, 0xb9, 0x52, 
	0xb8, 0x4f, 0x46, 0xad, 0x95, 0xb0, 0xb9, 0x52, 
	0xcd, 0x4f, 0x46, 0xad, 0xe4, 0xb0, 0xb9, 0x52, 
	0xfd, 0x4f, 0x46, 0xad, 0xe3, 0xb0, 0xb9, 0x52, 
	0xab, 0x4f, 0x46, 0xad, 0xe9, 0xb0, 0xb9, 0x52, 
	0xfa, 0x4f, 0x46, 0xad, 0x95, 0xb0, 0xb9, 0x52, 
	0xdd, 0x4f, 0x46, 0xad, 0xea, 0xb0, 0xb9, 0x52, 
	0xf9, 0x4f, 0x46, 0xad, 0x95, 0xb0, 0xb9, 0x52, 
	0xab, 0x4f, 0x46, 0xad, 
	0x50, 0x77, 0xe9, 0x03, 0x75, 0xb0, 0xb9, 0x52 // Bytes de verificação
};

// Ponteiro para a mensagem (para acessar como 32 bits)
uint32_t* msg = (uint32_t*) MSG_TO_USE;

// Buffer para guardar a mensagem decodificada
uint32_t decoded[MSG_WORD_COUNT];

// Número de chave atual
uint32_t key;

// Número de primo anterior para o segundo teste
uint32_t previousPrime;

// Arquivo de log das threads
FILE* file;
int ticksFactor = 10000;

// Contexto da tela
tContext sContext;

// ---String para debug---
char string[12];

// Preenche o buffer com uma string que representa o número fornecido
static void intToString(int64_t value, char * pBuf, uint32_t len, uint32_t base, uint8_t zeros){
	static const char* pAscii = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	bool n = false;
	int pos = 0, d = 0;
	int64_t tmpValue = value;

	// the buffer must not be null and at least have a length of 2 to handle one
	// digit and null-terminator
	if (pBuf == NULL || len < 2)
			return;

	// a valid base cannot be less than 2 or larger than 36
	// a base value of 2 means binary representation. A value of 1 would mean only zeros
	// a base larger than 36 can only be used if a larger alphabet were used.
	if (base < 2 || base > 36)
			return;

	if (zeros > len)
		return;
	
	// negative value
	if (value < 0)
	{
			tmpValue = -tmpValue;
			value    = -value;
			pBuf[pos++] = '-';
			n = true;
	}

	// calculate the required length of the buffer
	do {
			pos++;
			tmpValue /= base;
	} while(tmpValue > 0);


	if (pos > len)
			// the len parameter is invalid.
			return;

	if(zeros > pos){
		pBuf[zeros] = '\0';
		do{
			pBuf[d++ + (n ? 1 : 0)] = pAscii[0]; 
		}
		while(zeros > d + pos);
	}
	else
		pBuf[pos] = '\0';

	pos += d;
	do {
			pBuf[--pos] = pAscii[value % base];
			value /= base;
	} while(value > 0);
}

// Retorna true se um número é primo, e false se não.
bool isPrime(unsigned int num) {
	unsigned int i;
	
    if (num < 2) {
        return false;
    }

    for (i = 2; i <= sqrt(num); i++) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}

// Thread 1: gera uma chave
void generateKey(void const* arg) {
	uint32_t time;
	uint32_t startKey;
	bool wrongStartGuess = false;
	
	while (true) {
		while (threadNum != 1) {
			osThreadYield();
		}
		if (deuboa) {
			// Finaliza thread
			threadNum = 2;
			return;
		}
		
		time = osKernelSysTick()/ticks_factor;
		
		if (firstIter) {
			key = (msg[0] & 0xFFFFFF00) - 1; // -1 para deixar ímpar (será feito key += 2 embaixo)
			startKey = key + 2; // Armazena a primeira chave testada
			
			firstIter = false;
		}
		
		// Gera chave
		if (!wrongStartGuess) {
			key += 2; // Tentativas do chute inicial (maioria das vezes).
		}
		else {
			key -= 2; // Só entra aqui se ocorreu carry na codificação
		}
		
		// Caso tenha ocorrido carry na codificação, o chute inicial não terá êxito.
		// Isso pode ser detectado se a chave passar do chute + 0x100
		// Caso isso ocorra, volta a chave para a primeira chave, e a partir daí,
		// a thread passa a fazer key -= 2.
		if (key >= (startKey + 0x100)) {
			wrongStartGuess = true;
			key = startKey - 2;
		}
		
		// ALTAMENTE IMPROVÁVEL, mas necessário para garantir que funciona para todos os casos
		// Caso a chave inicial seja 1, é necessário testar a única chave par possível (2)
		// Na próxima iteração, quando a chave for 4 (key += 2), voltar aos ímpares.
		// É melhor fazer essa checagem do que incrementar a chave de 1 em 1 toda vez.
		// (Fazer todo o processo para uma chave par diferente de 2 é desperdício).
		if (key == 1) {
			key = 2;
		}
		else if (key == 4) {
			key = 3;
		}
		
		if (GENERATE_GANTT) {
			printf("    generateKey: %d, %d\n", (int) time, (int) osKernelSysTick()/ticks_factor);
		}
		
		// Vai para a decodificação
		threadNum = 2;
	}
}

// Thread 2: faz a decodificação usando a chave, preenchendo o buffer "decoded"
void decodeMsg(void const* arg) {
	uint32_t time;
	uint8_t i;
	
	while (true) {
		while (threadNum != 2) {
			osThreadYield();
		}
		if (deuboa) {
			threadNum = 3;
			return;
		}
		
		time = osKernelSysTick()/ticks_factor;
		
		for (i = 0; i < MSG_WORD_COUNT - 2; i++) {
			if (i % 2 == 0) {
				decoded[i] = msg[i] - key;
			}
			else {
				decoded[i] = msg[i] + key;
			}
		}
		
		decoded[MSG_WORD_COUNT - 2] = msg[MSG_WORD_COUNT - 2] - key;
		decoded[MSG_WORD_COUNT - 1] = msg[MSG_WORD_COUNT - 1] + key;
		
		if (GENERATE_GANTT) {
			printf("    decodeMsg: %d, %d\n", (int) time, (int) osKernelSysTick()/ticks_factor);
		}
		
		// Vai para o primeiro teste
		threadNum = 3;
	}
}

// Thread 3: faz o teste da penúltima word (divisão por dois)
void testDivTwo(void const* arg) {
	uint32_t time;
	
	while (true) {
		while (threadNum != 3) {
			osThreadYield();
		}
		if (deuboa) {
			threadNum = 4;
			return;
		}
		
		time = osKernelSysTick()/ticks_factor;
		
		if (decoded[MSG_WORD_COUNT - 2] == key/2) {
			// Vai para o próximo teste
			threadNum = 4;
		}
		else {
			// Falhou, vai embora
			threadNum = 6;
		}
		
		if (GENERATE_GANTT) {
			printf("    testDivTwo: %d, %d\n", (int) time, (int) osKernelSysTick()/ticks_factor);
		}
	}
}

// Thread 4: faz o teste da última word (quadrado da chave dividido pelo primo anterior)
void testSquareDiv(void const* arg) {
	uint32_t time;
	
	while (true) {
		while (threadNum != 4) {
			osThreadYield();
		}
		if (deuboa) {
			threadNum = 5;
			return;
		}
		
		time = osKernelSysTick()/ticks_factor;
		
		previousPrime = key - 1;
		while (!isPrime(previousPrime)) {
			previousPrime--;
		}
		
		if (decoded[MSG_WORD_COUNT - 1] == key * key / previousPrime) {
			// Vai para o próximo teste
			threadNum = 5;
		}
		else {
			// Falhou, vai embora
			threadNum = 6;
		}
		
		if (GENERATE_GANTT) {
			printf("    testSquareDiv: %d, %d\n", (int) time, (int) osKernelSysTick()/ticks_factor);
		}
	}
}

// Thread 5: faz o teste de primalidade da chave
void testPrime(void const* arg) {
	uint32_t time;
	
	while (true) {
		while (threadNum != 5) {
			osThreadYield();
		}
		if (deuboa) {
			return;
		}
		
		time = osKernelSysTick()/ticks_factor;
		
		if (isPrime(key)) {
			// Deu boa
			deuboa = true;
		}
		
		if (GENERATE_GANTT) {
			printf("    testPrime: %d, %d\n", (int) time, (int) osKernelSysTick()/ticks_factor);
		}
		
		// Mostra resultados
		threadNum = 6;
	}
}

// Thread 6: Mostra os resultados no console
void printResult(void const* arg) {
	int i;
	int j;
	int pos;
	
	uint32_t time;
	
	while (true) {
		while (threadNum != 6) {
			osThreadYield();
		}
		
		time = osKernelSysTick()/ticks_factor;
		
		// Mostra a chave na tela
		if (!GENERATE_GANTT) {
			intToString(key, string, 10, 10, 0);
			printf("Chave");
			if (deuboa) {
				printf(" (FOUND): %s\n", string);
			}
			else {
				printf(" (FAIL): %s\n", string);
			}
		
			printf("Msg: ");
			for (i = 0; i < (MSG_WORD_COUNT - 2); i++) {
				printf("%c", (char) decoded[i]);
			}
			printf("\n\n");
		}
		
		if (deuboa) {
			if (GENERATE_GANTT) {
				printf("    printResult: %d, %d\n", (int) time, (int) osKernelSysTick()/ticks_factor);
			}
			else {
				intToString(osKernelSysTick()/ticks_factor, string, 10, 10, 0);
				printf("Tempo total de execução: %s\n", string);
			}
			
			threadNum = 1;
			osThreadTerminate(osThreadGetId());
		}
		else {
			if (GENERATE_GANTT) {
				printf("    printResult: %d, %d\n", (int) time, (int) osKernelSysTick()/ticks_factor);
			}
			
			threadNum = 1;
		}
		
	}
}

// Define as threads
osThreadDef(generateKey, osPriorityNormal, 1, 0);
osThreadDef(decodeMsg, osPriorityNormal, 1, 0);
osThreadDef(testDivTwo, osPriorityNormal, 1, 0);
osThreadDef(testSquareDiv, osPriorityNormal, 1, 0);
osThreadDef(testPrime, osPriorityNormal, 1, 0);
osThreadDef(printResult, osPriorityNormal, 1, 0);

// Função main: apenas faz inicializações
int main(void) {
	// Inicializa o kernel
	osKernelInitialize();
	
	// Cria as threads
	osThreadCreate(osThread(generateKey), NULL);
	osThreadCreate(osThread(decodeMsg), NULL);
	osThreadCreate(osThread(testDivTwo), NULL);
	osThreadCreate(osThread(testSquareDiv), NULL);
	osThreadCreate(osThread(testPrime), NULL);
	osThreadCreate(osThread(printResult), NULL);
	
	// Indica a primeira thread a executar
	threadNum = 1;
	
	osKernelStart();
	
	if (GENERATE_GANTT) {
		printf("gantt\n");
		printf("    title Lab 02\n");
		printf("    dateFormat x\n");
	}
	
	osDelay(osWaitForever);
}

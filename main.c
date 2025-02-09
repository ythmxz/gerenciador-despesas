#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


struct tm *atual;
time_t segundos;

int dia;
int mes;
int ano;

int hora;
int minuto;


FILE *p_arquivo;


int slot = 0;
int flag = 0;


typedef struct {

	char categoria[31];
	double valor;
	int dia;

} registro;


char *selecionarArquivo(int slot);
char *conteudoArquivo(char *nomeArquivo);
void salvarArquivo(registro *despesa, int *p_tamanho);
void carregarArquivo(registro **p_despesa, int *p_tamanho);

void criarRegistro(registro *despesa);
void exibirRegistro(registro *despesa, int *p_tamanho);

void adicionarDespesa(registro **p_despesa, int *p_tamanho);
void removerDespesa(registro **p_despesa, int *p_tamanho, int remover);
void editarDespesa(registro *despesa, int *p_tamanho, int editar);

double somarGastos(registro *despesa, int *p_tamanho);


int main() {

	int tamanho = 1;
	int *p_tamanho = &tamanho;

	const int remover = 0;
	const int editar = 0;

	registro *despesa = (registro*)malloc((*p_tamanho) * sizeof(registro));

	if ( despesa == NULL ) {

		printf("\nErro de alocação!\n");
		return 1;

	}

	registro **p_despesa = &despesa;

	ENTRADA_SLOT:

	printf("\nGERENCIADOR DE DESPESAS v3.0\n");

	printf("\nEscolha o slot que deseja acessar:\n");

	printf("\n0 - Sair\n");

	printf("\n1 - %s", conteudoArquivo("despesas1.bin"));
	printf("\n2 - %s", conteudoArquivo("despesas2.bin"));
	printf("\n3 - %s", conteudoArquivo("despesas3.bin"));

	printf("\n\nResposta: ");
	scanf("%d", &slot);

	if ( slot == 0 ) {

		free(despesa);
		return 0;

	}

	while ( slot < 0 || slot > 3 ) {

		printf("\nValor inválido!\n");
		goto ENTRADA_SLOT;

	}

	system("cls");

	carregarArquivo(p_despesa, p_tamanho);

	if ( flag == 1 ) {

		printf("\nOlá, seja bem-vindo(a) ao Gerenciador de Despesas! Para"
				"\ncomeçar, crie a sua primeira despesa:\n");

		criarRegistro(despesa);
		system("cls");
		salvarArquivo(despesa, p_tamanho);

	}

	int opcao = 1;

		ENTRADA_OPCAO:

		printf("\nMENU\n");

		printf("\n0 - Voltar\n\n1 - Adicionar Despesa\n2 - Remover Despesa"
				"\n3 - Editar Despesa\n4 - Exibir Registro\n5 - Excluir Arquivo\n");

		printf("\nResposta: ");
		scanf("%d", &opcao);

		while ( opcao < 0 || opcao > 5 ) {

			system("cls");
			printf("\nValor inválido!\n");

			goto ENTRADA_OPCAO;

		}

		system("cls");

		if ( opcao == 0 )
			goto ENTRADA_SLOT;

		else if ( opcao == 1 ) {

			adicionarDespesa(p_despesa, p_tamanho);
			salvarArquivo(despesa, p_tamanho);

		}

		else if ( opcao == 2 ) {

			removerDespesa(p_despesa, p_tamanho, remover);
			salvarArquivo(despesa, p_tamanho);

		}

		else if ( opcao == 3 ) {

			editarDespesa(despesa, p_tamanho, editar);
			salvarArquivo(despesa, p_tamanho);

		}

		else if ( opcao == 4 )
			exibirRegistro(despesa, p_tamanho);

		else if ( opcao == 5 ) {

			char nomeArquivo[15];

			if ( slot == 1 )
				strcpy(nomeArquivo, "despesas1.bin");

			else if ( slot == 2 )
				strcpy(nomeArquivo, "despesas2.bin");

			else if ( slot == 3 )
				strcpy(nomeArquivo, "despesas3.bin");

			if ( remove(nomeArquivo) == 0 )
				printf("\nArquivo excluído com sucesso!\n");
			else
				printf("\nErro ao excluír o arquivo!\n");

			goto ENTRADA_SLOT;

		}

	goto ENTRADA_OPCAO;

	free(despesa);
	return 0;

}


char *selecionarArquivo(int slot) {

	switch ( slot ) {

		case 1:
			return "despesas1.bin";

		case 2:
			return "despesas2.bin";

		case 3:
			return "despesas3.bin";

		default:
			return "despesas1.bin";

	}

}


char *conteudoArquivo(char *nomeArquivo) {

	int tamanho;

	char *quantidade = (char*)malloc(41 * sizeof(char));

	if ( quantidade == NULL ) {

		printf("\nErro de alocação!\n");
		exit(1);

	}

	p_arquivo = fopen(nomeArquivo, "rb");

	if ( p_arquivo == NULL ) {

		strcpy(quantidade, "Vazio");
		return quantidade;

	}

	fread(&tamanho, sizeof(int), 1, p_arquivo);

	fseek(p_arquivo, tamanho * sizeof(registro), SEEK_CUR);

	fread(&hora, sizeof(int), 1, p_arquivo);
	fread(&minuto, sizeof(int), 1, p_arquivo);
	fread(&dia, sizeof(int), 1, p_arquivo);
	fread(&mes, sizeof(int), 1, p_arquivo);
	fread(&ano, sizeof(int), 1, p_arquivo);

	if ( tamanho == 0 )
		strcpy(quantidade, "Vazio");

	else if ( tamanho == 1 )
		sprintf(quantidade, "%d Despesa | %02d:%02d (%02d/%02d/%04d)", tamanho, hora, minuto, dia, mes, ano);

	else if ( tamanho > 1 )
		sprintf(quantidade, "%d Despesas | %02d:%02d (%02d/%02d/%04d)", tamanho, hora, minuto, dia, mes, ano);

	else if ( tamanho < 0 ) {

		printf("\nErro ao carregar!\n");
		exit(1);

	}

	fclose(p_arquivo);

	return quantidade;

}


void salvarArquivo(registro *despesa, int *p_tamanho) {

	time(&segundos);
	atual = localtime(&segundos);

	dia = atual->tm_mday;
	mes = (atual->tm_mon + 1);
	ano = (atual->tm_year + 1900);

	hora = atual->tm_hour;
	minuto = atual->tm_min;

	char *p_nomeArquivo = selecionarArquivo(slot);
	char nomeArquivo[15];

	strcpy(nomeArquivo, p_nomeArquivo);

	p_arquivo = fopen(nomeArquivo, "wb");

	if ( p_arquivo == NULL ) {

		printf("\nErro ao abrir o arquivo!\n");
		exit(1);

	}

	fwrite(p_tamanho, sizeof(int), 1, p_arquivo);
	fwrite(despesa, sizeof(registro), *p_tamanho, p_arquivo);

	fwrite(&hora, sizeof(int), 1, p_arquivo);
	fwrite(&minuto, sizeof(int), 1, p_arquivo);
	fwrite(&dia, sizeof(int), 1, p_arquivo);
	fwrite(&mes, sizeof(int), 1, p_arquivo);
	fwrite(&ano, sizeof(int), 1, p_arquivo);

	if ( ferror(p_arquivo) ) {

		printf("\nErro ao escrever no arquivo!\n");
		exit(1);

	}

	printf("\nArquivo salvo!\n");

	fclose(p_arquivo);

}


void carregarArquivo(registro **p_despesa, int *p_tamanho) {

	char *p_nomeArquivo = selecionarArquivo(slot);
	char nomeArquivo[15];

	strcpy(nomeArquivo, p_nomeArquivo);

	p_arquivo = fopen(nomeArquivo, "rb");

	if ( p_arquivo == NULL ) {

		p_arquivo = fopen(nomeArquivo, "wb");

		if ( p_arquivo == NULL ) {

			printf("\nErro ao abrir o arquivo!\n");
			exit(1);

		}

		printf("\nArquivo criado!\n");

		flag = 1;

		fclose(p_arquivo);
		return;

	}

	fread(p_tamanho, sizeof(int), 1, p_arquivo);

	*p_despesa = (registro*)realloc(*p_despesa, (*p_tamanho) * sizeof(registro));

	if ( *p_despesa == NULL ) {

		printf("\nErro de realocação!\n");
		exit(1);

	}

	fread(*p_despesa, sizeof(registro), *p_tamanho, p_arquivo);

	if ( ferror(p_arquivo) ) {

		printf("\nErro ao ler o arquivo!\n");
		exit(1);

	}

	printf("\nArquivo carregado!\n");

	fclose(p_arquivo);

}


void criarRegistro(registro *despesa) {

	printf("\nCRIANDO REGISTRO\n");

	printf("\nDESPESA 1\n");

	printf("\nCategoria: ");
	scanf("%30s", despesa[0].categoria);

	printf("\nValor: R$");
	scanf("%lf", &despesa[0].valor);

	printf("\nDia: ");
	scanf("%d", &despesa[0].dia);

}


void exibirRegistro(registro *despesa, int *p_tamanho) {

	printf("\nEXIBINDO REGISTRO\n");

	for ( int i = 0; i < *p_tamanho; i++ ) {

		printf("\nDESPESA %d\n", (i + 1));

		printf("\nCategoria: %s", despesa[i].categoria);
		printf("\nValor: %.2lf", despesa[i].valor);
		printf("\nDia: %02d", despesa[i].dia);

	}

	printf("\n\nTOTAL GASTO: R$%.2lf\n", somarGastos(despesa, p_tamanho));

}


void adicionarDespesa(registro **p_despesa, int *p_tamanho) {

	int tamanhoAtual = *p_tamanho;
	int tamanhoNovo = (tamanhoAtual + 1);

	printf("\nADICIONANDO DESPESA\n");

	*p_despesa = (registro*) realloc(*p_despesa, tamanhoNovo * sizeof(registro));

	if ( *p_despesa == NULL ) {

		printf("\nErro de realocação!\n");
		exit(1);

	}

	printf("\nDESPESA %d\n", tamanhoNovo);

	printf("\nCategoria: ");
	scanf("%30s", (*p_despesa)[tamanhoAtual].categoria);

	printf("Valor: R$");
	scanf("%lf", &(*p_despesa)[tamanhoAtual].valor);

	printf("Dia: ");
	scanf("%d", &(*p_despesa)[tamanhoAtual].dia);

	*p_tamanho += 1;

}


void removerDespesa(registro **p_despesa, int *p_tamanho, int remover) {

	int tamanhoAtual = *p_tamanho;
	int tamanhoNovo = (tamanhoAtual - 1);

	printf("\nREMOVENDO DESPESA\n");

ENTRADA_REMOVER:

	printf("\nInsira a despesa que deseja remover: ");
	scanf("%d", &remover);

	if ( remover == 0 )
		return;

	while( remover < 0 || remover > *p_tamanho ) {

		printf("\nValor inválido!\n");
		goto ENTRADA_REMOVER;

	}

	remover -= 1;

	printf("\nDESPESA %d\n", (remover + 1));

	printf("\nCategoria: %s", (*p_despesa)[remover].categoria);
	printf("\nValor: R$%.2lf", (*p_despesa)[remover].valor);
	printf("\nDia: %02d", (*p_despesa)[remover].dia);

	for ( int i = remover; i < tamanhoNovo; i ++ )
		(*p_despesa)[i] = (*p_despesa)[i + 1];

	*p_despesa = (registro*)realloc(*p_despesa, tamanhoNovo * sizeof(registro));

	if ( *p_despesa == NULL ) {

		printf("\nErro de realocação!\n");
		exit(1);

	}

	*p_tamanho -= 1;

}


void editarDespesa(registro *despesa, int *p_tamanho, int editar) {

	printf("\nEDITANDO DESPESA\n");

ENTRADA_EDITAR:

	printf("\nInsira a despesa que deseja editar: ");
	scanf("%d", &editar);

	if ( editar == 0 )
		return;

	while( editar < 0 || editar > *p_tamanho ) {

		printf("\nValor inválido!\n");
		goto ENTRADA_EDITAR;

	}

	editar -= 1;

	printf("\nANTIGA DESPESA %d\n", (editar + 1));

	printf("\nCategoria: %s", despesa[editar].categoria);
	printf("\nValor: R$%.2lf", despesa[editar].valor);
	printf("\nDia: %02d\n", despesa[editar].dia);

	printf("\nNOVA DESPESA %d\n", (editar + 1));

	printf("\nCategoria: ");
	scanf("%30s", despesa[editar].categoria);

	printf("Valor: R$");
	scanf("%lf", &despesa[editar].valor);

	printf("Dia: ");
	scanf("%d", &despesa[editar].dia);

}


double somarGastos(registro *despesa, int *p_tamanho) {

	double soma = 0.0;

	for ( int i = 0; i < *p_tamanho; i++ )
		soma += despesa[i].valor;

	return soma;

}

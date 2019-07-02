/* 
 * File:   main.c
 * Author: alcides
 *
 * Created on 30 de Junho de 2018, 14:51
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct { //Cria o tipo/struct de jogada
    int p, i, j;
} Jogada;

typedef struct { //Cria o tipo/struct dos dados recebidos pelos formulários
    Jogada jogadas[9];
    Jogada ultJogada;
    int qtJogadas;
    int modo;
    int jogadorAtual;
    char* data;
} Dados;

int recebeJogador(Dados *dados) { //Determina qual jogador está jogando. Obs: o 0 (P1) sempre começa.
    if ((dados->qtJogadas - 1) % 2 == 0) {
        return 1;
    }
    return 0;
}

void recebeJogadaAtual(char *aux, Dados *dados) {
    char *p;
    p = strstr(aux, "p="); //Encontra a jogada atual enviada pelo navegador
    sscanf(p, "p=%d&x=%d&y=%d", &dados->ultJogada.p, &dados->ultJogada.i, &dados->ultJogada.j); //Lê as jogadas
}

void recebeModo(char *aux, Dados *dados) {
    char *p;
    p = strstr(aux, "modo="); //Encontra o modo de jogo
    sscanf(p, "modo=%d", &dados->modo); //Lê o modo de jogo

}

void recebeJogadas(char *aux, Dados *dados) {
    char *p;
    p = strstr(aux, "jogadas="); //Encontra as jogadas já existentes enviadas pelo navegador
    p += strlen("jogadas="); //Pula 8 posições na memória (tamanho 8*char bytes = 8 bytes)
    dados->qtJogadas = 0;
    while (sscanf(p, "%d_%d_%d-", &dados->jogadas[dados->qtJogadas].p, &dados->jogadas[dados->qtJogadas].i, &dados->jogadas[dados->qtJogadas].j) > 0) { //Lê as jogadas enquanto existirem jogadas para serem lidas
        p = strstr(p, "-") + 1;
        dados->qtJogadas++; //Incremente o contador de jogadas
    }
    dados->jogadas[dados->qtJogadas].p = dados->ultJogada.p; //Adiciona a jogada atual as jogadas existentes
    dados->jogadas[dados->qtJogadas].i = dados->ultJogada.i;
    dados->jogadas[dados->qtJogadas].j = dados->ultJogada.j;
    dados->qtJogadas++; //Incremente o contador de jogadas
}

void receberDados(Dados *dados) {
    if (dados->data != NULL) {
        recebeModo(dados->data, dados); //Chama a função para receber o modo de jogo
        recebeJogadaAtual(dados->data, dados); //Chama a função para receber a jogada atual
        recebeJogadas(dados->data, dados); //Chama a função para receber existentes
        if (dados->modo != 1) dados->jogadorAtual = recebeJogador(dados); //Chama a função para definir qual o jogador se o modo não for pvc
        else dados->jogadorAtual = 0;
    }
}

void atualizaTabuleiro(Dados dados, int *matriz) { //Função que atualiaza a matriz com base nas informações recebidas
    int i;
    for (i = 0; i < dados.qtJogadas; i++) {
        int l = dados.jogadas[i].i;
        int c = dados.jogadas[i].j;
        matriz[3 * l + c] = dados.jogadas[i].p;
    }
}

/*
0 1 2
3 4 5
6 7 8
 */
int acabou(int *matriz, Dados dados) { //Função que verifica se o jogo acabou: vitória de um dos jogadores ou empate.
    if (matriz[0] == matriz[1] && matriz[1] == matriz[2]) return matriz[0];
    else if (matriz[3] == matriz[4] && matriz[4] == matriz[5]) return matriz[3];
    else if (matriz[6] == matriz[7] && matriz[7] == matriz[8]) return matriz[6];
    else if (matriz[0] == matriz[3] && matriz[3] == matriz[6]) return matriz[0];
    else if (matriz[1] == matriz[4] && matriz[4] == matriz[7]) return matriz[1];
    else if (matriz[2] == matriz[5] && matriz[5] == matriz[8]) return matriz[2];
    else if (matriz[0] == matriz[4] && matriz[4] == matriz[8]) return matriz[0];
    else if (matriz[2] == matriz[4] && matriz[4] == matriz[6]) return matriz[2];
    else if (dados.qtJogadas == 9) {
        return 3;
    } else return -1;
}

void imprimeResultado(int status) { //Função que imprime o resultado baseado em quem foi o vencedor
    switch (status) {
        case 0: //p1
            printf("<meta http-equiv=\"refresh\" content=\"0; url=http://cap.dc.ufscar.br/~760479/jogoCAP/html/vitp1.html\" />");
            break;
        case 1: //p2
            printf("<meta http-equiv=\"refresh\" content=\"0; url=http://cap.dc.ufscar.br/~760479/jogoCAP/html/vitp2.html\" />");
            break;
        case 2: //pc
            printf("<meta http-equiv=\"refresh\" content=\"0; url=http://cap.dc.ufscar.br/~760479/jogoCAP/html/vitpc.html\" />");
            break;
        case 3: //empate
            printf("<meta http-equiv=\"refresh\" content=\"0; url=http://cap.dc.ufscar.br/~760479/jogoCAP/html/empate.html\" />");
            break;
    }
}

void imprimeJogo(int *matriz, Dados dados) {
    printf("<br/><br/>"); //Quebras de linha
    printf("<table id=\"tabuleiro\" align=\"center\">"); //Cria a table da matriz do jogo da velha

    int i, j;
    for (i = 0; i < 3; i++) {
        printf("<tr>");
        for (j = 0; j < 3; j++) { //Dentro do loop, imprime na posição da tabela (3x3) a imagem correspondente ao seu status atual.
            if (matriz[3 * i + j] == 0) {
                printf("<td><img src=\"http://cap.dc.ufscar.br/~760479/jogoCAP/img/X.png\" width=\"150px\" height=\"150px\"></td>");
            } else if (matriz[3 * i + j] == 1 || matriz[3 * i + j] == 2) {
                printf("<td><img src=\"http://cap.dc.ufscar.br/~760479/jogoCAP/img/O.png\" width=\"150px\" height=\"150px\"></td>");
            } else {
                printf("<td><img onclick=\"setPXY(%d,%d,%d);\" src=\"http://cap.dc.ufscar.br/~760479/jogoCAP/img/vazio.png\" width=\"150px\" height=\"150px\" style=\"opacity:0.7;\"></td>", dados.jogadorAtual, i, j);
            }
        }
        printf("</tr>");
    }

    printf("</table><br/></br>"); //Fim da tabela

    printf("<form method=\"GET\" id=\"formulario\" action=\"/~760479/cgi-bin/main.cgi\">"); //Imprime o formulário pelo qual as informações serão enviadas para o .cgi novamente
    printf("<input type=\"hidden\" id=\"modo\"name=\"modo\" value=\"%d\">", dados.modo);
    printf("<input type=\"hidden\" id=\"p\" name=\"p\"><br>");
    printf("<input type=\"hidden\" id=\"x\" name=\"x\"><br>");
    printf("<input type=\"hidden\" id=\"y\" name=\"y\"><br>");

    printf("<input type=\"hidden\" name=\"jogadas\" value=\"");

    for (i = 0; i < dados.qtJogadas; i++) {
        printf("%d_%d_%d-", dados.jogadas[i].p, dados.jogadas[i].i, dados.jogadas[i].j);
    }
    printf("\">");

    printf("</form>"); //Fim do formulário
}

Jogada sorteiaJogada(Dados dados, int *matriz) {
    srand(time(NULL));
    int posUsada[dados.qtJogadas];
    int i;
    for (i = 0; i < dados.qtJogadas; i++) {
        posUsada[i] = 3 * dados.jogadas[i].i + dados.jogadas[i].j;
    }
    int conseguiu = 0, x, y;
    while (!conseguiu) { //Sorteia uma jogada para uma posição da matriz que não foi utilizada ainda
        x = rand() % 3;
        y = rand() % 3;
        conseguiu = 1;
        int i;
        for (i = 0; i < dados.qtJogadas; i++) {
            if (posUsada[i] == 3 * x + y) conseguiu = 0;
        }
    }
    Jogada maq;
    maq.p = 2;
    maq.i = x;
    maq.j = y;
    return maq;
}

void atualizaJogadas(Dados *dados, int *matriz) {
    atualizaTabuleiro(*dados, matriz); //Chama a função para atualizar a matriz baseada nos dados atuais
    int status = acabou(matriz, *dados); //Chama a função que verifica se o jogo foi encerrado

    if (status >= 0) { //Verifica se o jogo acabou, se acabou chama a função para imprimir os resultados no navegador. 0 = p1, 1 = p2, 2 = pc, 3 = empate
        imprimeResultado(status);
        return;
    }
    if (dados->modo == 1) { //0 = pvp, 1 = pvc. Se for PVC -> Realiza a jogada do computador
        Jogada maquina = sorteiaJogada(*dados, matriz);
        dados->jogadas[dados->qtJogadas] = maquina;
        dados->qtJogadas++;
        atualizaTabuleiro(*dados, matriz);
    }
    if (status >= 0) { //Verifica se o jogo acabou após a jogada do PC (se modo = 1), se acabou chama a função para imprimir os resultados no navegador. 0 = p1, 1 = p2, 2 = pc, 3 = empate
        imprimeResultado(status);
        return;
    }
    imprimeJogo(matriz, *dados); //Chama a função para imprimir o jogo atual atualizado no navegador
}

void zeraMatriz(int *matriz) { //Função para adicionar valores diferentes e menores que 0 na matriz
    int i;
    for (i = 0; i < 9; i++) {
        matriz[i] = i - 10;
    }
}

int main(int argc, char** argv) {
    printf("%s%c%c\n", "Content-Type:text/html;charset=utf-8", 13, 10); //Imprime linha necessária para o cgi funcionar
    printf("<head><title>Jogo da velha</title>"); //Título da página
    printf(" <link rel=\"stylesheet\" type=\"text/css\""
            "href=\"http://cap.dc.ufscar.br/~760479/jogoCAP/css/jogo.css\"> "); //CSS
    printf("<meta charset=\"UTF-8\"></head>"); //Charset
    printf("<body>");
    Dados dados;
    dados.data = getenv("QUERY_STRING"); //Recebe dados do navegador
    int *matriz = calloc(9, sizeof (int)); //Aloca memória para a matriz que vai ser utilizada como a do jogo da velha

    zeraMatriz(matriz); //Chama função para zerar a matriz

    receberDados(&dados); //Chama a função para processar os dados recebidos pelo navegador

    if (dados.modo != 1) { //Imprime o jogador atual na telas
        int aux;
        if (dados.jogadorAtual == 0) aux = 1;
        else aux = 0;
        printf("<h1 align = \"center\"> Jogador %d: está na sua vez! </h1>", aux);
    } else {
        printf("<h1 align = \"center\"> O PC jogou, é a sua vez! </h1>");
    }

    atualizaJogadas(&dados, matriz); //Chama a função para gerenciar as jogadas

    free(matriz); //Libera a memória alocada para a matriz

    printf("<script type=\"text/javascript\">"); //Imprime o javascript utilizado
    printf("function setPXY(p,x,y) { document.getElementById(\"p\").value=p;document.getElementById(\"x\").value=x; document.getElementById(\"y\").value=y; document.getElementById(\"formulario\").submit();}");
    printf("</script>");
    printf("</BODY>");

    return (EXIT_SUCCESS);
}



#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<fcntl.h>
#include<termios.h>
#include<unistd.h>

int getch()
{
    struct termios oldattr, newattr;

    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

/* Lê um caractere diretamente do console sem buffer*/
int getche()
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}


/* Determina se uma tecla do teclado foi pressionada */
int kbhit()
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

void inicializa_matriz(int **matriz,int tam){
    int i, j;
    for(i = 0; i < tam; i++){
        for(j = 0; j < tam; j++){
            matriz[i][j] = 0;
        }
    }
}

void desenha_matriz(int **matriz,int tam){
    int i,j,k;
    for(i = 0; i < tam; i++){
        for(k = 0; k < 16*tam; k++)
            printf("-");
        printf("\n");
        printf("|");
        for(j = 0; j < tam; j++){
            printf("\t");
            if(matriz[i][j] != 0)
                printf("%d",matriz[i][j]);
            printf("\t|");
        }
        printf("\n");
    }
    for(k = 0; k < 16*tam; k++)
        printf("-");
    printf("\n");
}

int procura_2048(int **matriz, int tam){
    int i, j;
    for(i = 0; i < tam; i++){
        for(j = 0; j < tam; j++){
            if(matriz[i][j] == 2048)
                return 1;
        }
    }
    return 0;
}

void novo_valor(int **matriz, int tam){
    int newI,newJ;
    srand((unsigned)time(NULL));
    do{
        newI = rand()%tam;
        newJ = rand()%tam;
    }while(matriz[newI][newJ] != 0);
    matriz[newI][newJ] = 2;
}

int** copia_matriz(int **matriz,int tam){
    int **nova = (int**)malloc(tam*sizeof(int*)), i,j;
    for(i = 0; i < tam; i++){
        nova[i] = (int*)malloc(tam*sizeof(int));
    }
    for(i = 0; i < tam; i++){
        for(j = 0; j < tam; j++){
            nova[i][j] = matriz[i][j];
        }
    }
    return nova;
}

void desaloca_copia(int **copia, int tam){
    int i;
    for(i = 0; i < tam; i++){
        free(copia[i]);
    }
    free(copia);
}

int matriz_moveu(int **ant, int **nova, int tam){
    int i, j;
    for(i = 0; i < tam; i++){
        for(j = 0; j < tam; j++){
            if(ant[i][j] != nova[i][j])
                return 1;
        }
    }
    return 0;
}

void movimenta_direita(int **matriz,int tam, int *p){
    int i, j, a;
    for(i = 0; i < tam; i++){
        a = tam - 1;
        for(j = tam - 2; j >= 0; j--){
            if(matriz[i][a] == matriz[i][j]){
                *p += matriz[i][a] *= 2;
                matriz[i][j] = 0;
            }
            else if(matriz[i][j] != 0){
                a = j;
            }            
        }
        for(j = tam-2; j >= 0; j--){
            a = j;
            while(matriz[i][a+1] == 0 && a+1 < tam){
                matriz[i][a+1] = matriz[i][a];
                matriz[i][a] = 0;
                a++;
            }
        }
    }
}

void movimenta_baixo(int **matriz,int tam, int *p){
    int i, j, a;
    for(i = 0; i < tam; i++){
        a = tam - 1;
        for(j = tam - 2; j >= 0; j--){
            if(matriz[a][i] == matriz[j][i]){
                *p += matriz[a][i] *= 2;
                matriz[j][i] = 0;
            }
            else if(matriz[j][i] != 0){
                a = j;
            }            
        }
        for(j = tam-2; j >= 0; j--){
            a = j;
            while(a+1 < tam && matriz[a+1][i] == 0){
                matriz[a+1][i] = matriz[a][i];
                matriz[a][i] = 0;
                a++;
            }
        }
    }
}

void movimenta_esquerda(int **matriz,int tam, int *p){
    int i, j, a;
    for(i = 0; i < tam; i++){
        a = 0;
        for(j = 1; j < tam; j++){
            if(matriz[i][a] == matriz[i][j]){
                *p += matriz[i][a] *= 2;
                matriz[i][j] = 0;
            }
            else if(matriz[i][j] != 0){
                a = j;
            }            
        }
        for(j = 1; j < tam; j++){
            a = j;
            while(matriz[i][a-1] == 0 && a-1 >= 0){
                matriz[i][a-1] = matriz[i][a];
                matriz[i][a] = 0;
                a--;
            }
        }
    }
}

void movimenta_cima(int **matriz,int tam, int *p){
    int i, j, a;
    for(i = 0; i < tam; i++){
        a = 0;
        for(j = 1; j < tam; j++){
            if(matriz[a][i] == matriz[j][i]){
                *p += matriz[a][i] *= 2;
                matriz[j][i] = 0;
            }
            else if(matriz[j][i] != 0){
                a = j;
            }          
        }
        for(j = 1; j < tam; j++){
            a = j;
            while((a-1 >= 0) && (matriz[a-1][i] == 0)){
                matriz[a-1][i] = matriz[a][i];
                matriz[a][i] = 0;
                a--;
            }
        }
    }
}

int main(){
    int n = 4,**matriz,**nova;
    int inicial[4], i, fim = 0, score = 0;
    char movimento;

    //APOS LER N ALOCA MATRIZ
    matriz = (int**)malloc(n*sizeof(int*));
    for(i = 0; i < n; i++){
        matriz[i] = (int*)malloc(n*sizeof(int));
    }

    //INICIA MATRIZ COM VALORES INICIAS
    inicializa_matriz(matriz,n);
    srand((unsigned)time(NULL));
    for(i = 0; i < n; i++){
        inicial[i] = rand() % n;
    }
    matriz[inicial[0]][inicial[1]] = 2;
    matriz[inicial[2]][inicial[3]] = 2;
    
    //LOOP PRINCIPAL
    while(!fim){
        //DESENHA MATRIZ
        system("clear");
        printf("Pontuacao: %d\n",score);
        desenha_matriz(matriz,n);
        kbhit();
        movimento = getch();
        //scanf("\n%c",&movimento);
        nova = copia_matriz(matriz,n);
        switch(movimento){
            case 67:
                movimenta_direita(matriz,n, &score);
            break;
            case 68:
                movimenta_esquerda(matriz,n, &score);
            break;
            case 65:
                movimenta_cima(matriz,n, &score);
            break;
            case 66:
                movimenta_baixo(matriz,n, &score);
            break;
        }
        if(procura_2048(matriz,n)){
            fim = 1;
        }
        if(matriz_moveu(matriz,nova,n)){
            novo_valor(matriz,n);
        }
        
        desaloca_copia(nova,n);
    }

    printf("\nFIM DE JOGO, PONTUACAO: %d\n",score);
    
    //FREE
    for(i = 0; i < n; i++){
        free(matriz[i]);
    }
    free(matriz);

    return 0;
}
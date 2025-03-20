#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <stdbool.h>

typedef struct produto {
    int id;
    char nome[31];
    float pcusto;
    float pvenda;
    float estoque;
    int ativo;
    float lucro;
    char unidade[11];
} Produto;

typedef struct registrocompras {
    char data[11];
    int id;
    char nome[31];
    float qtdComprada;
    float pUnitario;
    float pTotal;
    char unidade[11];
    
} registroCompras;

typedef struct registrovenda {
    char data[11];
    int id;
    char nome[31];
    float qtdVendida;
    float total;
    float punitario;

}registroVendas;

typedef struct {
    int id;
    char nome[31];
    float qtdVendida;
    float subtotal;
    char unidade[11];
    float valor;
} Resumo;

typedef struct {
    int id;
    float total;
    char data [16];

} registroVendasTotal;

int i = 0;

#define arquivoProduto "produtos.bin"
#define arquivoEstoque "entradas.bin"
#define arquivoVenda "vendasitem.bin"
#define arquivoTotal "vendastotal.bin"

void adicionarProduto();
void venderProduto();
void balancoGeral();
void listarProduto();
void listarEstoque();
void procurarEstoque();
void listarVendasItem();
void listarVendasTotal();
void editarProduto();
void procurarProduto();
void inativarProduto();
void reativarProduto();
void entradaProdutos();
void doacao();
void menuPrincipal();
void menuFinanceiro();
void submenu();
void dataAtual();
void linha();
void menuOperador();
void excluirProduto();
void formatarString();

int contarProduto();
int contarVenda();

int main() {

    int login;
    char senha [21];
    char tentativa [21];
    char data[11];

    strcpy(senha, "admin");

    do{
        system("cls");
        dataAtual(data);
        for(i = 0; i <= 50; i++){
            printf("_");
        }
        printf("\n");
        printf("\n\tBem Vindo ao DuMato Hortifruti!\n");
        printf("\t\tDia %s\n", data);
        printf("\n\t\tLogin\n");
        printf("\n\t1 - Operador\n");
        printf("\n\t2 - Administrador\n");
        printf("\n\t3 - Sair\n");

        printf("\n\t\tInsira uma das opcoes: ");

        while (scanf("%d", &login) != 1) {
            printf("\n\tInsira uma opcao valida entre 1 e 3: ");
            while (getchar() != '\n');
        }

    switch(login){
        case 1:
            menuOperador();
            continue;
        break;

        case 2:
            printf("\n\t\tInsira a senha de administrador: ");
            scanf("%s20", tentativa);

            if(strcmp(tentativa, senha) == 0) {
                menuPrincipal();
            } else {
                system("cls");
                printf("\n\n\tPerdeu o que aqui? Senha Incorreta\n\n");
                system("pause");
            }
        break;

        case 3:
            return 0;
        break;

        default:
            system("cls");
            printf("\nNumero invalido, digite entre 1 e 3\n");
            system("pause");
            continue;
    }

    }while(login != 1 || login != 2 || login != 3);

    return 0;
}

void adicionarProduto() {

    Produto produto, produtoDuplicado;

    produto.id = contarProduto() + 1;
    produto.ativo = 1;
    produto.estoque = 0;
    produto.pvenda = 0;
    produto.pcusto = 0;

    FILE* file = fopen(arquivoProduto, "rb+");
    if (file == NULL) {
        fclose(file);
        file = fopen(arquivoProduto, "ab");
    }

    system("cls");

    printf("\n\t\tCadastro de Produto\n");
    printf("\n\tDigite apenas \"un\", \"kg\" ou \"lt\"\n");

    while (1) {

        printf("\n\t0 > Voltar\n\tInsira a unidade de medida do produto: ");
        scanf("%10s", produto.unidade);

        if(strcmp(produto.unidade, "0") == 0){
            return;
        }

        if (strcmp(produto.unidade, "un") == 0 || 
            strcmp(produto.unidade, "kg") == 0 || 
            strcmp(produto.unidade, "lt") == 0) {
            break;
        } else {
            printf("\n\tEntrada invalida, Insira apenas \"un\", \"kg\" ou \"lt\".\n");
        }

    }

    printf("\n\tInsira o nome do produto: ");
    getchar();
    fgets(produto.nome, 31, stdin);
    produto.nome[strcspn(produto.nome, "\n")] = '\0';


    while (fread(&produtoDuplicado, sizeof(produtoDuplicado), 1, file)) {
        if (strcmp(produtoDuplicado.nome, produto.nome) == 0) {

            printf("\nProduto ja registrado.\n\n");
            fclose(file);
            system("pause");
            system("cls");
            return;
        }
    }

    printf("\n\tOBS: escreva apenas o numero de 1 a 100(%%)\n");
    printf("\n\tInsira a margem de lucro do produto: ");

    while (scanf("%f", &produto.lucro) != 1 || produto.lucro < 1 || produto.lucro > 100) {
        printf("\n\tEntrada invalida. Insira um numero entre 1 e 100: ");
        while (getchar() != '\n');
    }
    
    fseek(file, 0, SEEK_END);
    fwrite(&produto, sizeof(produto), 1, file);
    
    fclose(file);

    printf("\n\tRegistrado com sucesso.\n\n");
    system("pause");
    system("cls");
}

void venderProduto(){

    Resumo resumo[100];
    Produto produto;
    registroVendas venda;
    registroVendasTotal vendaTotal;

    int idProduto, contador = -1;
    float total = 0, preco = 0, subtotal = 0, qtd = 0;
    bool encontrado = false, cancelar = false;
    char data[11];

    FILE* file = fopen(arquivoProduto, "rb+");
    if (file == NULL) {
        printf("\nNao foi possivel abrir o arquivo\n\n");
        system("pause");
        return;
    }    

    FILE* file2 = fopen(arquivoVenda, "ab");
    if (file2 == NULL) {
        printf("Nao foi possivel abrir o arquivo financeiro.bin");
        return;
    }    

    do{

        system("cls");
        listarProduto();
        printf("\n\t\t\t\tVENDAS\n\n");
        
        if(contador >= 0) {
            printf("\tID  NOME              QTD     UNIDADE  VALOR     SUBTOTAL\n\n");
            for (i = 0; i <= contador; i++) {

                char idStr[4];
                sprintf(idStr, "%d", resumo[i].id);
                formatarString(idStr, 3, 1);

                formatarString(resumo[i].nome, 17, 1);
                formatarString(resumo[i].unidade, 8, 1);

                char vendaStr[20];
                sprintf(vendaStr, "%.2f", resumo[i].valor);
                formatarString(vendaStr, 7, 1);

                char subtotalStr[20];
                sprintf(subtotalStr, "%.2f", resumo[i].subtotal);
                formatarString(subtotalStr, 7, 1);

                char qtdvendidaStr[20];
                sprintf(qtdvendidaStr, "%.2f", resumo[i].qtdVendida);
                formatarString(qtdvendidaStr, 7, 1);


                printf("\t|%s|%s|%s|%s|R$%s|R$%s\n", idStr, resumo[i].nome, qtdvendidaStr, resumo[i].unidade, vendaStr, subtotalStr);
            }
            printf("\n\t\t\tTOTAL = %.2f\n\n", total);
            linha();
        }

        printf("\n\t\t 0 > Finalizar Venda/Gravar");
        printf("\n\t\t-1 > Cancelar/Abandonar\n");
        printf("\n\t\tInsira o ID do produto: ");
        
        while (scanf("%d", &idProduto) != 1) {
            printf("\n\t\tEntrada invalida. Insira apenas numeros para o ID do produto: ");
            while (getchar() != '\n');
        }

        if (idProduto == -1) {
            cancelar = true;
            break;
        }

        if (idProduto != 0) {

            encontrado = false;

            fseek(file, 0, SEEK_SET); 

            while (fread(&produto, sizeof(produto), 1, file)) {

                if (produto.id == idProduto && produto.ativo == 1) {
                    encontrado = true;

                    printf("\n\t\tInsira a quantidade de [%s]: ", produto.nome);
                    while (scanf("%f", &qtd) != 1) {
                        printf("\n\t\tEntrada invalida. Insira apenas numeros: ");
                        while (getchar() != '\n');
                    }

                    if(qtd > produto.estoque){
                        printf("\n\t\tNao ha estoque suficiente deste produto\n\n");
                        system("pause");
                        continue;
                    }

                    preco = produto.pvenda;
                    total += qtd * preco;
                    subtotal = qtd * preco;

                    //Atualiza o estoque do produto
                    produto.estoque -= qtd;
                    
                    contador++;

                    //Grava no resumo da venda
                    resumo[contador].id = produto.id;
                    strcpy(resumo[contador].nome, produto.nome);
                    resumo[contador].qtdVendida = qtd;
                    resumo[contador].subtotal = subtotal;
                    strcpy(resumo[contador].unidade, produto.unidade);
                    resumo[contador].valor = produto.pvenda;

                    printf("\n\tSubtotal da Venda: R$%.2f\n\n", subtotal);
                    break;
                }
            }

            if (!encontrado) {
                printf("\n\nID nao encontrado...\n");
                system("pause");
                continue;
            }

        }        

        if (idProduto == 0) {
            
            // if (vendaTotal.total == 0) {
            //     printf("entrou no if");
            //     system("pause");
            //     fclose(file);
            //     fclose(file2);
            //     return;
            // }

            FILE* file3 = fopen(arquivoTotal, "ab");
             if (file3 == NULL) {
                printf("Nao foi possivel abrir o arquivo arquivoVendas.bin");
                return;
            }    

            //capturando variaveis pra gravar o total da venda
            vendaTotal.id = contarVenda() + 1;
            dataAtual(data);
            strcpy(vendaTotal.data, data);
            vendaTotal.total = 0;

            for (i = 0; i <= contador; i++) {

                fseek(file, 0, SEEK_SET); 

                idProduto = resumo[i].id;

                while (fread(&produto, sizeof(produto), 1, file)) {

                    if (produto.id == idProduto) {

                        //Atualiza o estoque do produto
                        produto.estoque -= resumo[i].qtdVendida;

                        //Esse bloco grava a venda no registro
                        dataAtual(data);
                        strcpy(venda.data, data);
                        venda.id = produto.id;
                        strcpy(venda.nome, resumo[i].nome);
                        venda.qtdVendida = resumo[i].qtdVendida;
                        venda.total = resumo[i].qtdVendida * produto.pvenda;
                        venda.punitario = produto.pvenda;

                        //salva a venda no registro
                        fwrite(&venda, sizeof(venda), 1, file2);

                        //atualizando produtos (estoque etc.)
                        fseek(file, -sizeof(produto), SEEK_CUR);
                        fwrite(&produto, sizeof(produto), 1, file);

                        vendaTotal.total += venda.total;

                        break;
                    }
                }
            }

            //grava o registro total
            fwrite(&vendaTotal, sizeof(vendaTotal), 1, file3);

            printf("\n\n\t\tVenda gravada com sucesso\n\n");
            system("pause");
            fclose(file);
            fclose(file2);
            fclose(file3);
            return;
        }

    }while(true);

    if (cancelar == false) {
        fclose(file2);
        fclose(file);
        printf("\n\tVenda no total de R$%.2f gravada com sucesso\n\n", total);
        system("pause");
    } else {
        printf("\n\t\tVenda Cancelada!\n\n");
        system("pause");
    }
}

void balancoGeral() {

    registroVendas vendas;
    registroCompras compras;

    float totalVendas = 0, totalCompras = 0, balanco = 0;

    FILE *file = fopen(arquivoVenda , "rb");
    if (file == NULL) {
        system("cls");
        printf("\nNao foi possivel abrir o arquivo\ntalvez financeiro.bin nao exista...\n");
        return;
    }

    while (fread(&vendas, sizeof(vendas), 1, file)) {
        totalVendas += vendas.total;
    }

    fclose(file);

    FILE *file2 = fopen(arquivoEstoque , "rb");
    if (file2 == NULL) {
        system("cls");
        printf("\nNao foi possivel abrir o arquivo\ntalvez estoque.bin nao exista...\n");
        return;
    }

    while (fread(&compras, sizeof(compras), 1, file)) {
        totalCompras += compras.pTotal;
    }
    fclose(file2);

    balanco = totalVendas - totalCompras;
    system("cls");
    linha();
    printf("\n\tFaturamento das Vendas: R$%.2f\n", totalVendas);
    printf("\n\tGastos com Estoque: R$%.2f\n", totalCompras);
    printf("\n\tBalanco: R$%.2f\n", balanco);
    linha();
    printf("\n");
    system("pause");
}

void listarProduto() {

    Produto produto;

    int total = 0, palavra = 0;

    FILE *file = fopen(arquivoProduto , "rb");
    if (file == NULL) {
        system("cls");
        printf("\nNao foi possivel abrir o arquivo\ntalvez produtos.bin nao exista...\n");
        return;
    }
        
    linha();
    printf("\n\n\t\t\t\t\tPRODUTOS ATIVOS");
    printf("\n\n\tID  NOME              UNIDADE   ESTOQUE     CUSTO         P.VENDA       MARGEM LUCRO\n\n");
    while (fread(&produto, sizeof(produto), 1, file)) {

        if(produto.ativo == 1) {
            
            char idStr[4];
            sprintf(idStr, "%d", produto.id);
            formatarString(idStr, 3, 1);

            formatarString(produto.nome, 15, 1);
            formatarString(produto.unidade, 8, 1);

            char estoqueStr[20];
            sprintf(estoqueStr, "%.2f", produto.estoque);
            formatarString(estoqueStr, 10, 1);

            char custoStr[20];
            sprintf(custoStr, "%.2f", produto.pcusto);
            formatarString(custoStr, 10, 1);

            char vendaStr[20];
            sprintf(vendaStr, "%.2f", produto.pvenda);
            formatarString(vendaStr, 10, 1);

            char lucroStr[20];
            sprintf(lucroStr, "%.2f%%", produto.lucro);
            formatarString(lucroStr, 10, 1);

            printf("\t|%s |%s |%s |%s |R$%s |R$%s |%s\n", idStr, produto.nome, produto.unidade, estoqueStr, custoStr, vendaStr, lucroStr);
            total++;
        }
    }

    fclose(file);
    linha();
    if(total == 1) {
        palavra = 1;
    }
    printf("\n\t\t\t\t\t%d %s no total\n", total, palavra ? "Registro" : "Registros");
    linha();
    printf("\n");
}

void listarVendasTotal() {

    registroVendasTotal venda;

    int total = 0, palavra = 0;

    FILE *file = fopen(arquivoTotal, "rb");
    if (file == NULL) {
        system("cls");
        printf("\nNao foi possivel abrir o arquivo\ntalvez arquivoTotal.bin nao exista...\n");
        return;
    }
        
    system("cls");
    linha();
    printf("\n\n\t\tTOTAL VENDAS");
    printf("\n\n\tDATA        ID  TOTAL\n\n");
    while (fread(&venda, sizeof(venda), 1, file)) {
            
            char idStr[4];
            sprintf(idStr, "%d", venda.id);
            formatarString(idStr, 3, 1);

            char totalStr[10];
            sprintf(totalStr, "%.2f", venda.total);
            formatarString(totalStr, 3, 1);

            formatarString(venda.data, 11, 1);

            printf("\t|%s|%s|%s\n", venda.data, idStr, totalStr);

            total++;
    }

    fclose(file);
    linha();
    if(total == 1) {
        palavra = 1;
    }
    printf("\n\t\t%d %s no total\n", total, palavra ? "Registro" : "Registros");
    linha();
    printf("\n");
}

void listarProdutoInativo() {

    Produto produto;

    int total = 0, palavra = 0;

    FILE *file = fopen(arquivoProduto , "rb");
    if (file == NULL) {
        system("cls");
        printf("\nNao foi possivel abrir o arquivo\ntalvez produtos.bin nao exista...\n");
        return;
    }
        
    linha();
    printf("\n\t\t\tProdutos Inativos:\n\n\tID   NOME            ESTOQUE    CUSTO      P. VENDA\n\n");
    while (fread(&produto, sizeof(produto), 1, file)) {

        if(produto.ativo == 0) {

            char idStr[4];
            sprintf(idStr, "%d", produto.id);
            formatarString(idStr, 4, 1);

            formatarString(produto.nome, 15, 1);

            char estoqueStr[20];
            sprintf(estoqueStr, "%.2f", produto.estoque);
            formatarString(estoqueStr, 10, 1);

            char custoStr[20];
            sprintf(custoStr, "%.2f", produto.pcusto);
            formatarString(custoStr, 10, 1);

            char vendaStr[20];
            sprintf(vendaStr, "%.2f", produto.pvenda);
            formatarString(vendaStr, 10, 1);

            printf("\t|%s|%s|%s|%s|%s\n", idStr, produto.nome, estoqueStr, custoStr, vendaStr);
            total++;
        }
    }
    fclose(file);
    linha();
    if(total == 1) {
        palavra = 1;
    }
    printf("\n\t\t\t%d %s no total\n", total, palavra ? "Registro" : "Registros");
    linha();
    printf("\n");
}

void listarEstoque() {

    registroCompras compras;

    FILE *file = fopen(arquivoEstoque , "rb");
    if (file == NULL) {
        system("cls");
        printf("\nNao foi possivel abrir o arquivo\ntalvez entradas.bin nao exista...\n");
        return;
    }

    system("cls");
    linha();
    printf("\n\t\tRegistro de Entradas\n");
    printf("\n\tDATA        ID NOME            MEDIDA      QTD        P UNIT       TOTAL\n\n");
    while (fread(&compras, sizeof(compras), 1, file)) {

        formatarString(compras.data, 10, 1);
        formatarString(compras.nome, 15, 1);
        formatarString(compras.unidade, 11, 1);

        char idStr[4];
        sprintf(idStr, " %d", compras.id);
        formatarString(idStr, 3, 1);

        char qtdcompradaStr[20];
        sprintf(qtdcompradaStr, "%.2f", compras.qtdComprada);
        formatarString(qtdcompradaStr,10, 1);

        char unitarioStr[20];
        sprintf(unitarioStr, "%.2f", compras.pUnitario);
        formatarString(unitarioStr,10, 1);

        char totalStr[20];
        sprintf(totalStr, "%.2f", compras.pTotal);
        formatarString(totalStr,10, 1);

        printf("\t|%s|%s|%s|%s|%s|R$%s|R$%s\n",compras.data, idStr, compras.nome, compras.unidade, qtdcompradaStr, unitarioStr, totalStr);
    }
    printf("\n");
    linha();
    printf("\n");
    fclose(file);
}

void listarVendasItem() {

    registroVendas venda;
    int nRegistros = 0, nProdutos = 0;
    float faturamento = 0;

    FILE *file = fopen(arquivoVenda , "rb");

    if (file == NULL) {
        system("cls");
        printf("\nNao foi possivel abrir o arquivo\ntalvez financeiro.bin nao exista...\n");
        return;
    }

    system("cls");

    linha();
    printf("\n\t\tRegistro de Vendas\n");
    printf("\n\tDATA        ID NOME              QTD        VALOR        TOTAL\n\n");

    while (fread(&venda, sizeof(venda), 1, file)) {

        formatarString(venda.data, 10, 1);
        formatarString(venda.nome, 15, 1);

        char idStr[4];
        sprintf(idStr, " %d", venda.id);
        formatarString(idStr, 3, 1);

        char qtdvendidaStr[20];
        sprintf(qtdvendidaStr, "%.2f", venda.qtdVendida);
        formatarString(qtdvendidaStr, 10, 1);

        char totalStr[20];
        sprintf(totalStr, "%.2f", venda.total);
        formatarString(totalStr, 10, 1);

        char punitarioStr[20];
        sprintf(punitarioStr, "%.2f", venda.punitario);
        formatarString(punitarioStr, 10, 1);

        printf("\t|%s|%s|%s|%s|R$%s|R$%s\n",venda.data, idStr, venda.nome, qtdvendidaStr, punitarioStr, totalStr);
        
        nRegistros++;
        nProdutos += venda.qtdVendida;
        faturamento += venda.total;
    }
    linha();
    printf("\n\tRegistros: %d\n", nRegistros);
    printf("\n\tProdutos Vendidos: %d\n", nProdutos);
    printf("\n\tFaturamento: R$%.2f\n", faturamento);

    linha();
    printf("\n");
    fclose(file);
}

void editarProduto() {

    int id_editar, encontrado = 0;
    Produto produto;
    
    FILE *file = fopen(arquivoProduto, "rb+");
    if (file == NULL) {
        printf("Nao foi possivel abrir o arquivo.\n");
        return;
    }

    system("cls");
    listarProduto();

    printf("\n\t0 > Voltar\n\tDigite o ID do produto que deseja editar: ");
    scanf("%d", &id_editar);

    if(id_editar == 0) {
        return;
    }

    while (fread(&produto, sizeof(produto), 1, file)) {

        if (produto.id == id_editar) {
            encontrado = 1;
            printf("\n\tNome atual: %s", produto.nome);
            printf("\n\tDigite o novo nome da produto: ");
            getchar();
            fgets(produto.nome, 31, stdin);
            produto.nome[strcspn(produto.nome, "\n")] = '\0';

            printf("\n\tUnidade atual: %s", produto.unidade);
            
            while (1) {

                printf("\n\tInsira a nova unidade de medida do produto: ");
                scanf("%10s", produto.unidade);

                if (strcmp(produto.unidade, "un") == 0 || 
                    strcmp(produto.unidade, "kg") == 0 || 
                    strcmp(produto.unidade, "lt") == 0) {
                    break;
                } else {
                printf("\n\tEntrada invalida, Insira apenas \"un\", \"kg\" ou \"lt\".\n");
                continue;
                }

            }
          
            printf("\n\tMargem de lucro atual: %2.f%%\n", produto.lucro);
            printf("\n\tDigite o novo lucro: ");
            while (scanf("%f", &produto.lucro) != 1 || produto.lucro < 1 || produto.lucro > 100) {
            printf("\n\tEntrada invalida. Insira um numero entre 1 e 100: ");
            while (getchar() != '\n');
            }
            
            fseek(file, -sizeof(produto), SEEK_CUR);
            fwrite(&produto, sizeof(produto), 1, file);
            printf("\n\tProduto atualizado com sucesso!\n");

            break;
        }
    }

    if (!encontrado) {
        system("cls");
        printf("\nID nao encontrado.\n");
    }
    fclose(file);
    system("pause");
    system("cls");
}

int contarProduto() {

    Produto produto;
    int id = 0;

    FILE *file = fopen(arquivoProduto , "rb");
    if (file == NULL) {
        return id;
    }

    while (fread(&produto, sizeof(produto), 1, file)) {
        id = produto.id;
    }
    fclose(file);

    return id;
}

int contarVenda() {

    registroVendasTotal vendas;
    int id = 0;

    FILE *file = fopen(arquivoTotal , "rb");
    if (file == NULL) {
        return id;
    }

    while (fread(&vendas, sizeof(vendas), 1, file)) {
        id = vendas.id;
    }

    fclose(file);
    return id;
}

void procurarProduto() {

    Produto produto;
    char nomeBusca[31];
    int encontrado = 0;

    FILE *file = fopen(arquivoProduto, "rb");
    if (file == NULL) {
        printf("Nao foi possivel abrir o arquivo.\n");
        return;
    }

    system("cls");
    linha();
    printf("\n\t0 > Voltar\n\tDigite o nome do produto que deseja procurar: ");
    getchar();
    fgets(nomeBusca, 31, stdin);
    
    nomeBusca[strcspn(nomeBusca, "\n")] = '\0';

    if(strcmp(nomeBusca, "0") == 0){
        return;
    }

    linha();
    printf("\n\t\tResultados da busca:\n\n");
    printf("\tID   NOME     ESTOQUE   CUSTO   P. VENDA   ATIVO\n\n");

    while (fread(&produto, sizeof(produto), 1, file)) {

        if (strstr(produto.nome, nomeBusca) != NULL) {
            printf("\t%d - %s - %.3f - R$%.2f - R$%.2f  -  %s\n", produto.id, produto.nome, produto.estoque, produto.pcusto, produto.pvenda ,produto.ativo ? "sim" : "nao");
            encontrado = 1;
        }
    }

    if (!encontrado) {
        system("cls");
        printf("\n\tNenhum produto encontrado com o nome \"%s\".\n", nomeBusca);
    }
    printf("\n");
    linha();

    fclose(file);
    system("pause");
    system("cls");
}

void inativarProduto() {

    int id_editar, encontrado = 0;
    Produto produto;
    
    FILE *file = fopen(arquivoProduto, "rb+");
    if (file == NULL) {
        printf("Nao foi possivel abrir o arquivo.\n");
        return;
    }

    system("cls");
    listarProduto();

    printf("\n\t0 > Voltar\n\tDigite o ID do produto que deseja inativar: ");
    scanf("%d", &id_editar);

    if(id_editar == 0) {
        return;
    }

    while (fread(&produto, sizeof(produto), 1, file)) {
        if (produto.id == id_editar && produto.ativo == 1) {
            encontrado = 1;
            
            produto.ativo = 0;
            // Volta o cursor no arquivo para o início da produto encontrada
            fseek(file, -sizeof(produto), SEEK_CUR);
            fwrite(&produto, sizeof(produto), 1, file);
            printf("\n\tproduto atualizada com sucesso!\n\n");
            break;
        }
    }

    if (!encontrado) {
        system("cls");
        printf("\nID nao encontrado.\n");
    }
    fclose(file);
    system("pause");
    system("cls");
}

void reativarProduto(){

    int id_editar, encontrado = 0;
    Produto produto;
    
    FILE *file = fopen(arquivoProduto, "rb+");
    if (file == NULL) {
        printf("Nao foi possivel abrir o arquivo.\n");
        return;
    }

    system("cls");
    listarProdutoInativo();

    printf("\n\t0 > Voltar\n\tDigite o ID do produto que deseja reativar: ");
    scanf("%d", &id_editar);

    if(id_editar == 0) {
        return;
    }

    while (fread(&produto, sizeof(produto), 1, file)) {

        if (produto.id == id_editar && produto.ativo == 0) {

            encontrado = 1;
            
            produto.ativo = 1;
            
            fseek(file, -sizeof(produto), SEEK_CUR);
            fwrite(&produto, sizeof(produto), 1, file);
            printf("\nproduto atualizada com sucesso!\n");
            break;
        }
    }

    if (!encontrado) {
        system("cls");
        printf("\nID nao encontrado.\n");
    }
    fclose(file);
    system("pause");
    system("cls");
}

void entradaProdutos() {

    Produto produto;
    registroCompras compras;

    int idProduto, encontrado = 0;
    float qtdEstoque, pTotal, totalAtual, totalNovo = 0;
    float estoqueAtual, estoqueNovo, totalGeral = 0;
    char Data[11];

    FILE* file = fopen(arquivoProduto, "rb+");
    if (file == NULL) {
        printf("Nao foi possivel abrir o arquivo");
        return;
    }

    system("cls");
    listarProduto();
    printf("\n\t\tEntrada de Estoque\n");
    printf("\n\t0 > Voltar\n\tInsira o ID do produto: ");
    scanf("%d", &idProduto);

    if(idProduto == 0) {
        return;
    }

    while (fread(&produto, sizeof(produto), 1, file)) {

        if (produto.id == idProduto) {
            encontrado = 1;

            printf("\n\tQual foi a quantidade adquirida?");
            while (scanf("%f", &qtdEstoque) != 1) {
                printf("\n\tEntrada invalida. Insira apenas numeros: ");
                while (getchar() != '\n');
            }
            printf("\n\tQual foi o total pago pelo lote?");
            while (scanf("%f", &pTotal) != 1) {
                printf("\n\tEntrada invalida. Insira apenas numeros: ");
                while (getchar() != '\n');
            }

            totalAtual = produto.estoque * produto.pcusto;
            
            totalNovo = pTotal;
            
            totalGeral = totalAtual + totalNovo;
            
            estoqueAtual = produto.estoque;
            
            estoqueNovo = qtdEstoque;

            produto.pcusto = totalGeral / (estoqueAtual + estoqueNovo);

            produto.estoque += qtdEstoque;
                                            
            produto.pvenda = produto.pcusto * (1 + produto.lucro / 100);

            fseek(file, -sizeof(produto), SEEK_CUR);
            fwrite(&produto, sizeof(produto), 1, file);

            dataAtual(Data);
            strcpy(compras.data, Data);
            compras.id = produto.id;
            strcpy(compras.nome, produto.nome);
            strcpy(compras.unidade, produto.unidade);
            compras.qtdComprada = qtdEstoque;
            compras.pUnitario = pTotal / qtdEstoque;
            compras.pTotal = pTotal;

            FILE* file2 = fopen(arquivoEstoque, "ab");
            if (file2 == NULL) {
            printf("Nao foi possivel abrir o arquivo");
            return;
            }
            fwrite(&compras, sizeof(compras), 1, file2);

            fclose(file2);
            printf("\n\tEstoque adicionado e registrado com sucesso!\n\n");
            break;
        }
    }

    if (!encontrado) {
        system("cls");
        printf("\n\tID nao encontrado.\n");
    }

    fclose(file);
    system("pause");
    system("cls");

}

void procurarEstoque() {

    registroCompras compras;
    char nomeBusca[31];
    int encontrado = 0;

    FILE *file = fopen(arquivoEstoque, "rb");
    if (file == NULL) {
        printf("Nao foi possivel abrir o arquivo, talvez estoque.bin nao exista.\n");
        return;
    }

    system("cls");
    linha();
    printf("\n\t0 > Voltar\n\tDigite o nome do produto que deseja procurar: ");
    getchar();
    fgets(nomeBusca, 31, stdin);
    
    nomeBusca[strcspn(nomeBusca, "\n")] = '\0';

    if(strcmp(nomeBusca, "0") == 0){
        return;
    }

    linha();
    printf("\n\t\tResultados da Busca:\n\n");
    printf("\n\tDATA       ID NOME            MEDIDA      QTD        P UNIT/KG    TOTAL\n\n");

    while (fread(&compras, sizeof(compras), 1, file)) {

        if (strstr(compras.nome, nomeBusca) != NULL) {
            
            formatarString(compras.data, 10, 1);
            formatarString(compras.nome, 15, 1);
            formatarString(compras.unidade, 11, 1);

            char idStr[4];
            sprintf(idStr, " %d", compras.id);
            formatarString(idStr, 3, 1);

            char qtdcompradaStr[20];
            sprintf(qtdcompradaStr, "%.2f", compras.qtdComprada);
            formatarString(qtdcompradaStr,10, 1);

            char unitarioStr[20];
            sprintf(unitarioStr, "%.2f", compras.pUnitario);
            formatarString(unitarioStr,10, 1);

            char totalStr[20];
            sprintf(totalStr, "%.2f", compras.pTotal);
            formatarString(totalStr,10, 1);

            printf("\t%s|%s|%s|%s|%s|R$%s|R$%s\n",compras.data, idStr, compras.nome, compras.unidade, qtdcompradaStr, unitarioStr, totalStr);

            encontrado = 1;

        }
    }

    if (!encontrado) {
        system("cls");
        printf("\nNenhum produto encontrado com o nome \"%s\".\n", nomeBusca);
    }
    printf("\n");
    fclose(file);
    linha();
    printf("\n");
    system("pause");
    system("cls");
}

void menuPrincipal() {

int opcao = -1;
char data[11];

    do {

        system("cls");
        dataAtual(data);
        for(i = 0; i <= 50; i++){
            printf("_");
        }
        printf("\n");
        printf("\n\tBem Vindo ao DuMato Hortifruti!\n");
        printf("\t\tDia %s\n", data);
        printf("\n\t\tMenu Principal\n");
        printf("\n\t1 - Vendas\n");
        printf("\n\t2 - Produtos\n");
        printf("\n\t3 - Adicionar Entradas\n");
        printf("\n\t4 - Doacoes\n");
        printf("\n\t5 - Painel Financeiro\n");
        printf("\n\t0 - Logout\n");
        printf("\n\nDigite uma Opcao: ");
    
        while (scanf("%d", &opcao) != 1) {
            printf("\n\tInsira uma opcao valida entre 0 e 5: ");
            while (getchar() != '\n');
        }

        switch(opcao){
            case 1:
                venderProduto();
            break;

            case 2:
                submenu();
            break;

            case 3:
                entradaProdutos();
            break;

            case 4:
                doacao();
            break;

            case 5:
                menuFinanceiro();
            break;

            case 0:
                return;
            break;

            default:
                system("cls");
                printf("\nNumero invalido, digite entre 0 e 5\n");
                system("pause");
                continue;
            break;
        }

    opcao = -1;

    } while (opcao != 0);
}

void menuFinanceiro() {

    int opcao = -1;
    char data[11];

    do{
        system("cls");
        dataAtual(data);
        for(i = 0; i <= 50; i++){
            printf("_");
        }
        printf("\n");
        printf("\n\tBem Vindo ao DuMato Hortifruti!\n");
        printf("\t\tDia %s\n", data);
        printf("\n\t\tPainel Financeiro\n");
        printf("\n\t1 - Registro das Vendas por item\n");
        printf("\n\t2 - Registro das Entradas\n");
        printf("\n\t3 - Pesquisar Compras de Estoque\n");
        printf("\n\t4 - Balanco Financeiro\n");
        printf("\n\t5 - Registro das vendas total\n");
        printf("\n\t0 - Voltar ao Menu Principal\n");
        printf("\n\nEscolha uma Opcao:");

        while (scanf("%d", &opcao) != 1) {
            printf("\n\tEntrada invalida. Insira um numero entre 0 e 4: ");
            while (getchar() != '\n');
        }

        switch(opcao){
            case 1:
                listarVendasItem();
                system("pause");
                system("cls");
            break;

            case 2:
                listarEstoque();
                system("pause");
                system("cls");
            break;

            case 3:
                procurarEstoque();
            break;

            case 4:
               balancoGeral();
            break;

            case 5:
               listarVendasTotal();
               system("pause");
               system("cls");
            break;

            case 0:
                return;
            break;

            default:
                system("cls");
                printf("\nNumero invalido, digite entre 0 e 5\n");
                system("pause");
            continue;

        }
        opcao = -1;
    }while(opcao != 0);
}

void doacao() {

    Produto produto;
    int id, encontrado = 0;
    float qtd;

    FILE *file = fopen(arquivoProduto , "rb+");
    if (file == NULL) {
        system("cls");
        printf("\nNao foi possivel abrir o arquivo\ntalvez produtos.bin nao exista...\n");
        return;
    }

    system("cls");
    listarProduto();

    printf("\n\t0 > Voltar\n\tInsira o ID do produto que deseja doar: ");
    while (scanf("%d", &id) != 1) {
        printf("\n\tEntrada invalida. Insira apenas numeros: ");
        while (getchar() != '\n');
    }

    if(id == 0) {
        return;
    }

    while (fread(&produto, sizeof(produto), 1, file)) {
        if (produto.id == id) {
            encontrado = 1;

            printf("\n\tInsira a quantidade para doar: ");
            while (scanf("%f", &qtd) != 1) {
                printf("\n\tEntrada invalida. Insira apenas numeros: ");
                while (getchar() != '\n');
            }

            if(qtd > produto.estoque) {
                printf("\n\tNao ha estoque suficiente para essa doacao\n");
                system("pause");
                return;
            }

            produto.estoque -= qtd;

            fseek(file, -sizeof(produto), SEEK_CUR);
            fwrite(&produto, sizeof(produto), 1, file);
            printf("\n\tProduto doado com sucesso.\n\tTodos os alimentos doados, sao encaminhados a ONG\n\tparceira do estabelecimento --> Anjos da Rua\n\n");
            break;
        }
    }

    if (!encontrado) {
        system("cls");
        printf("\nID nao encontrado.\n");
    }
    fclose(file);
    system("pause");
    system("cls");
}

void submenu() {

    int opcao = -1;
    char data[11];

    do{
        system("cls");
        dataAtual(data);
        for(i = 0; i <= 50; i++){
            printf("_");
        }
        printf("\n");
        printf("\n\tBem Vindo ao DuMato Hortifruti!\n");
        printf("\t\tDia %s\n", data);
        printf("\n\t\tMenu Produtos\n");
        printf("\n\t1 - Cadastrar Novo Produto\n");
        printf("\n\t2 - Exibir Ativos\n");
        printf("\n\t3 - Buscar\n");
        printf("\n\t4 - Editar\n");
        printf("\n\t5 - Inativar Produtos\n");
        printf("\n\t6 - Exibir inativos\n");
        printf("\n\t7 - Reativar Produtos\n");
        printf("\n\t8 - Excluir Produtos\n");
        printf("\n\t0 - Voltar ao Menu Principal\n");
        printf("\n\nEscolha uma Opcao:");

        while (scanf("%d", &opcao) != 1) {
            printf("\n\tInsira uma opcao valida entre 0 e 7: ");
            while (getchar() != '\n');
        }

        switch(opcao){
            case 1:
                adicionarProduto();
            break;

            case 2:
                system("cls");
                listarProduto();
                system("pause");
                system("cls");
            break;

            case 3:
                procurarProduto();
            break;

            case 4:
                editarProduto();
            break;

            case 5:
                inativarProduto();
            break;

            case 6:
                system("cls");
                listarProdutoInativo();
                system("pause");
                system("cls");
            break;

            case 7:
                reativarProduto();
            break;

             case 8:
                excluirProduto();
            break;

            case 0:
                return;
            break;

            default:
                system("cls");
                printf("\nNumero invalido, digite entre 0 e 7\n");
                system("pause");
            continue;

        }

        opcao = -1;

    }while(opcao != 0);
}

void dataAtual(char *data) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(data, 11, "%d/%m/%Y", tm_info);
}

void linha(){

    for(i = 0; i <= 100; i++){
        printf("_");
    }
    printf("\n");
}

void menuOperador() {
    int opcao;
    char data[11];

    system("cls");

    do{
        system("cls");
        dataAtual(data);
        for(i = 0; i <= 50; i++){
            printf("_");
        }
        printf("\n");
        printf("\n\tBem Vindo ao DuMato Hortifruti!\n");
        printf("\t\tDia %s\n", data);
        printf("\n\t\tMenu Principal\n\n");
        printf("\n\t1 - Vendas\n");
        printf("\n\t2 - Logout\n");
        printf("\n\t\tInsira uma das opcoes: ");

        while (scanf("%d", &opcao) != 1) {
            printf("\n\tInsira uma opcao valida entre 1 e 2: ");
            while (getchar() != '\n');
        }

        switch(opcao) {
            case 1:
                venderProduto();
                system("cls");
            break;

            case 2:
                system("cls");
                return;
            break;

            default:
                system("cls");
                printf("\nNumero invalido, digite entre 1 e 2\n\n");
                system("pause");
            continue;
        }
    }while(opcao != 2 || opcao != 1);
}

void excluirProduto() {
    Produto produto;
    Produto produtosAtualizados[300];
    int idProduto, encontrado = 0, count = 0;

    FILE *file = fopen(arquivoProduto, "rb");
    if (file == NULL) {
        printf("\nNao foi possivel abrir o arquivo\ntalvez produtos.bin nao exista...\n");
        return;
    }

    system("cls");
    listarProduto();

    printf("\n\t0 > Voltar\n\tInsira o ID do produto que deseja excluir: ");
    while (scanf("%d", &idProduto) != 1) {
        printf("\n\tInsira um ID de produto valido: ");
        while (getchar() != '\n'); 
    }

    if(idProduto == 0) {
        return;
    }

    while (fread(&produto, sizeof(Produto), 1, file)) {
        if (produto.id == idProduto) {
            encontrado = 1;
        } else {
            produtosAtualizados[count++] = produto;
        }
    }

    fclose(file);

    if (!encontrado) {
        printf("\nID nao encontrado.\n");
        return;
    }

    FILE *file2 = fopen(arquivoProduto, "wb");
    if (file2 == NULL) {
        printf("\nNao foi possivel abrir o arquivo\ntalvez produtos.bin nao exista...\n");
        return;
    }

    fwrite(produtosAtualizados, sizeof(Produto), count, file2);
    fclose(file2);

    printf("\n\tProduto excluido com sucesso.\n\n");
    system("pause");
}

void formatarString(char *str, int largura, int alinhamento) {

    int len = strlen(str);

    if (len < largura) {

        int espacos = largura - len;
        
        if (alinhamento == 1) {

            for (int i = len; i < largura; i++) {
                str[i] = ' ';
            }
            
            str[largura] = '\0'; 
        }
        else if (alinhamento == 2) { 

            memmove(str + espacos, str, len + 1);
            for (int i = 0; i < espacos; i++) {
                str[i] = ' ';
            }
        }
        else if (alinhamento == 3) {
            
            int espacos_esquerda = espacos / 2;
            int espacos_direita = espacos - espacos_esquerda;
            
            memmove(str + espacos_esquerda, str, len + 1);
            
            for (int i = 0; i < espacos_esquerda; i++) {
                str[i] = ' ';
            }
        
            for (int i = len + espacos_esquerda; i < largura; i++) {
                str[i] = ' ';
            }
        }
    }
}
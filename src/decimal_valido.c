#include <stdbool.h>
#include <stdio.h>

#include "usb_serial.h"

typedef enum
{
    ESTADO_AGUARDANDO = 0,
    ESTADO_SINAL,
    ESTADO_INTEIRO,
    ESTADO_PONTO,
    ESTADO_FRACAO,
    ESTADO_ERRO
} estado_t;

estado_t transicao(estado_t estado_atual, int char_lido)
{
    switch (estado_atual)
    {
    case ESTADO_AGUARDANDO:
        if (char_lido == '+' || char_lido == '-')
        {
            return ESTADO_SINAL;
        }
        else if (char_lido >= '0' && char_lido <= '9')
        {
            return ESTADO_INTEIRO;
        }
        break;

    case ESTADO_SINAL:
        if (char_lido >= '0' && char_lido <= '9')
        {
            return ESTADO_INTEIRO;
        }
        break;

    case ESTADO_INTEIRO:
        if (char_lido >= '0' && char_lido <= '9')
        {
            return ESTADO_INTEIRO;
        }
        else if (char_lido == '.')
        {
            return ESTADO_PONTO;
        }
        break;

    case ESTADO_PONTO:
        if (char_lido >= '0' && char_lido <= '9')
        {
            return ESTADO_FRACAO;
        }
        break;

    case ESTADO_FRACAO:
        if (char_lido >= '0' && char_lido <= '9')
        {
            return ESTADO_FRACAO;
        }
        break;
    }

    printf("\r\nEntrada inválida!\r\n> ");

    return ESTADO_AGUARDANDO;
}

int main(void)
{
    usb_serial_init();

    printf("\r\nNUMERO DECIMAL VALIDO\r\n");
    printf("Digite um numero em decimal e pressione Enter.\r\n> ");

    estado_t estado = ESTADO_AGUARDANDO;
    bool ignorar_lf = false;

    while (1)
    {
        int ch = usb_serial_getchar();

        if (ch < 0)
        {
            continue;
        }

        // Ignora o LF depois de CR
        if (ignorar_lf && ch == '\n')
        {
            ignorar_lf = false;
            continue;
        }

        ignorar_lf = false;

        // Enter encerra a entrada
        if (ch == '\r' || ch == '\n')
        {
            if (ch == '\r')
            {
                ignorar_lf = true;
            }

            switch (estado)
            {
            case ESTADO_INTEIRO:
            case ESTADO_FRACAO:
                printf("\r\nVALIDO\r\n");
                break;

            case ESTADO_ERRO:
                // Ja foi informado assim que o erro ocorreu
                printf("\r\n");
                break;

            case ESTADO_AGUARDANDO:
                printf("\r\nINVALIDO: entrada vazia.\r\n");
                break;

            case ESTADO_SINAL:
            case ESTADO_PONTO:
            default:
                printf("\r\nINVALIDO: numero incompleto.\r\n");
                break;
            }

            estado = ESTADO_AGUARDANDO;
            printf("> ");
            continue;
        }

        estado = transicao(estado, ch);
    }
}
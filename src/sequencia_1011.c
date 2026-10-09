#include <stdbool.h>
#include <stdio.h>

#include "usb_serial.h"

typedef enum
{
    AGUARDANDO = 0,
    PRIMEIRO_BIT,
    SEGUNDO_BIT,
    TERCEIRO_BIT
} estado_t;

volatile int ocorrencias = 0;

estado_t transicao(estado_t estado_atual, int char_lido)
{
    if (estado_atual == AGUARDANDO) // -
    {
        return char_lido == '0' ? AGUARDANDO : PRIMEIRO_BIT; // 0 : 1
    }
    if (estado_atual == PRIMEIRO_BIT) // 1
    {
        return char_lido == '0' ? SEGUNDO_BIT : PRIMEIRO_BIT; // 10 : 11
    }
    if (estado_atual == SEGUNDO_BIT) // 10
    {
        return char_lido == '0' ? AGUARDANDO : TERCEIRO_BIT; // - : 101
    }
    // só pode ser TERCEIRO_BIT
    // 101
    if (char_lido == '0')
    {
        return SEGUNDO_BIT; // 10
    }
    ocorrencias++;
    printf("\r\nSequência 1101 identificada!\r\n Contagem: %d\r\n", ocorrencias);
    return PRIMEIRO_BIT; // 1011 (aproveita o 1)
}

int main(void)
{
    usb_serial_init();

    printf("\r\nIdentifica sequencias '1011'\r\n");
    printf("Digite um bit (1 ou 0).\r\n> ");

    estado_t estado = AGUARDANDO;
    bool ignorar_lf = false;

    while (1)
    {
        int ch = usb_serial_getchar();

        if (ch < 0)
        {
            continue;
        }
        else if (ch != '0' && ch != '1')
        {
            estado = AGUARDANDO;
            printf("\r\nValor inválido para bit\r\n> ");
            continue;
        }
        else
        {
            estado = transicao(estado, ch);
        }
    }
}

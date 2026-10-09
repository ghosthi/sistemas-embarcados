#include <stdbool.h>
#include <stdio.h>

#include "usb_serial.h"

typedef enum
{
    RESTO_0,
    RESTO_1,
    RESTO_2
} estado_t;

estado_t transicao(estado_t estado_atual, int char_lido)
{
    if (char_lido != '0' && char_lido != '1')
    {
        printf("\r\nEntrada inválida!\r\n> ");
        return RESTO_0;
    }
    if (estado_atual == RESTO_0)
    {
        return char_lido == '0' ? RESTO_0 : RESTO_1;
    }
    if (estado_atual == RESTO_1)
    {
        return char_lido == '0' ? RESTO_2 : RESTO_0;
    }
    // só pode ser RESTO 2
    return char_lido == '0' ? RESTO_1 : RESTO_2;
}

int main(void)
{
    usb_serial_init();

    printf("\r\nDIVISIVEL POR 3\r\n");
    printf("Digite um numero binário (MSB para LSB) e pressione Enter.\r\n> ");

    estado_t estado = RESTO_0;
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
            case RESTO_0:
                printf("\r\nDIVISIVEL POR 3\r\n");
                break;

            case RESTO_1:
                printf("\r\nNAO DIVISIVEL (RESTO 1)\r\n");
                break;

            case RESTO_2:
                printf("\r\nNAO DIVISIVEL (RESTO 2)\r\n");
                break;

                estado = RESTO_0;
                printf("> ");
                continue;
            }

            estado = transicao(estado, ch);
        }
    }
}

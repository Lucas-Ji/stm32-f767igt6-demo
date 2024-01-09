
#include "UartIF.h"

/* #if (__ARMCC_VERSION >= 6010050)
__asm(".global __use_no_semihosting\n\t");
__asm(".global __ARM_use_no_argv \n\t");

#else

#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;

};

#endif

int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
} */

/* FILE __stdout; */

/* int fputc(int ch, FILE *f)
{
    while ((USART1->ISR & 0X40) == 0);

    USART1->TDR = (uint8_t)ch;
    return ch;
} */

/* void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        if((g_usart_rx_sta & 0x8000) == 0)
        {
            if(g_usart_rx_sta & 0x4000)
            {
                if(g_rx_buffer[0] != 0x0a) 
                {
                    g_usart_rx_sta = 0;
                }
                else 
                {
                    g_usart_rx_sta |= 0x8000;
                }
            }
            else
            {
                if(g_rx_buffer[0] == 0x0d)
                {
                    g_usart_rx_sta |= 0x4000;
                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = g_rx_buffer[0] ;
                    g_usart_rx_sta++;
                    if(g_usart_rx_sta > (USART_REC_LEN - 1))
                    {
                        g_usart_rx_sta = 0;
                    }
                }
            }
        }
    }
} */

int VirtualPrintf (const char *__format, ...)
{
  int __retval;
/*   __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vprintf( __format, __local_argv );
  __builtin_va_end( __local_argv ); */
  return __retval;
}
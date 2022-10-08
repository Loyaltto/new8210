/******************************************************************************
 * uart.c
 * 
 * Copyright (c) 2011 by Li.Hua <lihua_338@163.com>.
 * 
 * DESCRIPTION: - 
 * 
 * Modification history
 * ----------------------------------------------------------------------------
 * Date         Version  Author       History
 * ----------------------------------------------------------------------------
 * 2011-01-20   1.0.0    Li.Hua       written
 ******************************************************************************/

#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <pthread.h>

#include "posapi.h"
#include "uart.h"
#include "postslib_extern.h"

typedef struct {
	const char		*devname;
	int				portnum;
	int				fd;
	int				refcnt;
	pthread_mutex_t	lock;
} Uart_t;

static Uart_t	global_uarts[] = {
	{
		.devname	= "/dev/ttyS3",
		.portnum	= PORT_COM1,
		.fd			= -1,
		.refcnt		= 0,
		.lock		= PTHREAD_MUTEX_INITIALIZER,
	},
};

static Uart_t *uart_find(int PortNum)
{
	int i;

	for (i=0; i<ARRAY_SIZE(global_uarts); i++) {
		if (global_uarts[i].portnum == PortNum)
			return &global_uarts[i];
	}

	return NULL;
}

/* 打开指定的串口 */
int portOpen(int PortNum, const char *PortPara)
{
	Uart_t	*uart = uart_find(PortNum);
	int		retval = PORT_RET_BUSY;
	
	if (!uart)
		return PORT_RET_PORTERR;

	pthread_mutex_lock(&uart->lock);

	if (uart->refcnt++ == 0) {
		uart->fd = open(uart->devname, O_RDWR);
		if (uart->fd >= 0) {
			int 		baudrate, databits, parity, stopbits;
			const char	*str;
			char		*endptr;
			
			str = PortPara;
			baudrate = strtol(str, &endptr, 10);
			
			str = endptr+1;
			databits = strtol(str, &endptr, 10);
			
			str = endptr+1;
			parity = *str;

			str += 2;
			stopbits = strtol(str, &endptr, 10);
			if (tty_property_config(uart->fd, baudrate, databits, parity, stopbits, 'n')) {
				close(uart->fd);
				uart->fd = -1;
				uart->refcnt--;
				retval = PORT_RET_PARAERR;
			} else {
				retval = PORT_RET_OK;
			}
		}
	}
	
	pthread_mutex_unlock(&uart->lock);

	return retval;
}

/* 关闭指定的通讯口 */
int portClose(int PortNum)
{
	Uart_t	*uart = uart_find(PortNum);
	
	if (!uart)
		return PORT_RET_PORTERR;

	if (uart->refcnt == 0)
		return PORT_RET_NOTOPEN;
			
	pthread_mutex_lock(&uart->lock);
	if (--uart->refcnt == 0) {
		close(uart->fd);
		uart->fd = -1;
	}
	pthread_mutex_unlock(&uart->lock);

	return 0;
}

/* 使用指定的逻辑串口号发送若干字节的数据 */
int portSends(int PortNum, const void *SendBuf, size_t SendLen)
{
	Uart_t	*uart = uart_find(PortNum);
	
	if (!uart)
		return PORT_RET_PORTERR;

	if (uart->refcnt == 0)
		return PORT_RET_NOTOPEN;

	safety_full_write(uart->fd, SendBuf, SendLen);
	return 0;
}

/* 使用指定的逻辑串口号发送若干字节的数据 */
int portPrintf(int PortNum, const char *FmtBuf, ...)
{
	Uart_t	*uart = uart_find(PortNum);
	char	buffer[8*1024];
	va_list ap;
	int		nbytes;
	
	if (!uart)
		return PORT_RET_PORTERR;

	if (uart->refcnt == 0)
		return PORT_RET_NOTOPEN;


	va_start(ap, FmtBuf);
	nbytes = vsnprintf(buffer, sizeof(buffer), FmtBuf, ap);
	va_end(ap);

	return safety_full_write(uart->fd, buffer, nbytes);
}

/* 从指定的逻辑串口号, 接收一个字节的数据 */
int portRecv(int PortNum, uint8_t *RecvBuf, uint32_t TimeOutMs)
{
	Uart_t			*uart = uart_find(PortNum);
	struct timeval	t;
	fd_set			rfds;
	int				ret;
	
	if (!uart)
		return PORT_RET_PORTERR;

	if (uart->refcnt == 0)
		return PORT_RET_NOTOPEN;

	t.tv_sec  = TimeOutMs / 1000;
	t.tv_usec = (TimeOutMs % 1000) * 1000;
	
	FD_ZERO(&rfds);
	FD_SET(uart->fd, &rfds);

	ret = select(uart->fd + 1, &rfds, NULL, NULL, &t);
	if (ret < 0)
		perror("Magcard swipe select");

	if (ret > 0 && FD_ISSET(uart->fd, &rfds)) 
		return read(uart->fd, RecvBuf, 1) == 1 ? 0 : 0xff;

	return 0xff;
}

/* 复位通讯口，该函数将清除串口接收缓冲区中的所有数据 */
int portReset(int PortNum)
{
	Uart_t	*uart = uart_find(PortNum);
	
	if (!uart)
		return PORT_RET_PORTERR;

	if (uart->refcnt == 0)
		return PORT_RET_NOTOPEN;

	tcflush(uart->fd, TCIOFLUSH);
	return 0;
}

/* 检查指定通讯口的发送缓冲区是否已无数据待发送 */
int portCheckSendBuf(int PortNum)
{
	Uart_t	*uart = uart_find(PortNum);
	
	if (!uart)
		return PORT_RET_PORTERR;

	if (uart->refcnt == 0)
		return PORT_RET_NOTOPEN;

	tcdrain(uart->fd);
	return 0;
}

/* 检查指定通讯口的接收缓冲区是否已无数据待接收 */
int portCheckRecvBuf(int PortNum)
{
	Uart_t			*uart = uart_find(PortNum);
	struct timeval	t;
	fd_set			rfds;
	int				ret;
	
	if (!uart)
		return PORT_RET_PORTERR;

	if (uart->refcnt == 0)
		return PORT_RET_NOTOPEN;

	t.tv_sec  = 0;
	t.tv_usec = 10;
	
	FD_ZERO(&rfds);
	FD_SET(uart->fd, &rfds);

	ret = select(uart->fd + 1, &rfds, NULL, NULL, &t);
	if (ret < 0) {
		perror("Magcard swipe select");
		return PORT_RET_OK;
	}

	if (ret && FD_ISSET(uart->fd, &rfds))
		return PORT_RET_NOTEMPTY;

	return PORT_RET_OK;
}


/*
 * stm32h723vgt6.h
 *
 *  Created on: Oct 4, 2023
 *      Author: Daniel Johnson
 */
#ifndef STM32H723VGT6_H_
#define STM32H723VGT6_H_

#include <cstdint>
#include "mcu.hpp"
#include "uart.hpp"
#include "debug.hpp"
#include "blinker.hpp"

extern const char* textClassNameStm32h723vgt6;

class Stm32h723vgt6 : public Mcu
{
private:
	//Uart*  _uart0;
	//Uart*  _uart1;
	Uart*  _uart2;
	//Uart*  _uart3;
	Uart*  _uart4;
	Uart*  _uart5;
	//Uart*  _uart6;
	Uart*  _uart7;
	//Uart*  _uart8;
	//Uart*  _uart9;
	Uart*  _uart10;
	//Iic*   _iic0;
	//Iic*   _iic1;
	//Iic*   _iic2;
	//Iic*   _iic3;
	Iic*   _iic4;
	Iic*   _iic5;
public:
	Stm32h723vgt6(
		//UART_HandleTypeDef* uart0,
		//UART_HandleTypeDef* uart1,
		UART_HandleTypeDef* uart2,
		//UART_HandleTypeDef* uart3,
		UART_HandleTypeDef* uart4,
		UART_HandleTypeDef* uart5,
		//UART_HandleTypeDef* uart6,
		UART_HandleTypeDef* uart7,
		//UART_HandleTypeDef* uart8,
		//UART_HandleTypeDef* uart9,
		UART_HandleTypeDef* uart10,
		//I2C_HandleTypeDef*  iic0,
		//I2C_HandleTypeDef*  iic1,
		//I2C_HandleTypeDef*  iic2,
		//I2C_HandleTypeDef*  iic3,
		I2C_HandleTypeDef*  iic4,
		I2C_HandleTypeDef*  iic5
	) : Mcu()
	{
		//_uart0  = new Uart(uart0,   0);
		//_uart1  = new Uart(uart1,   1);
		_uart2  = new Uart(uart2,   2);
		//_uart3  = new Uart(uart3,   3);
		_uart4  = new Uart(uart4,   4);
		_uart5  = new Uart(uart5,   5);
		//_uart6  = new Uart(uart6,   6);
		_uart7  = new Uart(uart7,   7);
		//_uart8  = new Uart(uart8,   8);
		//_uart9  = new Uart(uart9,   9);
		_uart10 = new Uart(uart10, 10);
		//_iic0   = new Iic(iic0, 0);
		//_iic1   = new Iic(iic1, 1);
		//_iic2   = new Iic(iic2, 2);
		//_iic3   = new Iic(iic3, 3);
		_iic4   = new Iic(iic4, 4);
		_iic5   = new Iic(iic5, 5);
	}
	//virtual Uart*  uart0()  const override { return _uart0; }
	//virtual Uart*  uart1()  const override { return _uart1; }
	virtual Uart*  uart2()  const override { return _uart2; }
	//virtual Uart*  uart3()  const override { return _uart3; }
	virtual Uart*  uart4()  const override { return _uart4; }
	virtual Uart*  uart5()  const override { return _uart5; }
	//virtual Uart*  uart6()  const override { return _uart6; }
	virtual Uart*  uart7()  const override { return _uart7; }
	//virtual Uart*  uart8()  const override { return _uart8; }
	//virtual Uart*  uart9()  const override { return _uart9; }
	virtual Uart*  uart10() const override { return _uart10; }
	//virtual Iic*   iic0()   const override { return _iic0;  }
	//virtual Iic*   iic1()   const override { return _iic1;  }
	//virtual Iic*   iic2()   const override { return _iic2;  }
	//virtual Iic*   iic3()   const override { return _iic3;  }
	virtual Iic*   iic4()   const override { return _iic4;  }
	virtual Iic*   iic5()   const override { return _iic5;  }
};

#endif /* STM32H723VGT6_H_ */

/*
 * Mcu.h
 *
 *  Created on: Oct 3, 2023
 *      Author: Daniel Johnson
 */

#ifndef MCU_H_
#define MCU_H_

#include "uart.hpp"
#include "iic.hpp"

class Mcu
{
protected:
	Mcu()
	{
	}
public:
	virtual ~Mcu()
	{
	}
	//virtual Uart*  uart0()  const { return NULL; }
	//virtual Uart*  uart1()  const { return NULL; }
	virtual Uart*  uart2()  const { return NULL; }
	//virtual Uart*  uart3()  const { return NULL; }
	virtual Uart*  uart4()  const { return NULL; }
	virtual Uart*  uart5()  const { return NULL; }
	//virtual Uart*  uart6()  const { return NULL; }
	virtual Uart*  uart7()  const { return NULL; }
	//virtual Uart*  uart8()  const { return NULL; }
	//virtual Uart*  uart9()  const { return NULL; }
	virtual Uart*  uart10() const { return NULL; }
	//virtual Iic*   iic0()   const { return NULL; }
	//virtual Iic*   iic1()   const { return NULL; }
	//virtual Iic*   iic2()   const { return NULL; }
	//virtual Iic*   iic3()   const { return NULL; }
	virtual Iic*   iic4()   const { return NULL; }
	virtual Iic*   iic5()   const { return NULL; }
};

#endif /* MCU_H_ */

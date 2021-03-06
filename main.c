/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the XMC MCU: FCE CRC example
*              for ModusToolbox. The example uses the XMC_FCE Low Level Driver 
*              to perform CRC8, CRC16 and CRC32 operation.
*
* Related Document: See README.md
*
******************************************************************************
*
* Copyright (c) 2021, Infineon Technologies AG
* All rights reserved.                        
*                                             
* Boost Software License - Version 1.0 - August 17th, 2003
* 
* Permission is hereby granted, free of charge, to any person or organization
* obtaining a copy of the software and accompanying documentation covered by
* this license (the "Software") to use, reproduce, display, distribute,
* execute, and transmit the Software, and to prepare derivative works of the
* Software, and to permit third-parties to whom the Software is furnished to
* do so, all subject to the following:
* 
* The copyright notices in the Software and this entire statement, including
* the above license grant, this restriction and the following disclaimer,
* must be included in all copies of the Software, in whole or in part, and
* all derivative works of the Software, unless such copies or derivative
* works are solely in the form of machine-executable object code generated by
* a source language processor.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
* SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
* FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*                                                                              
*****************************************************************************/

/******************************************************************************
* Include header files
******************************************************************************/
#include "cybsp.h"
#include "cy_utils.h"
#include "retarget_io.h"
#include <stdio.h>
#include "xmc_fce.h"

/*******************************************************************************
* Global Variables
*******************************************************************************/
/* Data Packet 1 */
int8_t usecase1_data1[] =   "XMC MCU: FCE CRC example";
/* Data Packet 2 */
int8_t usecase1_data2[] =   "FCE implements CRC in 4 CRC Kernels :- CRC Kernel0 "
                            "and CRC Kernel1 (CRC32), CRC Kernel2 (CRC16), and "
                            "CRC Kernel3 (CRC8).";
/* Data Packet 3 */
int8_t usecase1_data3[] =   "This code example uses the Flexible CRC Engine "
                            "(FCE) driver for kernel to demonstrate CRC-32, "
                            "CRC-16, and CRC-8 checksum calculations. The CRC "
                            "result and debug messages are displayed on a "
                            "serial terminal using UART communication. The "
                            "onboard User LED is used to indicate an error.";

/* FCE configuration for CRC32 operation using Kernel 0
 * Algorithm: IR Byte Wise Reflection disabled
 *          : CRC 32-Bit Wise Reflection disabled
 *          : XOR with final CRC enabled
 * Initial seedvalue: 0U
 */
XMC_FCE_t FCE_config0 =
{
    .kernel_ptr = XMC_FCE_CRC32_0,                          /* FCE Kernel Pointer */
    .fce_cfg_update.config_refin = XMC_FCE_REFIN_RESET,     /* Disables byte-wise reflection */
    .fce_cfg_update.config_refout = XMC_FCE_REFOUT_RESET,   /* Disables bit-wise reflection */
    .fce_cfg_update.config_xsel = XMC_FCE_INVSEL_SET,       /* Enables output inversion */
    .seedvalue = 0U                                         /* CRC seed value to be used */
};

/* FCE configuration for CRC32 operation using Kernel 1
 * Algorithm: IR Byte Wise Reflection disabled
 *          : CRC 32-Bit Wise Reflection disabled
 *          : XOR with final CRC disabled
 * Initial seedvalue: 0U
 */
XMC_FCE_t FCE_config1 =
{
    .kernel_ptr = XMC_FCE_CRC32_1,                          /* FCE Kernel Pointer */
    .fce_cfg_update.config_refin = XMC_FCE_REFIN_RESET,     /* Disables byte-wise reflection */
    .fce_cfg_update.config_refout = XMC_FCE_REFOUT_RESET,   /* Disables bit-wise reflection */
    .fce_cfg_update.config_xsel = XMC_FCE_INVSEL_RESET,     /* Disables output inversion */
    .seedvalue = 0U                                         /* CRC seed value to be used */
};

/* FCE configuration for CRC16 operation using Kernel 2
 * Algorithm: IR Byte Wise Reflection enabled
 *          : CRC 32-Bit Wise Reflection disabled
 *          : XOR with final CRC disabled
 * Initial seedvalue: 0U
 */
XMC_FCE_t FCE_config2 =
{
    .kernel_ptr = XMC_FCE_CRC16,                            /* FCE Kernel Pointer */
    .fce_cfg_update.config_refin = XMC_FCE_REFIN_SET,       /* Enables byte-wise reflection */
    .fce_cfg_update.config_refout = XMC_FCE_REFOUT_RESET,   /* Disables bit-wise reflection */
    .fce_cfg_update.config_xsel = XMC_FCE_INVSEL_RESET,     /* Disables output inversion */
    .seedvalue = 0U                                         /* CRC seed value to be used */
};

/* FCE configuration for CRC8 operation using Kernel 3
 * Algorithm: IR Byte Wise Reflection disabled
 *          : CRC 32-Bit Wise Reflection disabled
 *          : XOR with final CRC disabled
 * Initial seedvalue: 0U
 */
XMC_FCE_t FCE_config3 =
{
    .kernel_ptr = XMC_FCE_CRC8,                             /* FCE Kernel Pointer */
    .fce_cfg_update.config_refin = XMC_FCE_REFIN_RESET,     /* Disables byte-wise reflection */
    .fce_cfg_update.config_refout = XMC_FCE_REFOUT_RESET,   /* Disables bit-wise reflection */
    .fce_cfg_update.config_xsel = XMC_FCE_INVSEL_RESET,     /* Disables output inversion */
    .seedvalue = 0U                                         /* CRC seed value to be used */
};

/*******************************************************************************
* Function Name: handle_error
********************************************************************************
* Summary:
*  Function to handle any error that occurs. In this function the User LED is 
*  turned on and the CPU is halted to indicate the error.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void handle_error(void)
{
    XMC_GPIO_SetOutputHigh(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
    CY_ASSERT(0);
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  System entrance point. This function performs
*   - initial setup of device
*   - enables FCE module
*   - demonstrates the use of FCE CRC kernels
*   - debug messages and results are printed on the terminal
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    /* Variables to store the result of the CRC operation */
    uint32_t read_crc_result32;
    uint16_t read_crc_result16;
    uint8_t  read_crc_result8;
    uint32_t crc_result;

    /* Variables used in checking for CRC mismatch and Length errors */
    uint32_t temp_length;
    uint32_t temp_mismatch;

    /* Variables to store the status */
    cy_rslt_t result;
    bool flag_status;
    XMC_FCE_STATUS_t fce_status;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if(result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize printf retarget */
    retarget_io_init();

    /* \x1b[2J\x1b[;H - ANSI ESC sequence to clear screen. */
    printf("\x1b[2J\x1b[;H");
    printf("===============================================================\r\n");
    printf("XMC MCU: FCE CRC example\r\n");
    printf("===============================================================\r\n\n");

    /* Enable FCE module */
    XMC_FCE_Enable();

    /* Initialize the FCE Configuration */
    XMC_FCE_Init(&FCE_config0);
    XMC_FCE_Init(&FCE_config1);
    XMC_FCE_Init(&FCE_config2);
    XMC_FCE_Init(&FCE_config3);

    /* Initialize error counter */
    temp_mismatch = 0;

    /* Step 1: Perform a CRC32 check using Kernel 0 on usecase1_data1
     * Seed value is set to 0.
     * Output inversion enabled.
     * Expected Result : CRC = 0x209a5692, RES = 0xdf65a96d
     */
    printf("Step 1: CRC32 with output inversion\r\n\r\n");
    printf("Message = %s\r\n\r\n", (char*)usecase1_data1);

    XMC_FCE_InitializeSeedValue(&FCE_config0, 0);

    fce_status = XMC_FCE_CalculateCRC32(&FCE_config0, (uint32_t *) usecase1_data1, 
                                        strlen((const char *)(usecase1_data1)), 
                                        &read_crc_result32);
    if(fce_status == XMC_FCE_STATUS_ERROR)
    {
        /* Endless loop if error */
        printf("Error in calculating CRC: Step 1\r\n");
        handle_error();
    }
    
    XMC_FCE_GetCRCResult(&FCE_config0, &crc_result);

    printf("CRC = 0x%08x\r\n", (uint)read_crc_result32);
    printf("RES = 0x%08x\r\n", (uint)crc_result);
    printf("===============================================================\r\n\n");

    /* Step 2: Perform a CRC32 check using Kernel 1 on usecase1_data1
     * Seed value is set to 0. CRC check comparison is enabled
     * CRC checksum is using result from earlier CRC calculation
     * No CRC mismatch found.
     * Expected Result : CRC = 0x209a5692, RES = 0x209a5692
     */
    /* Enable CRC check */
    XMC_FCE_EnableOperation(&FCE_config1, XMC_FCE_CFG_CONFIG_CCE);

    /* Used in 32-bit FCE, therefore temp_length is divided by 4 */
    temp_length = (strlen((char *)(usecase1_data1))) >> 2;

    /* Update the CRC and length of the earlier CRC calculation for comparison */
    XMC_FCE_UpdateCRCCheck(&FCE_config1, read_crc_result32);
    XMC_FCE_UpdateLength(&FCE_config1, temp_length);

    printf("Step 2: CRC32 with CRC match enabled\r\n\r\n");
    printf("Message = %s\r\n\r\n", (char*)usecase1_data1);

    XMC_FCE_InitializeSeedValue(&FCE_config1, 0);

    fce_status = XMC_FCE_CalculateCRC32(&FCE_config1, (uint32_t *) usecase1_data1, 
                                        strlen((const char *)(usecase1_data1)), 
                                        &read_crc_result32);
    if(fce_status == XMC_FCE_STATUS_ERROR)
    {
        /* endless loop if error */
        printf("Error in calculating CRC: Step 2\r\n");
        handle_error();
    }

    XMC_FCE_GetCRCResult(&FCE_config1, &crc_result);
    if(XMC_FCE_GetEventStatus(&FCE_config1, XMC_FCE_STS_MISMATCH_CRC))
    {
        printf("CRC Mismatch occurred\r\n");
        temp_mismatch += 1U;
    }
    else
    {
        printf("CRC Matched!\r\n");
    }

    printf("CRC = 0x%08x\r\n", (uint)read_crc_result32);
    printf("RES = 0x%08x\r\n", (uint)crc_result);
    printf("===============================================================\r\n\n");

    /* Step 3: Perform a CRC32 check using Kernel 1 on usecase1_data2
     * Seed value is set to 0. CRC check comparison is enabled
     * CRC checksum is using result from earlier CRC calculation
     * CRC mismatch found and Length Error found.
     * Expected Result : CRC = 0x6a9255b2, RES = 0x6a9255b2
     */
    printf("Step 3: CRC32 with CRC Mismatch and Length error\r\n\r\n");
    printf("Message = %s\r\n\r\n", (char*)usecase1_data2);

    XMC_FCE_InitializeSeedValue(&FCE_config1, 0);

    XMC_FCE_UpdateLength(&FCE_config1, temp_length);

    fce_status = XMC_FCE_CalculateCRC32(&FCE_config1, (uint32_t *) usecase1_data2, 
                                        strlen((const char *)(usecase1_data2)), 
                                        &read_crc_result32);
    if(fce_status == XMC_FCE_STATUS_ERROR)
    {
        /* endless loop if error */
        printf("Error in calculating CRC: Step 3\r\n");
        handle_error();
    }

    XMC_FCE_GetCRCResult(&FCE_config1, &crc_result);
    if(XMC_FCE_GetEventStatus(&FCE_config1, XMC_FCE_STS_MISMATCH_CRC))
    {
        printf("CRC Mismatch occurred\r\n");
        temp_mismatch += 2U;
    }
    else
    {
        printf("CRC Matched!\r\n");
    }

    if(XMC_FCE_GetEventStatus(&FCE_config1,XMC_FCE_STS_LENGTH_ERROR))
    {
        printf("Length error occurred\r\n");
    }

    printf("CRC = 0x%08x\r\n", (uint)read_crc_result32);
    printf("RES = 0x%08x\r\n", (uint)crc_result);
    printf("===============================================================\r\n\n");

    /* Step 4: Perform a CRC16 check using Kernel 2 on usecase1_data3
     * Seed value is set to 0.
     * Byte-wise reflection is enabled.
     * Expected Result : CRC = 0xadf8, RES = 0xadf8
     */
    printf("Step 4: CRC16 with byte-wise reflection\r\n\r\n");
    printf("Message = %s\r\n\r\n", (char*)usecase1_data3);

    XMC_FCE_InitializeSeedValue(&FCE_config2, 0);

    fce_status = XMC_FCE_CalculateCRC16(&FCE_config2, (uint16_t *) usecase1_data3, 
                                        strlen((const char *)(usecase1_data3)), 
                                        &read_crc_result16);
    if(fce_status == XMC_FCE_STATUS_ERROR)
    {
        /* endless loop if error */
        printf("Error in calculating CRC: Step 4\r\n");
        handle_error();
    }

    XMC_FCE_GetCRCResult(&FCE_config2, &crc_result);

    printf("CRC = 0x%04x\r\n", (uint)read_crc_result16);
    printf("RES = 0x%04x\r\n", (uint)crc_result);
    printf("===============================================================\r\n\n");

    /* Step 5: Perform a CRC8 check using Kernel 3 on usecase1_data3
     * Seed value is set to 0. 
     * Expected Result : CRC = 0x7a, RES = 0x7a
     */
    printf("Step 5: CRC8 \r\n\r\n");
    printf("Message = %s\r\n\r\n", (char*)usecase1_data3);

    XMC_FCE_InitializeSeedValue(&FCE_config3, 0);

    fce_status = XMC_FCE_CalculateCRC8(&FCE_config3, (uint8_t *)usecase1_data3, 
                                        strlen((const char *)(usecase1_data3)), 
                                        &read_crc_result8);
    if(fce_status == XMC_FCE_STATUS_ERROR)
    {
        /* endless loop if error */
        printf("Error in calculating CRC: Step 5\r\n");
        handle_error();
    }

    XMC_FCE_GetCRCResult(&FCE_config3, &crc_result);
    
    printf("CRC = 0x%02x\r\n", (uint)read_crc_result8);
    printf("RES = 0x%02x\r\n", (uint)crc_result);
    printf("===============================================================\r\n\n");

    flag_status = XMC_FCE_GetEventStatus(&FCE_config3, XMC_FCE_STS_MISMATCH_CRC);
    if(flag_status)
    {
        /* endless loop if mismatch flag is triggered */
        printf("Mismatch flag is triggered: Step 5\r\n");
        handle_error();
    }

    /* Step 6: Trigger a mismatch flag */
    printf("Step 6: Triggering a mismatch flag \r\n\r\n");

    XMC_FCE_TriggerMismatch(&FCE_config3, XMC_FCE_CTR_MISMATCH_CRC);

    flag_status = XMC_FCE_GetEventStatus(&FCE_config3, XMC_FCE_STS_MISMATCH_CRC);
    if(flag_status)
    {
        /* endless loop if mismatch flag is triggered */
        printf("Mismatch flag is triggered: Step 6\r\n");
        handle_error();
    }

    for (;;)
    {
        
    }
}

/* [] END OF FILE */

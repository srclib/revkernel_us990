/*
 * include/linux/mfd/wm831x/status.h -- Status LEDs for WM831x
 *
 * Copyright 2009 Wolfson Microelectronics PLC.
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#ifndef __MFD_WM831X_STATUS_H__
#define __MFD_WM831X_STATUS_H__

#define WM831X_LED_SRC_MASK                    0xC000  /*                   */
#define WM831X_LED_SRC_SHIFT                       14  /*                   */
#define WM831X_LED_SRC_WIDTH                        2  /*                   */
#define WM831X_LED_MODE_MASK                   0x0300  /*                  */
#define WM831X_LED_MODE_SHIFT                       8  /*                  */
#define WM831X_LED_MODE_WIDTH                       2  /*                  */
#define WM831X_LED_SEQ_LEN_MASK                0x0030  /*                     */
#define WM831X_LED_SEQ_LEN_SHIFT                    4  /*                     */
#define WM831X_LED_SEQ_LEN_WIDTH                    2  /*                     */
#define WM831X_LED_DUR_MASK                    0x000C  /*                 */
#define WM831X_LED_DUR_SHIFT                        2  /*                 */
#define WM831X_LED_DUR_WIDTH                        2  /*                 */
#define WM831X_LED_DUTY_CYC_MASK               0x0003  /*                      */
#define WM831X_LED_DUTY_CYC_SHIFT                   0  /*                      */
#define WM831X_LED_DUTY_CYC_WIDTH                   2  /*                      */

#endif
/*
 * This file is part of Betaflight.
 *
 * Betaflight is free software. You can redistribute this software
 * and/or modify this software under the terms of the GNU General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later
 * version.
 *
 * Betaflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * T-Motor Pacer G4 Alpha HD: STM32G473CEU6 (48-pin), IIM-42652 gyro, no crystal.
 *
 * Pins come from T-Motor's Alpha 0.7.73 firmware run in an emulator, then checked on the board.
 *   Confirmed on the board: gyro (detected, 8 kHz), motors 1-8, and the T1/R1, T3/R3 and T4/R4 pads
 *   (metered to PA9/PA10, PB9/PB8 and PC10/PC11). With a DJI O4 air unit on the DJI plug, the OSD works
 *   on UART3 and SBUS from the DJI RC works on UART2 TX (PA14) in half-duplex mode.
 *   The LED strip on PB4, the beeper on PB7 (active high) and battery voltage on PA0 work on the board.
 *   LPUART1 TX (PA2) is the ESC plugs' TLM line. ESC Sensor reads the Pacer's BLHeli_32 telemetry on it
 *   with esc_sensor_halfduplex = ON. LPUART1 needs Betaflight PR #15723; 2026.6.2 never starts it.
 *   PB2 drives the status LED, lit when the pin is high. PC6 high enables the regulator that powers
 *   the DJI plug, and the blue LED shows it is on (both seen on the board).
 *   Not identified yet, so left out: the receiver SIG and TLM pads.
 *
 * Built and run on the board with Betaflight 2026.6.2: DShot bitbang on DMA1 channels 1-2, gyro SPI
 * on 3-4, LED strip on 5. PA15 is coupled to an SWD line on this board; driving it as an output kills SWD.
 *
 * No testable pin receives the gyro interrupt, so there is no GYRO_1_EXTI_PIN and the gyro is polled.
 * PB8 is both UART3 RX and BOOT0. Keep option byte nSWBOOT0 at 0, or a device idling high on R3
 * makes the board boot into ROM DFU.
 */

#pragma once

#define FC_TARGET_MCU     STM32G474

#define BOARD_NAME        TMOTORG4ALPHAHD
// COMM: a community target for a closed-source board. T-Motor did not submit it.
#define MANUFACTURER_ID   COMM

#define USE_ACC
#define USE_GYRO
#define USE_ACCGYRO_IIM42652
// Betaflight 2026.6.x only runs the ICM-426xx detector when an ICM driver is enabled too, so the
// IIM-42652 alone is never detected. Master fixed this in accgyro_mpu.c; SAKURAH743 uses the same workaround.
#define USE_ACC_SPI_ICM42688P
#define USE_GYRO_SPI_ICM42688P

// Pads M1 to M8. Alpha numbers motors left front, right front, right rear, left rear,
// so reorder them in Configurator's Motors tab for Betaflight's layout.
#define MOTOR1_PIN        PA6
#define MOTOR2_PIN        PA7
#define MOTOR3_PIN        PB0
#define MOTOR4_PIN        PB1
#define MOTOR5_PIN        PA5
#define MOTOR6_PIN        PB3
#define MOTOR7_PIN        PB10
#define MOTOR8_PIN        PB11

#define LED_STRIP_PIN     PB4

#define LED0_PIN          PB2     // beside the USB-C socket, below the power LED
#define LED0_INVERTED             // lit when the pin is high

// PC6 high enables the regulator that powers the DJI plug; the blue centre LED shows it is on.
// On by default, and the USER1 mode can switch it off.
#define PINIO1_PIN        PC6
#define PINIO1_CONFIG     129
#define PINIO1_BOX        40

#define BEEPER_PIN        PB7
#define BEEPER_INVERTED           // active high, push-pull

#define ADC_VBAT_PIN      PA0
#define ADC_INSTANCE      ADC1
#define ADC1_DMA_OPT      5       // DMA1 channel 6; without a DMA channel the ADC never starts

#define UART1_TX_PIN      PA9
#define UART1_RX_PIN      PA10
// UART2 TX is the DJI plug's SBUS pin and also SWCLK. DJI SBUS works on it with
// serialrx_provider = SBUS and serialrx_halfduplex = ON. Unplug any SWD probe first: it holds the line.
#define UART2_TX_PIN      PA14
#define UART2_RX_PIN      PA15    // no pad found; unused in half-duplex
#define UART3_TX_PIN      PB9
#define UART3_RX_PIN      PB8     // also BOOT0
#define UART4_TX_PIN      PC10
#define UART4_RX_PIN      PC11
#define LPUART1_TX_PIN    PA2     // ESC plugs' TLM line, half-duplex (esc_sensor_halfduplex = ON)
#define LPUART1_RX_PIN    PA3     // no ESC telemetry reaches PA3

#define SPI2_SCK_PIN      PB13
#define SPI2_SDI_PIN      PB14
#define SPI2_SDO_PIN      PB15

#define GYRO_1_CS_PIN     PB12

// DMA: DShot bitbang takes DMA1 channels 1-2 and gyro SPI takes 3-4 (seen on the board), so the
// LED strip uses DMA1 channel 5 (option 4), the ADC channel 6, and timer-mode DShot uses DMA2 (options 8-15).
#define TIMER_PIN_MAPPING \
    TIMER_PIN_MAP( 0, LED_STRIP_PIN, 1,  4 ) \
    TIMER_PIN_MAP( 1, MOTOR1_PIN,    2,  8 ) \
    TIMER_PIN_MAP( 2, MOTOR2_PIN,    2,  9 ) \
    TIMER_PIN_MAP( 3, MOTOR3_PIN,    1, 10 ) \
    TIMER_PIN_MAP( 4, MOTOR4_PIN,    1, 11 ) \
    TIMER_PIN_MAP( 5, MOTOR5_PIN,    1, 12 ) \
    TIMER_PIN_MAP( 6, MOTOR6_PIN,    1, 13 ) \
    TIMER_PIN_MAP( 7, MOTOR7_PIN,    1, 14 ) \
    TIMER_PIN_MAP( 8, MOTOR8_PIN,    1, 15 )

#define GYRO_1_SPI_INSTANCE     SPI2
// Checked on the board with tilt tests: component side up, ESC sockets forward (the manual's default).
#define GYRO_1_ALIGN             CW180_DEG

#define DEFAULT_DSHOT_BITBANG   DSHOT_BITBANG_ON
#define DEFAULT_DSHOT_BURST     DSHOT_DMAR_OFF
#define DEFAULT_DSHOT_TELEMETRY DSHOT_TELEMETRY_ON

#define DEFAULT_VOLTAGE_METER_SOURCE    VOLTAGE_METER_ADC
// The board's divider is 31:1. With ADC oversampling on, 157 matched a meter on a 6S battery (25.05 V) and
// on a bench supply (22.74 V); the earlier 155 came from noisy readings before oversampling.
#define DEFAULT_VOLTAGE_METER_SCALE     157
#define DEFAULT_VOLTAGE_METER_DIVIDER   5

#ifdef USE_OSD_HD
#define MSP_DISPLAYPORT_UART    SERIAL_PORT_USART3    // DJI plug: TX3, RX3
#endif

#define ESC_SENSOR_UART         SERIAL_PORT_LPUART1   // ESC plugs' TLM line

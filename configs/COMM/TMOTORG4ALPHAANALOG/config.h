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
 * T-Motor Pacer G4 Alpha Analog: STM32G473CEU6 (48-pin), IIM-42652 gyro (marked I4652), no crystal,
 * and an STM32G071GB OSD co-processor running FETtec's Alpha OSD firmware 0.1.18.
 *
 * Pins are carried over from TMOTORG4ALPHAHD. This board shipped with an unreleased Alpha (0.1-152)
 * that is read-protected, so it couldn't be emulated the way the HD board's 0.7.73 was. Checked on one
 * converted board, on the bench: gyro detection, the OSD on UART4, SmartAudio on UART3, the receiver on
 * UART1, ESC telemetry on LPUART1, motor order and battery voltage. Not checked yet: the beeper, the
 * LEDs, the LED strip and gyro alignment. Not flown yet.
 *
 * OSD: the G071 takes MSP DisplayPort and draws it over analog video. Under Alpha it sat on Alpha's
 * SERIAL 4 (disabling that port blanked the OSD), which is UART4 (PC10/PC11) on the HD board. Build
 * with USE_OSD_SD and without USE_OSD_HD: a device that sends MSP_SET_OSD_CANVAS makes an HD build
 * switch to HD, save and reboot. Set vcd_video_system to PAL or NTSC to match the camera; AUTO gives
 * the 13-row NTSC canvas.
 *
 * No DJI plug, and listings give this board a 5 V output only, so the HD board's PC6-switched
 * regulator is left out until PC6 is checked here.
 *
 * PB8 is both UART3 RX and BOOT0, and this board has no R3 pad. Keep option byte nSWBOOT0 at 0.
 */

#pragma once

#define FC_TARGET_MCU     STM32G474

#define BOARD_NAME        TMOTORG4ALPHAANALOG
// COMM: a community target for a closed-source board. T-Motor did not submit it.
#define MANUFACTURER_ID   COMM

#define USE_ACC
#define USE_GYRO
#define USE_ACCGYRO_IIM42652
// Betaflight 2026.6.x only runs the ICM-426xx detector when an ICM driver is enabled too, so the
// IIM-42652 alone is never detected. Master fixed this in accgyro_mpu.c; SAKURAH743 uses the same workaround.
#define USE_ACC_SPI_ICM42688P
#define USE_GYRO_SPI_ICM42688P

// Pads M1 to M8, as on the HD board.
#define MOTOR1_PIN        PA6
#define MOTOR2_PIN        PA7
#define MOTOR3_PIN        PB0
#define MOTOR4_PIN        PB1
#define MOTOR5_PIN        PA5
#define MOTOR6_PIN        PB3
#define MOTOR7_PIN        PB10
#define MOTOR8_PIN        PB11

#define LED_STRIP_PIN     PB4

#define LED0_PIN          PB2     // status LED on the HD board, lit when the pin is high
#define LED0_INVERTED

#define BEEPER_PIN        PB7
#define BEEPER_INVERTED           // active high, push-pull

#define ADC_VBAT_PIN      PA0
#define ADC_INSTANCE      ADC1
#define ADC1_DMA_OPT      5       // DMA1 channel 6; without a DMA channel the ADC never starts

#define UART1_TX_PIN      PA9     // T1, receiver
#define UART1_RX_PIN      PA10    // R1, receiver
// UART2 TX is the receiver plug's SIG pin on the HD board, and also SWCLK.
#define UART2_TX_PIN      PA14
#define UART2_RX_PIN      PA15
#define UART3_TX_PIN      PB9     // T3, SmartAudio
#define UART3_RX_PIN      PB8     // also BOOT0; no R3 pad on this board
#define UART4_TX_PIN      PC10    // OSD co-processor
#define UART4_RX_PIN      PC11    // OSD co-processor
#define LPUART1_TX_PIN    PA2     // ESC plugs' TLM line, half-duplex (esc_sensor_halfduplex = ON)
#define LPUART1_RX_PIN    PA3

#define SPI2_SCK_PIN      PB13
#define SPI2_SDI_PIN      PB14
#define SPI2_SDO_PIN      PB15

#define GYRO_1_CS_PIN     PB12

// DMA as on the HD board: DShot bitbang takes DMA1 channels 1-2 and gyro SPI 3-4, so the LED strip
// uses DMA1 channel 5 (option 4), the ADC channel 6, and timer-mode DShot uses DMA2 (options 8-15).
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
// The HD board's alignment; check with tilt tests on this board.
#define GYRO_1_ALIGN             CW180_DEG

#define DEFAULT_DSHOT_BITBANG   DSHOT_BITBANG_ON
#define DEFAULT_DSHOT_BURST     DSHOT_DMAR_OFF
#define DEFAULT_DSHOT_TELEMETRY DSHOT_TELEMETRY_ON

// The OSD on from first boot, so the co-processor shows Betaflight without CLI changes.
#define DEFAULT_FEATURES        (FEATURE_OSD | FEATURE_TELEMETRY)

#define DEFAULT_VOLTAGE_METER_SOURCE    VOLTAGE_METER_ADC
// The HD board's 31:1 divider and scale; check against a meter on this board.
#define DEFAULT_VOLTAGE_METER_SCALE     157
#define DEFAULT_VOLTAGE_METER_DIVIDER   5

// MSP DisplayPort to the OSD co-processor. This gives UART4 the MSP and VTX (MSP) functions, which
// displayport needs. SmartAudio still controls the VTX: it registers after VTX MSP and replaces it.
#define MSP_DISPLAYPORT_UART    SERIAL_PORT_UART4
// The G0 acts on CLEAR_SCREEN as soon as it arrives, so Betaflight's clear on every OSD frame makes it
// flicker and loses elements drawn early in the frame. This sends only the changed characters instead.
// Needs patches/2026.6.2/msp-displayport-diff.patch.
#define USE_MSP_DISPLAYPORT_DIFF
#define SERIALRX_UART           SERIAL_PORT_USART1    // CRSF by default
#define VTX_SMARTAUDIO_UART     SERIAL_PORT_USART3    // TX3 only
#define ESC_SENSOR_UART         SERIAL_PORT_LPUART1   // ESC plugs' TLM line

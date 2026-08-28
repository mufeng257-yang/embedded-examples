#include "ti_msp_dl_config.h"

#include <stdbool.h>
#include <stdint.h>

#define IMU_CAN_ID             (0x01U)
#define IMU_MASTER_ID          (0x11U)
#define IMU_REQUEST_PERIOD_MS  (5U)

#define ACCEL_MIN              (-235.2f)
#define ACCEL_MAX              (235.2f)
#define GYRO_MIN               (-34.88f)
#define GYRO_MAX               (34.88f)
#define PITCH_MIN              (-90.0f)
#define PITCH_MAX              (90.0f)
#define ANGLE_MIN              (-180.0f)
#define ANGLE_MAX              (180.0f)

typedef struct {
    float accel[3];
    float gyro[3];
    float pitch;
    float yaw;
    float roll;
    float quat[4];
} IMU_Data;

static volatile uint32_t gMcanInterruptStatus;
static volatile bool gMcanService;
static volatile int32_t gTxAddStatus;
static IMU_Data gImu;

static float uintToFloat(uint32_t value, float minValue, float maxValue,
    uint32_t bits)
{
    return ((float) value) * (maxValue - minValue) /
               (float) ((1UL << bits) - 1UL) +
           minValue;
}

static void uartPutChar(char value)
{
    DL_UART_Main_transmitDataBlocking(UART_0_INST, (uint8_t) value);
}

static void uartPutString(const char *text)
{
    while (*text != '\0') {
        uartPutChar(*text++);
    }
}

static void uartPutHex(uint32_t value, uint32_t digits)
{
    static const char hex[] = "0123456789ABCDEF";
    while (digits != 0U) {
        uint32_t shift = (--digits) * 4U;
        uartPutChar(hex[(value >> shift) & 0x0FU]);
    }
}

static void uartPutFloat3(float value)
{
    int32_t scaled = (int32_t) (value * 1000.0f +
        ((value >= 0.0f) ? 0.5f : -0.5f));
    uint32_t whole;
    uint32_t fraction;
    char digits[10];
    uint32_t count = 0;

    if (scaled < 0) {
        uartPutChar('-');
        scaled = -scaled;
    }
    whole    = (uint32_t) scaled / 1000U;
    fraction = (uint32_t) scaled % 1000U;

    do {
        digits[count++] = (char) ('0' + (whole % 10U));
        whole /= 10U;
    } while (whole != 0U);
    while (count != 0U) {
        uartPutChar(digits[--count]);
    }
    uartPutChar('.');
    uartPutChar((char) ('0' + (fraction / 100U)));
    uartPutChar((char) ('0' + ((fraction / 10U) % 10U)));
    uartPutChar((char) ('0' + (fraction % 10U)));
}

static void uartPutVector(const char *name, const float *values, uint32_t count)
{
    uint32_t i;

    uartPutString(name);
    for (i = 0; i < count; i++) {
        uartPutChar(',');
        uartPutFloat3(values[i]);
    }
    uartPutString("\r\n");
}

static void imuSendRequest(uint8_t rid)
{
    DL_MCAN_TxBufElement tx = {0};

    tx.id      = ((uint32_t) IMU_CAN_ID) << 18U;
    tx.rtr     = 0U;
    tx.xtd     = 0U;
    tx.esi     = 0U;
    tx.dlc     = 8U;
    tx.brs     = 0U;
    tx.fdf     = 0U;
    tx.efc     = 0U;
    tx.data[0] = 0xCCU;
    tx.data[1] = rid;
    tx.data[2] = 0x00U;
    tx.data[3] = 0xDDU;
		if (DL_MCAN_getTxBufReqPend(MCAN0_INST) != 0U) {
				return;
		}
    DL_MCAN_writeMsgRam(MCAN0_INST, DL_MCAN_MEM_TYPE_BUF, 0U, &tx);
    gTxAddStatus = DL_MCAN_TXBufAddReq(MCAN0_INST, 0U);
}

static void imuDecode(const uint8_t *data)
{
    uint16_t raw0 = ((uint16_t) data[3] << 8U) | data[2];
    uint16_t raw1 = ((uint16_t) data[5] << 8U) | data[4];
    uint16_t raw2 = ((uint16_t) data[7] << 8U) | data[6];
		if (data[0] != 3U) {
        return;
    }
		float euler[3];
		gImu.pitch = uintToFloat(raw0, PITCH_MIN, PITCH_MAX, 16U);
		gImu.yaw   = uintToFloat(raw1, ANGLE_MIN, ANGLE_MAX, 16U);
		gImu.roll  = uintToFloat(raw2, ANGLE_MIN, ANGLE_MAX, 16U);
		euler[0] = gImu.roll;
		euler[1] = gImu.pitch;
		euler[2] = gImu.yaw;
		uartPutVector("EUL", euler, 3U);
//    switch (data[0]) {
//        case 1U:
//            gImu.accel[0] = uintToFloat(raw0, ACCEL_MIN, ACCEL_MAX, 16U);
//            gImu.accel[1] = uintToFloat(raw1, ACCEL_MIN, ACCEL_MAX, 16U);
//            gImu.accel[2] = uintToFloat(raw2, ACCEL_MIN, ACCEL_MAX, 16U);
//            uartPutVector("ACC", gImu.accel, 3U);
//            break;
//        case 2U:
//            gImu.gyro[0] = uintToFloat(raw0, GYRO_MIN, GYRO_MAX, 16U);
//            gImu.gyro[1] = uintToFloat(raw1, GYRO_MIN, GYRO_MAX, 16U);
//            gImu.gyro[2] = uintToFloat(raw2, GYRO_MIN, GYRO_MAX, 16U);
//            uartPutVector("GYR", gImu.gyro, 3U);
//            break;
//        case 3U: {
//            float euler[3];
//            gImu.pitch = uintToFloat(raw0, PITCH_MIN, PITCH_MAX, 16U);
//            gImu.yaw   = uintToFloat(raw1, ANGLE_MIN, ANGLE_MAX, 16U);
//            gImu.roll  = uintToFloat(raw2, ANGLE_MIN, ANGLE_MAX, 16U);
//            euler[0] = gImu.roll;
//            euler[1] = gImu.pitch;
//            euler[2] = gImu.yaw;
//            uartPutVector("EUL", euler, 3U);
//            break;
//        }
//        case 4U: {
//            uint16_t w = ((uint16_t) data[1] << 6U) | ((data[2] & 0xFCU) >> 2U);
//            uint16_t x = ((uint16_t) (data[2] & 0x03U) << 12U) |
//                         ((uint16_t) data[3] << 4U) | ((data[4] & 0xF0U) >> 4U);
//            uint16_t y = ((uint16_t) (data[4] & 0x0FU) << 10U) |
//                         ((uint16_t) data[5] << 2U) | ((data[6] & 0xC0U) >> 6U);
//            uint16_t z = ((uint16_t) (data[6] & 0x3FU) << 8U) | data[7];
//            gImu.quat[0] = uintToFloat(w, -1.0f, 1.0f, 14U);
//            gImu.quat[1] = uintToFloat(x, -1.0f, 1.0f, 14U);
//            gImu.quat[2] = uintToFloat(y, -1.0f, 1.0f, 14U);
//            gImu.quat[3] = uintToFloat(z, -1.0f, 1.0f, 14U);
//            uartPutVector("QUA", gImu.quat, 4U);
//            break;
//        }
//        default:
//            break;
//    }
}

static void serviceMcan(void)
{
    DL_MCAN_RxFIFOStatus fifo = {0};
    static DL_MCAN_RxBufElement rx;

    if (!gMcanService) {
        return;
    }
    gMcanService = false;
    if ((gMcanInterruptStatus & MCAN_IR_RF0N_MASK) != 0U) {
        fifo.num = DL_MCAN_RX_FIFO_NUM_0;
        DL_MCAN_getRxFIFOStatus(MCAN0_INST, &fifo);
        while (fifo.fillLvl != 0U) {
            DL_MCAN_readMsgRam(
                MCAN0_INST, DL_MCAN_MEM_TYPE_FIFO, 0U, fifo.num, &rx);
            DL_MCAN_writeRxFIFOAck(MCAN0_INST, fifo.num, fifo.getIdx);
            uint32_t rxId = (rx.id & 0x1FFC0000UL) >> 18U;

						if ((rx.xtd == 0U) && (rxId == IMU_MASTER_ID)) {
								imuDecode(rx.data);
						}
            DL_MCAN_getRxFIFOStatus(MCAN0_INST, &fifo);
        }
        gMcanInterruptStatus &= ~MCAN_IR_RF0N_MASK;
    }
}

int main(void)
{
    uint8_t rid = 3U;
    uint16_t heartbeat = 0U;

    SYSCFG_DL_init();
    NVIC_EnableIRQ(MCAN0_INST_INT_IRQN);

    while (DL_MCAN_OPERATION_MODE_NORMAL != DL_MCAN_getOpMode(MCAN0_INST)) {
    }

    uartPutString("DM-IMU-L1 CAN bridge ready\r\n");
    uartPutString("CAN=1000000, CAN_ID=0x01, MST_ID=0x11\r\n");

    while (1) {
        imuSendRequest(rid);
//        rid = (rid >= 4U) ? 1U : (uint8_t) (rid + 1U);
        DL_Common_delayCycles((CPUCLK_FREQ / 1000U) * IMU_REQUEST_PERIOD_MS);
        serviceMcan();
        if (++heartbeat >= (1000U / IMU_REQUEST_PERIOD_MS)) {
            DL_MCAN_ErrCntStatus counters;
            DL_MCAN_ProtocolStatus protocol;
            heartbeat = 0U;
            DL_MCAN_getErrCounters(MCAN0_INST, &counters);
            DL_MCAN_getProtocolStatus(MCAN0_INST, &protocol);
            uartPutString("MCAN_STAT,TEC=");
            uartPutHex(counters.transErrLogCnt, 2U);
            uartPutString(",REC=");
            uartPutHex(counters.recErrCnt, 2U);
            uartPutString(",LEC=");
            uartPutHex(protocol.lastErrCode, 1U);
            uartPutString(",BO=");
            uartPutHex(protocol.busOffStatus, 1U);
            uartPutString(",ACT=");
            uartPutHex(protocol.act, 1U);
            uartPutString(",PEND=");
            uartPutHex(DL_MCAN_getTxBufReqPend(MCAN0_INST), 8U);
            uartPutString(",ADD=");
            uartPutHex((uint32_t) gTxAddStatus, 8U);
            uartPutString("\r\n");
        }
    }
}

void MCAN0_INST_IRQHandler(void)
{
    if (DL_MCAN_getPendingInterrupt(MCAN0_INST) == DL_MCAN_IIDX_LINE1) {
        uint32_t status;

        status = DL_MCAN_getIntrStatus(MCAN0_INST);
        status &= MCAN_IR_RF0N_MASK;

        if (status != 0U) {
            DL_MCAN_clearIntrStatus(
                MCAN0_INST,
                MCAN_IR_RF0N_MASK,
                DL_MCAN_INTR_SRC_MCAN_LINE_1);

            gMcanInterruptStatus |= MCAN_IR_RF0N_MASK;
            gMcanService = true;
        }
    }
}

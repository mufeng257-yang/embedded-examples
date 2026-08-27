#include "bsp_spi.h"
uint8_t spi_read_write_byte(uint8_t dat)
{
    DL_SPI_transmitDataBlocking8(SPI_INST, dat);
    return DL_SPI_receiveDataBlocking8(SPI_INST);
}































#ifndef app_temperature.h
#define app_temperature.h

//------------------------------------------------------------------------------
//         Local definitions
//------------------------------------------------------------------------------

/// TWI clock frequency in Hz.
#define TWCK            100000

///TWI0 ID
#define TWI_TEMP_ID    AT91C_ID_TWI0

#define INVALID_TEMP (-110)
/// Slave address of Temperature Sensor.
#define MCP9800_ADDRESS   0x48

/// Internal register within MCP9800
#define TEMP_REG       0x0
#define CONF_REG       0x1
#define HYST_REG       0x2
#define LIMT_REG       0x3


typedef struct 
{
  unsigned char bOneShot;
  unsigned char bRes;
  unsigned char bFaultQueue;
  unsigned char bAlrtPol;
  unsigned char bMode;
  unsigned char bShutdown;
}Confg_Reg;

//------------------------------------------------------------------------------
//         Local variables
//------------------------------------------------------------------------------

/// Pio pins to configure.
static const Pin pins_twi_temp[] = {PINS_TWI0};

/// TWI driver instance.
static Twid twid;


/// constant represent float value
const char* sixteenths[]={ "0","0625","125","1875","25","3125","375","4375","5","5625","625","6875","75","8125","875",
"9375"};

short GetRealTemp(unsigned short reg_value,unsigned char res)
{
  if( 8 < res && 13 > res)
  {
    return (reg_value>>(16-res));
    
  }
  return INVALID_TEMP;
}

//------------------------------------------------------------------------------
///  Fill the register value into specific structure
//MCP9800 configuration register:8bits
//bit 7 ONE-SHOT bits
//bit 5-6 Š²Š€ ADC RESOLUTION bit
//bit 3-4 FAULT QUEUE bit
//bit 2 ALERT POLARITY bit
//bit 1 COMP/INT bit
//bit 0 SHUTDOWN bit
//------------------------------------------------------------------------------

void FillConfigReg(unsigned char reg_value,Confg_Reg *conf)
{
  unsigned char res_array[]={9,10,11,12};
  unsigned char fault_queue_array[]={1,2,4,6};
  unsigned char temp;
  temp = ((reg_value>>7)&0x1);
  conf->bOneShot = temp;
  temp = ((reg_value>>5)&0x3);
  conf->bRes = res_array[temp];
  temp = ((reg_value>>3)&0x3);
  conf->bFaultQueue = fault_queue_array[temp];
  temp = ((reg_value>>2)&0x1);
  conf->bAlrtPol = temp;
  temp = ((reg_value>>1)&0x1);
  conf->bMode = temp;
  conf->bShutdown = (reg_value&0x1);
  
}

//------------------------------------------------------------------------------
/// Main function
//------------------------------------------------------------------------------
void print_temperature()
{
    //volatile unsigned int i;
    //Async async;

    char key;
    unsigned char Tdata[2];
    short temp;
    Confg_Reg confg;

    PIO_Configure(pins_twi_temp, PIO_LISTSIZE(pins_twi_temp));
    // Configure TWI
    AT91C_BASE_PMC->PMC_PCER = 1 << TWI_TEMP_ID;//AT91C_ID_TWI0;
    TWI_ConfigureMaster(AT91C_BASE_TWI0, TWCK, BOARD_MCK);
    TWID_Initialize(&twid, AT91C_BASE_TWI0);
    

        char index;
        short integer;
        TWID_Read(&twid, MCP9800_ADDRESS, CONF_REG, 0x01, Tdata, 0x01, 0);  
        FillConfigReg(Tdata[0],&confg);
        TWID_Read(&twid, MCP9800_ADDRESS, TEMP_REG, 0x01, Tdata, 0x02, 0);
        temp = ((Tdata[0]<<8)|(Tdata[1]));
        temp = GetRealTemp(temp,confg.bRes);
        index = temp%(2<<(confg.bRes-8-1));
        index =  index <<(4-(confg.bRes-8));
        integer =   temp/(2<<(confg.bRes-8-1));       
        printf("%d.%s",integer,sixteenths[index]);
        

}



#endif /* app_temperature.h */
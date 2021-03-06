#include "mcp23017.h"

#ifndef __SFX_PI__
#error This File is Specific To Processing Environement
#endif

#define NAME (sfx::formatString("MCP:0x%02x", this->deviceAddress))

/*****************************************************************
 * This is the default constructor for the class. It assigns
 * all private variables to default values and calls the openI2C()
 * function to open the default I2C device "/dev/i2c-0".
 *****************************************************************/
mcp23017::mcp23017(void){
    
    this->i2cFileName = "/dev/i2c-0";
    this->deviceAddress= 0;
	this->i2cDescriptor = -1;
	sfx::log(NAME, "Opening I2C Device\n");
    err = this->openI2C();
}

/*******************************************************************
 * This is the overloaded constructor. It allows the programmer to
 * specify a custom I2C device & device address
 * The device descriptor is determined by the openI2C() private member
 * function call.
 * *****************************************************************/
mcp23017::mcp23017(sfx::hex_t dev_addr, std::string i2c_file_name){
	
    this->i2cFileName = i2c_file_name;
	this->deviceAddress = dev_addr;
	this->i2cDescriptor = -1;
    sfx::log(NAME, "Open I2C Device : %s:%x\n", i2c_file_name, dev_addr);
    err = this->openI2C();
}

/**********************************************************************
 * This is the class destructor it simply closes the open I2C device
 * by calling the closeI2C() which in turn calls the close() system call
 * *********************************************************************/
mcp23017::~mcp23017(void){
	
    sfx::log(NAME, "Closing I2C Device\n");
    this->closeI2C();
}

/**********************************************************************
 * This function opens the I2C device by simply calling the open system
 * call on the I2C device specified in the i2cFileName string. The I2C
 * device is opened for writing and reading. The i2cDescriptor private
 * variable is set by the return value of the open() system call.
 * This variable will be used to reference the opened I2C device by the
 * ioctl() & close() system calls.
 * ********************************************************************/
int mcp23017::openI2C(){
	
    this->i2cDescriptor = open(i2cFileName.c_str(), O_RDWR);
    if(this->i2cDescriptor < 0){
        sfx::err(NAME, "Could not open file (1)\n");
        return 1;
    }
    return 0;
}

/*********************************************************************
 * This function closes the I2C device by calling the close() system call
 * on the I2C device descriptor.
 * *******************************************************************/
int mcp23017::closeI2C(){
	
	int retVal = -1;
	retVal = close(this->i2cDescriptor);
    if(retVal < 0){
        sfx::err(NAME, "Could not close file (1)\n");
        return 1;
    }
	return retVal;
}

/********************************************************************
 *This function writes a byte of data "data" to a specific register
 *"reg_addr" in the I2C device This involves sending these two bytes
 *in order to the i2C device by means of the ioctl() command. Since
 *both bytes are written (no read/write switch), both pieces
 *of information can be sent in a single message (i2c_msg structure)
 ********************************************************************/
int mcp23017::writeReg(sfx::hex_t reg_addr, sfx::hex_t data) const{

    int retVal = -1;
    
    sfx::hex_t buff[2];
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];

    buff[0] = reg_addr;
    buff[1] = data;

    messages[0].addr = deviceAddress;
    messages[0].flags = 0;
    messages[0].len = sizeof(buff);
    messages[0].buf = buff;

    packets.msgs = messages;
    packets.nmsgs = 1;

    retVal = ioctl(this->i2cDescriptor, I2C_RDWR, &packets);
    if(retVal < 0)
        sfx::err(NAME, "Write to I2C Device failed\n");
    
    return retVal;
}

int mcp23017::writeReg(sfx::hex_t reg_addr, sfx::hex_t mask, sfx::hex_t data) const{
    
    // Get curent register value
    sfx::hex_t reg_val = 0;
    this->readReg( reg_addr, reg_val );
    
    // Only change bits specified by mask and send new register value
    return this->writeReg( reg_addr, changeBit(reg_val, data, mask) );
}

/********************************************************************
 *This function reads a byte of data "data" from a specific register
 *"reg_addr" in the I2C device. This involves sending the register address
 *byte "reg_Addr" with "write" asserted and then instructing the
 *I2C device to read a byte of data from that address ("read asserted").
 *This necessitates the use of two i2c_msg structs. One for the register
 *address write and another for the read from the I2C device i.e.
 *I2C_M_RD flag is set. The read data is then saved into the reference
 *variable "data".
 ********************************************************************/
int mcp23017::readReg(sfx::hex_t reg_addr, sfx::hex_t &data) const{

    int retVal = -1;
    
    sfx::hex_t *inbuff, outbuff;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    outbuff = reg_addr;
    messages[0].addr = deviceAddress;
    messages[0].flags= 0;
    messages[0].len = sizeof(outbuff);
    messages[0].buf = &outbuff;

    inbuff = &data;
    messages[1].addr = deviceAddress;
    messages[1].flags = I2C_M_RD;
    messages[1].len = sizeof(*inbuff);
    messages[1].buf = inbuff;

    packets.msgs = messages;
    packets.nmsgs = 2;

    retVal = ioctl(this->i2cDescriptor, I2C_RDWR, &packets);
    if(retVal < 0)
        sfx::err(NAME, "Read from I2C Device failed\n");

    return retVal;
}

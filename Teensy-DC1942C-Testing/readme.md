# Arduino Script Info

## Term Definitions
- CSB -> CS or SS -> Chip Select or Slave Select. A pin used in SPI Communication to specify what device the SPI master is talking too. When the CS pin is pulled high, the SPI slave ignores all commands. Once the CS of a slave is pulled low the slave is now active on the bus

## Programming Examples
See https://www.analog.com/media/en/technical-documentation/data-sheets/680412fc.pdf page 54
### Waking Up Serial Interface

1. Send dummy byte 
2. Wait 3*t_wake. twake is normally 100us but can be up to 300 us, use the maximum. See page 7. 
3. Send commands

### Writing Configuration Registers
1. Create configuration register. See table 35
2. Send WRCFG command (LTC6804_wrcfg() in arduino). This command handles steps 


## Other Useful bits of info
### Calculating under- and over-voltage threshold
Table 36 contains information about writing to the configuration register and the bits. Table 46 contains the info for actually calculating the values to feed into the configuration register. the only put where you input the over and undervoltage threshold and then send it to the LTC6804. For an example see https://ez.analog.com/power/battery-management-system/f/qa/563804/ltc6804-2-vov
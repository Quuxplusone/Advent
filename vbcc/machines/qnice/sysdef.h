//;
//;  sysdef.asm: This file contains definitions to simplify assembler programming
//;              and for accessing the various hardware registers via MMIO
//;
//
//***************************************************************************************
//*  Assembler macros which make life much easier:
//***************************************************************************************
//


//
//  Some register short names:
//

//
//***************************************************************************************
//*  IO-page addresses: Default: 8 registers per block
//***************************************************************************************
//
#define IO_AREA_START  	0xFF00
//
//---------------------------------------------------------------------------------------
//  Block FF00: FUNDAMENTAL IO
//---------------------------------------------------------------------------------------
//
//  Switch-register:
//
#define IO_SWITCH_REG  	0xFF00 // 16 binary keys
//
//  Registers for TIL-display:
//
#define IO_TIL_DISPLAY 	0xFF01 // Address of TIL-display
#define IO_TIL_MASK    	0xFF02 // Mask register of TIL display
//
//  USB-keyboard-registers:
//
#define IO_KBD_STATE   	0xFF04 // Status register of USB keyboard
//    Bit  0 (read only):      New ASCII character avaiable for reading
//                             (bits 7 downto 0 of Read register)
//    Bit  1 (read only):      New special key available for reading
//                             (bits 15 downto 8 of Read register)
//    Bits 2..4 (read/write):  Locales: 000 = US English keyboard layout,
//                             001 = German layout, others: reserved for more locales
//    Bits 5..7 (read only):   Modifiers: 5 = shift, 6 = alt, 7 = ctrl
//                             Only valid, when bits 0 and/or 1 are '1'
//
#define IO_KBD_DATA    	0xFF05 // Data register of USB keyboard
//    Contains the ASCII character in bits 7 downto 0  or the special key code
//    in 15 downto 8. The "or" is meant exclusive, i.e. it cannot happen that
//    one transmission contains an ASCII character PLUS a special character.
//
//---------------------------------------------------------------------------------------
//  Block FF08: SYSTEM COUNTERS
//---------------------------------------------------------------------------------------
//
//  CYCLE-COUNT-registers       
//
#define IO_CYC_LO      	0xFF08     // low word of 48-bit counter
#define IO_CYC_MID     	0xFF09     // middle word of 48-bit counter
#define IO_CYC_HI      	0xFF0A     // high word of 48-bit counter
#define IO_CYC_STATE   	0xFF0B     // status register
//    Bit  0 (write only):     Reset counter to zero and start counting, i.e.
//                             bit 1 is automatically set to 1 when resetting
//    Bit  1 (read/write):     Start/stop counter
//
//  INSTRUCTION-COUNT-registers       
//
#define IO_INS_LO      	0xFF0C     // low word of 48-bit counter
#define IO_INS_MID     	0xFF0D     // middle word of 48-bit counter
#define IO_INS_HI      	0xFF0E     // high word of 48-bit counter
#define IO_INS_STATE   	0xFF0F     // status register
//    Bit  0 (write only):     Reset counter to zero and start counting, i.e.
//                             bit 1 is automatically set to 1 when resetting
//    Bit  1 (read/write):     Start/stop counter
//
//---------------------------------------------------------------------------------------
//  Block FF10: UART
//---------------------------------------------------------------------------------------
//
//  QNICE-FPGA supports: IO_UART_SRA, IO_UART_RHRA and IO_UART_THRA 
//  The other registers are mentioned for completeness to map real hardware (16550)
//
#define IO_UART_BASE_ADDRESS   	0xFF10
#define IO_UART_MR1A   	0xFF10 // n/a
#define IO_UART_MR1B   	0xFF10 // n/a
#define IO_UART_SRA    	0xFF11 // Status register (relative to base address)
#define IO_UART_RHRA   	0xFF12 // Receiving register (relative to base address)
#define IO_UART_THRA   	0xFF13 // Transmitting register (relative to base address)
//
//---------------------------------------------------------------------------------------
//  Block FF18: EAE
//---------------------------------------------------------------------------------------
//
//  EAE (Extended Arithmetic Element) registers:
//
#define IO_EAE_OPERAND_0   	0xFF18
#define IO_EAE_OPERAND_1   	0xFF19
#define IO_EAE_RESULT_LO   	0xFF1A
#define IO_EAE_RESULT_HI   	0xFF1B
#define IO_EAE_CSR         	0xFF1C // Command and Status Register
//
//  EAE-Opcodes (CSR):   0x0000  MULU  32-bit result in LO HI
//                       0x0001  MULS  32-bit result in LO HI
//                       0x0002  DIVU  result in LO, modulo in HI
//                       0x0003  DIVS  result in LO, modulo in HI
//  Bit 15 of CSR is the busy bit. If it is set, the EAE is still busy crunching numbers.
//
//---------------------------------------------------------------------------------------
//  Block FF20: SD CARD
//---------------------------------------------------------------------------------------
//
//  SD CARD INTERFACE registers
//
#define IO_SD_BASE_ADDRESS 	0xFF20
#define IO_SD_ADDR_LO  	0xFF20 // low word of 32bit linear SD card block address
#define IO_SD_ADDR_HI  	0xFF21 // high word of 32bit linear SD card block address
#define IO_SD_DATA_POS 	0xFF22 // "Cursor" to navigate the 512-byte data buffer
#define IO_SD_DATA     	0xFF23 // read/write 1 byte from/to the 512-byte data buffer
#define IO_SD_ERROR    	0xFF24 // error code of last operation (read only)
#define IO_SD_CSR      	0xFF25 // Command and Status Register (write to execute command)
//
//  SD-Opcodes (CSR):    0x0000  Reset SD card
//                       0x0001  Read 512 bytes from the linear block address
//                       0x0002  Write 512 bytes to the linear block address
//  Bits 0 .. 2 are write-only (reading always returns 0)
//  Bits 13 .. 12 return the card type: 00 = no card / unknown card
//                                      01 = SD V1
//                                      10 = SD V2
//                                      11 = SDHC                       
//  Bit 14 of the CSR is the error bit: 1, if the last operation failed. In such
//                                      a case, the error code is in IO_SD_ERROR and
//                                      you need to reset the controller to go on
//  Bit 15 of the CSR is the busy bit: 1, if current operation is still running
//
//---------------------------------------------------------------------------------------
//  Block FF28: TIMER 0 and 1
//---------------------------------------------------------------------------------------
//
//  Interrupt timer: There are two timers capable of generating interrupts.
//                   Each timer is controlled by three 16 bit registers:
//
//  IO_TIMER_x_PRE: The 100 kHz timer clock is divided by the value stored in
//                  this device register. 100 (which corresponds to 0x0064 in
//                  the prescaler register) yields a 1 millisecond pulse which
//                  in turn is fed to the actual counter.
//  IO_TIMER_x_CNT: When the number of output pulses from the prescaler circuit 
//                  equals the number stored in this register, an interrupt will
//                  be generated (if the interrupt address is 0x0000, the
//                  interrupt will be suppressed).
//  IO_TIMER_x_INT: This register contains the address of the desired interrupt 
//                  service routine.
//
#define IO_TIMER_BASE_ADDRESS  	0xFF28
#define IO_TIMER_0_PRE 	0xFF28
#define IO_TIMER_0_CNT 	0xFF29
#define IO_TIMER_0_INT 	0xFF2A
#define IO_TIMER_1_PRE 	0xFF2B
#define IO_TIMER_1_CNT 	0xFF2C
#define IO_TIMER_1_INT 	0xFF2D
//
//---------------------------------------------------------------------------------------
//  Block FF30: VGA (double block, 16 registers)
//---------------------------------------------------------------------------------------
//
#define VGA_STATE          	0xFF30 // VGA status register
    // Bits 11-10: Hardware scrolling / offset enable: Bit #10 enables the use
    //             of the offset register #4 (display offset) and bit #11
    //             enables the use of register #5 (read/write offset).
    // Bit      9: Busy: VGA is currently busy, e.g. clearing the screen,
    //             printing, etc. While busy, commands will be ignored, but
    //             they can still be written into the registers, though
    // Bit      8: Set bit to clear screen. Read bit to find out, if clear
    //             screen is still active
    // Bit      7: VGA enable (1 = on; 0: no VGA signal is generated)
    // Bit      6: Hardware cursor enable
    // Bit      5: Hardware cursor blink enable
    // Bit      4: Hardware cursor mode: 1 - small
    //                                   0 - large
    // Bits   2-0: Output color for the whole screen, bits (2, 1, 0) = RGB
#define VGA_CR_X           	0xFF31 // VGA cursor X position
#define VGA_CR_Y           	0xFF32 // VGA cursor Y position
#define VGA_CHAR           	0xFF33 // write: VGA character to be displayed
                                // read: character "under" the cursor
#define VGA_OFFS_DISPLAY   	0xFF34 // Offset in bytes that is used when displaying
                                // the video RAM. Scrolling forward one line
                                // means adding 0x50 to this register.
                                // Only works, if bit #10 in VGA_STATE is set.
#define VGA_OFFS_RW        	0xFF35 // Offset in bytes that is used, when you read
                                // or write to the video RAM using VGA_CHAR.
                                // Works independently from VGA_OFFS_DISPLAY.
                                // Active, when bit #11 in VGA_STATE is set.
#define VGA_HDMI_H_MIN     	0xFF36 // HDMI Data Enable: X: minimum valid column
#define VGA_HDMI_H_MAX     	0xFF37 // HDMI Data Enable: X: maximum valid column
#define VGA_HDMI_V_MAX     	0xFF38 // HDMI Data Enable: Y: maximum row (line)                                
//
//---------------------------------------------------------------------------------------
//  Block FFF0: MEGA65 (double block, 16 registers)
//---------------------------------------------------------------------------------------
//
//  HyperRAM
//
#define IO_M65HRAM_LO      	0xFFF0 // Low word of address  (15 downto 0)
#define IO_M65HRAM_HI      	0xFFF1 // High word of address (26 downto 16)
#define IO_M65HRAM_DATA8   	0xFFF2 // HyperRAM native 8-bit data in/out
#define IO_M65HRAM_DATA16  	0xFFF3 // HyperRAM 16-bit data in/out

//
//***************************************************************************************
//* Constant definitions
//***************************************************************************************
//

// ========== VGA ==========

#define VGA_MAX_X              	79                      // Max. X-coordinate in decimal!
#define VGA_MAX_Y              	39                      // Max. Y-coordinate in decimal!
#define VGA_MAX_CHARS          	3200                    // 80 * 40 chars
#define VGA_CHARS_PER_LINE     	80  

#define VGA_EN_HW_CURSOR       	0x0040                  // Show hardware cursor
#define VGA_EN_HW_SCRL         	0x0C00                  // Hardware scrolling enable
#define VGA_CLR_SCRN           	0x0100                  // Clear screen
#define VGA_BUSY               	0x0200                  // VGA is currently performing a task

#define VGA_COLOR_RED          	0x0004
#define VGA_COLOR_GREEN        	0x0002
#define VGA_COLOR_BLUE         	0x0001
#define VGA_COLOR_WHITE        	0x0007

// ========== CYCLE COUNTER ==========

#define CYC_RESET              	0x0001                  // Reset cycle counter
#define CYC_RUN                	0x0002                  // Start/stop counter

// ========== CYCLE COUNTER ==========

#define INS_RESET              	0x0001                  // Reset instruction counter
#define INS_RUN                	0x0002                  // Start/stop counter

// ========== EAE ==========

#define EAE_MULU               	0x0000                  // Unsigned 16 bit multiplication
#define EAE_MULS               	0x0001                  // Signed 16 bit multiplication
#define EAE_DIVU               	0x0002                  // Unsigned 16 bit division with remainder
#define EAE_DIVS               	0x0003                  // Signed 16 bit division with remainder
#define EAE_BUSY               	0x8000                  // Busy flag (1 = operation still running)

// ========== SD CARD ==========

#define SD_CMD_RESET           	0x0000                  // Reset SD card
#define SD_CMD_READ            	0x0001                  // Read 512 bytes from SD to internal buffer
#define SD_CMD_WRITE           	0x0002                  // Write 512 bytes from int. buf. to SD
#define SD_BIT_ERROR           	0x4000                  // Error flag: 1, if last operation failed
#define SD_BIT_BUSY            	0x8000                  // Busy flag: 1, if current op. is still running
#define SD_TIMEOUT_MID         	0x0479                  // equals ~75.000.000 cycles, i.e. 1.5sec @ 50 MHz

#define SD_ERR_MASK            	0x00FF                  // AND mask for errors: HI byte = state machine info, so mask it for error checks 
#define SD_ERR_R1_ERROR        	0x0001                  // SD Card R1 error (R1 bit 6-0)
#define SD_ERR_CRC_OR_TIMEOUT  	0x0002                  // Read CRC error or Write Timeout error
#define SD_ERR_RESPONSE_TOKEN  	0x0003                  // Data Response Token error (Token bit 3)
#define SD_ERR_ERROR_TOKEN     	0x0004                  // Data Error Token error (Token bit 3-0)
#define SD_ERR_WRITE_PROTECT   	0x0005                  // SD Card Write Protect switch
#define SD_ERR_CARD_UNUSABLE   	0x0006                  // Unusable SD card
#define SD_ERR_NO_CARD         	0x0007                  // No SD card (no response from CMD0)
#define SD_ERR_READ_TIMEOUT    	0x0008                  // Timeout while trying to receive the read start token "FE"
#define SD_ERR_TIMEOUT         	0xEEFF                  // General timeout

#define SD_CT_SD_V1            	0x0001                  // Card type: SD Version 1
#define SD_CT_SD_V2            	0x0002                  // Card type: SD Version 2
#define SD_CT_SDHC             	0x0003                  // Card type: SDHC (or SDXC)

// ========== FAT32 =============

// FAT32 ERROR CODES

#define FAT32_ERR_MBR          	0xEE10                  // no or illegal Master Boot Record (MBR) found
#define FAT32_ERR_PARTITION_NO 	0xEE11                  // the partition number needs to be in the range 1 .. 4
#define FAT32_ERR_PARTTBL      	0xEE12                  // no or illegal partition table entry found (e.g. no FAT32 partition)
#define FAT32_ERR_NOTIMPL      	0xEE13                  // functionality is not implemented
#define FAT32_ERR_SIZE         	0xEE14                  // partition size or volume size too large (see doc/constraints.txt)
#define FAT32_ERR_NOFAT32      	0xEE15                  // illegal volume id (either not 512 bytes per sector, or not 2 FATs or wrong magic)
#define FAT32_ERR_ILLEGAL_SIC  	0xEE16                  // trying to read/write a sector within a cluster that is out of range
#define FAT32_ERR_ILLEGAL_CLUS 	0xEE17                  // trying to access an illegal cluster number
#define FAT32_ERR_CORRUPT_DH   	0xEE18                  // corrupt directory handle (e.g. because current to-be-read offs > sector size)
#define FAT32_ERR_DIRNOTFOUND  	0xEE19                  // directory not found (illegal path name passed to change directory command)
#define FAT32_ERR_FILENOTFOUND 	0xEE20                  // file not found
#define FAT23_ERR_SEEKTOOLARGE 	0xEE21                  // seek position > file size

// FAT32 STATUS CODES

#define FAT32_EOF              	0xEEEE                  // end of file reached

// LAYOUT OF THE MOUNT DATA STRUCTURE (DEVICE HANDLE)

#define FAT32_DEV_RESET        	0x0000                  // pointer to device reset function
#define FAT32_DEV_BLOCK_READ   	0x0001                  // pointer to 512-byte block read function
#define FAT32_DEV_BLOCK_WRITE  	0x0002                  // pointer to 512-byte block write function
#define FAT32_DEV_BYTE_READ    	0x0003                  // pointer to 1-byte read function (within block buffer)
#define FAT32_DEV_BYTE_WRITE   	0x0004                  // pointer to 1-byte write function (within block buffer)
#define FAT32_DEV_PARTITION    	0x0005                  // number of partition to be mounted
#define FAT32_DEV_FS_LO        	0x0006                  // file system start address (LBA): low word
#define FAT32_DEV_FS_HI        	0x0007                  // file system start address (LBA): high word
#define FAT32_DEV_FAT_LO       	0x0008                  // fat start address (LBA): low word
#define FAT32_DEV_FAT_HI       	0x0009                  // fat start address (LBA): high word
#define FAT32_DEV_CLUSTER_LO   	0x000A                  // cluster start address (LBA): low word
#define FAT32_DEV_CLUSTER_HI   	0x000B                  // cluster start address (LBA): high word
#define FAT32_DEV_SECT_PER_CLUS	0x000C                  // sectors per cluster
#define FAT32_DEV_RD_1STCLUS_LO	0x000D                  // root directory first cluster: low word
#define FAT32_DEV_RD_1STCLUS_HI	0x000E                  // root directory first cluster: high word
#define FAT32_DEV_AD_1STCLUS_LO	0x000F                  // currently active directory first cluster: low word
#define FAT32_DEV_AD_1STCLUS_HI	0x0010                  // currently active directory first cluster: high word
#define FAT32_DEV_BUFFERED_FDH 	0x0011                  // FDH which is responsible for the current 512 byte hardware buffer filling

#define FAT32_DEV_STRUCT_SIZE  	0x0012                  // size (words) of the mount data structure (device handle)

// LAYOUT OF THE FILE HANDLE AND DIRECTORY HANDLE (FDH)

#define FAT32_FDH_DEVICE       	0x0000                  // pointer to the device handle
#define FAT32_FDH_CLUSTER_LO   	0x0001                  // current cluster (low word)
#define FAT32_FDH_CLUSTER_HI   	0x0002                  // current cluster (high word)
#define FAT32_FDH_SECTOR       	0x0003                  // current sector
#define FAT32_FDH_INDEX        	0x0004                  // current byte index within current sector
#define FAT32_FDH_SIZE_LO      	0x0005                  // only in case FDH is a file: low word of file size, otherwise undefined
#define FAT32_FDH_SIZE_HI      	0x0006                  // only in case FDH is a file: high word of file size, otherwise undefined
#define FAT32_FDH_READ_LO      	0x0007                  // only in case FDH is a file: low word of already read amount of bytes
#define FAT32_FDH_READ_HI      	0x0008                  // only in case FDH is a file: high word of already read amount of bytes

#define FAT32_FDH_STRUCT_SIZE  	0x0009                  // size of the directory handle structure

// FILE ATTRIBUTES

#define FAT32_FA_READ_ONLY     	0x0001                  // read only file
#define FAT32_FA_HIDDEN        	0x0002                  // hidden file
#define FAT32_FA_SYSTEM        	0x0004                  // system file
#define FAT32_FA_VOLUME_ID     	0x0008                  // volume id (name of the volume)
#define FAT32_FA_DIR           	0x0010                  // directory
#define FAT32_FA_ARCHIVE       	0x0020                  // archive flag

#define FAT32_FA_DEFAULT       	0x0035                  // browse for non hidden files and directories but not for the volume id
#define FAT32_FA_ALL           	0x0037                  // browse for all files, but not for the volume id

// LAYOUT OF THE DIRECTORY ENTRY STRUCTURE

#define FAT32_DE_NAME          	0x0000                  // volume, file or directory name, zero terminated (max 256 characters)
#define FAT32_DE_ATTRIB        	0x0101                  // file attributes (read-only, hidden, system, volume id, directory, archive)
#define FAT32_DE_SIZE_LO       	0x0102                  // file size: low word
#define FAT32_DE_SIZE_HI       	0x0103                  // file size: high word
#define FAT32_DE_YEAR          	0x0104                  // last file write: year   (valid range 1980 .. 2107)
#define FAT32_DE_MONTH         	0x0105                  // last file write: month
#define FAT32_DE_DAY           	0x0106                  // last file write: day
#define FAT32_DE_HOUR          	0x0107                  // last file write: hour
#define FAT32_DE_MINUTE        	0x0108                  // last file write: minute
#define FAT32_DE_SECOND        	0x0109                  // last file write: second (in 2 second steps, valid range 0 .. 58)
#define FAT32_DE_CLUS_LO       	0x010A                  // start cluster: low word
#define FAT32_DE_CLUS_HI       	0x010B                  // start cluster: high word

#define FAT32_DE_STRUCT_SIZE   	0x010C                  // size (words) of the directory entry data structure of the

// DISPLAY FLAGS FOR FILE ENTRY PRETTY PRINTER

#define FAT32_PRINT_SHOW_DIR   	0x0001                  // show "<DIR>" indicator
#define FAT32_PRINT_SHOW_ATTRIB	0x0002                  // show attributes as "HRSA"
#define FAT32_PRINT_SHOW_SIZE  	0x0004                  // show file size
#define FAT32_PRINT_SHOW_DATE  	0x0008                  // show file date as YYYY-MM-DD
#define FAT32_PRINT_SHOW_TIME  	0x0010                  // show file time as HH:MM

#define FAT32_PRINT_DEFAULT    	0x001D                  // print <DIR> indicator, size, date and time (no attributes)
#define FAT32_PRINT_ALL        	0x001F                  // print all details

// ========== KEYBOARD ==========

// STATUS REGISTER

#define KBD_NEW_ASCII          	0x0001                  // new ascii character available
#define KBD_NEW_SPECIAL        	0x0002                  // new special key available
#define KBD_NEW_ANY            	0x0003                  // any new key available 

#define KBD_ASCII              	0x00FF                  // mask the special keys
#define KBD_SPECIAL            	0xFF00                  // mask the ascii keys

#define KBD_LOCALE             	0x001C                  // bit mask for checking locales
#define KBD_LOCALE_US          	0x0000                  // default: US keyboard layout
#define KBD_LOCALE_DE          	0x0004                  // DE: German keyboard layout

#define KBD_MODIFIERS          	0x00E0                  // bit mask for checking modifiers
#define KBD_SHIFT              	0x0020                  // modifier "SHIFT" pressed
#define KBD_ALT                	0x0040                  // modifier "ALT" pressed
#define KBD_CTRL               	0x0080                  // modifier "CTRL" pressed

// READ REGISTER: COMMON ASCII CODES

#define KBD_SPACE              	0x0020
#define KBD_ENTER              	0x000D
#define KBD_ESC                	0x001B
#define KBD_TAB                	0x0009
#define KBD_BACKSPACE          	0x0008

// READ REGISTER: SPECIAL KEYS

#define KBD_F1                 	0x0100
#define KBD_F2                 	0x0200
#define KBD_F3                 	0x0300
#define KBD_F4                 	0x0400
#define KBD_F5                 	0x0500
#define KBD_F6                 	0x0600
#define KBD_F7                 	0x0700
#define KBD_F8                 	0x0800
#define KBD_F9                 	0x0900
#define KBD_F10                	0x0A00
#define KBD_F11                	0x0B00
#define KBD_F12                	0x0C00

#define KBD_CUR_UP             	0x1000
#define KBD_CUR_DOWN           	0x1100
#define KBD_CUR_LEFT           	0x1200
#define KBD_CUR_RIGHT          	0x1300
#define KBD_PG_UP              	0x1400
#define KBD_PG_DOWN            	0x1500
#define KBD_HOME               	0x1600
#define KBD_END                	0x1700
#define KBD_INS                	0x1800
#define KBD_DEL                	0x1900

// READ REGISTER: CTRL + character is also mapped to an ASCII code

#define KBD_CTRL_A             	0x0001 
#define KBD_CTRL_B             	0x0002 
#define KBD_CTRL_C             	0x0003 
#define KBD_CTRL_D             	0x0004 
#define KBD_CTRL_E             	0x0005 
#define KBD_CTRL_F             	0x0006 
#define KBD_CTRL_G             	0x0007 
#define KBD_CTRL_H             	0x0008 
#define KBD_CTRL_I             	0x0009 
#define KBD_CTRL_J             	0x000A 
#define KBD_CTRL_K             	0x000B 
#define KBD_CTRL_L             	0x000C 
#define KBD_CTRL_M             	0x000D 
#define KBD_CTRL_N             	0x000E 
#define KBD_CTRL_O             	0x000F 
#define KBD_CTRL_P             	0x0010 
#define KBD_CTRL_Q             	0x0011 
#define KBD_CTRL_R             	0x0012 
#define KBD_CTRL_S             	0x0013 
#define KBD_CTRL_T             	0x0014 
#define KBD_CTRL_U             	0x0015 
#define KBD_CTRL_V             	0x0016 
#define KBD_CTRL_W             	0x0017 
#define KBD_CTRL_X             	0x0018 
#define KBD_CTRL_Y             	0x0019 
#define KBD_CTRL_Z             	0x001A 

//
//  Useful ASCII constants:
//
#define CHR_BELL       	0x0007 // ASCII-BELL character
#define CHR_TAB        	0x0009 // ASCII-TAB character
#define CHR_SPACE      	0x0020 // ASCII-Space
#define CHR_CR         	0x000d // Carriage return
#define CHR_LF         	0x000a // Line feed

/***************************************************************************

    memory.h

    Functions which handle the CPU memory accesses.

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#pragma once

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "mamecore.h"
#include "devintrf.h"
#include "tokenize.h"



/***************************************************************************
    BASIC TYPE DEFINITIONS
***************************************************************************/

/* ----- typedefs for data and offset types ----- */
typedef UINT32			offs_t;

/* ----- typedefs for the various common data access handlers ----- */
typedef UINT8			(*read8_machine_func)  (ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset);
typedef void			(*write8_machine_func) (ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT8 data);
typedef UINT16			(*read16_machine_func) (ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT16 mem_mask);
typedef void			(*write16_machine_func)(ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT16 data, ATTR_UNUSED UINT16 mem_mask);
typedef UINT32			(*read32_machine_func) (ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT32 mem_mask);
typedef void			(*write32_machine_func)(ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT32 data, ATTR_UNUSED UINT32 mem_mask);
typedef UINT64			(*read64_machine_func) (ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT64 mem_mask);
typedef void			(*write64_machine_func)(ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT64 data, ATTR_UNUSED UINT64 mem_mask);

typedef UINT8			(*read8_device_func)  (ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset);
typedef void			(*write8_device_func) (ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT8 data);
typedef UINT16			(*read16_device_func) (ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT16 mem_mask);
typedef void			(*write16_device_func)(ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT16 data, ATTR_UNUSED UINT16 mem_mask);
typedef UINT32			(*read32_device_func) (ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT32 mem_mask);
typedef void			(*write32_device_func)(ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT32 data, ATTR_UNUSED UINT32 mem_mask);
typedef UINT64			(*read64_device_func) (ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT64 mem_mask);
typedef void			(*write64_device_func)(ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT64 data, ATTR_UNUSED UINT64 mem_mask);

typedef offs_t			(*opbase_handler_func) (ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t address);

/* ----- this struct contains pointers to the live read/write routines ----- */
typedef struct _data_accessors data_accessors;
struct _data_accessors
{
	UINT8			(*read_byte)(offs_t offset);
	UINT16			(*read_word)(offs_t offset);
	UINT32			(*read_dword)(offs_t offset);
	UINT64			(*read_qword)(offs_t offset);

	void			(*write_byte)(offs_t offset, UINT8 data);
	void			(*write_word)(offs_t offset, UINT16 data);
	void			(*write_dword)(offs_t offset, UINT32 data);
	void			(*write_qword)(offs_t offset, UINT64 data);
};



/***************************************************************************
    BASIC MACROS
***************************************************************************/

/* ----- macros for declaring the various common data access handlers ----- */
#define READ8_HANDLER(name) 	UINT8  name(ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset)
#define WRITE8_HANDLER(name) 	void   name(ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT8 data)
#define READ16_HANDLER(name)	UINT16 name(ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT16 mem_mask)
#define WRITE16_HANDLER(name)	void   name(ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT16 data, ATTR_UNUSED UINT16 mem_mask)
#define READ32_HANDLER(name)	UINT32 name(ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT32 mem_mask)
#define WRITE32_HANDLER(name)	void   name(ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT32 data, ATTR_UNUSED UINT32 mem_mask)
#define READ64_HANDLER(name)	UINT64 name(ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT64 mem_mask)
#define WRITE64_HANDLER(name)	void   name(ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT64 data, ATTR_UNUSED UINT64 mem_mask)

#define READ8_DEVICE_HANDLER(name) 		UINT8  name(ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset)
#define WRITE8_DEVICE_HANDLER(name) 	void   name(ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT8 data)
#define READ16_DEVICE_HANDLER(name)		UINT16 name(ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT16 mem_mask)
#define WRITE16_DEVICE_HANDLER(name)	void   name(ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT16 data, ATTR_UNUSED UINT16 mem_mask)
#define READ32_DEVICE_HANDLER(name)		UINT32 name(ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT32 mem_mask)
#define WRITE32_DEVICE_HANDLER(name)	void   name(ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT32 data, ATTR_UNUSED UINT32 mem_mask)
#define READ64_DEVICE_HANDLER(name)		UINT64 name(ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT64 mem_mask)
#define WRITE64_DEVICE_HANDLER(name)	void   name(ATTR_UNUSED device_config *device, ATTR_UNUSED offs_t offset, ATTR_UNUSED UINT64 data, ATTR_UNUSED UINT64 mem_mask)

#define OPBASE_HANDLER(name)	offs_t name(ATTR_UNUSED running_machine *machine, ATTR_UNUSED offs_t address)

/* ----- macros for accessing bytes and words within larger chunks ----- */
#ifdef LSB_FIRST
	#define BYTE_XOR_BE(a)  	((a) ^ 1)				/* read/write a byte to a 16-bit space */
	#define BYTE_XOR_LE(a)  	(a)
	#define BYTE4_XOR_BE(a) 	((a) ^ 3)				/* read/write a byte to a 32-bit space */
	#define BYTE4_XOR_LE(a) 	(a)
	#define WORD_XOR_BE(a)  	((a) ^ 2)				/* read/write a word to a 32-bit space */
	#define WORD_XOR_LE(a)  	(a)
	#define BYTE8_XOR_BE(a) 	((a) ^ 7)				/* read/write a byte to a 64-bit space */
	#define BYTE8_XOR_LE(a) 	(a)
	#define WORD2_XOR_BE(a)  	((a) ^ 6)				/* read/write a word to a 64-bit space */
	#define WORD2_XOR_LE(a)  	(a)
	#define DWORD_XOR_BE(a)  	((a) ^ 4)				/* read/write a dword to a 64-bit space */
	#define DWORD_XOR_LE(a)  	(a)
#else
	#define BYTE_XOR_BE(a)  	(a)
	#define BYTE_XOR_LE(a)  	((a) ^ 1)				/* read/write a byte to a 16-bit space */
	#define BYTE4_XOR_BE(a) 	(a)
	#define BYTE4_XOR_LE(a) 	((a) ^ 3)				/* read/write a byte to a 32-bit space */
	#define WORD_XOR_BE(a)  	(a)
	#define WORD_XOR_LE(a)  	((a) ^ 2)				/* read/write a word to a 32-bit space */
	#define BYTE8_XOR_BE(a) 	(a)
	#define BYTE8_XOR_LE(a) 	((a) ^ 7)				/* read/write a byte to a 64-bit space */
	#define WORD2_XOR_BE(a)  	(a)
	#define WORD2_XOR_LE(a)  	((a) ^ 6)				/* read/write a word to a 64-bit space */
	#define DWORD_XOR_BE(a)  	(a)
	#define DWORD_XOR_LE(a)  	((a) ^ 4)				/* read/write a dword to a 64-bit space */
#endif



/***************************************************************************
    ADDRESS ARRAY CONSTANTS
****************************************************************************
    These apply to values in the array of read/write handlers that is
    declared within each driver.
***************************************************************************/

/* ----- static data access handler constants ----- */
enum
{
	STATIC_INVALID = 0,	/* invalid - should never be used */
	STATIC_BANK1 = 1,	/* banked memory */
	/* entries 1-32 are for fixed banks specified by the driver */
	/* entries 33-67 are for dynamically allocated internal banks */
	STATIC_RAM = 68,	/* RAM - standard reads/writes */
	STATIC_ROM,			/* ROM - standard reads, no writes */
	STATIC_NOP,			/* NOP - reads return unmapped value; writes do nothing */
	STATIC_UNMAP,		/* unmapped - all unmapped memory goes here */
	STATIC_COUNT		/* total number of static handlers */
};

/* ----- banking constants ----- */
#define STATIC_BANKMAX			(STATIC_RAM - 1)		/* handler constant of last bank */

/* SMH_* macros are Static Memory Handler definitions with
   pre-cast versions of the STATIC_* functions */
#define SMH_RAM					((void *)STATIC_RAM)
#define SMH_ROM					((void *)STATIC_ROM)
#define SMH_NOP					((void *)STATIC_NOP)
#define SMH_UNMAP				((void *)STATIC_UNMAP)
#define SMH_BANK(n)				((void *)(STATIC_BANK1 + (n) - 1))

#define SMH_BANK1				SMH_BANK(1)
#define SMH_BANK2				SMH_BANK(2)
#define SMH_BANK3				SMH_BANK(3)
#define SMH_BANK4				SMH_BANK(4)
#define SMH_BANK5				SMH_BANK(5)
#define SMH_BANK6				SMH_BANK(6)
#define SMH_BANK7				SMH_BANK(7)
#define SMH_BANK8				SMH_BANK(8)
#define SMH_BANK9				SMH_BANK(9)
#define SMH_BANK10				SMH_BANK(10)
#define SMH_BANK11				SMH_BANK(11)
#define SMH_BANK12				SMH_BANK(12)
#define SMH_BANK13				SMH_BANK(13)
#define SMH_BANK14				SMH_BANK(14)
#define SMH_BANK15				SMH_BANK(15)
#define SMH_BANK16				SMH_BANK(16)
#define SMH_BANK17				SMH_BANK(17)
#define SMH_BANK18				SMH_BANK(18)
#define SMH_BANK19				SMH_BANK(19)
#define SMH_BANK20				SMH_BANK(20)
#define SMH_BANK21				SMH_BANK(21)
#define SMH_BANK22				SMH_BANK(22)
#define SMH_BANK23				SMH_BANK(23)
#define SMH_BANK24				SMH_BANK(24)
#define SMH_BANK25				SMH_BANK(25)
#define SMH_BANK26				SMH_BANK(26)
#define SMH_BANK27				SMH_BANK(27)
#define SMH_BANK28				SMH_BANK(28)
#define SMH_BANK29				SMH_BANK(29)
#define SMH_BANK30				SMH_BANK(30)
#define SMH_BANK31				SMH_BANK(31)
#define SMH_BANK32				SMH_BANK(32)



/***************************************************************************
    ADDRESS SPACE ARRAY TYPE DEFINITIONS
****************************************************************************
    Note that the data access handlers are not passed the actual address
    where the operation takes place, but the offset from the beginning
    of the block they are assigned to. This makes handling of mirror
    addresses easier, and makes the handlers a bit more "object oriented".
    If you handler needs to read/write the main memory area, provide a
    "base" pointer: it will be initialized by the main engine to point to
    the beginning of the memory block assigned to the handler. You may
    also provide a pointer to "size": it will be set to the length of
    the memory area processed by the handler.
***************************************************************************/

typedef struct _handler_data handler_data;

/* ----- a union of all the different read handler types ----- */
typedef union _read_handlers read_handlers;
union _read_handlers
{
	genf *					handler;
	read8_machine_func		mhandler8;
	read16_machine_func		mhandler16;
	read32_machine_func		mhandler32;
	read64_machine_func		mhandler64;
	read8_device_func		dhandler8;
	read16_device_func		dhandler16;
	read32_device_func		dhandler32;
	read64_device_func		dhandler64;
};

/* ----- a union of all the different write handler types ----- */
typedef union _write_handlers write_handlers;
union _write_handlers
{
	genf *					handler;
	write8_machine_func		mhandler8;
	write16_machine_func	mhandler16;
	write32_machine_func	mhandler32;
	write64_machine_func	mhandler64;
	write8_device_func		dhandler8;
	write16_device_func		dhandler16;
	write32_device_func		dhandler32;
	write64_device_func		dhandler64;
};

typedef union _memory_handlers memory_handlers;
union _memory_handlers
{
	read_handlers			read;
	write_handlers			write;
};

/* ----- a generic address map type ----- */
typedef struct _address_map_entry address_map_entry;
struct _address_map_entry
{
	address_map_entry *	next;				/* pointer to the next entry */

	offs_t				start, end;			/* start/end values */
	offs_t				mirror;				/* mirror bits */
	offs_t				mask;				/* mask bits */
	read_handlers 		read;				/* read handler callback */
	const char *		read_name;			/* read handler callback name */
	device_type			read_devtype;		/* read device type for device references */
	const char *		read_devtag;		/* read tag for the relevant device */
	write_handlers 		write;				/* write handler callback */
	const char *		write_name;			/* write handler callback name */
	device_type			write_devtype;		/* read device type for device references */
	const char *		write_devtag;		/* read tag for the relevant device */
	UINT32				share;				/* index of a shared memory block */
	void **				baseptr;			/* receives pointer to memory (optional) */
	size_t *			sizeptr;			/* receives size of area in bytes (optional) */
	UINT32				baseptroffs_plus1;	/* offset of base pointer within driver_data struct, plus 1 */
	UINT32				sizeptroffs_plus1;	/* offset of size pointer within driver_data struct, plus 1 */
	UINT32				region;				/* region containing the memory backing this entry */
	offs_t				region_offs;		/* offset within the region */

	void *				memory;				/* pointer to memory backing this entry */
	offs_t				bytestart, byteend;	/* byte-adjusted start/end values */
	offs_t				bytemirror;			/* byte-adjusted mirror bits */
	offs_t				bytemask;			/* byte-adjusted mask bits */
};

typedef struct _address_map address_map;
struct _address_map
{
	UINT8				spacenum;			/* space number of the map */
	UINT8				databits;			/* data bits represented by the map */
	UINT8				unmapval;			/* unmapped memory value */
	offs_t				globalmask;			/* global mask */
	address_map_entry *	entrylist;			/* list of entries */
};

/* ----- structs to contain internal data ----- */
typedef struct _address_space address_space;
struct _address_space
{
	offs_t				addrmask;			/* address mask */
	UINT8 *				readlookup;			/* read table lookup */
	UINT8 *				writelookup;		/* write table lookup */
	handler_data *		readhandlers;		/* read handlers */
	handler_data *		writehandlers;		/* write handlers */
	const data_accessors *accessors;		/* pointers to the data access handlers */
};


typedef union _addrmap_token addrmap_token;
union _addrmap_token
{
	TOKEN_COMMON_FIELDS
	const addrmap_token *	tokenptr;
	read_handlers			read;				/* generic read handlers */
	write_handlers			write;				/* generic write handlers */
	device_type				devtype;			/* device type */
	UINT8 **				memptr;				/* memory pointer */
	size_t *				sizeptr;			/* size pointer */
};


typedef union _addrmap8_token addrmap8_token;
union _addrmap8_token
{
	TOKEN_COMMON_FIELDS
	const addrmap_token *	tokenptr;
	read8_machine_func		mread;				/* pointer to native machine read handler */
	write8_machine_func		mwrite;				/* pointer to native machine write handler */
	read8_device_func		dread;				/* pointer to native device read handler */
	write8_device_func		dwrite;				/* pointer to native device write handler */
	read_handlers			read;				/* generic read handlers */
	write_handlers			write;				/* generic write handlers */
	device_type				devtype;			/* device type */
	UINT8 **				memptr;				/* memory pointer */
	size_t *				sizeptr;			/* size pointer */
};


typedef union _addrmap16_token addrmap16_token;
union _addrmap16_token
{
	TOKEN_COMMON_FIELDS
	const addrmap_token *	tokenptr;
	read16_machine_func		mread;				/* pointer to native read handler */
	write16_machine_func 	mwrite;				/* pointer to native write handler */
	read16_device_func		dread;				/* pointer to native device read handler */
	write16_device_func		dwrite;				/* pointer to native device write handler */
	read8_machine_func		mread8;				/* pointer to 8-bit machine read handler */
	write8_machine_func		mwrite8;			/* pointer to 8-bit machine write handler */
	read_handlers			read;				/* generic read handlers */
	write_handlers			write;				/* generic write handlers */
	device_type				devtype;			/* device type */
	UINT16 **				memptr;				/* memory pointer */
	size_t *				sizeptr;			/* size pointer */
};


typedef union _addrmap32_token addrmap32_token;
union _addrmap32_token
{
	TOKEN_COMMON_FIELDS
	const addrmap_token *	tokenptr;
	read32_machine_func		mread;				/* pointer to native read handler */
	write32_machine_func 	mwrite;				/* pointer to native write handler */
	read32_device_func		dread;				/* pointer to native device read handler */
	write32_device_func		dwrite;				/* pointer to native device write handler */
	read8_machine_func		mread8;				/* pointer to 8-bit machine read handler */
	write8_machine_func		mwrite8;			/* pointer to 8-bit machine write handler */
	read16_machine_func		mread16;			/* pointer to 16-bit machine read handler */
	write16_machine_func	mwrite16;			/* pointer to 16-bit machine write handler */
	read_handlers			read;				/* generic read handlers */
	write_handlers			write;				/* generic write handlers */
	device_type				devtype;			/* device type */
	UINT32 **				memptr;				/* memory pointer */
	size_t *				sizeptr;			/* size pointer */
};


typedef union _addrmap64_token addrmap64_token;
union _addrmap64_token
{
	TOKEN_COMMON_FIELDS
	const addrmap_token *	tokenptr;
	read64_machine_func		mread;				/* pointer to native read handler */
	write64_machine_func 	mwrite;				/* pointer to native write handler */
	read64_device_func		dread;				/* pointer to native device read handler */
	write64_device_func		dwrite;				/* pointer to native device write handler */
	read8_machine_func		mread8;				/* pointer to 8-bit machine read handler */
	write8_machine_func		mwrite8;			/* pointer to 8-bit machine write handler */
	read16_machine_func		mread16;			/* pointer to 16-bit machine read handler */
	write16_machine_func	mwrite16;			/* pointer to 16-bit machine write handler */
	read32_machine_func		mread32;			/* pointer to 32-bit machine read handler */
	write32_machine_func	mwrite32;			/* pointer to 32-bit machine write handler */
	read_handlers			read;				/* generic read handlers */
	write_handlers			write;				/* generic write handlers */
	device_type				devtype;			/* device type */
	UINT64 **				memptr;				/* memory pointer */
	size_t *				sizeptr;			/* size pointer */
};


/***************************************************************************
    ADDRESS MAP ARRAY CONSTRUCTORS
***************************************************************************/

#define address_map_0 NULL

enum
{
	ADDRMAP_TOKEN_INVALID,

	ADDRMAP_TOKEN_START,
	ADDRMAP_TOKEN_END,
	ADDRMAP_TOKEN_INCLUDE,
	
	ADDRMAP_TOKEN_GLOBAL_MASK,
	ADDRMAP_TOKEN_UNMAP_VALUE,

	ADDRMAP_TOKEN_RANGE,
	ADDRMAP_TOKEN_MASK,
	ADDRMAP_TOKEN_MIRROR,
	ADDRMAP_TOKEN_READ,
	ADDRMAP_TOKEN_WRITE,
	ADDRMAP_TOKEN_DEVICE_READ,
	ADDRMAP_TOKEN_DEVICE_WRITE,
	ADDRMAP_TOKEN_READ_PORT,
	ADDRMAP_TOKEN_REGION,
	ADDRMAP_TOKEN_SHARE,
	ADDRMAP_TOKEN_BASEPTR,
	ADDRMAP_TOKEN_BASE_MEMBER,
	ADDRMAP_TOKEN_SIZEPTR,
	ADDRMAP_TOKEN_SIZE_MEMBER
};


/* start/end tags for the address map */
#define ADDRESS_MAP_START(_name, _space, _bits) \
	const addrmap##_bits##_token address_map_##_name[] = { \
	TOKEN_UINT32_PACK3(ADDRMAP_TOKEN_START, 8, _space, 8, _bits, 8),

#define ADDRESS_MAP_END \
	TOKEN_UINT32_PACK1(ADDRMAP_TOKEN_END, 8) };

/* use this to declare external references to an address map */
#define ADDRESS_MAP_EXTERN(_name, _bits) \
	extern const addrmap##_bits##_token address_map_##_name[]


/* global controls */
#define ADDRESS_MAP_GLOBAL_MASK(_mask) \
	TOKEN_UINT64_PACK2(ADDRMAP_TOKEN_GLOBAL_MASK, 8, _mask, 32),

#define ADDRESS_MAP_UNMAP_LOW \
	TOKEN_UINT32_PACK2(ADDRMAP_TOKEN_UNMAP_VALUE, 8, 0, 1),

#define ADDRESS_MAP_UNMAP_HIGH \
	TOKEN_UINT32_PACK2(ADDRMAP_TOKEN_UNMAP_VALUE, 8, 1, 1),


/* importing data from other address maps */
#define AM_IMPORT_FROM(_name) \
	TOKEN_UINT32_PACK1(ADDRMAP_TOKEN_INCLUDE, 8), \
	TOKEN_PTR(tokenptr, address_map_##_name),


/* address ranges */
#define AM_RANGE(_start, _end) \
	TOKEN_UINT32_PACK1(ADDRMAP_TOKEN_RANGE, 8), \
	TOKEN_UINT64_PACK2(_start, 32, _end, 32),

#define AM_MASK(_mask) \
	TOKEN_UINT64_PACK2(ADDRMAP_TOKEN_MASK, 8, _mask, 32),

#define AM_MIRROR(_mirror) \
	TOKEN_UINT64_PACK2(ADDRMAP_TOKEN_MIRROR, 8, _mirror, 32),

#define AM_READ(_handler) \
	TOKEN_UINT32_PACK1(ADDRMAP_TOKEN_READ, 8), \
	TOKEN_PTR(mread, _handler), \
	TOKEN_STRING(#_handler),

#define AM_WRITE(_handler) \
	TOKEN_UINT32_PACK1(ADDRMAP_TOKEN_WRITE, 8), \
	TOKEN_PTR(mwrite, _handler), \
	TOKEN_STRING(#_handler),

#define AM_DEVREAD(_type, _tag, _handler) \
	TOKEN_UINT32_PACK1(ADDRMAP_TOKEN_DEVICE_READ, 8), \
	TOKEN_PTR(dread, _handler), \
	TOKEN_STRING(#_handler), \
	TOKEN_PTR(devtype, _type), \
	TOKEN_STRING(_tag),

#define AM_DEVWRITE(_type, _tag, _handler) \
	TOKEN_UINT32_PACK1(ADDRMAP_TOKEN_DEVICE_WRITE, 8), \
	TOKEN_PTR(dwrite, _handler), \
	TOKEN_STRING(#_handler), \
	TOKEN_PTR(devtype, _type), \
	TOKEN_STRING(_tag),

#define AM_READ_PORT(_tag) \
	TOKEN_UINT32_PACK1(ADDRMAP_TOKEN_READ_PORT, 8), \
	TOKEN_STRING(_tag),

#define AM_REGION(_region, _offs) \
	TOKEN_UINT64_PACK3(ADDRMAP_TOKEN_REGION, 8, _region, 24, _offs, 32),

#define AM_SHARE(_index) \
	TOKEN_UINT32_PACK2(ADDRMAP_TOKEN_SHARE, 8, _index, 24),

#define AM_BASE(_base) \
	TOKEN_UINT32_PACK1(ADDRMAP_TOKEN_BASEPTR, 8), \
	TOKEN_PTR(memptr, _base),

#define AM_BASE_MEMBER(_struct, _member) \
	TOKEN_UINT32_PACK2(ADDRMAP_TOKEN_BASE_MEMBER, 8, offsetof(_struct, _member), 24),

#define AM_SIZE(_size) \
	TOKEN_UINT32_PACK1(ADDRMAP_TOKEN_SIZEPTR, 8), \
	TOKEN_PTR(sizeptr, _size),

#define AM_SIZE_MEMBER(_struct, _member) \
	TOKEN_UINT32_PACK2(ADDRMAP_TOKEN_SIZE_MEMBER, 8, offsetof(_struct, _member), 24),

/* ----- common shortcuts ----- */
#define AM_READWRITE(_read,_write)			AM_READ(_read) AM_WRITE(_write)
#define AM_DEVREADWRITE(_type,_tag,_read,_write) AM_DEVREAD(_type,_tag,_read) AM_DEVWRITE(_type,_tag,_write)
#define AM_ROM								AM_READ(SMH_ROM)
#define AM_RAM								AM_READWRITE(SMH_RAM, SMH_RAM)
#define AM_WRITEONLY						AM_WRITE(SMH_RAM)
#define AM_UNMAP							AM_READWRITE(SMH_UNMAP, SMH_UNMAP)
#define AM_ROMBANK(_bank)					AM_READ(SMH_BANK(_bank))
#define AM_RAMBANK(_bank)					AM_READWRITE(SMH_BANK(_bank), SMH_BANK(_bank))
#define AM_NOP								AM_READWRITE(SMH_NOP, SMH_NOP)
#define AM_READNOP							AM_READ(SMH_NOP)
#define AM_WRITENOP							AM_WRITE(SMH_NOP)



/***************************************************************************
    ADDRESS MAP LOOKUP CONSTANTS
***************************************************************************/

/* ----- address spaces ----- */
enum
{
	ADDRESS_SPACE_PROGRAM = 0,			/* program address space */
	ADDRESS_SPACE_DATA,				/* data address space */
	ADDRESS_SPACE_IO,				/* I/O address space */
	ADDRESS_SPACES					/* maximum number of address spaces */
};

/***************************************************************************
    FUNCTION PROTOTYPES FOR CORE READ/WRITE ROUTINES
***************************************************************************/

/* ----- declare program address space handlers ----- */
UINT8 program_read_byte_8(offs_t address);
void program_write_byte_8(offs_t address, UINT8 data);

UINT8 program_read_byte_16be(offs_t address);
UINT16 program_read_word_16be(offs_t address);
void program_write_byte_16be(offs_t address, UINT8 data);
void program_write_word_16be(offs_t address, UINT16 data);

UINT8 program_read_byte_16le(offs_t address);
UINT16 program_read_word_16le(offs_t address);
void program_write_byte_16le(offs_t address, UINT8 data);
void program_write_word_16le(offs_t address, UINT16 data);

UINT8 program_read_byte_32be(offs_t address);
UINT16 program_read_word_32be(offs_t address);
UINT32 program_read_dword_32be(offs_t address);
UINT32 program_read_masked_32be(offs_t address, UINT32 mem_mask);
void program_write_byte_32be(offs_t address, UINT8 data);
void program_write_word_32be(offs_t address, UINT16 data);
void program_write_dword_32be(offs_t address, UINT32 data);
void program_write_masked_32be(offs_t address, UINT32 data, UINT32 mem_mask);

UINT8 program_read_byte_32le(offs_t address);
UINT16 program_read_word_32le(offs_t address);
UINT32 program_read_dword_32le(offs_t address);
UINT32 program_read_masked_32le(offs_t address, UINT32 mem_mask);
void program_write_byte_32le(offs_t address, UINT8 data);
void program_write_word_32le(offs_t address, UINT16 data);
void program_write_dword_32le(offs_t address, UINT32 data);
void program_write_masked_32le(offs_t address, UINT32 data, UINT32 mem_mask);

UINT8 program_read_byte_64be(offs_t address);
UINT16 program_read_word_64be(offs_t address);
UINT32 program_read_dword_64be(offs_t address);
UINT64 program_read_qword_64be(offs_t address);
UINT64 program_read_masked_64be(offs_t address, UINT64 mem_mask);
void program_write_byte_64be(offs_t address, UINT8 data);
void program_write_word_64be(offs_t address, UINT16 data);
void program_write_dword_64be(offs_t address, UINT32 data);
void program_write_qword_64be(offs_t address, UINT64 data);
void program_write_masked_64be(offs_t address, UINT64 data, UINT64 mem_mask);

UINT8 program_read_byte_64le(offs_t address);
UINT16 program_read_word_64le(offs_t address);
UINT32 program_read_dword_64le(offs_t address);
UINT64 program_read_qword_64le(offs_t address);
UINT64 program_read_masked_64le(offs_t address, UINT64 mem_mask);
void program_write_byte_64le(offs_t address, UINT8 data);
void program_write_word_64le(offs_t address, UINT16 data);
void program_write_dword_64le(offs_t address, UINT32 data);
void program_write_qword_64le(offs_t address, UINT64 data);
void program_write_masked_64le(offs_t address, UINT64 data, UINT64 mem_mask);

/* ----- declare data address space handlers ----- */
UINT8 data_read_byte_8(offs_t address);
void data_write_byte_8(offs_t address, UINT8 data);

UINT8 data_read_byte_16be(offs_t address);
UINT16 data_read_word_16be(offs_t address);
void data_write_byte_16be(offs_t address, UINT8 data);
void data_write_word_16be(offs_t address, UINT16 data);

UINT8 data_read_byte_16le(offs_t address);
UINT16 data_read_word_16le(offs_t address);
void data_write_byte_16le(offs_t address, UINT8 data);
void data_write_word_16le(offs_t address, UINT16 data);

UINT8 data_read_byte_32be(offs_t address);
UINT16 data_read_word_32be(offs_t address);
UINT32 data_read_dword_32be(offs_t address);
UINT32 data_read_masked_32be(offs_t address, UINT32 mem_mask);
void data_write_byte_32be(offs_t address, UINT8 data);
void data_write_word_32be(offs_t address, UINT16 data);
void data_write_dword_32be(offs_t address, UINT32 data);
void data_write_masked_32be(offs_t address, UINT32 data, UINT32 mem_mask);

UINT8 data_read_byte_32le(offs_t address);
UINT16 data_read_word_32le(offs_t address);
UINT32 data_read_dword_32le(offs_t address);
UINT32 data_read_masked_32le(offs_t address, UINT32 mem_mask);
void data_write_byte_32le(offs_t address, UINT8 data);
void data_write_word_32le(offs_t address, UINT16 data);
void data_write_dword_32le(offs_t address, UINT32 data);
void data_write_masked_32le(offs_t address, UINT32 data, UINT32 mem_mask);

UINT8 data_read_byte_64be(offs_t address);
UINT16 data_read_word_64be(offs_t address);
UINT32 data_read_dword_64be(offs_t address);
UINT64 data_read_qword_64be(offs_t address);
UINT64 data_read_masked_64be(offs_t address, UINT64 mem_mask);
void data_write_byte_64be(offs_t address, UINT8 data);
void data_write_word_64be(offs_t address, UINT16 data);
void data_write_dword_64be(offs_t address, UINT32 data);
void data_write_qword_64be(offs_t address, UINT64 data);
void data_write_masked_64be(offs_t address, UINT64 data, UINT64 mem_mask);

UINT8 data_read_byte_64le(offs_t address);
UINT16 data_read_word_64le(offs_t address);
UINT32 data_read_dword_64le(offs_t address);
UINT64 data_read_qword_64le(offs_t address);
UINT64 data_read_masked_64le(offs_t address, UINT64 mem_mask);
void data_write_byte_64le(offs_t address, UINT8 data);
void data_write_word_64le(offs_t address, UINT16 data);
void data_write_dword_64le(offs_t address, UINT32 data);
void data_write_qword_64le(offs_t address, UINT64 data);
void data_write_masked_64le(offs_t address, UINT64 data, UINT64 mem_mask);

/* ----- declare I/O address space handlers ----- */
UINT8 io_read_byte_8(offs_t address);
void io_write_byte_8(offs_t address, UINT8 data);

UINT8 io_read_byte_16be(offs_t address);
UINT16 io_read_word_16be(offs_t address);
void io_write_byte_16be(offs_t address, UINT8 data);
void io_write_word_16be(offs_t address, UINT16 data);

UINT8 io_read_byte_16le(offs_t address);
UINT16 io_read_word_16le(offs_t address);
void io_write_byte_16le(offs_t address, UINT8 data);
void io_write_word_16le(offs_t address, UINT16 data);

UINT8 io_read_byte_32be(offs_t address);
UINT16 io_read_word_32be(offs_t address);
UINT32 io_read_dword_32be(offs_t address);
UINT32 io_read_masked_32be(offs_t address, UINT32 mem_mask);
void io_write_byte_32be(offs_t address, UINT8 data);
void io_write_word_32be(offs_t address, UINT16 data);
void io_write_dword_32be(offs_t address, UINT32 data);
void io_write_masked_32be(offs_t address, UINT32 data, UINT32 mem_mask);

UINT8 io_read_byte_32le(offs_t address);
UINT16 io_read_word_32le(offs_t address);
UINT32 io_read_dword_32le(offs_t address);
UINT32 io_read_masked_32le(offs_t address, UINT32 mem_mask);
void io_write_byte_32le(offs_t address, UINT8 data);
void io_write_word_32le(offs_t address, UINT16 data);
void io_write_dword_32le(offs_t address, UINT32 data);
void io_write_masked_32le(offs_t address, UINT32 data, UINT32 mem_mask);

UINT8 io_read_byte_64be(offs_t address);
UINT16 io_read_word_64be(offs_t address);
UINT32 io_read_dword_64be(offs_t address);
UINT64 io_read_qword_64be(offs_t address);
UINT64 io_read_masked_64be(offs_t address, UINT64 mem_mask);
void io_write_byte_64be(offs_t address, UINT8 data);
void io_write_word_64be(offs_t address, UINT16 data);
void io_write_dword_64be(offs_t address, UINT32 data);
void io_write_qword_64be(offs_t address, UINT64 data);
void io_write_masked_64be(offs_t address, UINT64 data, UINT64 mem_mask);

UINT8 io_read_byte_64le(offs_t address);
UINT16 io_read_word_64le(offs_t address);
UINT32 io_read_dword_64le(offs_t address);
UINT64 io_read_qword_64le(offs_t address);
UINT64 io_read_masked_64le(offs_t address, UINT64 mem_mask);
void io_write_byte_64le(offs_t address, UINT8 data);
void io_write_word_64le(offs_t address, UINT16 data);
void io_write_dword_64le(offs_t address, UINT32 data);
void io_write_qword_64le(offs_t address, UINT64 data);
void io_write_masked_64le(offs_t address, UINT64 data, UINT64 mem_mask);



/***************************************************************************
    FUNCTION PROTOTYPES FOR CORE MEMORY FUNCTIONS
***************************************************************************/

/* ----- memory setup function ----- */
void		memory_init(running_machine *machine);
void		memory_set_context(int activecpu);

/* ----- address map functions ----- */
address_map *address_map_alloc(const machine_config *drv, int cpunum, int spacenum);
void 		address_map_free(address_map *map);
const address_map *memory_get_map(int cpunum, int spacenum);

/* ----- opcode base control ---- */
opbase_handler_func memory_set_opbase_handler(int cpunum, opbase_handler_func function);
void		memory_set_opbase(offs_t offset);

/* ----- separate opcode/data encryption helper ---- */
void 		memory_set_decrypted_region(int cpunum, offs_t start, offs_t end, void *base);

/* ----- return a base pointer to memory ---- */
void *		memory_get_read_ptr(int cpunum, int spacenum, offs_t offset);
void *		memory_get_write_ptr(int cpunum, int spacenum, offs_t offset);
void *		memory_get_op_ptr(int cpunum, offs_t offset, int arg);

/* ----- memory banking ----- */
void		memory_configure_bank(int banknum, int startentry, int numentries, void *base, offs_t stride);
void		memory_configure_bank_decrypted(int banknum, int startentry, int numentries, void *base, offs_t stride);
void		memory_set_bank(int banknum, int entrynum);
int 		memory_get_bank(int banknum);
void		memory_set_bankptr(int banknum, void *base);

/* ----- debugging ----- */
void		memory_set_debugger_access(int debugger);
void		memory_set_log_unmap(int spacenum, int log);
int			memory_get_log_unmap(int spacenum);

/* ----- dynamic address space mapping ----- */
void *		_memory_install_read_handler   (int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, FPTR handler, const char *handler_name);
UINT8 *		_memory_install_read8_handler  (int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, read8_machine_func handler, const char *handler_name);
UINT16 *	_memory_install_read16_handler (int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, read16_machine_func handler, const char *handler_name);
UINT32 *	_memory_install_read32_handler (int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, read32_machine_func handler, const char *handler_name);
UINT64 *	_memory_install_read64_handler (int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, read64_machine_func handler, const char *handler_name);
void *		_memory_install_write_handler  (int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, FPTR handler, const char *handler_name);
UINT8 *		_memory_install_write8_handler (int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, write8_machine_func handler, const char *handler_name);
UINT16 *	_memory_install_write16_handler(int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, write16_machine_func handler, const char *handler_name);
UINT32 *	_memory_install_write32_handler(int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, write32_machine_func handler, const char *handler_name);
UINT64 *	_memory_install_write64_handler(int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, write64_machine_func handler, const char *handler_name);
void *		_memory_install_readwrite_handler  (int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, FPTR rhandler, FPTR whandler, const char *rhandler_name, const char *whandler_name);
UINT8 *		_memory_install_readwrite8_handler (int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, read8_machine_func rhandler, write8_machine_func whandler, const char *rhandler_name, const char *whandler_name);
UINT16 *	_memory_install_readwrite16_handler(int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, read16_machine_func rhandler, write16_machine_func whandler, const char *rhandler_name, const char *whandler_name);
UINT32 *	_memory_install_readwrite32_handler(int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, read32_machine_func rhandler, write32_machine_func whandler, const char *rhandler_name, const char *whandler_name);
UINT64 *	_memory_install_readwrite64_handler(int cpunum, int spacenum, offs_t start, offs_t end, offs_t mask, offs_t mirror, read64_machine_func rhandler, write64_machine_func whandler, const char *rhandler_name, const char *whandler_name);

/* ----- memory debugging ----- */
void 		memory_dump(FILE *file);
const char *memory_get_handler_string(int read0_or_write1, int cpunum, int spacenum, offs_t offset);



/***************************************************************************
    GLOBAL VARIABLES
***************************************************************************/

extern UINT8 			opcode_entry;				/* current entry for opcode fetching */
extern UINT8 *			opcode_base;				/* opcode ROM base */
extern UINT8 *			opcode_arg_base;			/* opcode RAM base */
extern offs_t			opcode_mask;				/* mask to apply to the opcode address */
extern offs_t			opcode_memory_min;			/* opcode memory minimum */
extern offs_t			opcode_memory_max;			/* opcode memory maximum */
extern address_space	active_address_space[];		/* address spaces */

extern const char *const address_space_names[ADDRESS_SPACES];


/***************************************************************************
    HELPER MACROS AND INLINES
***************************************************************************/

/* ----- 16/32-bit memory accessing ----- */
#define COMBINE_DATA(varptr)		(*(varptr) = (*(varptr) & mem_mask) | (data & ~mem_mask))

/* ----- 16-bit memory accessing ----- */
#define ACCESSING_LSB16				((mem_mask & 0x00ff) == 0)
#define ACCESSING_MSB16				((mem_mask & 0xff00) == 0)
#define ACCESSING_LSB				ACCESSING_LSB16
#define ACCESSING_MSB				ACCESSING_MSB16

/* ----- 32-bit memory accessing ----- */
#define ACCESSING_LSW32				((mem_mask & 0x0000ffff) == 0)
#define ACCESSING_MSW32				((mem_mask & 0xffff0000) == 0)
#define ACCESSING_LSB32				((mem_mask & 0x000000ff) == 0)
#define ACCESSING_MSB32				((mem_mask & 0xff000000) == 0)

/* ----- opcode range safety checks ----- */
#define address_is_unsafe(A)		((UNEXPECTED((A) < opcode_memory_min) || UNEXPECTED((A) > opcode_memory_max)))

/* ----- dynamic memory installation ----- */
#define memory_install_read_handler(cpu, space, start, end, mask, mirror, handler)						\
	_memory_install_read_handler(cpu, space, start, end, mask, mirror, handler, #handler)
#define memory_install_read8_handler(cpu, space, start, end, mask, mirror, handler)						\
	_memory_install_read8_handler(cpu, space, start, end, mask, mirror, handler, #handler)
#define memory_install_read16_handler(cpu, space, start, end, mask, mirror, handler)					\
	_memory_install_read16_handler(cpu, space, start, end, mask, mirror, handler, #handler)
#define memory_install_read32_handler(cpu, space, start, end, mask, mirror, handler)					\
	_memory_install_read32_handler(cpu, space, start, end, mask, mirror, handler, #handler)
#define memory_install_read64_handler(cpu, space, start, end, mask, mirror, handler)					\
	_memory_install_read64_handler(cpu, space, start, end, mask, mirror, handler, #handler)

#define memory_install_write_handler(cpu, space, start, end, mask, mirror, handler)						\
	_memory_install_write_handler(cpu, space, start, end, mask, mirror, handler, #handler)
#define memory_install_write8_handler(cpu, space, start, end, mask, mirror, handler)					\
	_memory_install_write8_handler(cpu, space, start, end, mask, mirror, handler, #handler)
#define memory_install_write16_handler(cpu, space, start, end, mask, mirror, handler)					\
	_memory_install_write16_handler(cpu, space, start, end, mask, mirror, handler, #handler)
#define memory_install_write32_handler(cpu, space, start, end, mask, mirror, handler)					\
	_memory_install_write32_handler(cpu, space, start, end, mask, mirror, handler, #handler)
#define memory_install_write64_handler(cpu, space, start, end, mask, mirror, handler)					\
	_memory_install_write64_handler(cpu, space, start, end, mask, mirror, handler, #handler)

#define memory_install_readwrite_handler(cpu, space, start, end, mask, mirror, rhandler, whandler)			\
	_memory_install_readwrite_handler(cpu, space, start, end, mask, mirror, rhandler, whandler, #rhandler, #whandler)
#define memory_install_readwrite8_handler(cpu, space, start, end, mask, mirror, rhandler, whandler)			\
	_memory_install_readwrite8_handler(cpu, space, start, end, mask, mirror, rhandler, whandler, #rhandler, #whandler)
#define memory_install_readwrite16_handler(cpu, space, start, end, mask, mirror, rhandler, whandler)		\
	_memory_install_readwrite16_handler(cpu, space, start, end, mask, mirror, rhandler, whandler, #rhandler, #whandler)
#define memory_install_readwrite32_handler(cpu, space, start, end, mask, mirror, rhandler, whandler)		\
	_memory_install_readwrite32_handler(cpu, space, start, end, mask, mirror, rhandler, whandler, #rhandler, #whandler)
#define memory_install_readwrite64_handler(cpu, space, start, end, mask, mirror, rhandler, whandler)		\
	_memory_install_readwrite64_handler(cpu, space, start, end, mask, mirror, rhandler, whandler, #rhandler, #whandler)

/* ----- generic memory access ----- */
INLINE UINT8  program_read_byte (offs_t offset) { return (*active_address_space[ADDRESS_SPACE_PROGRAM].accessors->read_byte)(offset); }
INLINE UINT16 program_read_word (offs_t offset) { return (*active_address_space[ADDRESS_SPACE_PROGRAM].accessors->read_word)(offset); }
INLINE UINT32 program_read_dword(offs_t offset) { return (*active_address_space[ADDRESS_SPACE_PROGRAM].accessors->read_dword)(offset); }
INLINE UINT64 program_read_qword(offs_t offset) { return (*active_address_space[ADDRESS_SPACE_PROGRAM].accessors->read_qword)(offset); }

INLINE void	program_write_byte (offs_t offset, UINT8  data) { (*active_address_space[ADDRESS_SPACE_PROGRAM].accessors->write_byte)(offset, data); }
INLINE void	program_write_word (offs_t offset, UINT16 data) { (*active_address_space[ADDRESS_SPACE_PROGRAM].accessors->write_word)(offset, data); }
INLINE void	program_write_dword(offs_t offset, UINT32 data) { (*active_address_space[ADDRESS_SPACE_PROGRAM].accessors->write_dword)(offset, data); }
INLINE void	program_write_qword(offs_t offset, UINT64 data) { (*active_address_space[ADDRESS_SPACE_PROGRAM].accessors->write_qword)(offset, data); }

INLINE UINT8  data_read_byte (offs_t offset) { return (*active_address_space[ADDRESS_SPACE_DATA].accessors->read_byte)(offset); }
INLINE UINT16 data_read_word (offs_t offset) { return (*active_address_space[ADDRESS_SPACE_DATA].accessors->read_word)(offset); }
INLINE UINT32 data_read_dword(offs_t offset) { return (*active_address_space[ADDRESS_SPACE_DATA].accessors->read_dword)(offset); }
INLINE UINT64 data_read_qword(offs_t offset) { return (*active_address_space[ADDRESS_SPACE_DATA].accessors->read_qword)(offset); }

INLINE void	data_write_byte (offs_t offset, UINT8  data) { (*active_address_space[ADDRESS_SPACE_DATA].accessors->write_byte)(offset, data); }
INLINE void	data_write_word (offs_t offset, UINT16 data) { (*active_address_space[ADDRESS_SPACE_DATA].accessors->write_word)(offset, data); }
INLINE void	data_write_dword(offs_t offset, UINT32 data) { (*active_address_space[ADDRESS_SPACE_DATA].accessors->write_dword)(offset, data); }
INLINE void	data_write_qword(offs_t offset, UINT64 data) { (*active_address_space[ADDRESS_SPACE_DATA].accessors->write_qword)(offset, data); }

INLINE UINT8  io_read_byte (offs_t offset) { return (*active_address_space[ADDRESS_SPACE_IO].accessors->read_byte)(offset); }
INLINE UINT16 io_read_word (offs_t offset) { return (*active_address_space[ADDRESS_SPACE_IO].accessors->read_word)(offset); }
INLINE UINT32 io_read_dword(offs_t offset) { return (*active_address_space[ADDRESS_SPACE_IO].accessors->read_dword)(offset); }
INLINE UINT64 io_read_qword(offs_t offset) { return (*active_address_space[ADDRESS_SPACE_IO].accessors->read_qword)(offset); }

INLINE void	io_write_byte (offs_t offset, UINT8  data) { (*active_address_space[ADDRESS_SPACE_IO].accessors->write_byte)(offset, data); }
INLINE void	io_write_word (offs_t offset, UINT16 data) { (*active_address_space[ADDRESS_SPACE_IO].accessors->write_word)(offset, data); }
INLINE void	io_write_dword(offs_t offset, UINT32 data) { (*active_address_space[ADDRESS_SPACE_IO].accessors->write_dword)(offset, data); }
INLINE void	io_write_qword(offs_t offset, UINT64 data) { (*active_address_space[ADDRESS_SPACE_IO].accessors->write_qword)(offset, data); }

/* ----- safe opcode and opcode argument reading ----- */
UINT8	cpu_readop_safe(offs_t offset);
UINT16	cpu_readop16_safe(offs_t offset);
UINT32	cpu_readop32_safe(offs_t offset);
UINT64	cpu_readop64_safe(offs_t offset);
UINT8	cpu_readop_arg_safe(offs_t offset);
UINT16	cpu_readop_arg16_safe(offs_t offset);
UINT32	cpu_readop_arg32_safe(offs_t offset);
UINT64	cpu_readop_arg64_safe(offs_t offset);

/* ----- unsafe opcode and opcode argument reading ----- */
#define cpu_opptr_unsafe(A)			((void *)&opcode_base[(A) & opcode_mask])
#define cpu_readop_unsafe(A)		(opcode_base[(A) & opcode_mask])
#define cpu_readop16_unsafe(A)		(*(UINT16 *)&opcode_base[(A) & opcode_mask])
#define cpu_readop32_unsafe(A)		(*(UINT32 *)&opcode_base[(A) & opcode_mask])
#define cpu_readop64_unsafe(A)		(*(UINT64 *)&opcode_base[(A) & opcode_mask])
#define cpu_readop_arg_unsafe(A)	(opcode_arg_base[(A) & opcode_mask])
#define cpu_readop_arg16_unsafe(A)	(*(UINT16 *)&opcode_arg_base[(A) & opcode_mask])
#define cpu_readop_arg32_unsafe(A)	(*(UINT32 *)&opcode_arg_base[(A) & opcode_mask])
#define cpu_readop_arg64_unsafe(A)	(*(UINT64 *)&opcode_arg_base[(A) & opcode_mask])

/* ----- opcode and opcode argument reading ----- */
INLINE void * cpu_opptr(offs_t A)			{ if (address_is_unsafe(A)) { memory_set_opbase(A); } return cpu_opptr_unsafe(A); }
INLINE UINT8  cpu_readop(offs_t A)			{ if (address_is_unsafe(A)) { memory_set_opbase(A); } return cpu_readop_unsafe(A); }
INLINE UINT16 cpu_readop16(offs_t A)		{ if (address_is_unsafe(A)) { memory_set_opbase(A); } return cpu_readop16_unsafe(A); }
INLINE UINT32 cpu_readop32(offs_t A)		{ if (address_is_unsafe(A)) { memory_set_opbase(A); } return cpu_readop32_unsafe(A); }
INLINE UINT64 cpu_readop64(offs_t A)		{ if (address_is_unsafe(A)) { memory_set_opbase(A); } return cpu_readop64_unsafe(A); }
INLINE UINT8  cpu_readop_arg(offs_t A)		{ if (address_is_unsafe(A)) { memory_set_opbase(A); } return cpu_readop_arg_unsafe(A); }
INLINE UINT16 cpu_readop_arg16(offs_t A)	{ if (address_is_unsafe(A)) { memory_set_opbase(A); } return cpu_readop_arg16_unsafe(A); }
INLINE UINT32 cpu_readop_arg32(offs_t A)	{ if (address_is_unsafe(A)) { memory_set_opbase(A); } return cpu_readop_arg32_unsafe(A); }
INLINE UINT64 cpu_readop_arg64(offs_t A)	{ if (address_is_unsafe(A)) { memory_set_opbase(A); } return cpu_readop_arg64_unsafe(A); }

/* ----- bank switching for CPU cores ----- */
#define change_pc(pc)				do { memory_set_opbase(pc); } while (0)

/* ----- forces the next branch to generate a call to the opbase handler ----- */
#define catch_nextBranch()			do { opcode_entry = 0xff; } while (0)


#endif	/* __MEMORY_H__ */

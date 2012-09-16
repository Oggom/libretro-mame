#pragma once

#ifndef __TLCS90_H__
#define __TLCS90_H__


DECLARE_LEGACY_CPU_DEVICE(TMP90840, tmp90840);
DECLARE_LEGACY_CPU_DEVICE(TMP90841, tmp90841);
DECLARE_LEGACY_CPU_DEVICE(TMP91640, tmp91640);
DECLARE_LEGACY_CPU_DEVICE(TMP91641, tmp91641);

CPU_DISASSEMBLE( t90 );

#define T90_IOBASE	0xffc0

enum e_ir
{
	T90_P0=T90_IOBASE,	T90_P1,		T90_P01CR_IRFL,	T90_IRFH,	T90_P2,		T90_P2CR,	T90_P3,		T90_P3CR,
	T90_P4,				T90_P4CR,	T90_P5,			T90_SMMOD,	T90_P6,		T90_P7,		T90_P67CR,	T90_SMCR,
	T90_P8,				T90_P8CR,	T90_WDMOD,		T90_WDCR,	T90_TREG0,	T90_TREG1,	T90_TREG2,	T90_TREG3,
	T90_TCLK,			T90_TFFCR,	T90_TMOD,		T90_TRUN,	T90_CAP1L,	T90_CAP1H,	T90_CAP2L,	T90_CAL2H,
	T90_TREG4L,			T90_TREG4H,	T90_TREG5L,		T90_TREG5H,	T90_T4MOD,	T90_T4FFCR,	T90_INTEL,	T90_INTEH,
	T90_DMAEH,			T90_SCMOD,	T90_SCCR,		T90_SCBUF,	T90_BX,		T90_BY,		T90_ADREG,	T90_ADMOD
};

#endif /* __TLCS90_H__ */

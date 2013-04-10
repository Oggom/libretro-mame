/*****************************************************************************
 *
 * includes/pc1251.h
 *
 * Pocket Computer 1251
 *
 ****************************************************************************/

#ifndef PC1251_H_
#define PC1251_H_

#include "machine/nvram.h"

#define PC1251_CONTRAST (ioport("DSW0")->read() & 0x07)


class pc1251_state : public driver_device
{
public:
	pc1251_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu") { }

	UINT8 m_outa;
	UINT8 m_outb;
	int m_power;
	UINT8 m_reg[0x100];

	DECLARE_DRIVER_INIT(pc1251);
	UINT32 screen_update_pc1251(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_CALLBACK_MEMBER(pc1251_power_up);
	DECLARE_WRITE8_MEMBER(pc1251_outa);
	DECLARE_WRITE8_MEMBER(pc1251_outb);
	DECLARE_WRITE8_MEMBER(pc1251_outc);

	DECLARE_READ_LINE_MEMBER(pc1251_reset);
	DECLARE_READ_LINE_MEMBER(pc1251_brk);
	DECLARE_READ8_MEMBER(pc1251_ina);
	DECLARE_READ8_MEMBER(pc1251_inb);
	DECLARE_READ8_MEMBER(pc1251_lcd_read);
	DECLARE_WRITE8_MEMBER(pc1251_lcd_write);
	virtual void machine_start();
	DECLARE_MACHINE_START(pc1260);
	required_device<cpu_device> m_maincpu;
};

#endif /* PC1251_H_ */

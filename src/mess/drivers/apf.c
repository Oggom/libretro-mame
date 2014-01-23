/******************************************************************************

driver by ?

PeT   around February 2008:
 added apfm1000 cartridge loading
 fixed apfm1000 pads
 added apf video mode

todo for apf m1000:
 add exact cpu/video timing. memory controller+6847 memory operations hold cpu
  (backgammon relies on exact video timing)
 support special cartridges (basic, space destroyer)


0000- 2000-2003 PIA of M1000. is itself repeated until 3fff.
She controls " keypads ", the way of video and the loudspeaker. Putting to 0 one of the 4 least
significant bits of 2002, the corresponding line of keys is ***reflxed mng in 2000 (32 keys altogether).
The other four bits of 2002 control the video way. Bit 0 of 2003 controls the interruptions. Bit 3 of 2003 controls
the loudspeaker. 4000-47ff ROM of M1000. is repeated until 5fff and in e000-ffff. 6000-6003 PIA of the APF.
is itself repeated until 63ff. She controls the keyboard of the APF and the cassette. The value of the
number that represents the three bits less significant of 6002 they determine the line of eight
keys that it is ***reflxed mng in 6000. Bit 4 of 6002 controls the motor of the cassette. Bit 5 of
6002 activates or deactivates the recording. Bit 5 of 6002 indicates the level of recording in
the cassette. 6400-64ff would be the interface Here series, optional. 6500-66ff would be the
disk controller Here, optional. 6800-77ff ROM (" cartridge ") 7800-7fff Probably ROM. The BASIC
leaves frees this zone. 8000-9fff ROM (" cartridge ") A000-BFFF ram (8 Kb) C000-DFFF ram
additional (8 Kb) E000-FFFF ROM of M1000 (to see 4000-47FF) The interruption activates with
the vertical synchronism of the video. routine that it executes is in the ROM of M1000
and puts the video in way text during a short interval, so that the first line is seen of
text screen in the superior part of the graphical screen.


6600, 6500-6503 wd179x disc controller? 6400, 6401


- The M1000 contains the RAM, ROM, CPU, PIA0, handsets, Video and cart slot, and thus was a TV Game computer.
- The MPA-10 was a base unit containing the main keyboard, custom cassette recorder and PIA1.
- When the two were joined, they formed the Imagination Machine.
- Although the BASIC cart could be plugged into the M1000, it could not be used as it needs the main keyboard.
- BB-01 Building Block - provides 4 cart slots. Includes a RS-232 cart for a printer or modem.
- R8-K 8K RAM Expansion cart.
- FI-100 Minifloppy Disk Interface Cartridge - drives 1 or 2 5 1/4" floppy drives on AS-400 bus.
- A cassette program must be loaded on the same memory size it was saved from. Since the standard machine
  had 8K, almost all tapes require this exact amount of RAM to be present.


ToDo:
-----
- When pasting a large program, characters can be lost
- Graphics are corrupted
- Tape loading is not very reliable
- Add back the disk support
- Need disk-based software


******************************************************************************************************************/

#include "emu.h"
#include "cpu/m6800/m6800.h"
#include "video/mc6847.h"
#include "sound/speaker.h"
#include "sound/wave.h"
#include "machine/6821pia.h"
#include "machine/wd17xx.h"
#include "imagedev/flopdrv.h"
#include "formats/basicdsk.h"
#include "imagedev/cassette.h"
#include "imagedev/cartslot.h"
#include "formats/apf_apt.h"
#include "machine/ram.h"


class apf_state : public driver_device
{
public:
	apf_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_p_videoram(*this, "videoram")
		, m_maincpu(*this, "maincpu")
		, m_ram(*this, RAM_TAG)
		, m_crtc(*this, "mc6847")
		, m_speaker(*this, "speaker")
		, m_pia0(*this, "pia0")
		, m_pia1(*this, "pia1")
		, m_cass(*this, "cassette")
		, m_fdc(*this, "fdc")
	{ }

	DECLARE_READ8_MEMBER(apf_mc6847_videoram_r);
	DECLARE_WRITE_LINE_MEMBER(apf_mc6847_fs_w);
	DECLARE_READ8_MEMBER(pia0_porta_r);
	DECLARE_WRITE8_MEMBER(pia0_portb_w);
	DECLARE_WRITE_LINE_MEMBER(pia0_ca2_w);
	DECLARE_READ8_MEMBER(pia1_porta_r);
	DECLARE_READ8_MEMBER(pia1_portb_r);
	DECLARE_WRITE8_MEMBER(pia1_portb_w);
	DECLARE_WRITE8_MEMBER(apf_dischw_w);
	DECLARE_READ8_MEMBER(serial_r);
	DECLARE_WRITE8_MEMBER(serial_w);
	DECLARE_WRITE8_MEMBER(apf_wd179x_command_w);
	DECLARE_WRITE8_MEMBER(apf_wd179x_track_w);
	DECLARE_WRITE8_MEMBER(apf_wd179x_sector_w);
	DECLARE_WRITE8_MEMBER(apf_wd179x_data_w);
	DECLARE_READ8_MEMBER(apf_wd179x_status_r);
	DECLARE_READ8_MEMBER(apf_wd179x_track_r);
	DECLARE_READ8_MEMBER(apf_wd179x_sector_r);
	DECLARE_READ8_MEMBER(apf_wd179x_data_r);
private:
	UINT8 m_keyboard_data;
	UINT8 m_pad_data;
	UINT8 m_portb;
	bool m_ca2;
	virtual void machine_reset();
	required_shared_ptr<UINT8> m_p_videoram;
	required_device<m6800_cpu_device> m_maincpu;
	required_device<ram_device> m_ram;
	required_device<mc6847_base_device> m_crtc;
	required_device<speaker_sound_device> m_speaker;
	required_device<pia6821_device> m_pia0;
	optional_device<pia6821_device> m_pia1;
	optional_device<cassette_image_device> m_cass;
	optional_device<wd1770_device> m_fdc;
};


READ8_MEMBER( apf_state::apf_mc6847_videoram_r )
{
	if BIT(m_pad_data, 7) // ag line
	{
		UINT16 part1 = offset & 0x1f;
		UINT16 part2 = (offset & 0x1e0) >> 5;
		UINT16 part3 = (offset & 0x1e00) >> 4;
		UINT16 latch = m_p_videoram[part3 | part1] & 0x3f;
		m_crtc->css_w(BIT(latch, 5));
		latch = (latch & 0x1f) << 4;
		return m_p_videoram[latch | part2 | 0x200];
	}
	else
	{
		offset = (offset & 0x1ff) | 0x200;
		m_crtc->css_w(m_ca2 && BIT(m_p_videoram[offset], 6));
		m_crtc->inv_w(BIT(m_p_videoram[offset], 6));
		m_crtc->as_w(BIT(m_p_videoram[offset], 7));

		return m_p_videoram[offset];
	}
}

WRITE_LINE_MEMBER( apf_state::apf_mc6847_fs_w )
{
	m_pia0->cb1_w(state);
}

READ8_MEMBER( apf_state::pia0_porta_r )
{
	UINT8 data = 0xff;

	if (!BIT(m_pad_data, 3))
		data &= ioport("joy3")->read();
	if (!BIT(m_pad_data, 2))
		data &= ioport("joy2")->read();
	if (!BIT(m_pad_data, 1))
		data &= ioport("joy1")->read();
	if (!BIT(m_pad_data, 0))
		data &= ioport("joy0")->read();

	return data;
}

WRITE8_MEMBER( apf_state::pia0_portb_w )
{
	/* bit 7..6 video control */
	m_crtc->ag_w(BIT(data, 7));
	m_crtc->gm0_w(BIT(data, 6));

	/* bit 3..0 keypad line select */
	m_pad_data = data;
}

WRITE_LINE_MEMBER( apf_state::pia0_ca2_w )
{
	m_ca2 = state;
}

READ8_MEMBER( apf_state::pia1_porta_r )
{
	static const char *const keynames[] = { "key0", "key1", "key2", "key3", "key4", "key5", "key6", "key7" };
	return ioport(keynames[m_keyboard_data])->read();
}

READ8_MEMBER( apf_state::pia1_portb_r )
{
	UINT8 data = m_portb;

	if (m_cass->input() > 0.0038)
		data |= 0x80;

	return data;
}


WRITE8_MEMBER( apf_state::pia1_portb_w )
{
	/* bits 2..0 = keyboard line */
	/* bit 3 = cass audio enable */
	/* bit 4 = cassette motor */
	/* bit 5 = /cass write enable */
	/* bit 6 = cass out */
	/* bit 7 = cass in */

	m_portb = data & 0x7f;
	m_keyboard_data = data & 7;

	m_cass->change_state(BIT(data, 4) ? CASSETTE_MOTOR_ENABLED : CASSETTE_MOTOR_DISABLED, CASSETTE_MASK_MOTOR);

	if (!BIT(data, 5))
		m_cass->output(BIT(data, 6) ? -1.0 : 1.0);
}

void apf_state::machine_reset()
{
	m_portb = 0;
	m_ca2 = 0;
	m_crtc->gm1_w(1);
	m_crtc->gm2_w(1);

	if (m_cass) // apfimag only
		m_cass->change_state(CASSETTE_MOTOR_DISABLED, CASSETTE_MASK_MOTOR);

	/* if we specified 8K of RAM, delete the extended RAM */
	address_space &space = m_maincpu->space(AS_PROGRAM);
	if (m_ram->size() == 8*1024)
		space.unmap_readwrite(0xc000, 0xdfff);
}

WRITE8_MEMBER( apf_state::apf_dischw_w)
{
	/* bit 3 is index of drive to select */
	UINT8 drive = (data>>3) & 0x01;

	wd17xx_set_drive(m_fdc, drive);

	logerror("disc w %04x %04x\n",offset,data);
}

READ8_MEMBER( apf_state::serial_r)
{
	logerror("serial r %04x\n",offset);
	return 0;
}

WRITE8_MEMBER( apf_state::serial_w)
{
	logerror("serial w %04x %04x\n",offset,data);
}

WRITE8_MEMBER( apf_state::apf_wd179x_command_w)
{
	wd17xx_command_w(m_fdc, space, offset,~data);
}

WRITE8_MEMBER( apf_state::apf_wd179x_track_w)
{
	wd17xx_track_w(m_fdc, space, offset,~data);
}

WRITE8_MEMBER( apf_state::apf_wd179x_sector_w)
{
	wd17xx_sector_w(m_fdc, space, offset,~data);
}

WRITE8_MEMBER( apf_state::apf_wd179x_data_w)
{
	wd17xx_data_w(m_fdc, space, offset,~data);
}

READ8_MEMBER( apf_state::apf_wd179x_status_r)
{
	return ~wd17xx_status_r(m_fdc, space, offset);
}

READ8_MEMBER( apf_state::apf_wd179x_track_r)
{
	return ~wd17xx_track_r(m_fdc, space, offset);
}

READ8_MEMBER( apf_state::apf_wd179x_sector_r)
{
	return ~wd17xx_sector_r(m_fdc, space, offset);
}

READ8_MEMBER( apf_state::apf_wd179x_data_r)
{
	return wd17xx_data_r(m_fdc, space, offset); // should this be inverted like the rest?
}

#if 0
static ADDRESS_MAP_START( apf_imagination_map, AS_PROGRAM, 8, apf_state )
	AM_RANGE( 0x0000, 0x03ff) AM_RAM AM_SHARE("p_videoram") AM_MIRROR(0x1c00)
	AM_RANGE( 0x2000, 0x3fff) AM_DEVREADWRITE("pia_0", pia6821_device, read, write)
	AM_RANGE( 0x4000, 0x47ff) AM_ROM AM_REGION("maincpu", 0x10000) AM_MIRROR(0x1800)
	AM_RANGE( 0x6000, 0x63ff) AM_DEVREADWRITE("pia_1", pia6821_device, read, write)
	AM_RANGE( 0x6400, 0x64ff) AM_READWRITE(serial_r, serial_w)
	AM_RANGE( 0x6500, 0x6500) AM_READWRITE(apf_wd179x_status_r, apf_wd179x_command_w)
	AM_RANGE( 0x6501, 0x6501) AM_READWRITE(apf_wd179x_track_r, apf_wd179x_track_w)
	AM_RANGE( 0x6502, 0x6502) AM_READWRITE(apf_wd179x_sector_r, apf_wd179x_sector_w)
	AM_RANGE( 0x6503, 0x6503) AM_READWRITE(apf_wd179x_data_r, apf_wd179x_data_w)
	AM_RANGE( 0x6600, 0x6600) AM_WRITE(apf_dischw_w)
	AM_RANGE( 0x6800, 0x77ff) AM_ROM
	AM_RANGE( 0x7800, 0x7fff) AM_NOP
	AM_RANGE( 0x8000, 0x9fff) AM_ROM
	AM_RANGE( 0xa000, 0xdfff) AM_RAM
	AM_RANGE( 0xe000, 0xe7ff) AM_ROM AM_REGION("maincpu", 0x10000) AM_MIRROR(0x1800)
ADDRESS_MAP_END
#endif

static ADDRESS_MAP_START( apfm1000_map, AS_PROGRAM, 8, apf_state )
	AM_RANGE( 0x0000, 0x03ff) AM_MIRROR(0x1c00) AM_RAM AM_SHARE("videoram")
	AM_RANGE( 0x2000, 0x3fff) AM_MIRROR(0x1ffc) AM_DEVREADWRITE("pia0", pia6821_device, read, write)
	AM_RANGE( 0x4000, 0x47ff) AM_MIRROR(0x1800) AM_ROM AM_REGION("roms", 0)
	AM_RANGE( 0x6800, 0x7fff) AM_ROM AM_REGION("cart", 0x2000)
	AM_RANGE( 0x8000, 0x9fff) AM_ROM AM_REGION("cart", 0)
	AM_RANGE( 0xa000, 0xbfff) AM_RAM // standard
	AM_RANGE( 0xc000, 0xdfff) AM_RAM // expansion
	AM_RANGE( 0xe000, 0xe7ff) AM_MIRROR(0x1800) AM_ROM AM_REGION("roms", 0)
ADDRESS_MAP_END

static ADDRESS_MAP_START( apfimag_map, AS_PROGRAM, 8, apf_state )
	AM_IMPORT_FROM(apfm1000_map)
	AM_RANGE( 0x6000, 0x63ff) AM_MIRROR(0x03fc) AM_DEVREADWRITE("pia1", pia6821_device, read, write)
ADDRESS_MAP_END


/* The following input ports definitions are wrong and can't be debugged unless the driver
   is capable of running more cartridges. However each of the controllers supported by the M-1000
   have these features:

   1 8-way joystick
   1 big red fire button on the upper side
   12-keys keypad with the following layout

   7 8 9 0
   4 5 6 Cl
   1 2 3 En

   On the control panel of the M-1000 there are two big buttons: a Reset key and the Power switch

   Reference: http://www.nausicaa.net/~lgreenf/apfpage2.htm
*/

static INPUT_PORTS_START( apfm1000 )

	/*
	   Using another known APF emulator, this simple Basic program can be used to read
	   the joysticks and the keyboard:

	   10 PRINT KEY$(n);
	   20 GOTO 10

	   where n = 0, 1 or 2 - 0 = keyboard, 1,2 = joysticks #1 and #2

	   When reading the keyboard KEY$(0) returns the character associated to the key, with the
	   following exceptions:

	   Ctrl =    CHR$(1)
	   Rept =    CHR$(2)
	   Here Is = CHR$(4)
	   Rubout =  CHR$(8)

	   When reading the joysticks, KEY$() = "N", "S", "E", "W" for the directions
	                                        "0" - "9" for the keypad digits
	                                        "?" for "Cl"
	                                        "!" for "En"

	*/

/*
  ? player right is player 1
 */

	/* line 0 */
	PORT_START("joy0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 1") PORT_CODE(KEYCODE_1) PORT_PLAYER(1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 0") PORT_CODE(KEYCODE_0) PORT_PLAYER(1)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 4") PORT_CODE(KEYCODE_4) PORT_PLAYER(1)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 7") PORT_CODE(KEYCODE_7) PORT_PLAYER(1)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 1") PORT_CODE(KEYCODE_1_PAD) PORT_PLAYER(2)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 0") PORT_CODE(KEYCODE_0_PAD) PORT_PLAYER(2)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 4") PORT_CODE(KEYCODE_4_PAD) PORT_PLAYER(2)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 7") PORT_CODE(KEYCODE_7_PAD) PORT_PLAYER(2)

	/* line 1 */
	PORT_START("joy1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN) PORT_NAME("PAD 1/RIGHT down") PORT_PLAYER(1) PORT_8WAY
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT) PORT_NAME("PAD 1/RIGHT right") PORT_PLAYER(1) PORT_8WAY
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP) PORT_NAME("PAD 1/RIGHT up") PORT_PLAYER(1) PORT_8WAY
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT) PORT_NAME("PAD 1/RIGHT left") PORT_PLAYER(1) PORT_8WAY
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN) PORT_NAME("PAD 2/LEFT down") PORT_PLAYER(2) PORT_8WAY
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT) PORT_NAME("PAD 2/LEFT right") PORT_PLAYER(2) PORT_8WAY
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_UP) PORT_NAME("PAD 2/LEFT up") PORT_PLAYER(2) PORT_8WAY
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT) PORT_NAME("PAD 2/LEFT left") PORT_PLAYER(2) PORT_8WAY

	/* line 2 */
	PORT_START("joy2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 3") PORT_CODE(KEYCODE_3) PORT_PLAYER(1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT clear") PORT_CODE(KEYCODE_DEL) PORT_PLAYER(1)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 6") PORT_CODE(KEYCODE_6) PORT_PLAYER(1)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 9") PORT_CODE(KEYCODE_9) PORT_PLAYER(1)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 3") PORT_CODE(KEYCODE_3_PAD) PORT_PLAYER(2)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT clear") PORT_CODE(KEYCODE_DEL_PAD) PORT_PLAYER(2)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 6") PORT_CODE(KEYCODE_6_PAD) PORT_PLAYER(2)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 9") PORT_CODE(KEYCODE_9_PAD) PORT_PLAYER(2)

	/* line 3 */
	PORT_START("joy3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 2") PORT_CODE(KEYCODE_2) PORT_PLAYER(1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT enter/fire") PORT_CODE(KEYCODE_ENTER) PORT_PLAYER(1)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 5") PORT_CODE(KEYCODE_5) PORT_PLAYER(1)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 8") PORT_CODE(KEYCODE_8) PORT_PLAYER(1)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 2") PORT_CODE(KEYCODE_2_PAD) PORT_PLAYER(2)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT enter/fire") PORT_CODE(KEYCODE_ENTER_PAD) PORT_PLAYER(2)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 5") PORT_CODE(KEYCODE_5_PAD) PORT_PLAYER(2)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 8") PORT_CODE(KEYCODE_8_PAD) PORT_PLAYER(2)
INPUT_PORTS_END


static INPUT_PORTS_START( apfimag )

	PORT_INCLUDE( apfm1000 )

	/* Reference: http://www.nausicaa.net/~lgreenf/apfpage2.htm */

	/* keyboard line 0 */
	PORT_START("key0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("X")               PORT_CODE(KEYCODE_X)          PORT_CHAR('X')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Z")               PORT_CODE(KEYCODE_Z)          PORT_CHAR('Z')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Q       IF")      PORT_CODE(KEYCODE_Q)          PORT_CHAR('Q')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("2   \"    LET")   PORT_CODE(KEYCODE_2)          PORT_CHAR('2') PORT_CHAR('\"')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A")               PORT_CODE(KEYCODE_A)          PORT_CHAR('A')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1   !   GOSUB")   PORT_CODE(KEYCODE_1)          PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("W       STEP")    PORT_CODE(KEYCODE_W)          PORT_CHAR('W')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("S")               PORT_CODE(KEYCODE_S)          PORT_CHAR('S')

	/* keyboard line 1 */
	PORT_START("key1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C")               PORT_CODE(KEYCODE_C)          PORT_CHAR('C')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("V")               PORT_CODE(KEYCODE_V)          PORT_CHAR('V')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("R       READ")    PORT_CODE(KEYCODE_R)          PORT_CHAR('R')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("3   #   DATA")    PORT_CODE(KEYCODE_3)          PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F")               PORT_CODE(KEYCODE_F)          PORT_CHAR('F')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("4   $   INPUT")   PORT_CODE(KEYCODE_4)          PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E       STOP")    PORT_CODE(KEYCODE_E)          PORT_CHAR('E')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D")               PORT_CODE(KEYCODE_D)          PORT_CHAR('D')

	/* keyboard line 2 */
	PORT_START("key2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("N   ^")           PORT_CODE(KEYCODE_N)          PORT_CHAR('N') PORT_CHAR('^')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B")               PORT_CODE(KEYCODE_B)          PORT_CHAR('B')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("T       NEXT")    PORT_CODE(KEYCODE_T)          PORT_CHAR('T')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("6   &   FOR")     PORT_CODE(KEYCODE_6)          PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G")               PORT_CODE(KEYCODE_G)          PORT_CHAR('G')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("5   %   DIM")     PORT_CODE(KEYCODE_5)          PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Y       PRINT")   PORT_CODE(KEYCODE_Y)          PORT_CHAR('Y')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("H")               PORT_CODE(KEYCODE_H)          PORT_CHAR('H')

	/* keyboard line 3 */
	PORT_START("key3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("M   ]")           PORT_CODE(KEYCODE_M)          PORT_CHAR('M') PORT_CHAR(']')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(",   <")           PORT_CODE(KEYCODE_COMMA)      PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("I       LIST")    PORT_CODE(KEYCODE_I)          PORT_CHAR('I')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("7   '   RETURN")  PORT_CODE(KEYCODE_7)          PORT_CHAR('7') PORT_CHAR('\'')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("K   [")           PORT_CODE(KEYCODE_K)          PORT_CHAR('K') PORT_CHAR('[')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("8   (   THEN")    PORT_CODE(KEYCODE_8)          PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("U       END")     PORT_CODE(KEYCODE_U)          PORT_CHAR('U')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("J")               PORT_CODE(KEYCODE_J)          PORT_CHAR('J')

	/* keyboard line 4 */
	PORT_START("key4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("/   ?")           PORT_CODE(KEYCODE_SLASH)      PORT_CHAR('/') PORT_CHAR('?')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(".   >")           PORT_CODE(KEYCODE_STOP)       PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("O   _   REM")     PORT_CODE(KEYCODE_O)          PORT_CHAR('O') PORT_CHAR('_')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("0       GOTO")    PORT_CODE(KEYCODE_0)          PORT_CHAR('0')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("L   \\")          PORT_CODE(KEYCODE_L)          PORT_CHAR('L') PORT_CHAR('\\')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("9   )   ON")      PORT_CODE(KEYCODE_9)          PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("P   @   USING")   PORT_CODE(KEYCODE_P)          PORT_CHAR('P') PORT_CHAR('@')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(";   +")           PORT_CODE(KEYCODE_COLON)      PORT_CHAR(';') PORT_CHAR('+')

	/* keyboard line 5 */
	PORT_START("key5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Space")           PORT_CODE(KEYCODE_SPACE)      PORT_CHAR(32)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(":   *")           PORT_CODE(KEYCODE_MINUS)      PORT_CHAR(':') PORT_CHAR('*')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Return")          PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR(13)
	PORT_BIT(0x08, 0x08, IPT_UNUSED)
	PORT_BIT(0x10, 0x10, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("-   =   RESTORE") PORT_CODE(KEYCODE_EQUALS)     PORT_CHAR('-') PORT_CHAR('=')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Line Feed")       PORT_CODE(KEYCODE_OPENBRACE)  PORT_CHAR(10)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Rubout")          PORT_CODE(KEYCODE_QUOTE)      PORT_CHAR(8)

	/* line 6 */
	PORT_START("key6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift")     PORT_CODE(KEYCODE_RSHIFT) PORT_CODE(KEYCODE_LSHIFT)     PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Esc")             PORT_CODE(KEYCODE_TAB)        PORT_CHAR(27)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ctrl")            PORT_CODE(KEYCODE_LCONTROL)   PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Rept")            PORT_CODE(KEYCODE_ENTER)      PORT_CHAR(UCHAR_MAMEKEY(TAB))
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Break")           PORT_CODE(KEYCODE_BACKSLASH)  PORT_CHAR(UCHAR_MAMEKEY(DEL))
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Here Is")         PORT_CODE(KEYCODE_BACKSPACE)  PORT_CHAR(UCHAR_MAMEKEY(HOME))
	PORT_BIT(0x40, 0x40, IPT_UNUSED) // another X
	PORT_BIT(0x80, 0x80, IPT_UNUSED) // another Z

	/* line 7 */
	PORT_START("key7")
	PORT_BIT(0xff, 0xff, IPT_UNUSED)
INPUT_PORTS_END



static const cassette_interface apf_cassette_interface =
{
	apf_cassette_formats,
	NULL,
	(cassette_state)(CASSETTE_PLAY),
	NULL,
	NULL
};

static LEGACY_FLOPPY_OPTIONS_START(apfimag)
	LEGACY_FLOPPY_OPTION(apfimag, "apd", "APF disk image", basicdsk_identify_default, basicdsk_construct_default, NULL,
		HEADS([1])
		TRACKS([40])
		SECTORS([8])
		SECTOR_LENGTH([256])
		FIRST_SECTOR_ID([1]))
LEGACY_FLOPPY_OPTIONS_END

static const floppy_interface apfimag_floppy_interface =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	FLOPPY_STANDARD_5_25_SSDD_40,
	LEGACY_FLOPPY_OPTIONS_NAME(apfimag),
	NULL,
	NULL
};

static const mc6847_interface apf_mc6847_intf =
{
	"screen",
	DEVCB_DRIVER_MEMBER(apf_state, apf_mc6847_videoram_r),
	DEVCB_NULL,
	DEVCB_DRIVER_LINE_MEMBER(apf_state, apf_mc6847_fs_w)
};

static MACHINE_CONFIG_START( apfm1000, apf_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6800, XTAL_3_579545MHz / 4 )  // divided by 4 in external clock circuit
	MCFG_CPU_PROGRAM_MAP(apfm1000_map)

	/* video hardware */
	MCFG_SCREEN_MC6847_NTSC_ADD("screen", "mc6847")
	MCFG_MC6847_ADD("mc6847", MC6847_NTSC, XTAL_3_579545MHz, apf_mc6847_intf)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	/* Devices */
	MCFG_DEVICE_ADD("pia0", PIA6821, 0)
	MCFG_PIA_READPA_HANDLER(READ8(apf_state, pia0_porta_r))
	MCFG_PIA_WRITEPB_HANDLER(WRITE8(apf_state, pia0_portb_w))
	MCFG_PIA_CA2_HANDLER(WRITELINE(apf_state, pia0_ca2_w))
	MCFG_PIA_CB2_HANDLER(DEVWRITELINE("speaker", speaker_sound_device, level_w))
	MCFG_PIA_IRQA_HANDLER(DEVWRITELINE("maincpu", m6800_cpu_device, irq_line))
	MCFG_PIA_IRQB_HANDLER(DEVWRITELINE("maincpu", m6800_cpu_device, irq_line))

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("8K")
	MCFG_RAM_EXTRA_OPTIONS("16K")

	MCFG_CARTSLOT_ADD("cart")
	MCFG_CARTSLOT_INTERFACE("apfm1000_cart")

	/* software lists */
	MCFG_SOFTWARE_LIST_ADD("cart_list","apfm1000")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( apfimag, apfm1000 )
	MCFG_CPU_MODIFY( "maincpu" )
	MCFG_CPU_PROGRAM_MAP( apfimag_map)

	MCFG_SOUND_WAVE_ADD(WAVE_TAG, "cassette")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.15)

	MCFG_DEVICE_ADD("pia1", PIA6821, 0)
	MCFG_PIA_READPA_HANDLER(READ8(apf_state, pia1_porta_r))
	MCFG_PIA_READPB_HANDLER(READ8(apf_state, pia1_portb_r))
	MCFG_PIA_WRITEPB_HANDLER(WRITE8(apf_state, pia1_portb_w))

	//MCFG_CASSETTE_ADD( "cassette", apf_cassette_interface )
	MCFG_CASSETTE_ADD( "cassette", default_cassette_interface )
	//MCFG_FD1793_ADD("fdc", default_wd17xx_interface ) // TODO confirm type
	//MCFG_LEGACY_FLOPPY_2_DRIVES_ADD(apfimag_floppy_interface)
MACHINE_CONFIG_END


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START(apfimag)
	ROM_REGION(0x0800,"roms", 0)
	ROM_LOAD("apf_4000.rom", 0x0000, 0x0800, CRC(2a331a33) SHA1(387b90882cd0b66c192d9cbaa3bec250f897e4f1) )

	ROM_REGION(0x3800,"cart", ROMREGION_ERASEFF)
	ROM_CART_LOAD("cart", 0x0000, 0x3800, ROM_OPTIONAL)
ROM_END

ROM_START(apfm1000)
	ROM_REGION(0x0800,"roms", 0)
	ROM_LOAD("apf_4000.rom", 0x0000, 0x0800, CRC(2a331a33) SHA1(387b90882cd0b66c192d9cbaa3bec250f897e4f1) )
//  ROM_LOAD("apf-m1000rom.bin", 0x0000, 0x0800, CRC(cc6ac840) SHA1(1110a234bcad99bd0894ad44c591389d16376ca4) )

	ROM_REGION(0x3800,"cart", ROMREGION_ERASEFF)
	ROM_CART_LOAD("cart", 0x0000, 0x3800, ROM_OPTIONAL)
ROM_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

/*    YEAR  NAME     PARENT     COMPAT  MACHINE     INPUT      CLASS          INIT         COMPANY               FULLNAME */
COMP(1977, apfimag,  apfm1000,  0,      apfimag,    apfimag,   driver_device,  0,   "APF Electronics Inc", "APF Imagination Machine" , GAME_NOT_WORKING )
CONS(1978, apfm1000, 0,         0,      apfm1000,   apfm1000,  driver_device,  0,   "APF Electronics Inc", "APF M-1000" , GAME_NOT_WORKING)

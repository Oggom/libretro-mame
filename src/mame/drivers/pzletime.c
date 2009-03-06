/*****************************************************************************************

	Puzzle Time (Prototype)
	Elettronica Video-Games S.R.L, 199?
	
	driver by Angelo Salese and Pierpaolo Prazzoli
	dump and info provided by Yoshi
	
	Notes:
	- Is the brightness effect right or there's a different video effect?
	- In the service menu, where you can configure game options, and when you have to
	  choose the Game Mode, you can't see what is selected, becase the 2 halves of the
	  palette used by txt tilemap have the same data. Is it a real game bug?

*****************************************************************************************/

#include "driver.h"
#include "cpu/m68000/m68000.h"
#include "sound/okim6295.h"
#include "machine/eeprom.h"

static UINT16 *bg_videoram, *mid_videoram, *txt_videoram, *tilemap_regs, *video_regs;
static tilemap *mid_tilemap, *txt_tilemap;
static int ticket = 0;

static TILE_GET_INFO( get_mid_tile_info )
{
	int tileno,colour;

	tileno = mid_videoram[tile_index] & 0x0fff;
	colour = mid_videoram[tile_index] & 0xf000;
	colour = colour >> 12;
	SET_TILE_INFO(2,tileno,colour,0);
}

static TILE_GET_INFO( get_txt_tile_info )
{
	int tileno,colour;

	tileno = txt_videoram[tile_index] & 0x0fff;
	colour = txt_videoram[tile_index] & 0xf000;
	colour = colour >> 12;
	SET_TILE_INFO(0,tileno,colour,0);
}

static VIDEO_START( pzletime )
{
	mid_tilemap = tilemap_create(machine, get_mid_tile_info,tilemap_scan_cols, 16,16,64,16);
	txt_tilemap = tilemap_create(machine, get_txt_tile_info,tilemap_scan_rows,  8, 8,64,32);
	
	tilemap_set_transparent_pen(mid_tilemap,0);
	tilemap_set_transparent_pen(txt_tilemap,0);
}

static VIDEO_UPDATE( pzletime )
{
	int count;
	int y,x;

	bitmap_fill(bitmap, cliprect, screen->machine->pens[0]); //bg pen

	tilemap_set_scrolly(txt_tilemap, 0, tilemap_regs[0]-3);
	tilemap_set_scrollx(txt_tilemap, 0, tilemap_regs[1]);
	
	tilemap_set_scrolly(mid_tilemap, 0, tilemap_regs[2]-3);
	tilemap_set_scrollx(mid_tilemap, 0, tilemap_regs[3]-7);

	if(video_regs[2] & 1)
	{
		count = 0;

		for(y=255;y>=0;y--)
		{
			for(x=0;x<512;x++)
			{
				if(bg_videoram[count] & 0x8000)
				{
					*BITMAP_ADDR16(bitmap, (y - 18) & 0xff, (x - 32) & 0x1ff) = 0x300 + (bg_videoram[count] & 0x7fff);
				}

				count++;
			}
		}
	}
		
	tilemap_draw(bitmap,cliprect,mid_tilemap, 0,0);

	{
		int offs,spr_offs,colour,sx,sy;

		for(offs = 0; offs < 0x2000/2; offs += 4)
		{
			if(spriteram16[offs+0] == 8)
				break;
			
			spr_offs = spriteram16[offs+3] & 0x0fff;			
			sy = 0x200-(spriteram16[offs+0] & 0x1ff)-35;
			sx = (spriteram16[offs+1] & 0x1ff)-30;
			colour = (spriteram16[offs+0] & 0xf000)>>12;
			
			// is spriteram16[offs+0] & 0x200 flipy? it's always set
			
			drawgfx(bitmap,screen->machine->gfx[1],spr_offs,colour,0,1,sx,sy,cliprect,TRANSPARENCY_PEN,0);
		}
	}
	
	tilemap_draw(bitmap,cliprect,txt_tilemap,0,0);
	
	return 0;
}

static WRITE16_HANDLER( mid_videoram_w )
{
	COMBINE_DATA(&mid_videoram[offset]);
	tilemap_mark_tile_dirty(mid_tilemap,offset);
}

static WRITE16_HANDLER( txt_videoram_w )
{
	COMBINE_DATA(&txt_videoram[offset]);
	tilemap_mark_tile_dirty(txt_tilemap,offset);
}

static WRITE16_HANDLER( eeprom_w )
{
	if( ACCESSING_BITS_0_7 )
	{
		eeprom_write_bit(data & 0x01);
		eeprom_set_cs_line((data & 0x02) ? CLEAR_LINE : ASSERT_LINE );
		eeprom_set_clock_line((data & 0x04) ? ASSERT_LINE : CLEAR_LINE );
	}
}

static WRITE16_HANDLER( ticket_w )
{
	if( ACCESSING_BITS_0_7 )
	{
		ticket = data & 1;
	}
}

static WRITE16_HANDLER( video_regs_w )
{
	int i;
	
	COMBINE_DATA(&video_regs[offset]);
	
	if(offset == 0)
	{
		if(video_regs[0] > 0)
		{
			for (i=0;i<0x300;i++)
			{
				palette_set_pen_contrast(space->machine, i, (double)0x8000/(double)video_regs[0]);
			}
		}
	}
	else if(offset == 1)
	{
		if(video_regs[1] > 0)
		{
			for (i=0x300;i<32768 + 0x300;i++)
			{
				palette_set_pen_contrast(space->machine, i, (double)0x8000/(double)video_regs[1]);
			}
		}
	}
}

static WRITE16_DEVICE_HANDLER( oki_bank_w )
{
	okim6295_set_bank_base(device, 0x40000 * (data & 0x3));
}

static CUSTOM_INPUT( ticket_status_r )
{
	return ticket && !(video_screen_get_frame_number(field->port->machine->primary_screen)%128);
}

static ADDRESS_MAP_START( pzletime_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x3fffff) AM_ROM
	AM_RANGE(0x700000, 0x700005) AM_RAM_WRITE(video_regs_w) AM_BASE(&video_regs)
	AM_RANGE(0x800000, 0x800001) AM_DEVREADWRITE8("oki", okim6295_r, okim6295_w, 0x00ff)
	AM_RANGE(0x900000, 0x9005ff) AM_RAM_WRITE(paletteram16_xRRRRRGGGGGBBBBB_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0xa00000, 0xa00007) AM_RAM AM_BASE(&tilemap_regs)
	AM_RANGE(0xb00000, 0xb3ffff) AM_RAM AM_BASE(&bg_videoram)
	AM_RANGE(0xc00000, 0xc00fff) AM_RAM_WRITE(mid_videoram_w) AM_BASE(&mid_videoram)
	AM_RANGE(0xc01000, 0xc01fff) AM_RAM_WRITE(txt_videoram_w) AM_BASE(&txt_videoram)
	AM_RANGE(0xd00000, 0xd01fff) AM_RAM AM_BASE(&spriteram16)
	AM_RANGE(0xe00000, 0xe00001) AM_READ_PORT("INPUT") AM_WRITE(eeprom_w)
	AM_RANGE(0xe00002, 0xe00003) AM_READ_PORT("SYSTEM") AM_WRITE(ticket_w)
	AM_RANGE(0xe00004, 0xe00005) AM_DEVWRITE("oki", oki_bank_w)
	AM_RANGE(0xf00000, 0xf0ffff) AM_RAM
ADDRESS_MAP_END


static INPUT_PORTS_START( pzletime )
	PORT_START("SYSTEM")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_SERVICE_NO_TOGGLE( 0x0004, IP_ACTIVE_LOW )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_VBLANK )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(eeprom_bit_r, NULL) /* eeprom */
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(ticket_status_r, NULL) /* ticket dispenser */
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN )
	
	PORT_START("INPUT")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )
INPUT_PORTS_END

static const gfx_layout layout8x8 =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 1*4, 0*4, 3*4, 2*4, 5*4, 4*4, 7*4, 6*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static const gfx_layout layout16x16 =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 4, 0, 12, 8, 20, 16, 28, 24, 16*32+4, 16*32+0, 16*32+12, 16*32+8, 16*32+20, 16*32+16, 16*32+28, 16*32+24 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32, 8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	32*32
};

static GFXDECODE_START( pzletime )
	GFXDECODE_ENTRY( "gfx1", 0, layout8x8,   0x100, 0x10 )
	GFXDECODE_ENTRY( "gfx2", 0, layout16x16, 0x200, 0x10 )
	GFXDECODE_ENTRY( "gfx3", 0, layout16x16, 0x000, 0x10 )
GFXDECODE_END

static PALETTE_INIT( pzletime )
{
	int i;

	/* first 0x300 colors are dynamic */

	/* initialize 555 RGB lookup */
	for (i = 0;i < 32768;i++)
		palette_set_color_rgb(machine,i+0x300,pal5bit(i >> 10),pal5bit(i >> 5),pal5bit(i >> 0));
}

static MACHINE_DRIVER_START( pzletime )

	/* basic machine hardware */
	MDRV_CPU_ADD("cpu",M68000,10000000)
	MDRV_CPU_PROGRAM_MAP(pzletime_map,0)
	MDRV_CPU_VBLANK_INT("screen",irq4_line_hold)
	
	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500) /* not accurate */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 48*8-1, 0*8, 28*8-1)
	MDRV_GFXDECODE(pzletime)
	MDRV_PALETTE_LENGTH(0x300 + 32768)
	MDRV_NVRAM_HANDLER(93C46)

	MDRV_PALETTE_INIT(pzletime)
	MDRV_VIDEO_START(pzletime)
	MDRV_VIDEO_UPDATE(pzletime)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD("oki", OKIM6295, 937500) //freq & pin7 taken from stlforce
	MDRV_SOUND_CONFIG(okim6295_interface_pin7high)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( pzletime )
	ROM_REGION( 0x400000, "cpu", 0 )	
	ROM_LOAD16_BYTE( "5.bin", 0x000000, 0x80000, CRC(78b027dc) SHA1(6719908a075ecf0666bb817ac8a31056a7f315c6) )
	ROM_LOAD16_BYTE( "1.bin", 0x000001, 0x80000, CRC(0a69cbc7) SHA1(bae8b5746209c6773da27acaec7bd535a69019d2) )
	ROM_LOAD16_BYTE( "6.bin", 0x100000, 0x80000, CRC(526733ef) SHA1(21a921416d1ae7b9d49789d70ae99f240b012489) )
	ROM_LOAD16_BYTE( "2.bin", 0x100001, 0x80000, CRC(2a877266) SHA1(b8e909b3bd21af71782c501fa6eef590045b81e0) )
	ROM_LOAD16_BYTE( "7.bin", 0x200000, 0x80000, CRC(2efdd6d3) SHA1(de35d7a1bcd3ad608b8dfc184e06d6719253a1c7) )
	ROM_LOAD16_BYTE( "3.bin", 0x200001, 0x80000, CRC(1ddacade) SHA1(78f09fdb541e369765abfdf39607ca8f4c771d16) )
	ROM_LOAD16_BYTE( "8.bin", 0x300000, 0x80000, CRC(be7cf043) SHA1(5dadafb6f89f2fc373b77b18746b461117228f08) )
	ROM_LOAD16_BYTE( "4.bin", 0x300001, 0x80000, CRC(374ab900) SHA1(bd7f649bdf2927c1f5cb53492a08cc66c4658a72) )
	
	ROM_REGION( 0x80000, "user1", 0 ) /* Samples */
	ROM_LOAD( "12.bin",  0x00000, 0x80000,  CRC(203897c1) SHA1(c2495871c796bc7f2dabca1630317313b5aa740a) )
	
	ROM_REGION( 0x100000, "oki", 0 )
	ROM_COPY( "user1", 0x000000, 0x000000, 0x020000 )
	ROM_COPY( "user1", 0x000000, 0x020000, 0x020000 )
	ROM_COPY( "user1", 0x000000, 0x040000, 0x020000 )
	ROM_COPY( "user1", 0x020000, 0x060000, 0x020000 )
	ROM_COPY( "user1", 0x000000, 0x080000, 0x020000 )
	ROM_COPY( "user1", 0x040000, 0x0a0000, 0x020000 )
	ROM_COPY( "user1", 0x000000, 0x0c0000, 0x020000 )
	ROM_COPY( "user1", 0x060000, 0x0e0000, 0x020000 )
	
	ROM_REGION( 0x80000, "gfx1", ROMREGION_DISPOSE )
	ROM_LOAD( "10.bin",  0x00000, 0x80000, CRC(d6ed11a5) SHA1(585aad4e962e7c9ba33e96d4d53e2feddd1a6cd9) )

	ROM_REGION( 0x80000, "gfx2", ROMREGION_DISPOSE )
	ROM_LOAD( "11.bin",  0x000000, 0x80000, CRC(566e09a3) SHA1(b04d23bd82c609f35e6b651006b5c029f36f54dc) )

	ROM_REGION( 0x80000, "gfx3", ROMREGION_DISPOSE )
	ROM_LOAD( "9.bin",   0x000000, 0x80000, CRC(a8144a7e) SHA1(9dfdd6c17a91cad6b56c622671042ac2ee2c9ec8) )
ROM_END

GAME( 199?, pzletime, 0, pzletime,  pzletime,  0, ROT0, "Elettronica Video-Games S.R.L.", "Puzzle Time (Prototype)", 0 )

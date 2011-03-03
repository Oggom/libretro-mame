/***************************************************************************

    Renegade Video Hardware

***************************************************************************/

#include "emu.h"
#include "includes/renegade.h"


WRITE8_HANDLER( renegade_videoram_w )
{
	renegade_state *state = space->machine->driver_data<renegade_state>();
	UINT8 *videoram = state->videoram;
	videoram[offset] = data;
	offset = offset % (64 * 16);
	tilemap_mark_tile_dirty(state->bg_tilemap, offset);
}

WRITE8_HANDLER( renegade_videoram2_w )
{
	renegade_state *state = space->machine->driver_data<renegade_state>();
	state->videoram2[offset] = data;
	offset = offset % (32 * 32);
	tilemap_mark_tile_dirty(state->fg_tilemap, offset);
}

WRITE8_HANDLER( renegade_flipscreen_w )
{
	flip_screen_set(space->machine, ~data & 0x01);
}

WRITE8_HANDLER( renegade_scroll0_w )
{
	renegade_state *state = space->machine->driver_data<renegade_state>();
	state->scrollx = (state->scrollx & 0xff00) | data;
}

WRITE8_HANDLER( renegade_scroll1_w )
{
	renegade_state *state = space->machine->driver_data<renegade_state>();
	state->scrollx = (state->scrollx & 0xff) | (data << 8);
}

static TILE_GET_INFO( get_bg_tilemap_info )
{
	renegade_state *state = machine->driver_data<renegade_state>();
	UINT8 *videoram = state->videoram;
	const UINT8 *source = &videoram[tile_index];
	UINT8 attributes = source[0x400]; /* CCC??BBB */
	SET_TILE_INFO(
		1 + (attributes & 0x7),
		source[0],
		attributes >> 5,
		0);
}

static TILE_GET_INFO( get_fg_tilemap_info )
{
	renegade_state *state = machine->driver_data<renegade_state>();
	const UINT8 *source = &state->videoram2[tile_index];
	UINT8 attributes = source[0x400];
	SET_TILE_INFO(
		0,
		(attributes & 3) * 256 + source[0],
		attributes >> 6,
		0);
}

VIDEO_START( renegade )
{
	renegade_state *state = machine->driver_data<renegade_state>();
	state->bg_tilemap = tilemap_create(machine, get_bg_tilemap_info, tilemap_scan_rows,      16, 16, 64, 16);
	state->fg_tilemap = tilemap_create(machine, get_fg_tilemap_info, tilemap_scan_rows,   8, 8, 32, 32);

	tilemap_set_transparent_pen(state->fg_tilemap, 0);
	tilemap_set_scrolldx(state->bg_tilemap, 256, 0);

	state_save_register_global(machine, state->scrollx);
}

static void draw_sprites(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect)
{
	UINT8 *source = machine->generic.spriteram.u8;
	UINT8 *finish = source + 96 * 4;

	while (source < finish)
	{
		int sy = 240 - source[0];

		if (sy >= 16)
		{
			int attributes = source[1]; /* SFCCBBBB */
			int sx = source[3];
			int sprite_number = source[2];
			int sprite_bank = 9 + (attributes & 0xf);
			int color = (attributes >> 4) & 0x3;
			int xflip = attributes & 0x40;

			if (sx > 248)
				sx -= 256;

			if (flip_screen_get(machine))
			{
				sx = 240 - sx;
				sy = 240 - sy;
				xflip = !xflip;
			}

			if (attributes & 0x80) /* big sprite */
			{
				sprite_number &= ~1;
				drawgfx_transpen(bitmap, cliprect, machine->gfx[sprite_bank],
					sprite_number + 1,
					color,
					xflip, flip_screen_get(machine),
					sx, sy + (flip_screen_get(machine) ? -16 : 16), 0);
			}
			else
			{
				sy += (flip_screen_get(machine) ? -16 : 16);
			}
			drawgfx_transpen(bitmap, cliprect, machine->gfx[sprite_bank],
				sprite_number,
				color,
				xflip, flip_screen_get(machine),
				sx, sy, 0);
		}
		source += 4;
	}
}

SCREEN_UPDATE( renegade )
{
	renegade_state *state = screen->machine->driver_data<renegade_state>();
	tilemap_set_scrollx(state->bg_tilemap, 0, state->scrollx);
	tilemap_draw(bitmap, cliprect, state->bg_tilemap, 0 , 0);
	draw_sprites(screen->machine, bitmap, cliprect);
	tilemap_draw(bitmap, cliprect, state->fg_tilemap, 0 , 0);
	return 0;
}

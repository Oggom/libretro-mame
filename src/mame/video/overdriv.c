#include "emu.h"
#include "includes/overdriv.h"

/***************************************************************************

  Callbacks for the K053247

***************************************************************************/

void overdriv_sprite_callback( running_machine &machine, int *code, int *color, int *priority_mask )
{
	overdriv_state *state = machine.driver_data<overdriv_state>();
	int pri = (*color & 0xffe0) >> 5;   /* ??????? */
	if (pri)
		*priority_mask = 0x02;
	else
		*priority_mask = 0x00;

	*color = state->m_sprite_colorbase + (*color & 0x001f);
}


/***************************************************************************

  Callbacks for the K051316

***************************************************************************/

K051316_CB_MEMBER(overdriv_state::zoom_callback_1)
{
	*flags = (*color & 0x40) ? TILE_FLIPX : 0;
	*code |= ((*color & 0x03) << 8);
	*color = m_zoom_colorbase[0] + ((*color & 0x3c) >> 2);
}

K051316_CB_MEMBER(overdriv_state::zoom_callback_2)
{
	*flags = (*color & 0x40) ? TILE_FLIPX : 0;
	*code |= ((*color & 0x03) << 8);
	*color = m_zoom_colorbase[1] + ((*color & 0x3c) >> 2);
}


/***************************************************************************

  Display refresh

***************************************************************************/

UINT32 overdriv_state::screen_update_overdriv(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	m_sprite_colorbase  = m_k053251->get_palette_index(K053251_CI0);
	m_road_colorbase[1] = m_k053251->get_palette_index(K053251_CI1);
	m_road_colorbase[0] = m_k053251->get_palette_index(K053251_CI2);
	m_zoom_colorbase[1] = m_k053251->get_palette_index(K053251_CI3);
	m_zoom_colorbase[0] = m_k053251->get_palette_index(K053251_CI4);

	screen.priority().fill(0, cliprect);

	m_k051316_1->zoom_draw(screen, bitmap, cliprect, 0, 0);
	m_k051316_2->zoom_draw(screen, bitmap, cliprect, 0, 1);

	m_k053246->k053247_sprites_draw( bitmap,cliprect);
	return 0;
}

/******************************************************************************
* FILE
*   Yamaha 3812 emulator interface - MAME VERSION
*
* CREATED BY
*   Ernesto Corvi
*
* UPDATE LOG
*   JB  28-04-2002  Fixed simultaneous usage of all three different chip types.
*                       Used real sample rate when resample filter is active.
*       AAT 12-28-2001  Protected Y8950 from accessing unmapped port and keyboard handlers.
*   CHS 1999-01-09  Fixes new ym3812 emulation interface.
*   CHS 1998-10-23  Mame streaming sound chip update
*   EC  1998        Created Interface
*
* NOTES
*
******************************************************************************/
#include "emu.h"
#include "3812intf.h"
#include "fm.h"
#include "sound/fmopl.h"


static void IRQHandler(void *param,int irq)
{
	ym3812_device *ym3812 = (ym3812_device *) param;
	ym3812->_IRQHandler(irq);
}

void ym3812_device::_IRQHandler(int irq)
{
	if (!m_irq_handler.isnull())
		m_irq_handler(irq);
}

/* Timer overflow callback from timer.c */
void ym3812_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	switch(id)
	{
	case 0:
		ym3812_timer_over(m_chip,0);
		break;

	case 1:
		ym3812_timer_over(m_chip,1);
		break;
	}
}

static void timer_handler(void *param,int c,attotime period)
{
	ym3812_device *ym3812 = (ym3812_device *) param;
	ym3812->_timer_handler(c, period);
}

void ym3812_device::_timer_handler(int c, attotime period)
{
	if( period == attotime::zero )
	{   /* Reset FM Timer */
		m_timer[c]->enable(false);
	}
	else
	{   /* Start FM Timer */
		m_timer[c]->adjust(period);
	}
}


static void ym3812_update_request(void * param, int interval)
{
	ym3812_device *ym3812 = (ym3812_device *) param;
	ym3812->_ym3812_update_request();
}

void ym3812_device::_ym3812_update_request()
{
	m_stream->update();
}


//-------------------------------------------------
//  sound_stream_update - handle a stream update
//-------------------------------------------------

void ym3812_device::sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples)
{
	ym3812_update_one(m_chip, outputs[0], samples);
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void ym3812_device::device_start()
{
	int rate = clock()/72;

	m_irq_handler.resolve();

	/* stream system initialize */
	m_chip = ym3812_init(this,clock(),rate);
	assert_always(m_chip != NULL, "Error creating YM3812 chip");

	m_stream = machine().sound().stream_alloc(*this,0,1,rate);

	/* YM3812 setup */
	ym3812_set_timer_handler (m_chip, timer_handler, this);
	ym3812_set_irq_handler   (m_chip, IRQHandler, this);
	ym3812_set_update_handler(m_chip, ym3812_update_request, this);

	m_timer[0] = timer_alloc(0);
	m_timer[1] = timer_alloc(1);
}

//-------------------------------------------------
//  device_stop - device-specific stop
//-------------------------------------------------

void ym3812_device::device_stop()
{
	ym3812_shutdown(m_chip);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void ym3812_device::device_reset()
{
	ym3812_reset_chip(m_chip);
}


READ8_MEMBER( ym3812_device::read )
{
	return ym3812_read(m_chip, offset & 1);
}

WRITE8_MEMBER( ym3812_device::write )
{
	ym3812_write(m_chip, offset & 1, data);
}

READ8_MEMBER( ym3812_device::status_port_r ) { return read(space, 0); }
READ8_MEMBER( ym3812_device::read_port_r ) { return read(space, 1); }
WRITE8_MEMBER( ym3812_device::control_port_w ) { write(space, 0, data); }
WRITE8_MEMBER( ym3812_device::write_port_w ) { write(   space, 1, data); }


const device_type YM3812 = &device_creator<ym3812_device>;

ym3812_device::ym3812_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, YM3812, "YM3812", tag, owner, clock, "ym3812", __FILE__),
		device_sound_interface(mconfig, *this),
		m_irq_handler(*this)
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void ym3812_device::device_config_complete()
{
}

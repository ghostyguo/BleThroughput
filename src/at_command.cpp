/*
 ===============================================================================
 Name        : at_command
 Author      : uCXpresso
 Version     : v1.0.0
 Date		 : 2014/11/22
 Description : TODO AT Command Parse
 ===============================================================================
 History
 ---------+---------+--------------------------------------------+-------------
 DATE     |	VERSION |	DESCRIPTIONS							 |	By
 ---------+---------+--------------------------------------------+-------------
 2014/11/22	v1.0.0	First Edition									Jason
 ===============================================================================
 */

#include "at_command.h"

atCommand::atCommand(CStream &serial) {
	m_p_serial = &serial;
	m_step = 0;
}

void atCommand::parse() {
	byte ch;
	while ( m_p_serial->readable() ) {
		ch = m_p_serial->read();
		switch(m_step) {
		case 0:
			if ( ch=='A' || ch=='a' ) m_step++;
			break;

		case 1:
			if ( ch=='T' || ch=='t' ) {
				m_step++;
			} else {
				m_step = 0;
			}
			break;

		case 2:
			if ( ch=='#' || ch==0x0D ) {	// end of command by '#' or Enter (0x0D)
				if ( m_p_handle ) {
					m_p_handle(m_p_serial, m_command);
				}
				m_command.clear();
				m_step = 0;
			} else {
				m_command += (char)ch;
			}
			break;
		}
	}
}

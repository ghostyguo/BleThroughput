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

#ifndef AT_COMMAND_H_
#define AT_COMMAND_H_

#include <class/stream.h>
#include <class/string.h>

class atCommand: public CObject {
	/**@brief AT command handler type. */
	typedef void (*at_command_handle_t) (CStream *p_serial, CString &cmd);
public:
	atCommand(CStream &serial);

	void parse();

	inline void attachHandle(at_command_handle_t cmd_handle) {
		m_p_handle = cmd_handle;
	}

	inline void detachHandle() {
		m_p_handle = NULL;
	}

	//
	///@cond PARIVE
	//
protected:
	uint8_t	m_step;
	CStream *m_p_serial;
	CString m_command;
	at_command_handle_t m_p_handle;
	///@endcond
};

#endif /* AT_COMMAND_H_ */

/* PLEASE REPLACE "CORE_BOARD_REQUIRED" AND "IMF_BOARD_REQUIRED" WIT A APPROPRIATE BOARD SUPPORT FILE FROM module_board-support */
#include <CORE_C22-rev-a.inc>
#include <COM_ECAT-rev-a.inc>

/**
 * @file main.xc
 * @brief Test application for Ctrlproto on Somanet
 * @author Synapticon GmbH <support@synapticon.com>
 */

#include <ethercat_service.h>
#include <ctrlproto.h>


EthercatPorts ethercat_ports = SOMANET_COM_ETHERCAT_PORTS;

/* Test application handling pdos from EtherCat */
static void pdo_handler(chanend coe_out, chanend pdo_out, chanend pdo_in)
{
	timer t;

	unsigned int delay = 100000;
	unsigned int time = 0;

	uint16_t status = 255;
	int i = 0;
	ctrl_proto_values_t InOut;
	ctrl_proto_values_t InOutOld;
	InOut = init_ctrl_proto();
	t :> time;

	while(1)
	{
		ctrlproto_protocol_handler_function(pdo_out,pdo_in,InOut);

		i++;
		if(i >= 999) {
			i = 100;
                }

		InOut.position_actual = i;
		InOut.torque_actual = i;
		InOut.velocity_actual = i;
		InOut.status_word = status;
		InOut.operation_mode_display = InOut.operation_mode;


		if(InOutOld.control_word != InOut.control_word)
		{
			printstr("\nMotor: ");
			printintln(InOut.control_word);
		}

		if(InOutOld.operation_mode != InOut.operation_mode )
		{
			printstr("\nOperation mode: ");
			printintln(InOut.operation_mode);
		}

		if(InOutOld.target_position != InOut.target_position)
		{
			printstr("\nPosition: ");
			printintln(InOut.target_position);
		}

		if(InOutOld.target_velocity != InOut.target_velocity)
		{
			printstr("\nSpeed: ");
			printintln(InOut.target_velocity);
		}

		if(InOutOld.target_torque != InOut.target_torque )
		{
			printstr("\nTorque: ");
			printintln(InOut.target_torque);
		}
	   InOutOld.control_word 	= InOut.control_word;
	   InOutOld.target_position = InOut.target_position;
	   InOutOld.target_velocity = InOut.target_velocity;
	   InOutOld.target_torque = InOut.target_torque;
	   InOutOld.operation_mode = InOut.operation_mode;

	   t when timerafter(time+delay) :> time;

	}

}


int main(void)
{
	chan coe_in;  	 	// CAN from module_ethercat to consumer
	chan coe_out;  		// CAN from consumer to module_ethercat
	chan eoe_in;   		// Ethernet from module_ethercat to consumer
	chan eoe_out;  		// Ethernet from consumer to module_ethercat
	chan eoe_sig;
	chan foe_in;   		// File from module_ethercat to consumer
	chan foe_out;  		// File from consumer to module_ethercat
	chan pdo_in;
	chan pdo_out;

	par
	{
		/* Ethercat Communication Handler Loop */
		on tile[COM_TILE] : {
			ethercat_service(coe_out, coe_in, eoe_out, eoe_in, eoe_sig,
			                foe_out, foe_in, pdo_out, pdo_in, ethercat_ports);
		}

		/* Test application handling pdos from EtherCat */
		on tile[1] :
		{
			pdo_handler(coe_out, pdo_out, pdo_in);
		}
	}

	return 0;
}

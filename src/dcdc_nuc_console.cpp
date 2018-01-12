// Copyright (c) 2017 by McGill Robotics.
// Written by Bei Chen Liu <bei.liu@mail.mcgill.ca>
// All Rights Reserved
// Copyright (c) 2018 by Mini-Box.
// Written by Nicu Pavel <npavel@mini-box.com>
// All Rights Reserved
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "dcdc_nuc.h"
#include <string>

Dcdc_Nuc * dcdc_nuc;
Dcdc_Nuc_Data dcdc_data;


void show_state() {
    std::string state;
    switch (dcdc_data.state_machine_state) {
      case 1: state = "Low Power"; break;
      case 2: state = "Off"; break;
      case 3: state = "Wait ignition to output on"; break;
      case 4: state = "Ouput on"; break;
      case 5: state = "Output on to motherboard pulse"; break;
      case 6: state = "Motherboard pulse on"; break;
      case 7: state = "Normal"; break;
      case 8: state = "Ignition off to motherboard pulse"; break;
      case 9: state = "Hard off delay"; break;
      default: state = "Unknown"; break;
    }
    fprintf(stdout, "State: %s\n", state.c_str());
}

int main(int argc, char **argv) {
    dcdc_nuc = new Dcdc_Nuc();
    dcdc_data = dcdc_nuc->get_data();

    fprintf(stdout, "DCDC NUC PSU found in %s mode\n", dcdc_data.mode ?
      "Automotive" : "Dumb");

    fprintf(stdout, "Firmware Verion %d.%d\n", dcdc_data.firmware_version_major,
      dcdc_data.firmware_version_minor);


    fprintf(stdout, "Input Voltage: %.3f\n", dcdc_data.input_voltage);   
    fprintf(stdout, "Input Current: %.3f\n", dcdc_data.input_current);
    fprintf(stdout, "Output Voltage: %.3f\n", dcdc_data.output_voltage);    
    fprintf(stdout, "Output Current: %.3f\n", dcdc_data.output_current);    
    fprintf(stdout, "Output Power: %.3f\n", dcdc_data.output_power);
    fprintf(stdout, "Ignition Voltage: %.3f\n", dcdc_data.ignition_voltage);   
    fprintf(stdout, "Temperature: %.3f\n", dcdc_data.temperature);
    fprintf(stdout, "Thump Voltage: %.3f\n", dcdc_data.thump_voltage);

    show_state();

    fprintf(stdout, "Mode: %s\n", dcdc_data.mode ? "Automotive" : "Dumb");
    fprintf(stdout, "Output Enabled: %d\n", dcdc_data.output_enabled);
    fprintf(stdout, "Input Voltage Good: %d\n", dcdc_data.input_voltage_good);
    fprintf(stdout, "Ignition Voltage Good: %d\n", dcdc_data.ignition_voltage_good);
    fprintf(stdout, "Mobo Alive (USB): %d\n", dcdc_data.usb_sense);
    fprintf(stdout, "Mobo Alive (Power): %d\n", dcdc_data.mobo_alive_pout);
    fprintf(stdout, "Ignition Raised: %d\n", dcdc_data.ignition_raised);
    fprintf(stdout, "Thump Ouput Enbaled: %d\n", dcdc_data.thump_output_enabled);

    if (dcdc_data.timer_init) {
    fprintf(stdout, "Timer Init: %d\n", dcdc_data.timer_init);
  }


    if (dcdc_data.timer_ignition_cancel == 0) {
      fprintf(stdout, "Ignition Cancel timer reached\n");
      fprintf(stdout, "Auto-shutdown enabled\n");
    }

    if (!dcdc_data.ignition_voltage_good && dcdc_data.output_enabled) {
      fprintf(stdout, "Ignition voltage low, shutdown Soon\n");
    }

    unsigned int mobo_off = dcdc_data.timer_ignition_off_to_mobo_off_pulse;
    if (mobo_off > 0) {
      fprintf(stdout, "Sending motherboard off signal in %d second%s \n",
          mobo_off, mobo_off == 1 ? "" : "s");
    }

    unsigned int hard_off = dcdc_data.timer_hard_off;
    if (hard_off > 15) {
      fprintf(stdout, "Hard shutdown in %d seconds\n", hard_off);
    } else if (hard_off > 5) {
      fprintf(stdout, "Hard shutdown in %d seconds\n", hard_off);
    } else if (hard_off != 0) {
      fprintf(stdout, "Hard shutdown in %d second%s\n", hard_off,
              hard_off == 1 ? "" : "s");
    }

    return 0;
}

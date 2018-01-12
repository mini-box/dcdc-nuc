// Copyright (c) 2017 by McGill Robotics.
// Written by Bei Chen Liu <bei.liu@mail.mcgill.ca>
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

Dcdc_Nuc::Dcdc_Nuc() {
  connect_();
  setup_();
}

Dcdc_Nuc::~Dcdc_Nuc() {
  usb_close(h_);
}

void Dcdc_Nuc::connect_() {
  struct usb_bus *b;
  struct usb_device *d;

  usb_init();
  usb_set_debug(0);
  usb_find_busses();
  usb_find_devices();

  for (b = usb_get_busses(); b != NULL; b = b->next) {
    for (d = b->devices; d != NULL; d = d->next) {
      if ((d->descriptor.idVendor == NUC_VID) &&
        (d->descriptor.idProduct == NUC_PID)) {
        h_ = usb_open(d);
        break;
      }
    }
  }
  if (h_ == NULL) {
    throw std::runtime_error("connect: unable to connect to device.\n");
  }
}

void Dcdc_Nuc::setup_() {
  char buf[65535];

  if (usb_get_driver_np(h_, 0, buf, sizeof(buf)) == 0) {
    if (usb_detach_kernel_driver_np(h_, 0) < 0) {
      throw std::runtime_error("Cannot detach from kernel driver\n");
    }
  }

  if (usb_set_configuration(h_, 1) < 0) {
    throw std::runtime_error("Cannot set configuration 1 for the device\n");
  }

  usleep(1000);

  if (usb_claim_interface(h_, 0) < 0) {
    throw std::runtime_error("Cannot claim interface 0\n");
  }

  if (usb_set_altinterface(h_, 0) < 0) {
    throw std::runtime_error("Cannot set alternate configuration\n");
  }
}

int Dcdc_Nuc::send_(unsigned char *buff) {
  return usb_interrupt_write(h_, USB_ENDPOINT_OUT + 1,
                             reinterpret_cast<char *>(buff), 1, USB_TIMEOUT);
}

int Dcdc_Nuc::recv_(unsigned char *buff, int size) {
  return usb_interrupt_read(h_, USB_ENDPOINT_IN + 1,
                            reinterpret_cast<char *>(buff), size, USB_TIMEOUT);
}

double Dcdc_Nuc::therm_to_temp_(unsigned int termistor) {
  double value;
  if (termistor <= TERMAL_CURVE[0]) {
    value = -40.0;
  } else if (termistor >= TERMAL_CURVE[TERMISTOR_CONSTS_COUNT - 1]) {
    value = 125.0;
  } else {
    int pos = -1;
    for (int i = TERMISTOR_CONSTS_COUNT - 1; i >= 0; i--) {
      if (termistor >= TERMAL_CURVE[i]) {
        pos = i;
        break;
      }
    }

    if (termistor == TERMAL_CURVE[pos]) {
      value = pos * 5.0 - 40.0;
    } else {
      double t1 = pos * 5.0 - 40.0;
      double t2 = (pos + 1) * 5.0 - 40.0;

      double d1 = TERMAL_CURVE[pos];
      double d2 = TERMAL_CURVE[pos + 1];
      double dtemp = (termistor - d1) * (t2 - t1) / (d2 - d1);
      value = dtemp + t1;
    }
  }
  return value;
}

unsigned int Dcdc_Nuc::chars_to_uint_(unsigned char high, unsigned char low) {
  return (high << 8) | low;
}

struct Dcdc_Nuc_Data Dcdc_Nuc::get_data() {
  unsigned char buf[2][MAX_TRANSFER_SIZE];
  int ret = 0;

  unsigned char cmd_out[] = {NUC_OUT_REPORT_IO_DATA, NUC_OUT_REPORT_IO_DATA2};

  // Getting IO data from the PSU
  for (int x = 0; x < 2; x++) {
    if (send_(&cmd_out[x]) < 0) {
      throw std::runtime_error("get_io_data: send get data command failed!\n");
    }
    ret = recv_(buf[x], MAX_TRANSFER_SIZE);
    if (ret < 0) {
      throw std::runtime_error("get_io_data: cannot get device status!\n");
    } else if (ret < 24) {
      throw std::runtime_error("get_io_data: status received is too short!\n");
    }
  }

  struct Dcdc_Nuc_Data data = Dcdc_Nuc_Data();

  // Parsing IO DATA 1
  if (buf[0][0] != NUC_IN_REPORT_IO_DATA) {
    throw std::runtime_error("get_data: command1 in ID mismatched!");
  }

  unsigned int input_flag = chars_to_uint_(buf[0][1], buf[0][2]);
  unsigned int output_flag = chars_to_uint_(buf[0][3], buf[0][4]);

  data.protection_ok = input_flag & 0x0001;
  data.not_protection_fault = input_flag & 0x0002;
  data.not_open_led = input_flag & 0x0004;
  data.not_short_led = input_flag & 0x0008;
  data.cfg1 = input_flag & 0x0100;
  data.cfg2 = input_flag & 0x0200;
  data.cfg3 = input_flag & 0x0400;
  data.mode = input_flag & 0x0800;
  data.usb_sense = input_flag & 0x1000;

  data.input_voltage_good = output_flag & 0x0001;
  data.ignition_voltage_good = output_flag & 0x0002;
  data.mobo_alive_pout = output_flag & 0x0004;
  data.ignition_raised = output_flag & 0x0040;
  data.ignition_falled = output_flag & 0x0080;
  data.output_enabled = output_flag & 0x0100;
  data.thump_output_enabled = output_flag & 0x0200;
  data.control_frequency = output_flag & 0x0400;
  data.not_power_switch = output_flag & 0x0800;

  data.input_voltage = chars_to_uint_(buf[0][5], buf[0][6]) / 1000.0;
  data.input_current = chars_to_uint_(buf[0][7], buf[0][8]) / 1000.0;
  data.output_voltage = chars_to_uint_(buf[0][9], buf[0][10]) / 1000.0;
  data.output_current = chars_to_uint_(buf[0][11], buf[0][12]) / 1000.0;

  if (output_flag & 0x0100) {  // ENOUT
    data.temperature = therm_to_temp_(chars_to_uint_(buf[0][13], buf[0][14]));
  } else {
    data.temperature = 1000.0;
  }

  data.ignition_voltage = chars_to_uint_(buf[0][15], buf[0][16]) / 1000.0;

  data.output_power = ((buf[0][17] << 24) | (buf[0][18] << 16)
    | (buf[0][19] << 8) | buf[0][20]) / 1000.0;
  data.thump_voltage = chars_to_uint_(buf[0][21], buf[0][22]) / 1000.0;

  // Parsing IO DATA 2
  if (buf[1][0] != NUC_IN_REPORT_IO_DATA2) {
    throw std::runtime_error("get_data: command2 in ID mismatched!");
  }

  data.timer_output_on_to_mobo_on_pulse = chars_to_uint_(buf[1][1],
                                                         buf[1][2]) * 10;
  data.timer_init = chars_to_uint_(buf[1][3], buf[1][4]) * 10;
  data.timer_ignition_to_output_on = chars_to_uint_(buf[1][5], buf[1][6]);
  data.timer_thump_output_on_off = chars_to_uint_(buf[1][7], buf[1][8]);
  data.timer_mobo_pulse_width = chars_to_uint_(buf[1][9], buf[1][10]) * 10;
  data.timer_ignition_off_to_mobo_off_pulse = chars_to_uint_(buf[1][11],
                                                             buf[1][12]);
  data.timer_hard_off = chars_to_uint_(buf[1][13], buf[1][14]);
  data.timer_ignition_cancel = chars_to_uint_(buf[1][15],
                                              buf[1][16]);
  data.timer_input_voltage_count = chars_to_uint_(buf[1][17],
                                                  buf[1][18]) * 10;
  data.timer_ignition_voltage_count = chars_to_uint_(buf[1][19],
                                                     buf[1][20]) * 10;

  data.state_machine_state = buf[1][21];
  data.mode = buf[1][22];

  data.firmware_version_major = (buf[1][25] >> 4) & 0x0F;
  data.firmware_version_minor = buf[1][25] & 0x0F;

  return data;
}

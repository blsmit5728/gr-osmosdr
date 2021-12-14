/* -*- c++ -*- */
/*
 * Copyright 2012 Dimitri Stolnikov <horiz0n@gmx.net>
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <boost/foreach.hpp>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>

#include "arg_helpers.h"

#include "cyberradio_source_c.h"

using namespace boost::assign;

cyberradio_source_c_sptr make_cyberradio_source_c(const std::string &args)
{
  return gnuradio::get_initial_sptr(new cyberradio_source_c(args));
}

cyberradio_source_c::cyberradio_source_c(const std::string &args) :
    gr::hier_block2("cyberradio_source_c",
                   gr::io_signature::make(0, 0, 0),
                   gr::io_signature::make(1,
                                          1,
                                          sizeof(gr_complex))),
    _center_freq(0.0f)
{
    dict_t dict = params_to_dict(args);
    std::string host = "192.168.0.10";
    std::string radioType = "ndr551";

    if ( dict.count( "host" ) )
    {
      host = boost::lexical_cast<std::string>(dict["host"]);
    }
    if ( dict.count( "type" ) )
    {
      radioType = boost::lexical_cast<std::string>(dict["type"]);
    }
    mRadioCtlHandler = LibCyberRadio::Driver::getRadioObject(radioType, host);

    /*
    (const std::string &src_ip,
    unsigned short port,
    unsigned int header_byte_offset,
    int samples_per_packet,
    int bytes_per_packet,
    bool swap_bytes,
    bool swap_iq,
    bool tag_packets,
    bool vector_output,
    bool uses_v49_1 = true,
    bool narrowband = false,
    bool debug = false);
    */
    std::string ip("0.0.0.0");
    _src = gr::CyberRadio::vita_udp_rx::make(ip,4991,(7*4),2048,8224,true,false,false,false,false,false,true);
    connect( _src, 0, self(), 0 );

}

std::vector< std::string > cyberradio_source_c::get_devices( bool fake )
{
  std::vector<std::string> devices;
  if ( fake )
  {
    std::string args = "cyberradio,host=192.168.0.10,type=NDR551";
    args += ",label='CyberRadio NDR551'";
    devices.push_back( args );
  }
  return devices;
}

size_t cyberradio_source_c::get_num_channels( void )
{
    return boost::lexical_cast<size_t>(mRadioCtlHandler->getNumTuner());
}

osmosdr::meta_range_t cyberradio_source_c::get_sample_rates( void )
{
    osmosdr::meta_range_t rates;
    LibCyberRadio::Driver::WbddcRateSet wbddc_rates = mRadioCtlHandler->getWbddcRateSet();
    LibCyberRadio::Driver::NbddcRateSet nbddc_rates = mRadioCtlHandler->getNbddcRateSet();
    for(auto it = wbddc_rates.cbegin(); it != wbddc_rates.cend(); ++it)
    {
        std::cout << "Filter Index: " << it->first << " -- Rate: " << it->second << "\n";
        rates += osmosdr::range_t( it->second );
    }
    for(auto it = nbddc_rates.cbegin(); it != nbddc_rates.cend(); ++it)
    {
        std::cout << "Filter Index: " << it->first << " -- Rate: " << it->second << "\n";
        rates += osmosdr::range_t( it->second );
    }
    return rates;
}

double cyberradio_source_c::set_sample_rate( double rate )
{
  return 0;
}

double cyberradio_source_c::get_sample_rate( void )
{
  return 0;
}

osmosdr::freq_range_t cyberradio_source_c::get_freq_range( size_t chan )
{
    osmosdr::freq_range_t range;
    range += osmosdr::range_t( 20e6, 6000e6, 10e6 );
    return range;
}

double cyberradio_source_c::set_center_freq( double freq, size_t chan )
{
  _center_freq = freq;

  return get_center_freq(chan);
}

double cyberradio_source_c::get_center_freq( size_t chan )
{
  return _center_freq;
}

std::vector<std::string> cyberradio_source_c::get_gain_names( size_t chan )
{
  std::vector<std::string> a = {"t","o"};
  return a;
}

osmosdr::gain_range_t cyberradio_source_c::get_gain_range( size_t chan )
{
  osmosdr::gain_range_t range;
  range += osmosdr::range_t( 0.0, -40.0, -1.0 );
  return range;
}

osmosdr::gain_range_t cyberradio_source_c::get_gain_range( const std::string & name, size_t chan )
{
  osmosdr::gain_range_t range;
  range += osmosdr::range_t( 0.0, -40.0, -1.0 );
  return range;
}

double cyberradio_source_c::set_gain( double gain, size_t chan )
{
  return gain;
}

double cyberradio_source_c::set_gain( double gain, const std::string & name, size_t chan )
{
  return gain;
}

double cyberradio_source_c::get_gain( size_t chan )
{
  return 0;
}

double cyberradio_source_c::get_gain( const std::string & name, size_t chan )
{
  return 0;
}

std::vector< std::string > cyberradio_source_c::get_antennas( size_t chan )
{
  std::vector<std::string> a = {"t","o"};
  return a;
}

std::string cyberradio_source_c::set_antenna( const std::string & antenna, size_t chan )
{
  std::string a = "RX0";
  return a;
}

std::string cyberradio_source_c::get_antenna( size_t chan )
{
  std::string a = "RX0";
  return a;
}
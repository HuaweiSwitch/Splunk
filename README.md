Overview

Splunk is the data collection, indexing, and visualization engine for operational intelligence. This distribution include one reference implement of using splunk clollecting and indexing the operation feedback of Cloud Engine switch. 
This implementation has the following components:
 - Makefile
 - netconf.c
 - netconf.h
 - in.txt-Netconf API request for target Cloud Engine switch.
 - out.txt-Netconf API response of target Cloud Engine switch.

Installation

Main steps:
 - Install libnetconf.
 - Complile the netconf.c.
 - Edit the netconf.sh to target the CE switch.
 - Install splunk and login with username and password.
 - Register the script for data collecting.
 - Set the data collection source and frequency.  
 - 
Example usage

An example of using splunk monitoring the powersupply status of CE switch . The function is satisfied based on the assuming that splunk is already registered the netconf script and CE switch is available for netconf connection.
 - Edit the in.txt file with netconf API of CE switch requiring the power supply status.
 - Start and login the splunk GUI.
 - Register the out.txt and set the loading frequency.
 - Index the feedback of netconf API require for searching.
 
References
 
[1] Splunk online documentation is located at:	   
<http://docs.splunk.com/Documentation/Splunk>	   
	   
[2] For installation and set-up instructions, refer to the:	   
<http://docs.splunk.com/Documentation/Splunk/latest/Installation>	   
	   
[3] For release notes, refer to the Known issues in the Release Notes manual:	   
<http://docs.splunk.com/Documentation/Splunk/latest/ReleaseNotes/Knownissues>	 

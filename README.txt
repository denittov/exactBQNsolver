-------------------------------------------------------------------------------
 HOW TO INSTALL qnsolver
-------------------------------------------------------------------------------

Copy the qnsolver directory somewhere in your filesystem and qnsolver will be 
ready to use.

# add execution privilege to qnsolver.sh
chmod +x qnsolver.sh
chmod +x qnsolver_graph.sh
chmod +x qnsolver_multiple.sh

# compile solver
make cleancode
make

# example of execution
sh qnsolver.sh 5 rete1.txt

On 64-bit systems, qnsolver compilation needs multilib support (due to the use
of a 32-bit library). If problems occurs in compilation, try to install 
packages for multilib support.

On Arch Linux distribution 'sudo pacman -S gcc-multilib' (remove gcc packet)

-------------------------------------------------------------------------------
 HOW TO WRITE THE NET CONFIGURATION FILE
-------------------------------------------------------------------------------

The net configuration file required by qnsolver describes the topology of the
net and the characteristics of the stations.
A valid network configuration file for qnsolver must follow this schema:

Number_of_stations
Compact_routing_matrix_description (one row for each not-null matrix element)
Station_properties_definition (one row for each station)

The first row contains the number of stations in the net (hopefully, this need
not further explanations).
The routing must be described through a compact routing matrix description that
consist of one row for each not-null routing matrix element. Each row must have
this structure:

(source_station,destination_station,routing_probability)

source_station and destination_station must be integer values that represents 
indexes of stations involved in the routing (indexes of station are in the
range [0,N-1]). 
Station properties must be described through this structure:

[blocking_type,queue_capacity,number_of_servers,mean_service_time]

The blocking_type value could be 1, for BAS, or 2, for RS-RD.
queue_capacity and number_of_servers must be integer values, mean_service_time
must be specified as a floating point value.
Every other structure, for the file or for the rows, will be rejected by the 
program. No constraints are defined on the configuration file name.


An example:

3
(0,1,1.0)
(1,2,1.0)
(2,0,1.0)
[1,3,1,1.0]
[1,3,1,1.0]
[1,3,1,1.0]

-------------------------------------------------------------------------------
 HOW TO LAUNCH A TERMINAL EXECUTION
-------------------------------------------------------------------------------

1) enter in the qnsolver installation directory;
2) type "sh qnsolver.sh n_job conf_filepath" and press enter.

n_job: total number of job in the net
conf_filepath: path of the configuration file of the net (relative or absolute)

-------------------------------------------------------------------------------
 qnsolver_graph.sh
-------------------------------------------------------------------------------

The script qnsolver_graph.sh generates exact indices from 1 to N jobs in the
network given as parameter

Usage:
sh qnsolver_graph.sh <N> <net config file>

Example: Generate indices from 1 to 14 jobs in network of rete1.txt
sh qnsolver_graph.sh 14 rete1.txt

-------------------------------------------------------------------------------
 qnsolver_multiple.sh
-------------------------------------------------------------------------------

The script qnsolver_multiple.sh generate indices for each network configuration
file in folder MULTIPLE_NET_INPUT/

1) Put configuration file in MULTIPLE_NET_INPUT/ folder. Each file name MUST
have the following pattern

<net_name>_<N>.txt

Example:
1_15.txt

2) <N> is the total number of job. qnsolver_multiple generates indices for each file 
from 1 to <N> jobs. Output files are in folder MULTIPLE_NET_OUTPUT_EXACT/

3) OUTPUT FILES
- files for indices (X,T,N,U,block prob.)
- net.txt contains the input file of network
- station_num.txt is the number of stations for the input network
- result_verbose.txt contains the summary of indices for each value of job number






